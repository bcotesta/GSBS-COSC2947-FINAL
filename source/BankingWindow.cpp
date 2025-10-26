// Brandon Cotesta 10/16/2025

#include "BankingWindow.h"
#include <algorithm>
#include <chrono>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include < QTableWidgetItem>
#include <iostream>
#include <QHeaderView>
#include <QTimer>


BankingWindow::BankingWindow(QWidget* parent) : QWidget(parent),
    // eventually pull this data from database
    currentUser("John Doe", "jd@gmail.com", "reallygoodpassword123", "705-671-7171", "DummyAddress"),
    currentCustomer(1),
    currentAccount("123456789", AccountType::CHEQUING)
{
    setupUI();
    initializeData();
}

// -- onAccountChanged --
// -- Updates the currentAccount and refreshes the display
void BankingWindow::onAccountChanged() {
    QString selectedAccountNumber = accountSelector->currentData().toString();
    if (!selectedAccountNumber.isEmpty()) {
        auto accountsList = currentCustomer.accounts();
        auto it = std::find_if(accountsList.begin(), accountsList.end(),
            [&selectedAccountNumber](const Account& account) {
                return QString::fromStdString(account.accountNumber()) == selectedAccountNumber;
            });
        if (it != accountsList.end()) {
            currentAccount = *it;
            updateCurrentAccountDisplay();
        }
    }
}

// -- onViewBalance --
// -- Displays the current balance of the selected account
void BankingWindow::onViewBalance() {
    balanceLabel->setText(QString("Balance: $%1").arg(currentAccount.getBalance(), 0, 'f', 2));
    outputArea->append(QString("Account balance: $%1").arg(currentAccount.getBalance(), 0, 'f', 2));
   
}

// -- onDeposit --
// -- Prompts for deposit amount and updates the account
void BankingWindow::onDeposit() {
    bool ok = false;
    // Dialog modal
    QInputDialog dlg;
	dlg.setWindowTitle("Deposit");
	dlg.setLabelText("Enter deposit amount:");
	dlg.setInputMode(QInputDialog::DoubleInput);
	dlg.setDoubleRange(0.0, 999999.99);
	dlg.setDoubleDecimals(2);

    dlg.setFixedSize(200, 98);
	dlg.setParent(this, Qt::Dialog);

    ok = (dlg.exec() == QDialog::Accepted);
    
    if (ok && dlg.doubleValue() > 0) {
	    double amount = dlg.doubleValue();
        currentAccount.deposit(amount);
        updateCurrentAccountDisplay();
        outputArea->append(QString("Deposited $%1 to account %2")
            .arg(amount, 0, 'f', 2)
            .arg(QString::fromStdString(currentAccount.accountNumber())));
       
        // Update the account in customer's list
        updateAccountInCustomer();
    }
}

// -- onWithdraw --
// -- Prompts for withdrawal amount and updates the account
void BankingWindow::onWithdraw() {
    bool ok = false;
    // Dialog modal
    QInputDialog dlg;
    dlg.setWindowTitle("Withdraw");
    dlg.setLabelText("Enter withdrawl amount:");
    dlg.setInputMode(QInputDialog::DoubleInput);
    dlg.setDoubleRange(0.0, currentAccount.getBalance());
    dlg.setDoubleDecimals(2);

    dlg.setFixedSize(200, 98);
    dlg.setParent(this, Qt::Dialog);

    ok = (dlg.exec() == QDialog::Accepted);

    if (ok && dlg.doubleValue() > 0) {
		double amount = dlg.doubleValue();
        if (amount <= currentAccount.getBalance()) {
            currentAccount.withdraw(amount);
            updateCurrentAccountDisplay();
            outputArea->append(QString("Withdrew $%1 from account %2")
                .arg(amount, 0, 'f', 2)
                .arg(QString::fromStdString(currentAccount.accountNumber())));
			
            // Update the account in customer's list
            updateAccountInCustomer();
        } else {
            QMessageBox::warning(this, "Insufficient Funds", "Withdrawal amount exceeds current balance.");
        }
    }
}

// -- onTransfer --
// -- Prompts for transfer details and updates both accounts
void BankingWindow::onTransfer() {
    auto accountsList = currentCustomer.accounts();
	// edge case if you dont have a second account
    if (accountsList.size() < 2) {
        QMessageBox::information(nullptr, "Transfer", "You need at least 2 accounts to transfer funds.");
        return;
    }

	// Prepare list of account numbers excluding the current account
    QStringList accountNumbers;
    for (const auto& account : accountsList) {
        if (account.accountNumber() != currentAccount.accountNumber()) {
            accountNumbers << QString::fromStdString(account.accountNumber());
        }
    }

	// Prompt user to select destination account, and then an amount
	bool ok = false;
    // Dialog modal
    QInputDialog dlg;
    dlg.setWindowTitle("Transfer");
    dlg.setLabelText("Select destination account:");
    dlg.setInputMode(QInputDialog::TextInput);      // this is unused but needs to be set to prevent errors
	dlg.setComboBoxItems(accountNumbers);
	dlg.setComboBoxEditable(false);

	dlg.setModal(true);

    dlg.setFixedSize(200, 98);
    dlg.setParent(this, Qt::Dialog);

    ok = (dlg.exec() == QDialog::Accepted);
    
    if (ok && !dlg.textValue().isEmpty()) {
		QString toAccount = dlg.textValue();

        // Dialog modal
        QInputDialog dlg_i;
        dlg_i.setWindowTitle("Transfer");
        dlg_i.setLabelText("Enter transfer amount:");
        dlg_i.setInputMode(QInputDialog::DoubleInput);
        dlg_i.setDoubleRange(0.0, currentAccount.getBalance());
        dlg_i.setDoubleDecimals(2);

        dlg_i.setFixedSize(200, 98);
        dlg_i.setParent(this, Qt::Dialog);

        ok = (dlg_i.exec() == QDialog::Accepted);

        if (ok && dlg_i.doubleValue() > 0) {
			double amount = dlg_i.doubleValue();
            try {
                currentCustomer.transferFunds(currentAccount.accountNumber(), toAccount.toStdString(), amount);
                outputArea->append(QString("Transferred $%1 from %2 to %3")
                    .arg(amount, 0, 'f', 2)
                    .arg(QString::fromStdString(currentAccount.accountNumber()))
                    .arg(toAccount));
				
                // Refresh current account
                onAccountChanged();
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Transfer Error", QString::fromStdString(e.what()));
            }
        }
    }
}

