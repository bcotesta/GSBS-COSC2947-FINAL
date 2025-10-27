// Brandon Cotesta 10/08/2025
#include "User.h"
#include "databasemanager.h"

#include <iostream>

User::User(string n, string e, string p, string ph): 
	name_(n), email_(e), passwordHash_(p), phone_(ph)
{
	// Constructor should NOT call database - this is now just initialization
}

// This method should ONLY be called when registering a NEW user
void User::saveToDatabase() {
	databasemanager& db = databasemanager::getInstance();
	
	// Insert user info into userinfo table
	std::string tb = "userinfo (name,email,phone, password)";
	std::string ts = "('" + name_ + "', '" + email_ + "', '" + phone_ + "', '" + passwordHash_ + "')";
	db.addtoTable(tb, ts);
	
	// Get the userID that was just created
	std::string whereClause = "email = '" + email_ + "'";
	std::string userID = std::string(db.retStringW("userID", "userinfo", whereClause, "userID").c_str());
	
	// Create user-specific tables for accounts and transactions ONLY for new users
	if (!userID.empty()) {
		db.createUserAccountsTable(userID, name_);
		db.createUserTransactionsTable(userID, name_);
		std::cout << "Created user-specific tables for NEW user: " << name_ << " (ID: " << userID << ")" << std::endl;
	}
}

int User::userId() const
{
	return userId_;
}
string User::name() const
{
	return name_;
}
string User::email() const
{
	return email_;
}
string User::phone() const
{
	return phone_;
}
string User::address() const
{
	return address_;
}
string User::passwordHash() const
{
	return passwordHash_;
}
bool User::login(string u, string p) const
{
	// Dummy implementation for illustration
	return (u == email_ && p == passwordHash_);
}
void User::logout() const
{
	// Dummy implementation for illustration
	cout << "User " << name_ << " logged out." << endl;
}
void User::updateProfile(string id,string n, string e, string p, string ph)
{
	// Use Singleton instance
	databasemanager& db = databasemanager::getInstance();
	name_ = n;
	email_ = e;
	passwordHash_ = p;
	phone_ = ph;
	
	std::string tb = "userinfo";
	std::string setc = "name = " + n + ", email + " + e + ", phone = " + ph + ", password = " + p;
	std::string cond = " userID = " + id;
	db.updateTable(tb, setc, cond);
}
// Note: address_ is not updated as it's not passed in the parameters


