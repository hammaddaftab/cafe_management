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

struct Product_Count {
	std::string name;
	int count;
};

struct Order {
	std::string name;
	int Time;
	std::vector<Product> products;
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
void deleteProduct(int product_id);
std::vector<Product_Count> getOrderedProductsBySubgroup(std::string subgroup);
void updateProductStatus(std::string name);
Order getOrderById(int order_id);

// TODO: fix Time in orders table to use time_t type (aka long long) type instead of text
// and handle the db accordingly