// -- onViewTransactions --
// -- Displays recent transactions for the current account
void BankingWindow::onViewTransactions() {
    outputArea->clear();
    outputArea->append(QString("Recent transactions for account %1:").arg(QString::fromStdString(currentAccount.accountNumber())));
    outputArea->append("----------------------------------------");
	
    for (const auto& transaction : currentAccount.transactionHistory()) {
        QString transactionText = QString("%1 - %2: $%3")
            .arg(QString::fromStdString(transaction.date()))
            .arg(transaction.type() == TransactionType::DEPOSIT ? "Deposit" : "Withdrawal")
            .arg(transaction.amount(), 0, 'f', 2);
        outputArea->append(transactionText);
    }
}

// -- onMiniStatement --
// -- Displays a mini statement for the last 4 weeks
void BankingWindow::onMiniStatement() {
    outputArea->clear();
    outputArea->append(QString("Mini Statement for Account: %1").arg(QString::fromStdString(currentAccount.accountNumber())));
    outputArea->append("----------------------------------------");
    outputArea->append("Date\t\t\tType\tAmount\tDescription");
    outputArea->append("----------------------------------------");
    
    // Get current time and calculate 4 weeks ago
    auto now = std::chrono::system_clock::now();
    auto fourWeeksAgo = now - std::chrono::hours(24 * 28);
    
    for (const auto& transaction : currentAccount.transactionHistory()) {
        if(transaction.rawDate() >= fourWeeksAgo && transaction.rawDate() <= now) {
            QString transactionText = QString("%1\t%2\t$%3\t%4")
                .arg(QString::fromStdString(transaction.date()))
                .arg(transaction.type() == TransactionType::DEPOSIT ? "Deposit" : "Withdrawal")
                .arg(transaction.amount(), 0, 'f', 2)
                .arg(QString::fromStdString(transaction.description()));
            outputArea->append(transactionText);
        }
    }
    outputArea->append("----------------------------------------");
    outputArea->append(QString("Current Balance: $%1").arg(currentAccount.getBalance(), 0, 'f', 2));
    outputArea->append("----------------------------------------");
}

// -- onNewAccount --
// -- Prompts for new account type and creates it, and adds to customer
// -- this will later work with the database and it's controller
void BankingWindow::onNewAccount() {
    QStringList accountTypes;
    accountTypes << "Chequing" << "Savings" << "Credit";
    
    bool ok = false;
    // Dialog modal
    QInputDialog dlg;
    dlg.setWindowTitle("New Account");
    dlg.setLabelText("Select account type:");
    dlg.setInputMode(QInputDialog::TextInput);      // this is unused but needs to be set to prevent errors
    dlg.setComboBoxItems(accountTypes);
    dlg.setComboBoxEditable(false);

    dlg.setModal(true);

    dlg.setFixedSize(200, 98);
    dlg.setParent(this, Qt::Dialog);
    
	ok = (dlg.exec() == QDialog::Accepted);

    if (ok) {
		QString selectedType = dlg.textValue();
        
        // Generate a simple account number (in real app, this would be from database)
        QString newAccountNumber = QString::number(123456000 + currentCustomer.accounts().size() + 1);
        
        AccountType accType = AccountType::CHEQUING;
        if (selectedType == "Savings") accType = AccountType::SAVINGS;
        else if (selectedType == "Credit") accType = AccountType::CREDIT;
        
        Account newAccount(newAccountNumber.toStdString(), accType);
        currentCustomer.addAccount(newAccount);
        
        updateAccountSelector();
        outputArea->append(QString("Created new %1 account: %2").arg(selectedType, newAccountNumber));
    }
}

