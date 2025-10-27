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
#include <sstream>

// Singleton instance getter
databasemanager& databasemanager::getInstance() {
	static databasemanager instance;
	return instance;
}

databasemanager::databasemanager() 
    : stmt(nullptr), connection(nullptr), driver(nullptr)
{
    try {
        driver = sql::mysql::get_driver_instance();
        connection = driver->connect(host, user, password);
        connection->setSchema(schema);
        stmt = connection->createStatement();
        
        // Ensure UTF-8 encoding for all database operations
        stmt->execute("SET NAMES 'utf8mb4'");
        stmt->execute("SET CHARACTER SET utf8mb4");
        
        std::cout << "Database connected successfully" << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in constructor: " << e.what() << std::endl;
        // Clean up on error
        if (stmt) {
            delete stmt;
            stmt = nullptr;
        }
        if (connection) {
            delete connection;
            connection = nullptr;
        }
    }
}

void databasemanager::ensureConnection()
{
    try {
        if (!connection || connection->isClosed()) {
            if (connection) {
                delete connection;
            }
            if (stmt) {
                delete stmt;
            }
            
            driver = sql::mysql::get_driver_instance();
            connection = driver->connect(host, user, password);
            connection->setSchema(schema);
            stmt = connection->createStatement();
            std::cout << "Database reconnected" << std::endl;
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in ensureConnection: " << e.what() << std::endl;
        throw;
    }
}

std::string databasemanager::getCleanUsername(std::string username)
{
    std::string cleanUsername = username;
    cleanUsername.erase(std::remove(cleanUsername.begin(), cleanUsername.end(), ' '), cleanUsername.end());
    cleanUsername.erase(std::remove(cleanUsername.begin(), cleanUsername.end(), '-'), cleanUsername.end());
    cleanUsername.erase(std::remove(cleanUsername.begin(), cleanUsername.end(), '.'), cleanUsername.end());
    return cleanUsername;
}

void databasemanager::createAccount(std::string accN, std::string accT)
{
    try {
        ensureConnection();
        
        std::string statement = "CREATE TABLE IF NOT EXISTS account" + accN + 
            "(accountId INT NOT NULL, accountType VARCHAR(20) NOT NULL, Balance DECIMAL(15,2) NOT NULL)";
        
        stmt->execute(statement);
        std::cout << "Account table created successfully" << std::endl;
        
        std::string tvs = "('" + accN + "', '" + accT + "', 0.00)";
        addtoTable("account" + accN, tvs);
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in createAccount: " << e.what() << std::endl;
    }
}

void databasemanager::createTransactionTb(std::string accnID, std::string tID)
{
    try {
        ensureConnection();
        
        std::string statement = ctable + "Transaction" + tID + 
            " (TransactionID INT PRIMARY KEY, accountId INT NOT NULL, " +
            "accounttype VARCHAR(20) NOT NULL, transactiontype VARCHAR(20) NOT NULL, " +
            "amount DECIMAL(15,2) NOT NULL, transactiondate DATETIME NOT NULL, " +
            "description VARCHAR(255))";
        
        stmt->execute(statement);
        std::cout << "Transaction table created successfully" << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in createTransactionTb: " << e.what() << std::endl;
    }
}

<<<<<<< HEAD
void databasemanager::createUserAccountsTable(std::string userID, std::string username)
{
    try {
        ensureConnection();
        
        std::string cleanUsername = getCleanUsername(username);
        std::string tableName = userID + "_" + cleanUsername + "_acc";
        
        // Create table with account details
        std::string statement = ctable + tableName + 
            " (accountNumber VARCHAR(50) PRIMARY KEY, " +
            "accountType VARCHAR(20) NOT NULL, " +
            "balance DECIMAL(15,2) NOT NULL DEFAULT 0.00, " +
            "createdDate DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP)";
        
        stmt->execute(statement);
        std::cout << "User accounts table created successfully: " << tableName << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in createUserAccountsTable: " << e.what() << std::endl;
    }
}

void databasemanager::createUserTransactionsTable(std::string userID, std::string username)
{
    try {
        ensureConnection();
        
        std::string cleanUsername = getCleanUsername(username);
        std::string tableName = userID + "_" + cleanUsername + "_transactions";
        
        // Create table with transaction details
        std::string statement = ctable + tableName + 
            " (transactionID INT AUTO_INCREMENT PRIMARY KEY, " +
            "accountNumber VARCHAR(50) NOT NULL, " +
            "transactionType VARCHAR(20) NOT NULL, " +
            "amount DECIMAL(15,2) NOT NULL, " +
            "transactionDate DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, " +
            "description VARCHAR(255), " +
            "balanceAfter DECIMAL(15,2) NOT NULL)";
        
        stmt->execute(statement);
        std::cout << "User transactions table created successfully: " << tableName << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in createUserTransactionsTable: " << e.what() << std::endl;
    }
}

void databasemanager::addAccountToUserTable(std::string userID, std::string username, std::string accountNumber, std::string accountType, double balance)
{
    try {
        ensureConnection();
        
        std::string cleanUsername = getCleanUsername(username);
        std::string tableName = userID + "_" + cleanUsername + "_acc";
        
        std::ostringstream valueStream;
        valueStream << "('" << accountNumber << "', '" << accountType << "', " << balance << ", NOW())";
        
        addtoTable(tableName, valueStream.str());
        std::cout << "Account added to user table: " << accountNumber << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in addAccountToUserTable: " << e.what() << std::endl;
    }
}

void databasemanager::addTransactionToUserTable(std::string userID, std::string username, std::string accountNumber, std::string transactionType, double amount, std::string description, double balanceAfter)
{
    try {
        ensureConnection();
        
        std::string cleanUsername = getCleanUsername(username);
        std::string tableName = userID + "_" + cleanUsername + "_transactions";
        
        std::ostringstream valueStream;
        valueStream << "(NULL, '" << accountNumber << "', '" << transactionType << "', " 
                    << amount << ", NOW(), '" << description << "', " << balanceAfter << ")";
        
        addtoTable(tableName, valueStream.str());
        std::cout << "Transaction added to user table for account: " << accountNumber << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in addTransactionToUserTable: " << e.what() << std::endl;
    }
}

=======
>>>>>>> parent of 864110c (table creation on account registration)
void databasemanager::addtoTable(std::string tab, std::string val)
{
    try {
        ensureConnection();
        
        std::string statement = insert + tab + " " + values + val;
        stmt->execute(statement);
        std::cout << "Data added to table successfully" << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in addtoTable: " << e.what() << std::endl;
    }
}

sql::SQLString databasemanager::retString(std::string col, std::string tab, std::string specval)
{
    sql::SQLString tempstring;
    sql::ResultSet* res = nullptr;
    
    try {
        ensureConnection();
        
        statement = select + col + " " + from + tab;
        res = stmt->executeQuery(statement);
        
        if (res && res->next()) {
            tempstring = res->getString(specval);
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in retString: " << e.what() << std::endl;
    }
    
    if (res) {
        delete res;
    }
    
    return tempstring;
}

sql::SQLString databasemanager::retStringW(std::string col, std::string tab, std::string val, std::string specval)
{
    sql::SQLString tempstring;
    sql::ResultSet* res = nullptr;
    
    try {
        ensureConnection();
        
        statement = select + col + " " + from + tab + " " + where + val;
        res = stmt->executeQuery(statement);
        
        if (res && res->next()) {
            tempstring = res->getString(specval);
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in retStringW: " << e.what() << std::endl;
    }
    
    if (res) {
        delete res;
    }
    
    return tempstring;
}

void databasemanager::updateTable(std::string tab, std::string setv, std::string cond)
{
    try {
        ensureConnection();
        
        statement = update + tab + " " + set + setv + " " + where + cond;
        stmt->execute(statement);
        std::cout << "Table updated successfully" << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in updateTable: " << e.what() << std::endl;
    }
}

std::map<std::string, sql::SQLString> databasemanager::retMultipleColumns(
    std::string cols, 
    std::string tab, 
    std::string whereClause)
{
    std::map<std::string, sql::SQLString> results;
    sql::ResultSet* res = nullptr;
    
    try {
        ensureConnection();
        
        statement = select + cols + " " + from + tab + " " + where + whereClause;
        res = stmt->executeQuery(statement);
        
        if (res && res->next()) {
            // Parse the column names from the cols string
            std::istringstream ss(cols);
            std::string colName;
            
            while (std::getline(ss, colName, ',')) {
                // Trim whitespace
                colName.erase(0, colName.find_first_not_of(" \t"));
                colName.erase(colName.find_last_not_of(" \t") + 1);
                
                try {
                    results[colName] = res->getString(colName);
                } catch (sql::SQLException& e) {
                    std::cerr << "Error getting column '" << colName << "': " << e.what() << std::endl;
                }
            }
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in retMultipleColumns: " << e.what() << std::endl;
    }
    
    if (res) {
        delete res;
    }
    
    return results;
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

