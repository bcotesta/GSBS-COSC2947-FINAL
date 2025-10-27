// Brandon Coesta 10/10/2025

#include "Account.h"
#include "Customer.h"
#include "Card.h"
#include "User.h"
#include "Authenticator.h"
#include "BankingWindow.h"

#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
// Qt includes
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

using namespace std;

// Forward declarations for console functions
string switchAccount(Customer& customer);

string switchAccount(Customer& customer) {
    auto accountsList = customer.accounts();
    if (accountsList.empty()) {
        cout << "No accounts available." << endl;
        return "";
    }
    cout << "Select an account:" << endl;
    int accountNum = 1;
    for (const auto& account : accountsList) {
        cout << accountNum << ". " << account.accountNumber() << endl;
        accountNum++;
    }
    int choice = 0;
    cout << "Enter the number of the account to select: ";
    cin >> choice;
    if (choice < 1 || choice >= accountNum) {
        cout << "Invalid selection." << endl;
        return "";
    }
    auto it = accountsList.begin();
    std::advance(it, choice - 1);
    cout << "Switched to account: " << it->accountNumber() << endl;
    return it->accountNumber();
}

void registerNewUser(string user, string pass, string email, string phone) {
	std::cout << "\n[DEBUG] registerNewUser() - Starting registration" << std::endl;
	std::cout << "[DEBUG] registerNewUser() - Username: " << user << std::endl;
	std::cout << "[DEBUG] registerNewUser() - Email: " << email << std::endl;
	std::cout << "[DEBUG] registerNewUser() - Phone: " << phone << std::endl;
	std::cout << "[DEBUG] registerNewUser() - Password length: " << pass.length() << " chars" << std::endl;
	
	int id = 2; // This would be generated or retrieved from a database. 
    User newUser(user, email, phone, pass); // creates a new user with given info
    
    std::cout << "[DEBUG] registerNewUser() - User object created successfully" << std::endl;
}