// -- setupUI --
// -- Initializes and arranges all UI components
void BankingWindow::setupUI() {
    setWindowTitle("GSBS - Greater Sudbury Banking Service");
    setGeometry(0, 0, 500, 800);
    setMinimumSize(500, 800);
    
    // Create main layout and store reference
    mainLayout = new QVBoxLayout(this);
    
#pragma region <Top of screen bar>

	// Create top of screen toolbar'
	tbLayout = new QHBoxLayout();
    tbLayout->setContentsMargins(10, 10, 10, 10);
    tbLayout->setSpacing(15);
	mainLayout->addLayout(tbLayout);
    
	// app title label
	QLabel* titleLabel = new QLabel("GSBS");
	titleLabel->setStyleSheet("font-weight: bold; font-size: 24px;");
	tbLayout->addWidget(titleLabel);



    searchBar = new QLineEdit();
    searchBar->setPlaceholderText("🔍 Search...");
    searchBar->setFixedHeight(35);
    searchBar->setMinimumWidth(200);
    searchBar->setStyleSheet(
        "QLineEdit {"
        "  background-color: rgb(255,255,255);"   
        "  color: rgb(0,0,0);"                   
        "  border: 1px solid #cccccc;"
        "  border-radius: 8px;"
        "  padding-left: 10px;"
        "  font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #0078D7;"
        "  background-color: rgb(255,255,255);"
        "  color: rgb(0,0,0);"
        "}"
        "QLineEdit::placeholder {"
        "  color: rgb(136,136,136);"
        "}"
    );

    tbLayout->addWidget(searchBar);
  
    // profile button
	QPushButton* profileBtn = new QPushButton("Profile");
	profileBtn->setFixedHeight(40);
	profileBtn->setFixedWidth(60);
	styleNavigationButton(profileBtn);
	connect(profileBtn, &QPushButton::clicked, [this]() {
		setCurrentView(5); // Profile view index
    });
	tbLayout->addWidget(profileBtn);


#pragma endregion
    
    // Create stacked widget for view switching
    contentStack = new QStackedWidget();
    mainLayout->addWidget(contentStack);
    
    // Setup individual views
    setupViews();
    

    // Create navigation bar
    navLayout = new QHBoxLayout();
    mainLayout->addLayout(navLayout);
    
    QPushButton* homeBtn = new QPushButton("Home");
    homeBtn->setFixedHeight(70);
    styleNavigationButton(homeBtn);
    connect(homeBtn, &QPushButton::clicked, [this]() {
        setCurrentView(0);
    });
    navLayout->addWidget(homeBtn);

    QPushButton* transfersButton = new QPushButton("Transfers");
    transfersButton->setFixedHeight(70);
    styleNavigationButton(transfersButton);
    connect(transfersButton, &QPushButton::clicked, [this]() {
        setCurrentView(1);
    });
    navLayout->addWidget(transfersButton);

    QPushButton* paymentsButton = new QPushButton("Bills");
    paymentsButton->setFixedHeight(70);
    styleNavigationButton(paymentsButton);
    connect(paymentsButton, &QPushButton::clicked, [this]() {
        setCurrentView(2);
    });
    navLayout->addWidget(paymentsButton);

    QPushButton* adviceButton = new QPushButton("Advice");
    adviceButton->setFixedHeight(70);
    styleNavigationButton(adviceButton);
    connect(adviceButton, &QPushButton::clicked, [this]() {
        setCurrentView(3);
    });
    navLayout->addWidget(adviceButton);

    QPushButton* moreButton = new QPushButton("More");
    moreButton->setFixedHeight(70);
    styleNavigationButton(moreButton);
    connect(moreButton, &QPushButton::clicked, [this]() {
        setCurrentView(4);
    });
    navLayout->addWidget(moreButton);
    
    // Set initial view
    currentViewIndex = 0;
    setCurrentView(0);
}

