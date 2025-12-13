//
// Created by Kirill on 12.12.2025.
//

#include "../../include/transaction/Transaction.h"
#include <iostream>
#include <iomanip>

int Transaction::transactionCounter = 0;

Transaction::Transaction()
        : id(++transactionCounter), amount(0.0), transactionDate(Date()),
          operationType("unknown") {}

Transaction::Transaction(const std::string& from, const std::string& to,
                         double amt, const std::string& type)
        : id(++transactionCounter), fromAccount(from), toAccount(to),
          amount(amt), transactionDate(Date()), operationType(type) {}

void Transaction::displayInfo() const {
    std::cout << "\n=== Transaction Information ===" << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "Operation Type: " << operationType << std::endl;

    if (operationType == "deposit") {
        std::cout << "To Account: " << toAccount << std::endl;
    } else if (operationType == "withdrawal") {
        std::cout << "From Account: " << fromAccount << std::endl;
    } else if (operationType == "transfer") {
        std::cout << "From Account: " << fromAccount << std::endl;
        std::cout << "To Account: " << toAccount << std::endl;
    }

    std::cout << "Amount: " << std::fixed << std::setprecision(2) << amount << std::endl;
    std::cout << "Date & Time: ";
    transactionDate.displayFull();
    std::cout << std::endl;
}