int main(int argc, char *argv[])
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "   GSBS Banking Application" << std::endl;
    std::cout << "   Starting Up..." << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
    
    std::cout << "[DEBUG] main() - Initializing QApplication" << std::endl;
    QApplication app(argc, argv);
    std::cout << "[DEBUG] main() - QApplication initialized successfully" << std::endl;
    
    // Authenticator
    std::cout << "[DEBUG] main() - Creating Authenticator instance" << std::endl;
    Authenticator auth;
    std::cout << "[DEBUG] main() - Authenticator created" << std::endl;

    // Login Window
    std::cout << "[DEBUG] main() - Setting up login window" << std::endl;
    QWidget loginWindow;
    loginWindow.resize(320, 240);
    loginWindow.setWindowTitle("GSBS | Login");
    loginWindow.show();
    std::cout << "[DEBUG] main() - Login window displayed" << std::endl;

    QLabel* loginLabel = new QLabel("Sign in to GSBS", &loginWindow);
    loginLabel->setGeometry(60, 10, 200, 30);
    loginLabel->setAlignment(Qt::AlignCenter);
    loginLabel->show();

    // Username and Password fields
    QLineEdit* usernameField = new QLineEdit(&loginWindow);
    usernameField->setPlaceholderText("Username");
    usernameField->setGeometry(50, 50, 200, 30);
    usernameField->show();

    QLineEdit* passwordField = new QLineEdit(&loginWindow);
    passwordField->setPlaceholderText("Password");
    passwordField->setEchoMode(QLineEdit::Password);
    passwordField->setGeometry(50, 90, 200, 30);
    passwordField->show();

    // Show/Hide Password button
    QPushButton *showPasswordButton = new QPushButton("Show", &loginWindow);
    QObject::connect(showPasswordButton, &QPushButton::clicked, [passwordField, showPasswordButton]() {
        if (passwordField->echoMode() == QLineEdit::Password) {
            passwordField->setEchoMode(QLineEdit::Normal);
            showPasswordButton->setText("Hide");
            std::cout << "[DEBUG] Password visibility: VISIBLE" << std::endl;
        } else {
            passwordField->setEchoMode(QLineEdit::Password);
            showPasswordButton->setText("Show");
            std::cout << "[DEBUG] Password visibility: HIDDEN" << std::endl;
        }
    });
    showPasswordButton->setGeometry(260, 90, 60, 30);
    showPasswordButton->show();

    // Banking window (initially hidden)
    std::cout << "[DEBUG] main() - Creating BankingWindow instance" << std::endl;
    BankingWindow* bankingWindow = new BankingWindow();
    std::cout << "[DEBUG] main() - BankingWindow created (hidden)" << std::endl;

    // Login button
    QPushButton *loginButton = new QPushButton("Login", &loginWindow);
    QObject::connect(loginButton, &QPushButton::clicked, [&auth, usernameField, passwordField, &loginWindow, bankingWindow]() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "       LOGIN ATTEMPT STARTED" << std::endl;
        std::cout << "========================================" << std::endl;
        
        QString Qusername = usernameField->text();
        QString Qpassword = passwordField->text();
        
        std::cout << "[DEBUG] Login - Qt Username captured: '" << Qusername.toStdString() << "'" << std::endl;
        std::cout << "[DEBUG] Login - Qt Password length: " << Qpassword.length() << " characters" << std::endl;
        
        // Check for empty fields
        if (Qusername.isEmpty()) {
            std::cout << "[WARNING] Login - Username field is EMPTY" << std::endl;
        }
        if (Qpassword.isEmpty()) {
            std::cout << "[WARNING] Login - Password field is EMPTY" << std::endl;
        }
        
        // Use UTF-8 encoding to prevent corruption of special characters
        string uname = Qusername.toUtf8().constData();
        string pword = Qpassword.toUtf8().constData();
        
        std::cout << "[DEBUG] Login - UTF-8 Username: '" << uname << "'" << std::endl;
        std::cout << "[DEBUG] Login - UTF-8 Password length: " << pword.length() << " characters" << std::endl;
        
        // Check for any encoding issues
        if (uname.length() != Qusername.length()) {
            std::cout << "[WARNING] Login - Username length changed during UTF-8 conversion!" << std::endl;
            std::cout << "[WARNING] Login - Qt length: " << Qusername.length() << ", UTF-8 length: " << uname.length() << std::endl;
        }
        if (pword.length() != Qpassword.length()) {
            std::cout << "[WARNING] Login - Password length changed during UTF-8 conversion!" << std::endl;
            std::cout << "[WARNING] Login - Qt length: " << Qpassword.length() << ", UTF-8 length: " << pword.length() << std::endl;
        }

        //sets the validinfo to be compared to using the name and password to 
        //search the database for matching info
        std::cout << "[DEBUG] Login - Calling auth.setValidInfo()..." << std::endl;
        auth.setValidInfo(uname, pword);
        std::cout << "[DEBUG] Login - auth.setValidInfo() completed" << std::endl;

        std::cout << "[DEBUG] Login - Calling auth.verifyCredentials()..." << std::endl;
        bool loginSuccess = auth.verifyCredentials(uname, pword);
        std::cout << "[DEBUG] Login - auth.verifyCredentials() returned: " << (loginSuccess ? "TRUE" : "FALSE") << std::endl;

        if(loginSuccess){
            std::cout << "\n[SUCCESS] ✓ Login successful!" << std::endl;
            std::cout << "[DEBUG] Login - Closing login window" << std::endl;
            loginWindow.close();
            std::cout << "[DEBUG] Login - Showing banking window" << std::endl;
            bankingWindow->show();
            std::cout << "[DEBUG] Login - Banking window displayed" << std::endl;
        }
        else {
            std::cout << "\n[FAILURE] ✗ Login failed - Invalid credentials" << std::endl;
            std::cout << "[DEBUG] Login - Showing error dialog" << std::endl;
            QMessageBox::warning(&loginWindow, "Login Failed", "Invalid credentials. Please try again.");
        }
        
        std::cout << "========================================\n" << std::endl;
    });
    loginButton->setGeometry(110, 140, 100, 30);
    loginButton->show();

	// Register new user button
	QPushButton* registerButton = new QPushButton("Register", &loginWindow);
    QObject::connect(registerButton, &QPushButton::clicked, [&auth, &loginWindow]() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "    REGISTRATION DIALOG OPENED" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // Simple registration dialog (username, password, email, phone)
        QDialog* registerDialog = new QDialog(&loginWindow);
        registerDialog->setWindowTitle("Register New User");
        registerDialog->setModal(true);
        registerDialog->resize(350, 280);
    
        QVBoxLayout* layout = new QVBoxLayout(registerDialog);
    
        // Username field
        QLabel* usernameLabel = new QLabel("Username:", registerDialog);
        QLineEdit* usernameInput = new QLineEdit(registerDialog);
        layout->addWidget(usernameLabel);
        layout->addWidget(usernameInput);
    
        // Password field
        QLabel* passwordLabel = new QLabel("Password:", registerDialog);
        QLineEdit* passwordInput = new QLineEdit(registerDialog);
        passwordInput->setEchoMode(QLineEdit::Password);
        layout->addWidget(passwordLabel);
        layout->addWidget(passwordInput);

        // Confirm Password field
        QLabel* cpasswordLabel = new QLabel("Confirm password:", registerDialog);
        QLineEdit* cpasswordInput = new QLineEdit(registerDialog);
        cpasswordInput->setEchoMode(QLineEdit::Password);
        layout->addWidget(cpasswordLabel);
        layout->addWidget(cpasswordInput);
    
        // Email field
        QLabel* emailLabel = new QLabel("Email:", registerDialog);
        QLineEdit* emailInput = new QLineEdit(registerDialog);
        layout->addWidget(emailLabel);
        layout->addWidget(emailInput);
    
        // Phone field
        QLabel* phoneLabel = new QLabel("Phone:", registerDialog);
        QLineEdit* phoneInput = new QLineEdit(registerDialog);
        layout->addWidget(phoneLabel);
        layout->addWidget(phoneInput);
    
        // Buttons
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* submitButton = new QPushButton("Register", registerDialog);
        QPushButton* cancelButton = new QPushButton("Cancel", registerDialog);
        buttonLayout->addWidget(submitButton);
        buttonLayout->addWidget(cancelButton);
        layout->addLayout(buttonLayout);
    
        // Submit button logic
        QObject::connect(submitButton, &QPushButton::clicked, [registerDialog, usernameInput, passwordInput, cpasswordInput, emailInput, phoneInput]() {
            std::cout << "\n[DEBUG] Registration - Submit button clicked" << std::endl;
            
            QString username = usernameInput->text();
            QString password = passwordInput->text();
			QString cpassword = cpasswordInput->text();
            QString email = emailInput->text();
            QString phone = phoneInput->text();
            
            std::cout << "[DEBUG] Registration - Username: '" << username.toStdString() << "'" << std::endl;
            std::cout << "[DEBUG] Registration - Email: '" << email.toStdString() << "'" << std::endl;
            std::cout << "[DEBUG] Registration - Phone: '" << phone.toStdString() << "'" << std::endl;
            std::cout << "[DEBUG] Registration - Password length: " << password.length() << " chars" << std::endl;
            
            // edge cases
            std::cout << "[DEBUG] Registration - Running validation checks..." << std::endl;
            
            if (username.isEmpty() || password.isEmpty() || email.isEmpty() || phone.isEmpty()) {
                std::cout << "[VALIDATION] ✗ Failed: Empty fields detected" << std::endl;
                if (username.isEmpty()) std::cout << "  - Username is empty" << std::endl;
                if (password.isEmpty()) std::cout << "  - Password is empty" << std::endl;
                if (email.isEmpty()) std::cout << "  - Email is empty" << std::endl;
                if (phone.isEmpty()) std::cout << "  - Phone is empty" << std::endl;
                QMessageBox::warning(registerDialog, "Registration Failed", "All fields are required.");
                return;
            }

            if (password != cpassword) {
                std::cout << "[VALIDATION] ✗ Failed: Passwords do not match" << std::endl;
				QMessageBox::warning(registerDialog, "Registration Failed", "Passwords do not match.");
                return;
            }

            if (password.length() < 6) {
                std::cout << "[VALIDATION] ✗ Failed: Password too short (minimum 6 characters)" << std::endl;
				QMessageBox::warning(registerDialog, "Registration Failed", "Password must be at least 6 characters long.");
                return;
            }

            if (!email.contains("@")) {
                std::cout << "[VALIDATION] ✗ Failed: Invalid email format (no @ symbol)" << std::endl;
                QMessageBox::warning(registerDialog, "Registration Failed", "Please enter a valid email address.");
                return;
            }

            if (phone.length() < 10) { // simplistic phone validation of the format 7056717171
                std::cout << "[VALIDATION] ✗ Failed: Phone number too short (minimum 10 digits)" << std::endl;
                QMessageBox::warning(registerDialog, "Registration Failed", "Please enter a valid phone number.");
				return;
            }
            
            std::cout << "[VALIDATION] ✓ All checks passed" << std::endl;
            std::cout << "[DEBUG] Registration - Converting to UTF-8 and creating user..." << std::endl;
            
			// user registration logic - use UTF-8 encoding to prevent corruption
            registerNewUser(
                username.toUtf8().constData(), 
                password.toUtf8().constData(), 
                email.toUtf8().constData(), 
                phone.toUtf8().constData()
            );
            
            std::cout << "[SUCCESS] ✓ User registration completed!" << std::endl;
            QMessageBox::information(registerDialog, "Success", "User registered successfully!");
            registerDialog->accept();
        });
    
        // Cancel button logic
        QObject::connect(cancelButton, &QPushButton::clicked, [registerDialog]() {
            std::cout << "[DEBUG] Registration - Cancel button clicked" << std::endl;
            registerDialog->reject();
        });
    
        std::cout << "[DEBUG] Registration - Showing dialog" << std::endl;
        registerDialog->exec();
        delete registerDialog;
        std::cout << "[DEBUG] Registration - Dialog closed" << std::endl;
    });
    registerButton->setGeometry(110, 180, 100, 30);
    registerButton->show();

    std::cout << "[DEBUG] main() - All UI elements initialized" << std::endl;

    // -- DEBUGGING --
    //loginWindow.close();
   // bankingWindow->show();

    std::cout << "\n[DEBUG] main() - Entering Qt event loop..." << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return app.exec();
}