// New method to setup individual views
void BankingWindow::setupViews() {
#pragma region <Home View>
    homeView = new QWidget();
    QVBoxLayout* homeLayout = new QVBoxLayout(homeView);
    welcomeLabel = new QLabel("Welcome!");
    homeLayout->addWidget(welcomeLabel);
  //-------------------------------SAHARA------------------------------------
 
    // ---- Accounts box ----
    QGroupBox* accountTypesBox = new QGroupBox("Accounts");
    accountTypesBox->setStyleSheet(
        "QGroupBox {"
        "  font-weight: bold;"
        "  font-size: 16px;"
        "  border: 1px solid #555555;"
        "  border-radius: 8px;"
        "  margin-top: 10px;"
        "  padding: 10px;"
        "  color: white;"
        "  background-color: #2b2b2b;"
        "}"
        "QLabel { font-size: 14px; color: white; }"
    );

    // main layout inside the group box
    QVBoxLayout* typesLayout = new QVBoxLayout();
    typesLayout->setContentsMargins(8, 8, 8, 8);
    typesLayout->setSpacing(6);
    accountTypesBox->setLayout(typesLayout);

    // helper lambda to add a row
    auto addAccountRow = [&](const QString& typeName,
        const QString& accountNumber,
        const QString& amount,
        const QString& color = "#00aaff") {

            QWidget* row = new QWidget(accountTypesBox);
            QHBoxLayout* rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(0, 2, 0, 2);
            rowLayout->setSpacing(6);

            // left side: vertical stack for type + number
            QVBoxLayout* leftLayout = new QVBoxLayout();
            leftLayout->setContentsMargins(0, 0, 0, 0);
            leftLayout->setSpacing(0);

            
            QLabel* nameLabel = new QLabel(typeName, row);
            QLabel* numberLabel = new QLabel(accountNumber, row);

            nameLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white;");
            numberLabel->setStyleSheet("font-size: 12px; color: #bbbbbb; margin-left: 2px;");

            leftLayout->addWidget(nameLabel);
            leftLayout->addWidget(numberLabel);

            // right side: balance label
            QLabel* balanceLabel = new QLabel(amount, row);
            balanceLabel->setStyleSheet(QString("font-weight: bold; color: %1; font-size: 14px;").arg(color));
            balanceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            // combine left + right into a row
            rowLayout->addLayout(leftLayout);
            rowLayout->addStretch();
            rowLayout->addWidget(balanceLabel);

            typesLayout->addWidget(row);
        };

    // ---- Add rows ----
    addAccountRow("Chequing", "Acct #123-456-789", "$1,200.00");
    addAccountRow("Savings", "Acct #987-654-321", "$5,430.50");
    addAccountRow("Credit", "Acct #555-111-222", "-$320.00", "red");

    // divider before total
    QFrame* divider = new QFrame(accountTypesBox);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("background-color: #555555; height: 1px;");
    typesLayout->addWidget(divider);

    // total row
    QWidget* totalRow = new QWidget(accountTypesBox);
    QHBoxLayout* totalLayout = new QHBoxLayout(totalRow);
    totalLayout->setContentsMargins(0, 2, 0, 2);

    QLabel* totalLabel = new QLabel("Total:", totalRow);
    QLabel* totalValue = new QLabel("$6,950.50", totalRow);

    totalLabel->setStyleSheet("font-weight: bold; color: white;");
    totalValue->setStyleSheet("font-weight: bold; color: #00aaff;");
    totalValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    totalLayout->addWidget(totalLabel);
    totalLayout->addStretch();
    totalLayout->addWidget(totalValue);
    typesLayout->addWidget(totalRow);

    // finally add to home layout
    homeLayout->addWidget(accountTypesBox);


//---------------------------------SAHARA-------------------------------------


    currentAccountLabel = new QLabel("Account: ");
    balanceLabel = new QLabel("Balance: $0.00");
    accountSelector = new QComboBox();
    
   
    homeLayout->addWidget(currentAccountLabel);
    homeLayout->addWidget(balanceLabel);
    homeLayout->addWidget(accountSelector);
    
    viewBalanceBtn = new QPushButton("View Balance");
    depositBtn = new QPushButton("Deposit");
    withdrawBtn = new QPushButton("Withdraw");
    transferBtn = new QPushButton("Transfer");
    
    homeLayout->addWidget(viewBalanceBtn);
    homeLayout->addWidget(depositBtn);
    homeLayout->addWidget(withdrawBtn);
    homeLayout->addWidget(transferBtn);
    
    outputArea = new QTextEdit();
    homeLayout->addWidget(outputArea);
    
    // Connect signals
    connect(accountSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BankingWindow::onAccountChanged);
    connect(viewBalanceBtn, &QPushButton::clicked, this, &BankingWindow::onViewBalance);
    connect(depositBtn, &QPushButton::clicked, this, &BankingWindow::onDeposit);
    connect(withdrawBtn, &QPushButton::clicked, this, &BankingWindow::onWithdraw);
    connect(transferBtn, &QPushButton::clicked, this, &BankingWindow::onTransfer);
    
    contentStack->addWidget(homeView);
#pragma endregion
    
//#pragma region <Transfers View>
   // transfersView = new QWidget();
   // QVBoxLayout* transfersLayout = new QVBoxLayout(transfersView);
   // transfersLayout->addWidget(new QLabel("Transfers View"));
   // transfersLayout->addWidget(new QLabel("Transfer functionality will go here"));
   // contentStack->addWidget(transfersView);
//#pragma endregion


#pragma region <Transfers View>
    transfersView = new QWidget();
    QVBoxLayout* transfersLayout = new QVBoxLayout(transfersView);
    transfersLayout->setAlignment(Qt::AlignTop);
    transfersLayout->setContentsMargins(40, 40, 40, 40);
    transfersLayout->setSpacing(35);

    // --- Header Label ---
    QLabel* transfersHeader = new QLabel("Transfers & Banking Actions");
    transfersHeader->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: white; margin-bottom: 20px;"
    );
    transfersLayout->addWidget(transfersHeader);

    // --- Grid layout for 8 buttons (4 rows × 2 columns) ---
    QGridLayout* grid = new QGridLayout();
    grid->setHorizontalSpacing(60); // more space between columns
    grid->setVerticalSpacing(45);   // more space between rows
    grid->setContentsMargins(10, 10, 10, 10);

    // Reusable button style (white squares, black text)
    QString buttonStyle =
        "QPushButton {"
        "  background-color: white;"
        "  color: black;"
        "  border: 1px solid #cccccc;"
        "  border-radius: 12px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "  padding: 10px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #f5f5f5;"
        "  border: 1px solid #0078D7;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #e6e6e6;"
        "}";

    // Create buttons
    QPushButton* interacBtn = new QPushButton("Interac e-Transfer");
    QPushButton* fundsBtn = new QPushButton("Transfer Funds");
    QPushButton* billsBtn = new QPushButton("Bill Payments");
    QPushButton* depositBtn = new QPushButton("eDeposit");
    QPushButton* manageBtn = new QPushButton("Manage Transactions");
    QPushButton* investBtn = new QPushButton("Investing");
    QPushButton* tfsaAddBtn = new QPushButton("Contribute to TFSA");
    QPushButton* tfsaWithdrawBtn = new QPushButton("Withdraw from TFSA");

    // Apply style and smaller, consistent size
    for (auto btn : { interacBtn, fundsBtn, billsBtn, depositBtn,
                      manageBtn, investBtn, tfsaAddBtn, tfsaWithdrawBtn }) {
        btn->setStyleSheet(buttonStyle);
        btn->setFixedSize(180, 80); // smaller box size
    }

    // --- Add buttons to grid (4 rows × 2 columns) ---
    grid->addWidget(interacBtn, 0, 0);
    grid->addWidget(fundsBtn, 0, 1);

    grid->addWidget(billsBtn, 1, 0);
    grid->addWidget(depositBtn, 1, 1);

    grid->addWidget(manageBtn, 2, 0);
    grid->addWidget(investBtn, 2, 1);

    grid->addWidget(tfsaAddBtn, 3, 0);
    grid->addWidget(tfsaWithdrawBtn, 3, 1);

    // Add grid to layout
    transfersLayout->addLayout(grid);

    // Optional footer
    QLabel* infoLabel = new QLabel("Select an option to continue.");
    infoLabel->setStyleSheet("color: #bbbbbb; font-size: 13px; margin-top: 15px;");
    infoLabel->setAlignment(Qt::AlignHCenter);
    transfersLayout->addWidget(infoLabel);

    contentStack->addWidget(transfersView);
#pragma endregion


/*#pragma region <Bills View>
    billsView = new QWidget();
    QVBoxLayout* billsLayout = new QVBoxLayout(billsView);
    billsLayout->addWidget(new QLabel("Bills View"));
    billsLayout->addWidget(new QLabel("Bill payment functionality will go here"));
    contentStack->addWidget(billsView);
#pragma endregion*/

