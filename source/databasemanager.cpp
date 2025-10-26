//Everest Ashley SID: 0457240
// REFACTOR Brandon Cotesta 10/20/2025
#include "databasemanager.h"
#include <iostream>

databasemanager::databasemanager()
    : driver(nullptr), connection(nullptr), stmt(nullptr)
{
    try {
        driver = sql::mysql::get_driver_instance();
        std::cout << "Driver pointer: " << driver << std::endl;
        if (!driver) {
            std::cerr << "ERROR: get_driver_instance() returned null.\n";
            return;
        }

        std::string url = "tcp://136.114.146.175:3306";
        std::string user = "client";
        std::string password = "gsbsTeam20$";
        std::string db = "bankdatabase";

        connection = driver->connect(url, user, password);
        std::cout << "Connection pointer after connect: " << connection << std::endl;
        if (!connection) {
            std::cerr << "ERROR: connect() returned null.\n";
            return;
        }

        connection->setSchema(db);
        
        // Add connection validation before creating statement
        if (!connection->isValid()) {
            std::cerr << "ERROR: Connection is not valid.\n";
            return;
        }
        
        stmt = connection->createStatement();
        std::cout << "Statement pointer after createStatement: " << stmt << std::endl;
        
        if (!stmt) {
            std::cerr << "ERROR: createStatement() returned null.\n";
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "[SQL EXCEPTION] " << e.what() << " (code " << e.getErrorCode()
            << ", state " << e.getSQLState() << ")\n";
        // Ensure pointers are null on exception
        stmt = nullptr;
        connection = nullptr;
    }
    catch (std::exception& e) {
        std::cerr << "[STD EXCEPTION] " << e.what() << "\n";
        stmt = nullptr;
        connection = nullptr;
    }
}




//accn = account number, accT = account type
void databasemanager::createAccount(std::string accN, std::string accT)
{
    if (!stmt || !connection) {
        std::cerr << "Database statement/connection not initialized. Cannot execute query." << std::endl;
        return;
    }
    
    try {
        // Validate connection is still alive
        if (!connection->isValid()) {
            std::cerr << "Connection is no longer valid." << std::endl;
            return;
        }
        
        statement = "Create table if not exists account" + accN + "( accountId int not null, accountType VARCHAR(20) not NULL, Balance decimal(15,2) not null);";
        stmt->execute(statement);
        
        statement = insert + accN;
        std::string tvs = "(" + accN + ", " + accT+ ", 0.00)";
        addtoTable("account"+accN, tvs);
    }
    catch (sql::SQLException& e) {
        std::cerr << "[createAccount SQL EXCEPTION] " << e.what() 
                  << " (code " << e.getErrorCode() << ")\n";
    }
}

void databasemanager::createTransactionTb(std::string accnID, std::string tID)
{
    if (!stmt || !connection) {
        std::cerr << "Database statement/connection not initialized. Cannot execute query." << std::endl;
        return;
    }
    
    try {
        // Validate connection is still alive
        if (!connection->isValid()) {
            std::cerr << "Connection is no longer valid." << std::endl;
            return;
        }
        
        std::string statement = ctable + "Transaction " + tID + " (TransactionID int primary key, accountId INT not null, accounttype VARCHAR(20) not null, transactiontype VARCHAR(20) not null, amount decimal(15,2) not null, transactiondate DATETIME not null, description VARCHAR(255));";
        stmt->execute(statement);
    }
    catch (sql::SQLException& e) {
        std::cerr << "[createTransactionTb SQL EXCEPTION] " << e.what() 
                  << " (code " << e.getErrorCode() << ")\n";
    }
}



void databasemanager::addtoTable(std::string tab, std::string val)
{
    if (!stmt || !connection) {
        std::cerr << "Database statement/connection not initialized. Cannot execute query." << std::endl;
        return;
    }
    
    try {
        // Validate connection is still alive
        if (!connection->isValid()) {
            std::cerr << "Connection is no longer valid." << std::endl;
            return;
        }
        
        std::string statement = insert + tab + " " + values + val;
        stmt->execute(statement);
    }
    catch (sql::SQLException& e) {
        std::cerr << "[addtoTable SQL EXCEPTION] " << e.what() 
                  << " (code " << e.getErrorCode() << ")\n";
    }
}

//col= columns being selected, tab = table selected, specval = specfic value(column from search you want)
std::string databasemanager::retString(std::string col, std::string tab, std::string specval)
{
    std::string tempstring; 
    
    if (!stmt || !connection) {
        std::cerr << "Database statement/connection not initialized. Cannot execute query." << std::endl;
        return tempstring;
    }
    
    try {
        // Validate connection is still alive
        if (!connection->isValid()) {
            std::cerr << "Connection is no longer valid." << std::endl;
            return tempstring;
        }
        
        statement = select + col + " " + from + tab; 
        sql::ResultSet* res = stmt->executeQuery(statement); 
      
        if (res->next()) {
            tempstring = res->getString(specval); 
        }

        delete res;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[retString SQL EXCEPTION] " << e.what() 
                  << " (code " << e.getErrorCode() << ")\n";
    }
  
    return tempstring;
}


std::string databasemanager::retStringW(std::string col, std::string tab, std::string val, std::string specval)
{ 
    std::string tempstring; 
    
    if (!stmt || !connection) {
        std::cerr << "Database statement/connection not initialized. Cannot execute query." << std::endl;
        return tempstring;
    }
    
    try {
        // Validate connection is still alive
        if (!connection->isValid()) {
            std::cerr << "Connection is no longer valid." << std::endl;
            return tempstring;
        }
        
        statement = select + col + " " + from + tab + " " + where + val; 
        sql::ResultSet* res = stmt->executeQuery(statement);
        
        if (res->next()) {
            tempstring = res->getString(specval);
        }

        delete res;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[retStringW SQL EXCEPTION] " << e.what() 
                  << " (code " << e.getErrorCode() << ")\n";
    }
  
    return tempstring;
}

// tab = table, setv = set values, cond = condition
void databasemanager::updateTable(std::string tab, std::string setv, std::string cond)
{
    statement = update + tab + " " + set + setv + " " + where + cond; 
    //create statement for execution with UPDATE, SET, and WHERE
    //example UPDATE userinfo SET email = 'testemail' name = 'testname' WHERE userID = 1;
    //will update values for userID 1
    
    stmt->execute(statement);
    //Executes statement - use execute() for UPDATE statements
    //no return needed
}

databasemanager::~databasemanager()
{
    if (stmt) {
        delete stmt;
        stmt = nullptr;
    }
    if (connection) {
        if (!connection->isClosed()) connection->close();
        delete connection;
        connection = nullptr;
    }
}
