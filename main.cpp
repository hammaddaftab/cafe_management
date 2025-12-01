#include "header.h"


using namespace std;

int main()
{
    crow::SimpleApp app;
    sqlite3* db = openDB();
    CreateTables(db);

    CROW_ROUTE(app, "/test_route")([]() {
        return "Hello world";
        });

    CROW_ROUTE(app, "/admin/products/view")([](const crow::request req) {
        auto page = crow::mustache::load("products_view.html");
        crow::mustache::context ctx{};
        return page.render(ctx);
    });

    CROW_ROUTE(app, "/admin/products/add")([]() {
        auto page = crow::mustache::load("products_add.html");
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
            
        crow::response res{};
        res.redirect("/admin/products/add");
        return res;
    });

    CROW_ROUTE(app, "/admin/products/delete").methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req) {

        crow::multipart::message mlt = crow::multipart::message(req);

        string id = mlt.get_part_by_name("id").body;

        // TODO: implement this sqlite function
        //deleteProduct(id)

        crow::response res{};
        res.redirect("/admin/products/add");
        return res;
    });

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
            crow::json::wvalue error_res{
                {"status", "unsuccessful"},
                {"msg", "Invalid login or password"}
            };
            return crow::response(error_res);
        }
    });

    CROW_ROUTE(app, "/admin/orders/add").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req) {
        crow::query_string q = req.get_body_params();

        string name = q.get("name");
        time_t timestamp = time(NULL);

        // TODO: fix this constant order_id when the function supports timestamps i.e., long long
        // int order_id = addOrder(db, timestamp, name);
        int order_id = 32;

        int count = stoi(q.get("products_count"));
        for (int i = 1; i <= count; i++) {
            auto product = q.get_dict("product" + i);
            addProductToOrder(db, order_id, stoi(product["id"]), stoi(product["quantity"]));
        };
        
        return crow::response{
            {"status", "successful"},
            {"msg", "All ordered items have been successfully received."}
        };
        
    });



    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onaccept([&](const crow::request& req, void** userdata) {
        return false;
    });

    app.port(5000).multithreaded().run();
    
}