#pragma region <Bills View>
    billsView = new QWidget();
    QVBoxLayout* billsLayout = new QVBoxLayout(billsView);  // ✅ define it here
    billsLayout->setAlignment(Qt::AlignTop);
    billsLayout->setContentsMargins(40, 40, 40, 40);
    billsLayout->setSpacing(25);

    // --- Header ---
    QLabel* billsHeader = new QLabel("Bills & Payments");
    billsHeader->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: white; margin-bottom: 10px;"
    );
    billsLayout->addWidget(billsHeader);

    // --- Vertical Button Stack ---
    QVBoxLayout* btnColumn = new QVBoxLayout();
    btnColumn->setSpacing(20);

    QString billsButtonStyle =
        "QPushButton {"
        "  background-color: white;"
        "  color: black;"
        "  border: 1px solid #cccccc;"
        "  border-radius: 10px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  padding: 15px 25px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #f0f0f0;"
        "  border: 1px solid #0078D7;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #e6e6e6;"
        "}";

    QPushButton* makePaymentBtn = new QPushButton("Make a Payment");
    QPushButton* managePayeesBtn = new QPushButton("Manage Payees");
    QPushButton* manageScheduleBtn = new QPushButton("Manage Scheduled Payments");

    // Apply style
    for (auto btn : { makePaymentBtn, managePayeesBtn, manageScheduleBtn }) {
        btn->setStyleSheet(billsButtonStyle);
        btn->setFixedHeight(60);
        btn->setCursor(Qt::PointingHandCursor);
        btnColumn->addWidget(btn);
    }

    billsLayout->addLayout(btnColumn);

    // Now your new Scheduled Payments Box code
    // --- Scheduled Payments Box (styled like Budget) ---
    QGroupBox* scheduledBox = new QGroupBox("Scheduled Payments");
    scheduledBox->setStyleSheet(
        "QGroupBox {"
        "  font-weight: bold;"
        "  font-size: 16px;"
        "  color: white;"
        "  border: 1px solid #555555;"
        "  border-radius: 8px;"
        "  background-color: #2b2b2b;"
        "  margin-top: 15px;"
        "  padding: 10px;"
        "}"
        "QLabel { color: white; font-size: 14px; }"
    );
    billsLayout->addWidget(scheduledBox);

    QVBoxLayout* scheduledLayout = new QVBoxLayout();
    scheduledLayout->setContentsMargins(8, 8, 8, 8);
    scheduledLayout->setSpacing(10);
    scheduledBox->setLayout(scheduledLayout);

    // Header row
    {
        QWidget* headerRow = new QWidget();
        QHBoxLayout* headerLayout = new QHBoxLayout(headerRow);
        headerLayout->setContentsMargins(0, 0, 0, 0);
        headerLayout->setSpacing(0);

        QLabel* nameHeader = new QLabel("Payee Name");
        QLabel* freqHeader = new QLabel("Frequency");
        QLabel* amtHeader = new QLabel("Amount");

        nameHeader->setStyleSheet("font-weight: bold; color: #00aaff;");
        freqHeader->setStyleSheet("font-weight: bold; color: #00aaff;");
        amtHeader->setStyleSheet("font-weight: bold; color: #00aaff;");

        headerLayout->addWidget(nameHeader);
        headerLayout->addStretch();
        headerLayout->addWidget(freqHeader);
        headerLayout->addStretch();
        headerLayout->addWidget(amtHeader);
        scheduledLayout->addWidget(headerRow);
    }

    // Divider line
    {
        QFrame* divider = new QFrame();
        divider->setFrameShape(QFrame::HLine);
        divider->setStyleSheet("background-color: #555555; height: 1px;");
        scheduledLayout->addWidget(divider);
    }

    // Helper lambda to add rows
    auto addPaymentRow = [&](const QString& name, const QString& freq, const QString& amt, const QString& color = "#ffffff") {
        QWidget* row = new QWidget();
        QHBoxLayout* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(0);

        QLabel* nameLabel = new QLabel(name);
        QLabel* freqLabel = new QLabel(freq);
        QLabel* amtLabel = new QLabel(amt);

        nameLabel->setStyleSheet("font-size: 14px; color: white;");
        freqLabel->setStyleSheet("font-size: 14px; color: white;");
        amtLabel->setStyleSheet(QString("font-weight: bold; color: %1; font-size: 14px;").arg(color));

        rowLayout->addWidget(nameLabel);
        rowLayout->addStretch();
        rowLayout->addWidget(freqLabel);
        rowLayout->addStretch();
        rowLayout->addWidget(amtLabel);

        scheduledLayout->addWidget(row);
        };

    // Add rows
    addPaymentRow("Hydro One", "Monthly", "$120.00");
    addPaymentRow("Internet Co.", "Monthly", "$89.99");
    addPaymentRow("Credit Card", "Weekly", "$50.00");

    // Divider and total row
    {
        QFrame* divider = new QFrame();
        divider->setFrameShape(QFrame::HLine);
        divider->setStyleSheet("background-color: #555555; height: 1px;");
        scheduledLayout->addWidget(divider);

        QWidget* totalRow = new QWidget();
        QHBoxLayout* totalLayout = new QHBoxLayout(totalRow);
        totalLayout->setContentsMargins(0, 0, 0, 0);

        QLabel* totalLabel = new QLabel("Total Monthly Payments:");
        QLabel* totalValue = new QLabel("$259.99");

        totalLabel->setStyleSheet("font-weight: bold; color: white;");
        totalValue->setStyleSheet("font-weight: bold; color: #00aaff;");

        totalLayout->addWidget(totalLabel);
        totalLayout->addStretch();
        totalLayout->addWidget(totalValue);

        scheduledLayout->addWidget(totalRow);
    }

    contentStack->addWidget(billsView);
#pragma endregion





