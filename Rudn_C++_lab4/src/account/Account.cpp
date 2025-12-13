//
// Created by Kirill on 12.12.2025.
//

#include "../../include/account/Account.h"
#include "../../include/client/Client.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <regex>
#include <cmath>

int Account::accountCounter = 0;

const double Account::MIN_BALANCE = 0.0;
const double Account::MAX_BALANCE = 1000000000.0;
const double Account::MAX_TRANSACTION = 1000000.0;

Account::Account()
        : accountNumber(generateAccountNumber()), clientId(0), balance(0.0),
          accountType("Расчетный"), openingDate(Date()), isActive(true) {
    accountCounter++;
}

Account::Account(int ownerId, double initialBalance,
                         const std::string& type, const Date& openDate)
        : accountNumber(generateAccountNumber()), clientId(ownerId),
          balance(initialBalance), accountType(type),
          openingDate(openDate), isActive(true) {

    if (!Client::validateClientId(ownerId)) {
        throw BankException::invalidAccountData("Invalid client ID");
    }
    if (!validateAmount(initialBalance)) {
        throw BankException::invalidAccountData("Invalid initial balance");
    }
    if (!validateAccountType(type)) {
        throw BankException::invalidAccountData("Invalid account type");
    }
    if (!openDate.isValid()) {
        throw BankException::invalidAccountData("Invalid opening date");
    }

    accountCounter++;
}

Account::~Account() {
    accountCounter--;
}

bool Account::deposit(double amount) {
    if (!validateOperation()) {
        throw BankException::accountNotActive(accountNumber);
    }
    if (!validateAmountInternal(amount)) {
        throw BankException::invalidAmount(amount);
    }
    if (balance + amount > MAX_BALANCE) {
        throw BankException::transactionLimitExceeded(amount, MAX_BALANCE - balance);
    }

    balance += amount;
    return true;
}

bool Account::withdraw(double amount) {
    if (!validateOperation()) {
        throw BankException::accountNotActive(accountNumber);
    }
    if (!validateAmountInternal(amount)) {
        throw BankException::invalidAmount(amount);
    }
    if (amount > balance) {
        throw BankException::insufficientFunds(accountNumber, balance, amount);
    }
    if (balance - amount < MIN_BALANCE) {
        throw BankException::invalidTransaction("Withdrawal would result in balance below minimum");
    }

    balance -= amount;
    return true;
}

bool Account::transfer(Account& target, double amount) {
    if (!validateOperation()) {
        throw BankException::accountNotActive(accountNumber);
    }
    if (!target.validateOperation()) {
        throw BankException::accountNotActive(target.getAccountNumber());
    }

    if (accountNumber == target.getAccountNumber()) {
        throw BankException::sameAccountTransfer(accountNumber);
    }

    if (!validateAmountInternal(amount)) {
        throw BankException::invalidAmount(amount);
    }
    if (!canTransfer(amount, balance)) {
        throw BankException::insufficientFunds(accountNumber, balance, amount);
    }

    if (withdraw(amount)) {
        target.deposit(amount);
        return true;
    }
    return false;
}

void Account::activate() {
    isActive = true;
}

void Account::deactivate() {
    isActive = false;
}

void Account::close() {
    if (!isActive) {
        throw BankException::accountAlreadyClosed(accountNumber);
    }
    if (balance != 0.0) {
        throw BankException::invalidTransaction("Cannot close account with non-zero balance");
    }
    isActive = false;
}

void Account::displayInfo() const {
    std::cout << "\n=== Информация о счете ===" << std::endl;
    std::cout << "Номер счета: " << accountNumber << std::endl;
    std::cout << "Владелец (ID): " << clientId << std::endl;
    std::cout << "Тип счета: " << accountType << std::endl;
    std::cout << "Баланс: " << std::fixed << std::setprecision(2) << balance << " руб." << std::endl;
    std::cout << "Дата открытия: ";
    openingDate.display();
    std::cout << std::endl;
    std::cout << "Статус: " << (isActive ? "Активен ✓" : "Закрыт ✗") << std::endl;
}

std::string Account::generateAccountNumber() {
    std::stringstream ss;
    ss << "ACC" << std::setw(8) << std::setfill('0') << (accountCounter + 1);
    return ss.str();
}

bool Account::validateAccountNumber(const std::string& accNum) {
    std::regex pattern(R"(^ACC\d{8}$)");
    return std::regex_match(accNum, pattern);
}

bool Account::validateAccountType(const std::string& type) {
    return type == "Checking" || type == "Savings" ||
           type == "Расчетный" || type == "Сберегательный";
}

bool Account::validateAmount(double amount) {
    if (amount < 0) return false;
    if (amount > MAX_TRANSACTION) return false;
    if (std::isnan(amount) || std::isinf(amount)) return false;

    // Проверка на точность (не более 2 знаков после запятой)
    double rounded = std::round(amount * 100) / 100;
    return std::abs(amount - rounded) < 0.001;
}

bool Account::canTransfer(double amount, double balance) {
    return validateAmount(amount) &&
           amount <= balance &&
           (balance - amount) >= MIN_BALANCE;
}

bool Account::validateAmountInternal(double amount) const {
    return validateAmount(amount);
}

bool Account::validateOperation() const {
    return isActive;
}