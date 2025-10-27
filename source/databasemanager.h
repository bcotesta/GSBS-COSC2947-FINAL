//Everest Ashley SID: 0457240
// REFACTOR Brandon Cotesta 10/20/2025

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <iostream>
#include <mysql/jdbc.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <string>
#include <map>

using std::string;
using namespace std;

class databasemanager
{
public:
	// Singleton instance access
	static databasemanager& getInstance();
	
	// Delete copy constructor and assignment operator
	databasemanager(const databasemanager&) = delete;
	databasemanager& operator=(const databasemanager&) = delete;

//functions
	void createAccount(std::string accn, std::string accT);
	void createTransactionTb(std::string accnID, std::string tID);
	void addtoTable(std::string tab, std::string val);
	sql::SQLString retString(std::string col, std::string tab, std::string specval);
	sql::SQLString retStringW(std::string col, std::string tab, std::string val, std::string specval);
	void updateTable(std::string tab, std::string setv, std::string cond);
	std::map<std::string, sql::SQLString> retMultipleColumns(
		std::string cols, 
		std::string tab, 
		std::string whereClause
	);
	
	// New functions for user-specific tables
	void createUserAccountsTable(std::string userID, std::string username);
	void createUserTransactionsTable(std::string userID, std::string username);

private:
	// Private constructor for Singleton
	databasemanager();
	~databasemanager();
	
	void ensureConnection();
	
	std::string statement;
	sql::Statement* stmt;
	sql::Connection* connection;
	sql::mysql::MySQL_Driver* driver;

	// Connection details
	const std::string host = "tcp://136.114.146.175:3306";
	const std::string user = "client";
	const std::string password = "gsbsTeam20$";
	const std::string schema = "bankdatabase";

	// SQL keywords for easier queries
	const std::string insert = "INSERT INTO ";
	const std::string ctable = "CREATE TABLE ";
	const std::string values = "VALUES ";
	const std::string update = "UPDATE ";
	const std::string set = "SET ";
	const std::string select = "SELECT ";
	const std::string from = "FROM ";
	const std::string where = "WHERE ";
};

#endif // DATABASEMANAGER_H
