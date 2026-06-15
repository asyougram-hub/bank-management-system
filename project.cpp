/*
    Banking Management System
    Made by: Avika Sharma
    Subject: Object Oriented Programming (C++)

    Features:
    - Create savings and current accounts
    - Deposit and withdraw money
    - Calculate interest for savings accounts
    - View transaction history
    - Save and load data from files
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

// ============================================================
// Transaction class - stores one deposit or withdrawal record
// ============================================================
class Transaction {
public:
    string type;    // "Deposit" or "Withdrawal"
    double amount;

    Transaction(string t, double a) {
        type = t;
        amount = a;
    }
};

// ============================================================
// Base class - Account
// ============================================================
class Account {
protected:
    int accNumber;
    string holderName;
    double balance;
    vector<Transaction> history;  // stores all transactions

public:
    // Constructor
    Account(int num, string name, double bal) {
        accNumber = num;
        holderName = name;
        balance = bal;
    }

    // Getters (encapsulation - balance is protected, accessed via function)
    int getAccNumber() { return accNumber; }
    string getName() { return holderName; }
    double getBalance() { return balance; }

    // Deposit function (same for all accounts)
    void deposit(double amount) {
        if (amount <= 0) {
            cout << "Amount must be greater than 0!" << endl;
            return;
        }
        balance += amount;
        history.push_back(Transaction("Deposit", amount));
        cout << "Deposit successful! New balance: Rs " << fixed << setprecision(2) << balance << endl;
    }

    // Withdraw - virtual so child classes can override it
    virtual void withdraw(double amount) {
        if (amount <= 0) {
            cout << "Amount must be greater than 0!" << endl;
            return;
        }
        if (amount > balance) {
            cout << "Insufficient balance!" << endl;
            return;
        }
        balance -= amount;
        history.push_back(Transaction("Withdrawal", amount));
        cout << "Withdrawal successful! New balance: Rs " << fixed << setprecision(2) << balance << endl;
    }

    // Calculate interest - virtual, child classes will override
    virtual void calculateInterest() {
        cout << "No interest for this account type." << endl;
    }

    // Show transaction history
    void showHistory() {
        if (history.empty()) {
            cout << "No transactions yet." << endl;
            return;
        }
        cout << "\n--- Transaction History for Account " << accNumber << " ---" << endl;
        for (int i = 0; i < history.size(); i++) {
            cout << i + 1 << ". " << history[i].type << " - Rs " << fixed << setprecision(2) << history[i].amount << endl;
        }
    }

    // Display account info - virtual
    virtual void display() {
        cout << "\nAccount Number : " << accNumber << endl;
        cout << "Holder Name    : " << holderName << endl;
        cout << "Balance        : Rs " << fixed << setprecision(2) << balance << endl;
    }

    // Virtual destructor (good practice with inheritance)
    virtual ~Account() {}
};

// ============================================================
// Child class 1 - SavingsAccount (inherits from Account)
// ============================================================
class SavingsAccount : public Account {
private:
    double interestRate;    // 0.04 means 4%
    double minimumBalance;

public:
    SavingsAccount(int num, string name, double bal) : Account(num, name, bal) {
        interestRate = 0.04;       // 4% annual interest rate
        minimumBalance = 500.0;    // minimum balance required
    }

    // Override withdraw - savings accounts must keep minimum balance
    void withdraw(double amount) override {
        if (amount <= 0) {
            cout << "Amount must be greater than 0!" << endl;
            return;
        }
        if (balance - amount < minimumBalance) {
            cout << "Cannot withdraw! Minimum balance of Rs " << fixed << setprecision(2) << minimumBalance << " must be maintained." << endl;
            cout << "Maximum you can withdraw: Rs " << fixed << setprecision(2) << (balance - minimumBalance) << endl;
            return;
        }
        balance -= amount;
        history.push_back(Transaction("Withdrawal", amount));
        cout << "Withdrawal successful! New balance: Rs " << fixed << setprecision(2) << balance << endl;
    }

    // Override calculateInterest - savings account earns interest
    void calculateInterest() override {
        double interest = balance * interestRate;
        balance += interest;
        history.push_back(Transaction("Interest Added", interest));
        cout << "Interest calculated at 4% per year." << endl;
        cout << "Interest amount: Rs " << fixed << setprecision(2) << interest << endl;
        cout << "New balance after interest: Rs " << fixed << setprecision(2) << balance << endl;
    }

    // Override display - show savings specific info
    void display() override {
        cout << "\nAccount Number  : " << accNumber << endl;
        cout << "Account Type    : Savings Account" << endl;
        cout << "Holder Name     : " << holderName << endl;
        cout << "Balance         : Rs " << fixed << setprecision(2) << balance << endl;
        cout << "Interest Rate   : 4% per year" << endl;
        cout << "Minimum Balance : Rs " << minimumBalance << endl;
    }
};

// ============================================================
// Child class 2 - CurrentAccount (inherits from Account)
// ============================================================
class CurrentAccount : public Account {
private:
    double overdraftLimit;  // allowed to go below 0 up to this limit

public:
    CurrentAccount(int num, string name, double bal) : Account(num, name, bal) {
        overdraftLimit = 10000.0;  // can overdraft up to Rs 10000
    }

    // Override withdraw - current accounts allow overdraft
    void withdraw(double amount) override {
        if (amount <= 0) {
            cout << "Amount must be greater than 0!" << endl;
            return;
        }
        if (balance - amount < -overdraftLimit) {
            cout << "Cannot withdraw! Overdraft limit of Rs " << fixed << setprecision(2) << overdraftLimit << " exceeded." << endl;
            cout << "Maximum you can withdraw: Rs " << fixed << setprecision(2) << (balance + overdraftLimit) << endl;
            return;
        }
        balance -= amount;
        history.push_back(Transaction("Withdrawal", amount));
        cout << "Withdrawal successful! New balance: Rs " << fixed << setprecision(2) << balance << endl;

        if (balance < 0) {
            cout << "Warning: Your account is in overdraft!" << endl;
        }
    }

    // Current accounts do not earn interest
    void calculateInterest() override {
        cout << "Current accounts do not earn interest." << endl;
    }

    // Override display
    void display() override {
        cout << "\nAccount Number  : " << accNumber << endl;
        cout << "Account Type    : Current Account" << endl;
        cout << "Holder Name     : " << holderName << endl;
        cout << "Balance         : Rs " << fixed << setprecision(2) << balance << endl;
        cout << "Overdraft Limit : Rs " << overdraftLimit << endl;
    }
};

// ============================================================
// Global variables
// ============================================================
vector<Account*> accounts;   // stores all accounts
int nextAccountNumber = 101; // starting account number

// ============================================================
// Helper function - find account by account number
// ============================================================
Account* findAccount(int accNum) {
    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i]->getAccNumber() == accNum) {
            return accounts[i];
        }
    }
    return NULL;  // not found
}

// ============================================================
// File handling - save all accounts to file
// ============================================================
void saveAllToFile() {
    ofstream file("accounts.txt");
    for (int i = 0; i < accounts.size(); i++) {
        // Check account type using dynamic_cast
        SavingsAccount* sa = dynamic_cast<SavingsAccount*>(accounts[i]);
        CurrentAccount* ca = dynamic_cast<CurrentAccount*>(accounts[i]);

        if (sa != NULL) {
            file << "SAVINGS " << accounts[i]->getAccNumber() << " "
                 << accounts[i]->getName() << " "
                 << fixed << setprecision(2) << accounts[i]->getBalance() << endl;
        } else if (ca != NULL) {
            file << "CURRENT " << accounts[i]->getAccNumber() << " "
                 << accounts[i]->getName() << " "
                 << fixed << setprecision(2) << accounts[i]->getBalance() << endl;
        }
    }
    file.close();
    cout << "Data saved to file successfully." << endl;
}

// ============================================================
// File handling - load accounts from file
// ============================================================
void loadFromFile() {
    ifstream file("accounts.txt");
    if (!file) {
        return;  // file doesn't exist yet, that's okay on first run
    }

    string type, name;
    int num;
    double bal;

    while (file >> type >> num >> name >> bal) {
        if (type == "SAVINGS") {
            SavingsAccount* acc = new SavingsAccount(num, name, bal);
            accounts.push_back(acc);
        } else if (type == "CURRENT") {
            CurrentAccount* acc = new CurrentAccount(num, name, bal);
            accounts.push_back(acc);
        }
        // Update next account number so we don't reuse old ones
        if (num >= nextAccountNumber) {
            nextAccountNumber = num + 1;
        }
    }
    file.close();
    cout << "Previous data loaded from file." << endl;
}

// ============================================================
// Menu option functions
// ============================================================
void createAccount() {
    int choice;
    string name;
    double initialDeposit;

    cout << "\n--- Create New Account ---" << endl;
    cout << "1. Savings Account" << endl;
    cout << "2. Current Account" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    cout << "Enter account holder name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter initial deposit amount: Rs ";
    cin >> initialDeposit;

    if (initialDeposit < 0) {
        cout << "Initial deposit cannot be negative!" << endl;
        return;
    }

    if (choice == 1) {
        SavingsAccount* acc = new SavingsAccount(nextAccountNumber, name, initialDeposit);
        accounts.push_back(acc);
        cout << "\nSavings Account created successfully!" << endl;
        cout << "Your Account Number is: " << nextAccountNumber << endl;
        nextAccountNumber++;
    } else if (choice == 2) {
        CurrentAccount* acc = new CurrentAccount(nextAccountNumber, name, initialDeposit);
        accounts.push_back(acc);
        cout << "\nCurrent Account created successfully!" << endl;
        cout << "Your Account Number is: " << nextAccountNumber << endl;
        nextAccountNumber++;
    } else {
        cout << "Invalid choice!" << endl;
    }
}

void depositMoney() {
    int accNum;
    double amount;

    cout << "\n--- Deposit Money ---" << endl;
    cout << "Enter account number: ";
    cin >> accNum;

    Account* acc = findAccount(accNum);
    if (acc == NULL) {
        cout << "Account not found!" << endl;
        return;
    }

    cout << "Enter deposit amount: Rs ";
    cin >> amount;

    acc->deposit(amount);
}

void withdrawMoney() {
    int accNum;
    double amount;

    cout << "\n--- Withdraw Money ---" << endl;
    cout << "Enter account number: ";
    cin >> accNum;

    Account* acc = findAccount(accNum);
    if (acc == NULL) {
        cout << "Account not found!" << endl;
        return;
    }

    cout << "Enter withdrawal amount: Rs ";
    cin >> amount;

    acc->withdraw(amount);  // polymorphism - calls the correct version
}

void checkBalance() {
    int accNum;

    cout << "\n--- Check Balance ---" << endl;
    cout << "Enter account number: ";
    cin >> accNum;

    Account* acc = findAccount(accNum);
    if (acc == NULL) {
        cout << "Account not found!" << endl;
        return;
    }

    acc->display();  // polymorphism - calls correct display
}

void applyInterest() {
    int accNum;

    cout << "\n--- Apply Interest ---" << endl;
    cout << "Enter account number: ";
    cin >> accNum;

    Account* acc = findAccount(accNum);
    if (acc == NULL) {
        cout << "Account not found!" << endl;
        return;
    }

    acc->calculateInterest();  // polymorphism
}

void viewTransactions() {
    int accNum;

    cout << "\n--- View Transaction History ---" << endl;
    cout << "Enter account number: ";
    cin >> accNum;

    Account* acc = findAccount(accNum);
    if (acc == NULL) {
        cout << "Account not found!" << endl;
        return;
    }

    acc->showHistory();
}

void viewAllAccounts() {
    if (accounts.empty()) {
        cout << "\nNo accounts created yet." << endl;
        return;
    }

    cout << "\n--- All Accounts ---" << endl;
    for (int i = 0; i < accounts.size(); i++) {
        accounts[i]->display();
        cout << "-------------------" << endl;
    }
}

// ============================================================
// Main function
// ============================================================
int main() {
    loadFromFile();  // load existing data when program starts

    int choice;

    cout << "=====================================" << endl;
    cout << "    BANKING MANAGEMENT SYSTEM        " << endl;
    cout << "=====================================" << endl;

    do {
        cout << "\n========== MAIN MENU ==========" << endl;
        cout << "1. Create New Account" << endl;
        cout << "2. Deposit Money" << endl;
        cout << "3. Withdraw Money" << endl;
        cout << "4. Check Balance / Account Info" << endl;
        cout << "5. Apply Interest (Savings Only)" << endl;
        cout << "6. View Transaction History" << endl;
        cout << "7. View All Accounts" << endl;
        cout << "8. Save Data to File" << endl;
        cout << "0. Exit" << endl;
        cout << "================================" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                depositMoney();
                break;
            case 3:
                withdrawMoney();
                break;
            case 4:
                checkBalance();
                break;
            case 5:
                applyInterest();
                break;
            case 6:
                viewTransactions();
                break;
            case 7:
                viewAllAccounts();
                break;
            case 8:
                saveAllToFile();
                break;
            case 0:
                saveAllToFile();  // auto-save before exit
                cout << "\nThank you for using the Banking System. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }

    } while (choice != 0);

    // Free allocated memory
    for (int i = 0; i < accounts.size(); i++) {
        delete accounts[i];
    }
    accounts.clear();

    return 0;
}