// Brandon Cotesta - 0456970
#pragma once
#include <string>
#include <iostream>

using namespace std;

class Authenticator
{
public:
	Authenticator();
	~Authenticator();
	
	bool verifyCredentials(const string& username, const string& password);
	void resetPassword(const string& username);
	void setValidInfo(std::string username, std::string password);
	
private:
	// No longer need to store dbManager_ - will use Singleton
	
	string validUserID_;
	string validUsername_; // Placeholder for valid username
	string validPassword_; // Placeholder for valid password
	string validName_;
	string validPhone_;
	string validAddress_;
	string validAccountNum_;
};

