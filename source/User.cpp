// Brandon Cotesta 10/08/2025
#include "User.h"
#include "databasemanager.h"
#include "databasemanager.cpp"

#include <iostream>

User::User(string n, string e, string p, string ph, string add): 
	name_(n), email_(e), passwordHash_(p), phone_(ph), address_(add)
{
	databasemanager d1;
	std::string tb = "userinfo";
	std::string ts = "(" + n + ", " + e + ", " + p + ", " + ph + ", " + add + ")";
	d1.addtoTable(tb, ts);
	// create user
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
	databasemanager d1;
	name_ = n;
	email_ = e;
	passwordHash_ = p;
	phone_ = ph;
	
	std::string tb = "userinfo";
	std::string setc = "name = " + n + ", email + " + e + ", phone = " + ph + ", password = " + p;
	std::string cond = " userID = " + id;
	d1.updateTable(tb, setc, cond);

}
// Note: address_ is not updated as it's not passed in the parameters