/*#pragma region <Advice View>
    adviceView = new QWidget();
    QVBoxLayout* adviceLayout = new QVBoxLayout(adviceView);
    adviceLayout->addWidget(new QLabel("Financial Advice View"));
    adviceLayout->addWidget(new QLabel("Financial advice content will go here"));
    contentStack->addWidget(adviceView);
#pragma endregion*/
#pragma region <Advice View>
    adviceView = new QWidget();
    QVBoxLayout* adviceLayout = new QVBoxLayout(adviceView);
    adviceLayout->setAlignment(Qt::AlignTop);
    adviceLayout->setContentsMargins(40, 40, 40, 40);
    adviceLayout->setSpacing(25);

    // --- Header Label ---
    QLabel* adviceHeader = new QLabel("Financial Advice");
    adviceHeader->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: white; margin-bottom: 10px;"
    );
    adviceLayout->addWidget(adviceHeader);

    // --- My Goals Group Box ---
    QGroupBox* goalsBox = new QGroupBox("My Goals");
    goalsBox->setStyleSheet(
        "QGroupBox {"
        "  font-weight: bold;"
        "  font-size: 16px;"
        "  color: white;"
        "  border: 1px solid #555555;"
        "  border-radius: 10px;"
        "  margin-top: 10px;"
        "  padding: 15px;"
        "}"
    );
    QVBoxLayout* goalsLayout = new QVBoxLayout(goalsBox);
    goalsLayout->setSpacing(20);

    // --- Goal 1: Education ---
    QWidget* eduGoal = new QWidget();
    QHBoxLayout* eduLayout = new QHBoxLayout(eduGoal);
    eduLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* eduLabel = new QLabel("🎓 Saving for Education");
    eduLabel->setStyleSheet("font-size: 14px; color: white; font-weight: bold;");
    QProgressBar* eduProgress = new QProgressBar();
    eduProgress->setValue(70); // 70% complete
    eduProgress->setTextVisible(true);
    eduProgress->setFormat("%p%");
    eduProgress->setFixedHeight(20);
    eduProgress->setStyleSheet(
        "QProgressBar {"
        "  border: 1px solid #444;"
        "  border-radius: 5px;"
        "  background-color: #2b2b2b;"
        "  color: white;"
        "  text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #00aaff;"
        "  border-radius: 5px;"
        "}"
    );
    eduLayout->addWidget(eduLabel);
    eduLayout->addSpacing(20);
    eduLayout->addWidget(eduProgress);
    goalsLayout->addWidget(eduGoal);

    // --- Goal 2: Travel ---
    QWidget* travelGoal = new QWidget();
    QHBoxLayout* travelLayout = new QHBoxLayout(travelGoal);
    travelLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* travelLabel = new QLabel("✈️ Saving for Travel");
    travelLabel->setStyleSheet("font-size: 14px; color: white; font-weight: bold;");
    QProgressBar* travelProgress = new QProgressBar();
    travelProgress->setValue(40); // 40% complete
    travelProgress->setTextVisible(true);
    travelProgress->setFormat("%p%");
    travelProgress->setFixedHeight(20);
    travelProgress->setStyleSheet(
        "QProgressBar {"
        "  border: 1px solid #444;"
        "  border-radius: 5px;"
        "  background-color: #2b2b2b;"
        "  color: white;"
        "  text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #ffaa00;"
        "  border-radius: 5px;"
        "}"
    );
    travelLayout->addWidget(travelLabel);
    travelLayout->addSpacing(20);
    travelLayout->addWidget(travelProgress);
    goalsLayout->addWidget(travelGoal);

    // Add the goals box to the main layout
    adviceLayout->addWidget(goalsBox);

    // --- Optional Tip Section ---
    QLabel* tipsLabel = new QLabel(
        "💡 Tip: Set aside small, consistent amounts for your goals each week.\n"
        "Even $20/week can make a big difference over time!"
    );
    tipsLabel->setWordWrap(true);
    tipsLabel->setStyleSheet("font-size: 13px; color: #cccccc;");
    adviceLayout->addWidget(tipsLabel);

    contentStack->addWidget(adviceView);


    // --- Budget Overview Box ---
    QGroupBox* budgetBox = new QGroupBox("Budget Overview");
    budgetBox->setStyleSheet(
        "QGroupBox {"
        "  font-weight: bold;"
        "  font-size: 16px;"
        "  color: white;"
        "  border: 1px solid #555555;"
        "  border-radius: 10px;"
        "  margin-top: 20px;"
        "  padding: 15px;"
        "}"
        "QLabel { color: white; font-size: 13px; }"
    );

    QGridLayout* budgetLayout = new QGridLayout();
    budgetLayout->setContentsMargins(10, 10, 10, 10);
    budgetLayout->setHorizontalSpacing(40);
    budgetLayout->setVerticalSpacing(15);

    // --- Header Row ---
    QLabel* categoryHeader = new QLabel("Category");
    QLabel* projectedHeader = new QLabel("Projected ($)");
    QLabel* actualHeader = new QLabel("Actual ($)");

    categoryHeader->setStyleSheet("font-weight: bold; color: #00aaff;");
    projectedHeader->setStyleSheet("font-weight: bold; color: #00aaff;");
    actualHeader->setStyleSheet("font-weight: bold; color: #00aaff;");

    budgetLayout->addWidget(categoryHeader, 0, 0);
    budgetLayout->addWidget(projectedHeader, 0, 1);
    budgetLayout->addWidget(actualHeader, 0, 2);

    // --- Budget Rows ---
    auto addBudgetRow = [&](int row, const QString& category, double projected, double actual) {
        QLabel* catLabel = new QLabel(category);
        QLabel* projLabel = new QLabel(QString("$%1").arg(projected, 0, 'f', 2));
        QLabel* actLabel = new QLabel(QString("$%1").arg(actual, 0, 'f', 2));

        // Conditional color based on overspending
        QString color = (actual > projected) ? "red" : "#00ff88";
        actLabel->setStyleSheet(QString("font-weight: bold; color: %1;").arg(color));

        budgetLayout->addWidget(catLabel, row, 0);
        budgetLayout->addWidget(projLabel, row, 1);
        budgetLayout->addWidget(actLabel, row, 2);
        };

    // Example categories
    addBudgetRow(1, "Housing", 1200.00, 1180.00);
    addBudgetRow(2, "Groceries", 400.00, 425.00);
    addBudgetRow(3, "Entertainment", 200.00, 150.00);
    addBudgetRow(4, "Transportation", 180.00, 200.00);
    addBudgetRow(5, "Utilities", 250.00, 230.00);

    budgetBox->setLayout(budgetLayout);

    // Add it to the Advice layout
    adviceLayout->addWidget(budgetBox);

#pragma endregion

