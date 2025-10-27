#include "Authenticator.h"
#include "databasemanager.h"

string validUsername = "user";
string validPassword = "password123";

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
		cout << "\n[WARNING] Authenticator::setValidInfo() - Empty username or password provided!" << std::endl;
		return;
	}


	std::cout << "\n========================================" << std::endl;
	std::cout << "  Authenticator::setValidInfo()" << std::endl;
	std::cout << "========================================" << std::endl;
	
	std::cout << "[DEBUG] Input Parameters:" << std::endl;
	std::cout << "  - Username: '" << username << "'" << std::endl;
	std::cout << "  - Username length: " << username.length() << " chars" << std::endl;
	std::cout << "  - Password length: " << password.length() << " chars" << std::endl;
	
	// Print first/last few chars of password for debugging (be careful in production!)
	if (password.length() > 0) {
		std::cout << "  - Password first char: '" << password[0] << "' (ASCII: " << (int)password[0] << ")" << std::endl;
		if (password.length() > 1) {
			std::cout << "  - Password last char: '" << password[password.length()-1] 
			          << "' (ASCII: " << (int)password[password.length()-1] << ")" << std::endl;
		}
	}
	
	// Get the Singleton instance instead of creating a new one
	std::cout << "\n[DEBUG] Getting database singleton instance..." << std::endl;
	databasemanager& db = databasemanager::getInstance();
	std::cout << "[DEBUG] Database instance obtained" << std::endl;
	
	// Select ALL needed columns in ONE query
	std::string selectCols = "userID, name, email, phone, password";
	std::string whereClause = "email = '" + username + "' AND password = '" + password + "'";

	std::cout << "\n[DEBUG] Preparing database query:" << std::endl;
	std::cout << "  SELECT: " << selectCols << std::endl;
	std::cout << "  FROM: userinfo" << std::endl;
	std::cout << "  WHERE: email = '" << username << "' AND password = '<hidden>'" << std::endl;
	
	std::cout << "\n[DEBUG] Executing retMultipleColumns()..." << std::endl;

	// Single database query that fetches all columns at once
	auto results = db.retMultipleColumns(selectCols, "userinfo", whereClause);
	
	std::cout << "\n[DEBUG] Query execution completed" << std::endl;
	std::cout << "[DEBUG] Results map contains " << results.size() << " entries" << std::endl;
	
	// Extract values from the results map
	validUserID_ = std::string(results["userID"].c_str());
	validName_ = std::string(results["name"].c_str());
	validUsername_ = std::string(results["email"].c_str());
	validPhone_ = std::string(results["phone"].c_str());
	validPassword_ = std::string(results["password"].c_str());
	
	std::cout << "\n[DEBUG] Extracted values from database:" << std::endl;
	std::cout << "  - User ID: '" << validUserID_ << "'" << std::endl;
	std::cout << "  - Name: '" << validName_ << "'" << std::endl;
	std::cout << "  - Email: '" << validUsername_ << "'" << std::endl;
	std::cout << "  - Phone: '" << validPhone_ << "'" << std::endl;
	std::cout << "  - Password length: " << validPassword_.length() << " chars" << std::endl;
	
	if (validPassword_.length() > 0) {
		std::cout << "  - Password first char: '" << validPassword_[0] << "' (ASCII: " << (int)validPassword_[0] << ")" << std::endl;
		if (validPassword_.length() > 1) {
			std::cout << "  - Password last char: '" << validPassword_[validPassword_.length()-1] 
			          << "' (ASCII: " << (int)validPassword_[validPassword_.length()-1] << ")" << std::endl;
		}
	}
	
	// Check if we got valid data
	if (validUserID_.empty()) {
		std::cout << "\n[WARNING] No user found in database with provided credentials!" << std::endl;
	} else {
		std::cout << "\n[SUCCESS] User data retrieved from database successfully" << std::endl;
	}
	
	std::cout << "========================================\n" << std::endl;
}

