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
	string name;
	int total_quantity;   // pending quantity here
};

//Structure to display the info of an order to its customer
struct OrderItem
{
	int product_id;
	string name;
	int quantity;
	int is_ready;
};

struct Order
{
	int order_id;
	string customer_name;
	int time;
	vector<OrderItem> items;
};


sqlite3* openDB();
void CreateTables(sqlite3* db);
void addProduct(sqlite3* db, std::string name, std::string subgroup, double price);
void updateProduct(sqlite3* db, int id, std::string name, std::string subgroup, double price);
std::vector<Product> selectAllProducts(sqlite3* db);
int addOrder(sqlite3* db, std::string time, std::string name);
void addProductToOrder(sqlite3* db, int order_id, int product_id, int quantity);
void updateStatus(sqlite3* db, int order_id, int product_id);

// TODO: implement these
void deleteProduct(sqlite3* db, int id);
vector<Product_Count> getOrderedProductsBySubgroup(sqlite3* db, const string& subgroup);
void updateProductStatus(std::string name);
void markOldestPendingReady(sqlite3* db, const string& productName);
Order getOrderById(sqlite3* db, int order_id);

// TODO: fix Time in orders table to use time_t type (aka long long) type instead of text
// and handle the db accordingly