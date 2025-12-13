//
// Created by Kirill on 12.12.2025.
//

#pragma once
#include "../util/Date.h"
#include <string>

class Transaction {
private:
    static int transactionCounter;
    int id;
    std::string fromAccount;
    std::string toAccount;
    double amount;
    Date transactionDate;
    std::string operationType;

public:
    Transaction();
    Transaction(const std::string& from, const std::string& to,
                double amt, const std::string& type);

    int getId() const { return id; }
    std::string getFromAccount() const { return fromAccount; }
    std::string getToAccount() const { return toAccount; }
    double getAmount() const { return amount; }
    Date getTransactionDate() const { return transactionDate; }
    std::string getOperationType() const { return operationType; }

    void displayInfo() const;

    static int getTotalTransactions() { return transactionCounter; }
    static void resetCounter() { transactionCounter = 0; }
};
