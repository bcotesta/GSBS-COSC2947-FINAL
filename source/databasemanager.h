//Everest Ashley SID: 0457240
// REFACTOR Brandon Cotesta 10/20/2025

#include <iostream>
#include <mysql/jdbc.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <string>

using std::string;
using namespace std;

class databasemanager
{
public:
	databasemanager(); //connect to database
	~databasemanager();

//functions
	//tabn = table name
	void createAccount(std::string accn, std::string accT);

	void createTransactionTb(std::string accnID, std::string tID);


	void addtoTable(std::string tab, std::string val);
	// (t) table to add a row to, (v) values to insert in ('value1','value2') format 
	// or "('"+ name + "', '" + email + "')" format with variables

	std::string retString(std::string col, std::string tab, std::string specval);
	// select statement without where statement
  
	std::string retStringW(std::string col, std::string tab, std::string val, std::string specval);
	// (c)column/s to select, (t)table to select from, (v)value to select if using where 
	// ex select + c + from + t + where + v + ";";

	void updateTable(std::string tab, std::string setv, std::string cond);
	// ex string state = update + t + set + setv + where + cond + ";"

private:
	sql::mysql::MySQL_Driver* driver;

	std::string statement;
	/*std::string userinfo; // tables for easier sql queries
	std::string accountinfo;
	std::string investmentinfo;
	std::string transactioninfo;*/
	sql::Statement* stmt;        // Added missing member
	sql::Connection* connection; // Added missing member
// values for easier sql queries
	std::string insert = "INSERT INTO ";
	std::string ctable = "Create table ";
	std::string values = "VALUES ";
	std::string update =  "UPDATE ";
	std::string set = "SET ";
	std::string select = "SELECT ";
	std::string from = "FROM ";
	std::string where = "WHERE ";
};