#pragma region <More View>
    moreView = new QWidget();
    QVBoxLayout* moreLayout = new QVBoxLayout(moreView);
    moreLayout->setAlignment(Qt::AlignTop);
    moreLayout->setContentsMargins(40, 40, 40, 40);
    moreLayout->setSpacing(15);

    QLabel* moreHeader = new QLabel("More Options");
    moreHeader->setStyleSheet("font-size: 22px; font-weight: bold; color: white; margin-bottom: 10px;");
    moreLayout->addWidget(moreHeader);

    // Button style
    QString moreButtonStyle =
        "QPushButton {"
        "  background-color: white;"
        "  color: black;"
        "  border: 1px solid #cccccc;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  padding: 12px;"
        "}"
        "QPushButton:hover { background-color: #f0f0f0; border: 1px solid #0078D7; }"
        "QPushButton:pressed { background-color: #e6e6e6; }";

    // Create buttons
    QPushButton* productsBtn = new QPushButton("Products and Services");
    QPushButton* appInfoBtn = new QPushButton("Get to Know the App");
    QPushButton* contactBtn = new QPushButton("Contact Us");
    QPushButton* faqBtn = new QPushButton("FAQ");
    QPushButton* privacyBtn = new QPushButton("Privacy and Legal");
    QPushButton* signOutBtn = new QPushButton("Sign Out");

    // Apply styling & layout
    for (auto btn : { productsBtn, appInfoBtn, contactBtn, faqBtn, privacyBtn, signOutBtn }) {
        btn->setStyleSheet(moreButtonStyle);
        btn->setFixedHeight(50);
        btn->setCursor(Qt::PointingHandCursor);
        moreLayout->addWidget(btn);
    }

    contentStack->addWidget(moreView);

    // ------------------------------------
    // Helper to create sub-pages
    // ------------------------------------
    auto createSimplePage = [&](const QString& title, const QString& contentText) {
        QWidget* page = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(page);
        layout->setContentsMargins(40, 20, 40, 20);  // less top margin
        layout->setSpacing(10);
        layout->setAlignment(Qt::AlignTop);

        QLabel* header = new QLabel(title);
        header->setStyleSheet("font-size: 22px; font-weight: bold; color: white;");
        layout->addWidget(header);

        QLabel* content = new QLabel(contentText);
        content->setWordWrap(true);
        content->setStyleSheet("font-size: 14px; color: #cccccc; line-height: 1.4;");
        layout->addWidget(content);

        QPushButton* backBtn = new QPushButton("← Back");
        backBtn->setStyleSheet(
            "QPushButton { background-color: #0078D7; color: white; border-radius: 8px; padding: 8px 16px; }"
            "QPushButton:hover { background-color: #005fa3; }"
        );
        layout->addWidget(backBtn);

        connect(backBtn, &QPushButton::clicked, [=]() {
            contentStack->setCurrentWidget(moreView);
            });

        contentStack->addWidget(page);
        return page;
        };

    // ------------------------------------
    // Actual Page Content
    // ------------------------------------

    // Products & Services
    QString productsText =
        "At GSBS, we’re committed to providing a diverse portfolio of financial products and services "
        "that empower you to take control of your financial future.\n\n"
        "• Chequing Accounts – Manage your daily transactions with ease, no hidden fees, and instant e-Transfers.\n"
        "• Savings Accounts – Earn competitive interest rates while maintaining flexible access to your funds.\n"
        "• Credit Solutions – From personal loans to low-interest credit cards, our lending products are designed "
        "to help you achieve your goals responsibly.\n"
        "• Investments – Explore GICs, mutual funds, and long-term savings tools that match your financial ambitions.\n"
        "• Business Banking – Open business accounts, manage payroll, and streamline payments with enterprise-grade security.\n\n"
        "Our mission is simple: to deliver safe, transparent, and innovative banking options for individuals and businesses "
        "across Greater Sudbury and beyond.";

    // Get to Know the App
    QString appInfoText =
        "Welcome to the GSBS (Greater Sudbury Banking Service) application, your trusted partner in everyday banking.\n\n"
        "Designed for speed, security, and simplicity, our platform gives you the flexibility to bank anywhere, anytime. "
        "Whether you’re transferring money, paying bills, or setting financial goals, GSBS ensures that every transaction "
        "is secure and intuitive.\n\n"
        "• Manage multiple accounts and view balances in real time.\n"
        "• Make instant payments and transfers with Interac e-Transfer.\n"
        "• Track monthly budgets, spending habits, and personalized savings goals.\n"
        "• Access smart insights and tailored advice to help you plan ahead.\n"
        "• Enable biometric sign-in for faster, safer authentication.\n\n"
        "Our design philosophy is guided by three principles: accessibility, reliability, and trust. "
        "We continue to evolve with new features based on user feedback to make your banking experience seamless.";

    // Contact Us
    QString contactText =
        "Need assistance? Our dedicated support team is always ready to help.\n\n"
        "Phone: 705-671-7171 (Mon–Fri, 9 AM – 5 PM EST)\n"
        "Email: support@gsbsbank.ca\n"
        "Address: 123 Elm Street, Sudbury ON P3E 1B5\n\n"
        "You can also visit our Help Centre within the app for step-by-step guides, troubleshooting, "
        "and live chat support.\n\n"
        "We value your time and strive to respond to all messages within 24 hours. "
        "Your feedback directly shapes how we improve GSBS. Thank you for helping us serve you better!";

    // FAQ (clean plain text version)
    QString faqText =
        "How do I reset my password?\n"
        "Go to the Sign-In page, click 'Forgot Password?', and follow the recovery steps. "
        "A verification link will be sent to your registered email address.\n\n"
        "Can I open new accounts online?\n"
        "Yes, you can open chequing, savings, or credit accounts from the 'Accounts' tab in just a few clicks.\n\n"
        "Is GSBS secure?\n"
        "Absolutely. We use industry-standard AES-256 encryption, multi-factor authentication, "
        "and real-time fraud detection to protect your financial data.\n\n"
        "Can I access my account abroad?\n"
        "Yes, GSBS Online works internationally wherever internet access is available. "
        "Be sure to enable two-factor authentication for additional security.\n\n"
        "How can I contact support?\n"
        "You can use the 'Contact Us' section in the app or call 705-671-7171 for immediate assistance.";

    // Privacy & Legal
    QString privacyText =
        "At GSBS, your privacy is not just a policy. It’s a promise.\n\n"
        "We comply with Canada’s Personal Information Protection and Electronic Documents Act (PIPEDA) "
        "and adhere to strict internal safeguards that protect your personal and financial data.\n\n"
        "We never sell, rent, or share your information with unauthorized third parties. "
        "All data is encrypted at rest and in transit, stored on secure Canadian servers.\n\n"
        "By using GSBS services, you agree to our Terms of Use and Privacy Policy. "
        "We encourage all users to review these policies regularly as we update them to reflect "
        "new technologies and best practices.\n\n"
        "If you have any privacy-related inquiries, please reach out to privacy@gsbsbank.ca.";

    // Sign Out
    QString signOutText =
        "You have been signed out successfully.\n\n"
        "For your security, please close the application or log out of your account completely "
        "if you’re using a shared or public computer.\n\n"
        "To access your accounts again, simply return to the Home page and log in using your credentials.";

    // ------------------------------------
    // Create Pages
    // ------------------------------------
    QWidget* productsPage = createSimplePage("Products and Services", productsText);
    QWidget* appInfoPage = createSimplePage("Get to Know the App", appInfoText);
    QWidget* contactPage = createSimplePage("Contact Us", contactText);
    QWidget* faqPage = createSimplePage("Frequently Asked Questions (FAQ)", faqText);
    QWidget* privacyPage = createSimplePage("Privacy and Legal", privacyText);
    QWidget* signOutPage = createSimplePage("Sign Out", signOutText);

    // ------------------------------------
    // Connect Buttons to Pages
    // ------------------------------------
    connect(productsBtn, &QPushButton::clicked, [=]() { contentStack->setCurrentWidget(productsPage); });
    connect(appInfoBtn, &QPushButton::clicked, [=]() { contentStack->setCurrentWidget(appInfoPage); });
    connect(contactBtn, &QPushButton::clicked, [=]() { contentStack->setCurrentWidget(contactPage); });
    connect(faqBtn, &QPushButton::clicked, [=]() { contentStack->setCurrentWidget(faqPage); });
    connect(privacyBtn, &QPushButton::clicked, [=]() { contentStack->setCurrentWidget(privacyPage); });

    // ✅ Functional Sign Out (auto return to Home)
    connect(signOutBtn, &QPushButton::clicked, [=]() {
        contentStack->setCurrentWidget(signOutPage);
        qDebug() << "User signed out.";

        // Automatically return to home view after 2 seconds
        QTimer::singleShot(2000, [=]() {
            contentStack->setCurrentIndex(0);  // 0 = Home
            });
        });

