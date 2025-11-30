#pragma once
#include <vector>
#include <sqlite3.h>

std::string getValueFromCookie(const std::string cookie_str, const std::string name);
std::string getValueFromCookie(const std::string cookie_str, const std::string name);



// headers for sqlite functions

// structure to store product row in an array
struct Product {
	int id;
	std::string name;
	std::string subgroup;
	double price;
};

sqlite3* openDB();
void CreateTables(sqlite3* db);
void addProduct(sqlite3* db, std::string name, std::string subgroup, double price);
void updateProduct(sqlite3* db, int id, std::string name, std::string subgroup, double price);
std::vector<Product> selectAllProducts(sqlite3* db);
int addOrder(sqlite3* db, std::string time, std::string name);
void addProductToOrder(sqlite3* db, int order_id, int product_id, int quantity);
void updateStatus(sqlite3* db, int order_id, int product_id);