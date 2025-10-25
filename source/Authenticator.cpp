#include "Authenticator.h"
#include "databasemanager.h";

string validUsername = "user";
string validPassword = "password123";

Authenticator::Authenticator() {
	// this will initialize any needed resources related to the database for usernames and such
	// so like if(db exists){
	// 		connect to db
	//      get valid usernames and passwords
	//      store them in a suitable data structure
	//}

	databasemanager d1;
	
	//d1.retString();
	
	


}


//func desc: will use username and password from database to set valid username and password for login
void Authenticator::setValidInfo(std::string username, std::string password)
{
	databasemanager d1;

	std::string tempcol = "email";
	std::string tempcol2 = "password";
	std::string tempcol3 = "name";
	std::string tempcol4 = "phone";
	std::string tempcol5 = "address";
	std::string tempcol6 = "userID";

	std::string tempemail = "email = '" + username+ "';";
	std::string temppass = "password = '" + password + "';";

	validName_ = d1.retStringW(tempcol, "userinfo", tempemail, tempcol3);
	validUsername_ = d1.retStringW(tempcol, "userinfo", tempemail, tempcol);
	validPhone_ = d1.retStringW(tempcol, "userinfo", tempemail, tempcol4);
	validPassword_ = d1.retStringW(tempcol2, "userinfo", temppass, tempcol2);
}

bool Authenticator::verifyCredentials(const string& username, const string& password) 
{
	return (username == validUsername && password == validPassword);
}

void Authenticator::resetPassword(const string& username) 
{
	// In a real application, this would involve more secure steps
	if (username == validUsername) {
		cout << "Password reset link has been sent to your registered email." << endl;
	} else {
		cout << "Username not found." << endl;
	}
}