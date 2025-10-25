#pragma once
#include <string>
using namespace std;
class User
{
public:
	User(string n, string e, string p, string ph, string add); // since the database generates an id number when an account is created we wont need to establish it here

	int userId() const;
	string name() const;
	string email() const;
	string phone() const;
	string address() const;
	string passwordHash() const;

	// functions
	bool login(string u, string p) const;
	void logout() const;

	/// <param name="name"></param>
	/// <param name="email"></param>
	/// <param name="password"></param>
	/// <param name="phone number"></param>
	void updateProfile(string id,string n, string e, string p, string ph);
private:
	int userId_;
	string name_;
	string email_;
	string phone_;
	string address_;
	string passwordHash_;
};

