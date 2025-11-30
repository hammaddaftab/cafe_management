#include "crow.h";
#include "header.h"


using namespace std;

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/admin/products")([](const crow::request req) {
        auto page = crow::mustache::load("products_view.html");
        crow::mustache::context ctx{};
        return page.render(ctx);
    });

    CROW_ROUTE(app, "/test_route")([]() {
        return "Hello world";
    });

    CROW_ROUTE(app, "/admin/products/add")([]() {
        auto page = crow::mustache::load("products_add.html");
        crow::mustache::context ctx{};
        return page.render(ctx);
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

    //CROW_ROUTE(app, "/admin/products/multipart_test").methods(crow::HTTPMethod::POST)
    //([](const crow::request& req)
    //{
    //    crow::multipart::message mlt = crow::multipart::message(req);
    //    for (auto& part : mlt.parts) {
    //        // use all the ids to emit delete
    //        std::cout << part.body << " "; 
    //    }

    //   /* return std::unordered_map<string, string> {
    //        {"status", "successful"},
    //    };*/
    //    return "hello";
    //});

    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onaccept([&](const crow::request& req, void** userdata) {
        return false;
        });

    app.port(5000).multithreaded().run();
    
}