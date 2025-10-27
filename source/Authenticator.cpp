#include "Authenticator.h"
#include "databasemanager.h"

string validUsername = "user";
string validPassword = "password123";

Authenticator::Authenticator() {
	// No need to create database instance - using Singleton
}

Authenticator::~Authenticator() {
	// Nothing to clean up - Singleton manages itself
}

//func desc: will use username and password from database to set valid username and password for login
void Authenticator::setValidInfo(std::string username, std::string password)
{
	// Get the Singleton instance instead of creating a new one
	databasemanager& db = databasemanager::getInstance();
	
	// Select ALL needed columns in ONE query
	std::string selectCols = "userID, name, email, phone, password";
	std::string whereClause = "email = '" + username + "' AND password = '" + password + "'";

	// Single database query that fetches all columns at once
	auto results = db.retMultipleColumns(selectCols, "userinfo", whereClause);
	
	// Extract values from the results map
	validUserID_ = results["userID"];
	validName_ = results["name"];
	validUsername_ = results["email"];
	validPhone_ = results["phone"];
	validPassword_ = results["password"];
}

bool Authenticator::verifyCredentials(const string& username, const string& password) 
{
	return (username == validUsername_ && password == validPassword_);
}

void Authenticator::resetPassword(const string& username) 
{
	// In a real application, this would involve more secure steps
	if (username == validUsername) {
		//cout << "Password reset link has been sent to your registered email." << endl;
	} else {
		cout << "Username not found." << endl;
	}
}