#pragma endregion


}

void BankingWindow::setCurrentView(int index) {
    if (index >= 0 && index < contentStack->count()) {
        contentStack->setCurrentIndex(index);
        currentViewIndex = index;
        
        std::cout << "Switched to view: " << index << std::endl;
        
        switch (index) {
            case 0:
                std::cout << "Showing home view" << std::endl;
                break;
            case 1:
                std::cout << "Showing transfers view" << std::endl;
                break;
            case 2:
                std::cout << "Showing bills view" << std::endl;
                break;
            case 3:
                std::cout << "Showing advice view" << std::endl;
                break;
            case 4:
                std::cout << "Showing more view" << std::endl;
                break;
            case 5:
                std::cout << "Showing profile view" << std::endl;
				break;
            default:
                break;
        }
    }
}


// -- sets the color scheme of the buttons depending on the system theme
void BankingWindow::styleNavigationButton(QPushButton* button) {
    // Detect theme automatically
    QPalette palette = this->palette();
    QColor windowColor = palette.color(QPalette::Window);
    bool isDarkMode = windowColor.lightness() < 128;
    
    if (isDarkMode) {
        button->setStyleSheet(
            "QPushButton {"
            "    border: none;"
            "    background-color: transparent;"
            "    color: #ffffff;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 255, 255, 0.1);"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 255, 255, 0.2);"
            "}"
        );
    } else {
        button->setStyleSheet(
            "QPushButton {"
            "    border: none;"
            "    background-color: transparent;"
            "    color: #333333;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(0, 0, 0, 0.1);"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(0, 0, 0, 0.2);"
            "}"
        );
    }
}

void BankingWindow::initializeData() {
    currentCustomer.setUser(currentUser);
    currentCustomer.addAccount(currentAccount);
    
    //welcomeLabel->setText(QString("Welcome, %1!").arg(QString::fromStdString(currentUser.name())));
    
    //updateAccountSelector();
    //updateCurrentAccountDisplay();
}

void BankingWindow::updateAccountSelector() {
    accountSelector->clear();
    auto accountsList = currentCustomer.accounts();
    for (const auto& account : accountsList) {
        QString accountDisplay = QString("%1 (%2)")
            .arg(QString::fromStdString(account.accountNumber()))
            .arg(account.accountType() == AccountType::CHEQUING ? "Chequing" : 
                 account.accountType() == AccountType::SAVINGS ? "Savings" : "Credit");
        accountSelector->addItem(accountDisplay, QString::fromStdString(account.accountNumber()));
    }
}

void BankingWindow::updateCurrentAccountDisplay() {
    currentAccountLabel->setText(QString("Account: %1").arg(QString::fromStdString(currentAccount.accountNumber())));
    balanceLabel->setText(QString("Balance: $%1").arg(currentAccount.getBalance(), 0, 'f', 2));
}

void BankingWindow::updateAccountInCustomer() {
    // This is a workaround since we can't directly update the account in the customer's list
    // In a real application, this would be handled by the database layer
    currentCustomer.removeAccount(currentAccount.accountNumber());
    currentCustomer.addAccount(currentAccount);
}

// -- setCurrentView --
// switches the main view based on the index (for navbar)
void setCurrentView(int index) {
    switch (index) {
        case 0:
            // Show home view
			break;
        case 1:
		    // Show transfers view
			break;
        case 2:
            // Show bills view
            break;
        case 3:
            // Show advice view
            break;
        case 4:
            // Show more view
            break;
        default:
			break;
    }
}

