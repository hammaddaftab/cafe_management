#pragma once
#include <vector>
#include <sqlite3.h>
#include "crow.h"

std::string getValueFromCookie(const std::string cookie_str, const std::string name);
bool is_admin(const crow::request& req, const std::string secret_key);
crow::response custom_redirect(std::string path);


// headers and structs for sqlite functions

// structure to store product row in an array
struct Product {
	int id;
	std::string name;
	std::string subgroup;
	double price;
};

//Struct for pending orders for the shopkeeper
struct Product_Count
{
	int product_id;
	std::string name;
	int total_quantity;   // pending quantity here
};

//Structure to display the info of an order to its customer
struct OrderItem
{
	int product_id;
	std::string name;
	int quantity;
	int is_ready;
};

struct Order
{
	int order_id;
	std::string customer_name;
	int time;
	std::vector<OrderItem> items;
};


sqlite3* openDB();
void CreateTables(sqlite3* db);
void addProduct(sqlite3* db, std::string name, std::string subgroup, double price);
void updateProduct(sqlite3* db, int id, std::string name, std::string subgroup, double price);
std::vector<Product> selectAllProducts(sqlite3* db);
int addOrder(sqlite3* db, int time, std::string name);
std::string addProductToOrder(sqlite3* db, int order_id, int product_id, int quantity);
void updateStatus(sqlite3* db, int sr);

// DONE: implement these
void deleteProduct(sqlite3* db, int id);
std::vector<Product_Count> getOrderedProductsBySubgroup(sqlite3* db, const std::string& subgroup);
int markOldestPendingReady(sqlite3* db, const int product_id);
Order getOrderById(sqlite3* db, int order_id);

// Added by Hammad
Product selectProductById(sqlite3* db, int id);

// DONE: fix Time in orders table to use time_t type (aka long long) type instead of text
// and handle the db accordingly

crow::json::wvalue toData(const Product& p);
crow::json::wvalue toData(const Product_Count& pc);
crow::json::wvalue toData(const OrderItem& item);
crow::json::wvalue toData(const Order& o);
crow::json::wvalue toData(const std::vector<Product>& products);
crow::json::wvalue toData(const std::vector<Product_Count>& product_count);