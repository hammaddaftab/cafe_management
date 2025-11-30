#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

//Structure to store product row in an array
struct Product {
	int id;
	string name;
	string subgroup;
	double price;
};

//Open the connection to the database
sqlite3* openDB() {
	sqlite3* db;
	if (sqlite3_open("inventory.db", &db) != SQLITE_OK)
	{
		cout << "Failed to open the database!!\n";
		return nullptr;
	}

	return db;
}

//Creating all tables
void CreateTables(sqlite3* db)
{
	const char* productsSql =
		"CREATE TABLE IF NOT EXISTS Products("
		"Product_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"Name TEXT,"
		"Subgroup TEXT,"
		"Price REAL);";

	const char* Orders =
		"CREATE TABLE IF NOT EXISTS Orders ("
		"Order_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"Time TEXT,"
		"Customer_name TEXT);";

	const char* ordered_items =
		"CREATE TABLE IF NOT EXISTS OrderItems("
		"Sr INTEGER PRIMARY KEY AUTOINCREMENT,"
		"Order_id INT,"
		"Product_id INT,"
		"Quantity INT,"
		"is_ready INT DEFAULT 0);";

	char* err = nullptr;
	sqlite3_exec(db, productsSql, nullptr, nullptr, &err);
	sqlite3_exec(db, Orders, nullptr, nullptr, &err);
	sqlite3_exec(db, ordered_items, nullptr, nullptr, &err);

}

//Adding products
void addProduct(sqlite3* db, string name, string subgroup, double price)
{
	const char* sql = "INSERT INTO Products (Name, Subgroup, Price) VALUES (?,?,?);";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, subgroup.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_double(stmt, 3, price);

	if (sqlite3_step(stmt) == SQLITE_DONE)
	{
		cout << "Product Added!!\n";
	}

	sqlite3_finalize(stmt);
}

//Updating products
void updateProduct(sqlite3* db, int id, string name, string subgroup, double price)
{
	const char* sql =
		"UPDATE Products SET Name = ?, Subgroup = ?, Price = ? WHERE Product_id = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, subgroup.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_double(stmt, 3, price);
	sqlite3_bind_int(stmt, 4, id);

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	cout << "Product Updated!!";

}

//Selecting all products
vector<Product> selectAllProducts(sqlite3* db)
{
	vector<Product> list;

	const char* sql = "SELECT * FROM Products;";
	sqlite3_stmt* stmt;

	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		Product p;

		p.id = sqlite3_column_int(stmt, 0);
		p.name = (const char*)sqlite3_column_text(stmt, 1);
		p.subgroup = (const char*)sqlite3_column_text(stmt, 2);
		p.price = sqlite3_column_double(stmt, 3);

		list.push_back(p);

	}

	sqlite3_finalize(stmt);
	return list;

}

//Add orders
int addOrder(sqlite3* db, string time, string name)
{
	const char* sql = "INSERT INTO Orders (Time, Customer_name) VALUES (?,?);";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, time.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return sqlite3_last_insert_rowid(db);

}

//Add Products to order
void addProductToOrder(sqlite3* db, int order_id, int product_id, int quantity)
{
	const char* sql = "INSERT INTO OrderItems (Order_id, Product_id, Quantity) VALUES (?,?,?);";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_int(stmt, 1, order_id);
	sqlite3_bind_int(stmt, 2, product_id);
	sqlite3_bind_int(stmt, 3, quantity);

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	cout << "Product added to order!\n";

}

//Updating status of order
void updateStatus(sqlite3* db, int order_id, int product_id)
{
	const char* sql =
		"UPDATE OrderItems SET is_ready = 1 "
		"WHERE Order_id = ? AND Product_id = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_int(stmt, 1, order_id);
	sqlite3_bind_int(stmt, 2, product_id);

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	cout << "Product status updated to READY\n";

}

