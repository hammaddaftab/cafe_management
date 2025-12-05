#include <iostream>
#include "header.h"

using namespace std;


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
		"Time INTEGER,"
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
int addOrder(sqlite3* db, int timeValue, string name)
{
	const char* sql = "INSERT INTO Orders (Time, Customer_name) VALUES (?,?);";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_int(stmt, 1, timeValue);
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

//Function to delete a product from the inventory
void deleteProduct(sqlite3* db, int id)
{
	const char* sql = "DELETE FROM Products WHERE Product_id = ?;";

	sqlite3_stmt * stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	if (rc != SQLITE_OK)
	{
		cout<<"Prepare Failed!"<<sqlite3_errmsg(db)<<endl;
		return;
	}

	sqlite3_bind_int(stmt, 1, id);

	rc = sqlite3_step(stmt);

	if (rc == SQLITE_DONE) {
		cout << "Product deleted successfully!\n";
	}
	else {
		cerr << "Delete failed: " << sqlite3_errmsg(db) << endl;
	}

	sqlite3_finalize(stmt);
}


//Function to return the pending orders for each specific subgroup
vector<Product_Count> getOrderedProductsBySubgroup(sqlite3* db, const string& subgroup)
{
	vector<Product_Count> result;

	const char* sql =
		"SELECT Products.Product_id, Products.Name, SUM(OrderItems.Quantity) "
		"FROM OrderItems "
		"JOIN Products ON OrderItems.Product_id = Products.Product_id "
		"WHERE Products.Subgroup = ? AND OrderItems.is_ready = 0 "
		"GROUP BY Products.Product_id, Products.Name;";

	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	if (rc != SQLITE_OK)
	{
		cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
		return result;
	}

	sqlite3_bind_text(stmt, 1, subgroup.c_str(), -1, SQLITE_TRANSIENT);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		Product_Count pc;

		pc.product_id = sqlite3_column_int(stmt, 0);

		const unsigned char* txt = sqlite3_column_text(stmt, 1);
		pc.name = txt ? (const char*)txt : "";

		pc.total_quantity = sqlite3_column_int(stmt, 2);

		result.push_back(pc);
	}

	sqlite3_finalize(stmt);
	return result;
}


//Function to update the oldest order just by name
void markOldestPendingReady(sqlite3* db, const string& productName)
{
	// SQL: find the oldest pending OrderItems row for this product
	const char* sql =
		"UPDATE OrderItems "
		"SET is_ready = 1 "
		"WHERE Sr = ("
		"  SELECT Sr FROM OrderItems "
		"  JOIN Products ON OrderItems.Product_id = Products.Product_id "
		"  WHERE Products.Name = ? AND OrderItems.is_ready = 0 "
		"  ORDER BY OrderItems.Order_id ASC "
		"  LIMIT 1"
		");";

	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
	{
		cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
		return;
	}

	// Bind product name
	sqlite3_bind_text(stmt, 1, productName.c_str(), -1, SQLITE_TRANSIENT);

	// Execute
	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		cout << "Update failed: " << sqlite3_errmsg(db) << endl;
	}
	else
	{
		cout << "Oldest pending '" << productName << "' marked as READY.\n";
	}

	sqlite3_finalize(stmt);
}


//Function to display the order info to a customer on his device
Order getOrderById(sqlite3* db, int order_id)
{
	Order order;
	order.order_id = order_id;

	// First, get order info from Orders table
	const char* orderSql = "SELECT Customer_name, Time FROM Orders WHERE Order_id = ?;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, orderSql, -1, &stmt, nullptr) != SQLITE_OK)
	{
		cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
		return order;
	}

	sqlite3_bind_int(stmt, 1, order_id);

	if (sqlite3_step(stmt) == SQLITE_ROW)
	{
		const unsigned char* nameText = sqlite3_column_text(stmt, 0);
		order.customer_name = nameText ? (const char*)nameText : "";
		order.time = sqlite3_column_int(stmt, 1);
	}
	else
	{
		cout << "Order not found.\n";
		sqlite3_finalize(stmt);
		return order;
	}

	sqlite3_finalize(stmt);

	// Now get all items for this order
	const char* itemsSql =
		"SELECT Products.Product_id, Products.Name, OrderItems.Quantity, OrderItems.is_ready "
		"FROM OrderItems "
		"JOIN Products ON OrderItems.Product_id = Products.Product_id "
		"WHERE OrderItems.Order_id = ?;";

	if (sqlite3_prepare_v2(db, itemsSql, -1, &stmt, nullptr) != SQLITE_OK)
	{
		cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
		return order;
	}

	sqlite3_bind_int(stmt, 1, order_id);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		OrderItem item;
		item.product_id = sqlite3_column_int(stmt, 0);

		const unsigned char* nameText = sqlite3_column_text(stmt, 1);
		item.name = nameText ? (const char*)nameText : "";

		item.quantity = sqlite3_column_int(stmt, 2);
		item.is_ready = sqlite3_column_int(stmt, 3);

		order.items.push_back(item);
	}

	sqlite3_finalize(stmt);

	return order;
}


//Function to close the database
void closeDB(sqlite3* db)
{
	if (db != nullptr)
	{
		sqlite3_close(db);
		cout << "Database closed successfully.\n";
	}
	else
	{
		cout << "Database is already null.\n";
	}
}
