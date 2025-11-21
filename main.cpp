#include "crow.h";
#include "crow/multipart.h";


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

    CROW_ROUTE(app, "/admin/products/delete").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req)
    {
        crow::multipart::message mlt = crow::multipart::message(req);
        for (auto& part : mlt.parts) {
            // use all the ids to emit delete
            std::cout << part.body << " "; 
        }

       /* return std::unordered_map<string, string> {
            {"status", "successful"},
        };*/
        return "hello";
    });

    app.port(5000).multithreaded().run();
}