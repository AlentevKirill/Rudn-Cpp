//
// Created by Kirill on 13.12.2025.
//

#include "../../include/exception/BankException.h"
#include <sstream>
#include <iomanip>

BankException::BankException(const std::string& msg)
        : message(msg), errorCode("BANK-000") {}

BankException::BankException(const std::string& msg, const std::string& code)
        : message(msg), errorCode(code) {}

const char* BankException::what() const noexcept {
    return message.c_str();
}

std::string BankException::getErrorCode() const {
    return errorCode;
}

std::string BankException::formatErrorCode(ErrorType type, int subCode) {
    std::stringstream ss;
    ss << "BANK-" << std::setw(3) << std::setfill('0') << type;

    if (subCode > 0) {
        ss << "-" << std::setw(3) << std::setfill('0') << subCode;
    }

    return ss.str();
}

// Клиентские ошибки
BankException BankException::invalidClientData(const std::string& details) {
    std::string msg = "Invalid client data";
    if (!details.empty()) {
        msg += ": " + details;
    }
    return BankException(msg, formatErrorCode(ErrorType::VALIDATION_ERROR, 101));
}

BankException BankException::clientNotFound(int clientId) {
    std::stringstream ss;
    ss << "Client with ID " << clientId << " not found";
    return BankException(ss.str(), formatErrorCode(ErrorType::CLIENT_ERROR, 201));
}

BankException BankException::clientHasActiveAccounts(int clientId) {
    std::stringstream ss;
    ss << "Cannot remove client " << clientId << " - client has active accounts";
    return BankException(ss.str(), formatErrorCode(ErrorType::CLIENT_ERROR, 202));
}

// Ошибки счетов
BankException BankException::invalidAccountData(const std::string& details) {
    std::string msg = "Invalid account data";
    if (!details.empty()) {
        msg += ": " + details;
    }
    return BankException(msg, formatErrorCode(ErrorType::VALIDATION_ERROR, 102));
}

BankException BankException::accountNotFound(const std::string& accountNumber) {
    std::stringstream ss;
    ss << "Account " << accountNumber << " not found";
    return BankException(ss.str(), formatErrorCode(ErrorType::ACCOUNT_ERROR, 301));
}

BankException BankException::accountAlreadyClosed(const std::string& accountNumber) {
    std::stringstream ss;
    ss << "Account " << accountNumber << " is already closed";
    return BankException(ss.str(), formatErrorCode(ErrorType::ACCOUNT_ERROR, 302));
}

BankException BankException::insufficientFunds(const std::string& accountNumber, double balance, double amount) {
    std::stringstream ss;
    ss << "Insufficient funds in account " << accountNumber
       << ". Balance: " << std::fixed << std::setprecision(2) << balance
       << ", Attempted: " << amount;
    return BankException(ss.str(), formatErrorCode(ErrorType::ACCOUNT_ERROR, 303));
}

BankException BankException::accountNotActive(const std::string& accountNumber) {
    std::stringstream ss;
    ss << "Account " << accountNumber << " is not active";
    return BankException(ss.str(), formatErrorCode(ErrorType::ACCOUNT_ERROR, 304));
}

// Ошибки транзакций
BankException BankException::invalidTransaction(const std::string& details) {
    std::string msg = "Invalid transaction";
    if (!details.empty()) {
        msg += ": " + details;
    }
    return BankException(msg, formatErrorCode(ErrorType::VALIDATION_ERROR, 103));
}

BankException BankException::transactionLimitExceeded(double amount, double limit) {
    std::stringstream ss;
    ss << "Transaction amount " << std::fixed << std::setprecision(2) << amount
       << " exceeds limit of " << limit;
    return BankException(ss.str(), formatErrorCode(ErrorType::TRANSACTION_ERROR, 401));
}

BankException BankException::sameAccountTransfer(const std::string& accountNumber) {
    std::stringstream ss;
    ss << "Cannot transfer to the same account: " << accountNumber;
    return BankException(ss.str(), formatErrorCode(ErrorType::TRANSACTION_ERROR, 402));
}

BankException BankException::invalidAmount(double amount) {
    std::stringstream ss;
    ss << "Invalid amount: " << std::fixed << std::setprecision(2) << amount
       << ". Amount must be positive";
    return BankException(ss.str(), formatErrorCode(ErrorType::TRANSACTION_ERROR, 403));
}

// Системные ошибки
BankException BankException::systemError(const std::string& details) {
    std::string msg = "System error";
    if (!details.empty()) {
        msg += ": " + details;
    }
    return BankException(msg, formatErrorCode(ErrorType::SYSTEM_ERROR, 601));
}

BankException BankException::fileOperationError(const std::string& filename) {
    std::stringstream ss;
    ss << "File operation error for file: " << filename;
    return BankException(ss.str(), formatErrorCode(ErrorType::SYSTEM_ERROR, 602));
}