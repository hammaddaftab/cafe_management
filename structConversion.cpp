#include "header.h"

crow::json::wvalue toData(const Product& p) {
    crow::json::wvalue d;
    d["id"] = p.id;
    d["name"] = p.name;
    d["subgroup"] = p.subgroup;
    d["price"] = p.price;
    return d;
}
crow::json::wvalue toData(const Product_Count& pc) {
    crow::json::wvalue d;
    d["product_id"] = pc.product_id;
    d["name"] = pc.name;
    d["total_quantity"] = pc.total_quantity;
    return d;
}
crow::json::wvalue toData(const OrderItem& item) {
    crow::json::wvalue d;
    d["product_id"] = item.product_id;
    d["name"] = item.name;
    d["quantity"] = item.quantity;
    d["is_ready"] = item.is_ready;
    return d;
}
crow::json::wvalue toData(const Order& o) {
    crow::json::wvalue d;

    d["order_id"] = o.order_id;
    d["customer_name"] = o.customer_name;
    d["time"] = o.time;

    crow::json::wvalue::list items_list{};

    for (const auto& item : o.items) {
        items_list.push_back(toData(item));
    }

    d["items"] = crow::json::wvalue(items_list);

    return d;
}
crow::json::wvalue toData(const std::vector<Product>& products) {
    crow::json::wvalue::list products_list(crow::json::wvalue::list{});
    for (const auto& p : products) {
        products_list.push_back(toData(p));
    }
    crow::json::wvalue ret{};
    ret["products"] = crow::json::wvalue(products_list);
    return ret;
}
crow::json::wvalue toData(const std::vector<Product_Count>& product_count) {
    crow::json::wvalue::list products_list(crow::json::wvalue::list{});
    for (const auto& p : product_count) {
        products_list.push_back(toData(p));
    }
    crow::json::wvalue ret{};
    ret["product_count"] = crow::json::wvalue(products_list);
    return ret;
}