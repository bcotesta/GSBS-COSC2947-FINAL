//Everest Ashley SID: 0457240
// REFACTOR Brandon Cotesta 10/20/2025
#include "databasemanager.h"
#include <mysql/jdbc.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <string>
#include <iostream>

databasemanager::databasemanager()
{
    
   /* sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    try {
        driver = sql::mysql::get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "keypick1");
        con->setSchema("bankdatabase");
        sql::Statement* stmt = con->createStatement();
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    } */
}


//accn = account number, accT = account type
void databasemanager::createAccount(std::string accN, std::string accT)
{
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    std::string host = "tcp://localhost:3306";

    try {
        driver = sql::mysql::get_driver_instance();
        con = driver->connect("tcp://136.114.146.175:3306", "client", "gsbsTeam20$");
        con->setSchema("bankdatabase");
        cout << "connected";

        sql::Statement* stmt = con->createStatement();


        std::string statement = "Create table if not exists account" + accN + "( accountId int not null, accountType VARCHAR(20) not NULL, Balance decimal(15,2) not null);";

        stmt->execute(statement);
        //std::string statement2 = insert + accN;
        std::string tvs = "('" + accN + "', '" + accT + "', 0.00)";
        addtoTable("account" + accN, tvs);
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }


}

void databasemanager::createTransactionTb(std::string accnID, std::string tID)
{
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
    try {
        driver = sql::mysql::get_driver_instance();
        con = driver->connect("tcp://136.114.146.175:3306", "client", "gsbsTeam20$");
        con->setSchema("bankdatabase");
        cout << "connected";
        sql::Statement* stmt = con->createStatement();


        std::string statement = ctable + "Transaction " + tID + " (TransactionID int primary key, accountId INT not null, accounttype VARCHAR(20) not null, transactiontype VARCHAR(20) not null, amount decimal(15,2) not null, transactiondate DATETIME not null, description VARCHAR(255));";
        stmt->execute(statement);
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }

}

void databasemanager::addtoTable(std::string tab, std::string val)
{
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
 

    try {
        driver = sql::mysql::get_driver_instance();
        con = driver->connect("tcp://136.114.146.175:3306", "client", "gsbsTeam20$");
        con->setSchema("bankdatabase");

        cout << "connected";
        sql::Statement* stmt = con->createStatement();

        std::string statement = insert + tab + " " + values + val;
        //create statement to execute

        stmt->execute(statement);
        //execute query - use execute() for INSERT statements
        delete con;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}


//col= columns being selected, tab = table selected, specval = specfic value(column from search you want)
sql::SQLString databasemanager::retString(std::string col, std::string tab, std::string specval)
{
    sql::SQLString tempstring; 
    //string that will be returned;
  
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    try {
        driver = sql::mysql::get_driver_instance();
        con = driver->connect("tcp://136.114.146.175:3306", "client", "gsbsTeam20$");
        con->setSchema("bankdatabase");


        cout << "connected";
        sql::Statement* stmt = con->createStatement();


        statement = select + col + " " + from + tab;
        //uses the statement variable to make a string statement for execution

        sql::ResultSet* res = stmt->executeQuery(statement);
        //executes the statement assigns value to res (result set)

        while (res->next()) {
            tempstring = res->getString(specval);
            //searches returned statement for the specific value wanted.
        }

        delete res;  //clean up result set only
        // Don't delete stmt - it's a class member
        return tempstring;


    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    return tempstring;
 
}


sql::SQLString databasemanager::retStringW(std::string col, std::string tab, std::string val, std::string specval)
{ //select statement with where 
    std::string tempstring; 

    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    try {
        driver = sql::mysql::get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "keypick1");
        con->setSchema("bankdatabase");
        sql::Statement* stmt = con->createStatement();


        //same comments and functions as retString
        statement = select + col + " " + from + tab + " " + where + val;
        //except here it uses the WHERE statement to give a more specific search

        sql::ResultSet* res = stmt->executeQuery(statement);
        //select statement to retrieve data for database
        if (res->next()) {
            tempstring = res->getString(specval);
        }

        delete con;
        delete stmt;
        delete res;  //clean up result set only
        // Don't delete stmt - it's a class member

    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
  
    return tempstring;
}

// tab = table, setv = set values, cond = condition
void databasemanager::updateTable(std::string tab, std::string setv, std::string cond)
{
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    try {
        driver = sql::mysql::get_driver_instance();
        con = driver->connect("tcp://136.114.146.175:3306", "client", "gsbsTeam20$");
        con->setSchema("bankdatabase");
        cout << "connected";
        sql::Statement* stmt = con->createStatement();

        statement = update + tab + " " + set + setv + " " + where + cond;
        //create statement for execution with UPDATE, SET, and WHERE
        //example UPDATE userinfo SET email = 'testemail' name = 'testname' WHERE userID = 1;
        //will update values for userID 1

        stmt->execute(statement);
        //Executes statement - use execute() for UPDATE statements
        //no return needed
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

databasemanager::~databasemanager()
{
    if (stmt) {
        delete stmt;
        stmt = nullptr;
    }
    if (connection) {
        delete connection;
        connection = nullptr;
    }
} 