bool Authenticator::verifyCredentials(const string& username, const string& password) 
{
	if (username == "" || password == "") {
		cout << "\n[WARNING] Authenticator::verifyCredentials() - Empty username or password provided!" << std::endl;
		return false;
	}

	std::cout << "\n========================================" << std::endl;
	std::cout << "  Authenticator::verifyCredentials()" << std::endl;
	std::cout << "========================================" << std::endl;
	
	std::cout << "[DEBUG] Input credentials to verify:" << std::endl;
	std::cout << "  - Input username: '" << username << "'" << std::endl;
	std::cout << "  - Input username length: " << username.length() << " chars" << std::endl;
	std::cout << "  - Input password length: " << password.length() << " chars" << std::endl;
	
	std::cout << "\n[DEBUG] Stored credentials (from database):" << std::endl;
	std::cout << "  - Stored username: '" << validUsername_ << "'" << std::endl;
	std::cout << "  - Stored username length: " << validUsername_.length() << " chars" << std::endl;
	std::cout << "  - Stored password length: " << validPassword_.length() << " chars" << std::endl;
	
	// Detailed comparison
	std::cout << "\n[DEBUG] Performing comparison..." << std::endl;
	
	bool usernameMatch = (username == validUsername_);
	bool passwordMatch = (password == validPassword_);
	
	std::cout << "  Username comparison:" << std::endl;
	std::cout << "    Input:  '" << username << "'" << std::endl;
	std::cout << "    Stored: '" << validUsername_ << "'" << std::endl;
	std::cout << "    Match:  " << (usernameMatch ? "✓ YES" : "✗ NO") << std::endl;
	
	if (!usernameMatch && username.length() == validUsername_.length()) {
		std::cout << "    [INFO] Lengths match but strings differ - checking character by character:" << std::endl;
		for (size_t i = 0; i < username.length(); ++i) {
			if (username[i] != validUsername_[i]) {
				std::cout << "      Position " << i << ": '" << username[i] 
				          << "' (ASCII: " << (int)username[i] << ") != '" 
				          << validUsername_[i] << "' (ASCII: " 
				          << (int)validUsername_[i] << ")" << std::endl;
			}
		}
	}
	
	std::cout << "\n  Password comparison:" << std::endl;
	std::cout << "    Input length:  " << password.length() << " chars" << std::endl;
	std::cout << "    Stored length: " << validPassword_.length() << " chars" << std::endl;
	std::cout << "    Match:  " << (passwordMatch ? "✓ YES" : "✗ NO") << std::endl;
	
	if (!passwordMatch && password.length() == validPassword_.length()) {
		std::cout << "    [INFO] Lengths match but passwords differ - checking character by character:" << std::endl;
		for (size_t i = 0; i < password.length() && i < 5; ++i) { // Only show first 5 chars for security
			if (password[i] != validPassword_[i]) {
				std::cout << "      Position " << i << ": Input ASCII " << (int)password[i] 
				          << " != Stored ASCII " << (int)validPassword_[i] << std::endl;
			}
		}
		if (password.length() > 5) {
			std::cout << "      (Remaining characters not shown for security)" << std::endl;
		}
	}
	
	bool result = usernameMatch && passwordMatch;
	
	std::cout << "\n[DEBUG] Final Result: " << (result ? "✓ AUTHENTICATION SUCCESS" : "✗ AUTHENTICATION FAILURE") << std::endl;
	
	if (!result) {
		std::cout << "[DEBUG] Failure reason:" << std::endl;
		if (!usernameMatch) std::cout << "  - Username does not match" << std::endl;
		if (!passwordMatch) std::cout << "  - Password does not match" << std::endl;
	}
	
	std::cout << "========================================\n" << std::endl;
	
	return result;
}

void Authenticator::resetPassword(const string& username) 
{
	std::cout << "\n[DEBUG] Authenticator::resetPassword() - Called for username: '" << username << "'" << std::endl;
	
	// In a real application, this would involve more secure steps
	if (username == validUsername) {
		std::cout << "[DEBUG] Authenticator::resetPassword() - Username found in system" << std::endl;
		std::cout << "[INFO] Password reset would be initiated here" << std::endl;
		//cout << "Password reset link has been sent to your registered email." << endl;
	} else {
		std::cout << "[DEBUG] Authenticator::resetPassword() - Username NOT found" << std::endl;
		std::cout << "[ERROR] Username '" << username << "' does not exist in the system" << std::endl;
		cout << "Username not found." << endl;
	}
}