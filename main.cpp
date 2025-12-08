#include "header.h"


using namespace std;
std::unordered_map<std::string, crow::websocket::connection*> userSockets;


int main()
{
    crow::SimpleApp app;
    sqlite3* db = openDB();
    CreateTables(db);

    CROW_ROUTE(app, "/test_route")([]() {
        return "Hello world";
    });

    // Handles everything realted to products
    // in the cafe, adding, deleting, viewing and editing
    CROW_ROUTE(app, "/admin/products/view")([&](const crow::request req) {
        auto page = crow::mustache::load("viewData.html");
        auto products = selectAllProducts(db);
        crow::mustache::context ctx{ toData(products) };
        return page.render(ctx);
    });

    CROW_ROUTE(app, "/admin/products/add")([]() {
        auto page = crow::mustache::load("addData.html");
        crow::mustache::context ctx{};
        return page.render(ctx);
    });

    CROW_ROUTE(app, "/admin/products/add").methods(crow::HTTPMethod::POST)
     ([&](const crow::request& req) {

        crow::multipart::message mlt = crow::multipart::message(req);

        string name = mlt.get_part_by_name("name").body;
        string subgroup = mlt.get_part_by_name("subgroup").body;
        double price = stod(mlt.get_part_by_name("price").body);
        addProduct(db, name, subgroup, price);
            
        return custom_redirect("/admin/products/view");
    });

    CROW_ROUTE(app, "/admin/products/delete").methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req) {

        int id = crow::json::load(req.body.data())["id"].i();

        // Completed: implement this sqlite function
        deleteProduct(db, id);

        crow::json::wvalue res{
            {"status", "successful"},
            {"msg", "Successfully deleted the product given product"}
        };
        return res;
    });

    CROW_ROUTE(app, "/admin/products/edit")
    ([&](const crow::request& req) {
        crow::query_string q = req.url_params;

        int id = stoi(q.get("id"));
        Product p = selectProductById(db, id);

        auto page = crow::mustache::load("editData.html");
        crow::mustache::context ctx{ toData(p) };
        return page.render(ctx);
    });
    CROW_ROUTE(app, "/admin/products/edit").methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req) {
        crow::query_string q = req.get_body_params();

        string name = q.get("name");
        string subgroup = q.get("subgroup");
        int price = stod(q.get("price"));
        int id = stoi(q.get("id"));

        updateProduct(db, id, name, subgroup, price);

        return custom_redirect("/admin/products/view");
    });



    // Handles authentication for the admin
    // through manually added session cookie
    CROW_ROUTE(app, "/admin/auth").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req)
    {
        const string HARDCODED_USERNAME = "admin@123";
        const string HARDCODED_PASSWORD = "12345678";

        crow::query_string q = req.get_body_params();
        string password = q.get("password");
        string username = q.get("username");

        if (username == HARDCODED_USERNAME && password == HARDCODED_PASSWORD) {
            crow::response res{};

            // add session cookie header
            string key = { "unguessable_random_number" };
            res.add_header(
                "Set-Cookie", string("session_id=") + key + "; Path=/"
            );

            res.redirect("/admin/products");
            return res;
        }
        else {
            crow::response error_res{
                {"status", "unsuccessful"},
                {"msg", "Invalid login or password"}
            };
            return error_res;
        }
    });



    // Handles placing orders, distribution to respective sections
    // updating the statuses and everything
    CROW_ROUTE(app, "/admin/orders/place") // unused
        ([&](const crow::request& req) {
        auto products = selectAllProducts(db);
        auto page = crow::mustache::load("adminOrder.html");

        crow::mustache::context ctx{ toData(products) };

        return page.render(ctx);
    });

    CROW_ROUTE(app, "/admin/orders/subgroup/<string>")
    ([&](const crow::request& req, string subgroup) {
        auto products_count = getOrderedProductsBySubgroup(db, subgroup);
        auto page = crow::mustache::load("subgroupView.html");

        crow::mustache::context ctx{ toData(products_count) };
        ctx["subgroup"] = subgroup;
     
        return page.render(ctx);
    });

    CROW_ROUTE(app, "/admin/orders/done").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req) {
        auto data = crow::json::load(req.body);
        markOldestPendingReady(db, data["product_id"].i());
        return crow::json::wvalue{
            {"status", "successful"},
            {"msg", "Successfully updated the order status."}
        };
    });

    CROW_ROUTE(app, "/admin/orders/add").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req) {
        crow::query_string q = req.get_body_params();
        string name = q.get("name");
        time_t timestamp = time(NULL);

        // DONE: fix this constant order_id when the function supports timestamps i.e., long long
        int order_id = addOrder(db, timestamp, name);
        int count = stoi(q.get("products_count"));
        for (int i = 1; i <= count; i++) {
            string pre = "product" + to_string(i);
            string index_id = pre + "[id]";
            string index_quantity = pre + "[quantity]";

            int product_id = stoi(q.get(index_id));
            int total_quantity = stoi(q.get(index_quantity));

            auto subgroup = addProductToOrder(db, order_id, product_id, total_quantity);

            // for websocket connection
            crow::json::wvalue product_status{
                {"product_id", product_id},
                {"name", name},
                {"total_quantity", total_quantity}
            };
            cout << "order product received, subgroup is: " << subgroup;
            auto it = userSockets.find(subgroup);
            if (it != userSockets.end() && it->second) {
                it->second->send_text(product_status.dump());
            }
        }

        
        crow::json::wvalue success_res{
            {"status", "successful"},
            {"msg", "All ordered items have been successfully received."}
        };
        success_res["data"]["order_id"] = static_cast<uint64_t>(order_id);
        return success_res;
        
    });




    // Handles client side view where they track their order status
    CROW_ROUTE(app, "/orders/status")
        ([&](const crow::request& req) {
        auto q = req.url_params.get("order_id");
        Order client_order = getOrderById(db, stoi(q));

        auto ctx{ toData(client_order) };
        auto page = crow::mustache::load("clientView.html");
        return page.render(ctx);
    });


    // Handles websocket connections for customers
    // as well as for sub-admins in each subgroup of the cafe
    CROW_WEBSOCKET_ROUTE(app, "/subgroup")
    .onaccept([&](const crow::request& req, void** userdata) {
        auto subgroup = new std::string(req.url_params.get("subgroup")); // allocate
        // TODO: verfiy if this is a valid subgroup
        *userdata = subgroup;
        return true;
    })
    .onopen([&](crow::websocket::connection& conn) {
        auto subgroup = static_cast<std::string*>(conn.userdata());
        cout << "the subgroup on open is: " << *subgroup;
        userSockets[*subgroup] = &conn;
    })
    .onmessage([&](crow::websocket::connection& conn, const std::string& message, bool is_binary) {
        auto data = crow::json::load(message);
        auto product_id = data["product_id"].i();
        auto subgroup = string{ data["subgroup"] };
        int order_id = markOldestPendingReady(db, product_id);

        crow::json::wvalue product_status{
            {"product_id", product_id},
            {"total_quantity", -1}
        };

        auto it = userSockets.find(subgroup);
        if (it != userSockets.end() && it->second) {
            it->second->send_text(product_status.dump());
        }

        auto it2 = userSockets.find(to_string(order_id));
        if (it2 != userSockets.end() && it2->second) {
            it2->second->send_text(product_status.dump());
        }

    })
    .onclose([&](crow::websocket::connection& conn, const std::string& reason, uint16_t status) {
        for (auto it = userSockets.begin(); it != userSockets.end(); ) {
            if (it->second == &conn)
                it = userSockets.erase(it);
            else
                ++it;
        }
        auto subgroup = static_cast<std::string*>(conn.userdata());
        delete subgroup; // free memory
    });

    app.port(5000).multithreaded().run();
    
}