#include "Authenticator.h"
#include "databasemanager.h"


Authenticator::Authenticator() {
	std::cout << "\n[DEBUG] Authenticator::Authenticator() - Constructor called" << std::endl;
	std::cout << "[DEBUG] Authenticator::Authenticator() - Using Singleton database pattern" << std::endl;
	// No need to create database instance - using Singleton
}

Authenticator::~Authenticator() {
	std::cout << "[DEBUG] Authenticator::~Authenticator() - Destructor called" << std::endl;
	// Nothing to clean up - Singleton manages itself
}

//func desc: will use username and password from database to set valid username and password for login
void Authenticator::setValidInfo(std::string username, std::string password)
{
	if (username == "" || password == "") {
		return;
	}

	// Get the Singleton instance
	databasemanager& db = databasemanager::getInstance();
	
	// STEP 1: Get the userID from the email
	std::string whereClause = "email = '" + username + "'";
	std::string userIDResult = std::string(db.retStringW("userID", "userinfo", whereClause, "userID").c_str());
	
	// Check if user exists
	if (userIDResult.empty()) {
		// User not found - clear all valid data
		validUserID_ = "";
		validName_ = "";
		validUsername_ = "";
		validPhone_ = "";
		validPassword_ = "";
		return;
	}
	
	// STEP 2: Now use the userID to get ALL user information
	std::string selectCols = "userID, name, email, phone, password";
	whereClause = "userID = '" + userIDResult + "'";

	// Single database query that fetches all columns using the primary key
	auto results = db.retMultipleColumns(selectCols, "userinfo", whereClause);
	
	// Extract values from the results map
	validUserID_ = std::string(results["userID"].c_str());
	validName_ = std::string(results["name"].c_str());
	validUsername_ = std::string(results["email"].c_str());
	validPhone_ = std::string(results["phone"].c_str());
	validPassword_ = std::string(results["password"].c_str());
}

bool Authenticator::verifyCredentials(const string& username, const string& password) 
{
	if (username == "" || password == "") {
		return false;
	}

	// Simple comparison - the actual password verification
	bool usernameMatch = (username == validUsername_);
	bool passwordMatch = (password == validPassword_);
	
	return usernameMatch && passwordMatch;
}

void Authenticator::resetPassword(const string& username) 
{
	// In a real application, this would involve more secure steps
	if (username == validUsername_) {
		//cout << "Password reset link has been sent to your registered email." << endl;
	} else {
		cout << "Username not found." << endl;
	}
}