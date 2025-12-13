//
// Created by Kirill on 12.12.2025.
//

#pragma once
#include "../util/Date.h"
#include "../exception/BankException.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <limits>

class Account {
private:
    static int accountCounter;
    std::string accountNumber;
    int clientId;
    double balance;
    std::string accountType;
    Date openingDate;
    bool isActive;

    static const double MIN_BALANCE;
    static const double MAX_BALANCE;
    static const double MAX_TRANSACTION;

public:
    Account();
    Account(int ownerId, double initialBalance,
                const std::string& type, const Date& openDate);
    ~Account();

    std::string getAccountNumber() const { return accountNumber; }
    int getClientId() const { return clientId; }
    double getBalance() const { return balance; }
    std::string getAccountType() const { return accountType; }
    Date getOpeningDate() const { return openingDate; }
    bool getIsActive() const { return isActive; }

    bool deposit(double amount);
    bool withdraw(double amount);
    bool transfer(Account& target, double amount);

    void activate();
    void deactivate();
    void close();

    void displayInfo() const;

    static int getTotalAccounts() { return accountCounter; }
    static void resetCounter() { accountCounter = 0; }
    static std::string generateAccountNumber();
    static bool validateAccountNumber(const std::string& accNum);
    static bool validateAccountType(const std::string& type);
    static bool validateAmount(double amount);
    static bool canTransfer(double amount, double balance);

private:
    bool validateAmountInternal(double amount) const;
    bool validateOperation() const;
};
