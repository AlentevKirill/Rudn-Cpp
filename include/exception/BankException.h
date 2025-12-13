//
// Created by Kirill on 13.12.2025.
//

#pragma once
#include <exception>
#include <string>

class BankException : public std::exception {
private:
    std::string message;
    std::string errorCode;

public:
    // Конструкторы
    BankException() = default;
    explicit BankException(const std::string& msg);
    BankException(const std::string& msg, const std::string& code);

    // Получение сообщения об ошибке
    const char* what() const noexcept override;

    // Получение кода ошибки
    std::string getErrorCode() const;

    // Типы ошибок
    enum ErrorType {
        VALIDATION_ERROR = 100,
        CLIENT_ERROR = 200,
        ACCOUNT_ERROR = 300,
        TRANSACTION_ERROR = 400,
        SECURITY_ERROR = 500,
        SYSTEM_ERROR = 600
    };

    // Статические методы для создания стандартных исключений
    static BankException invalidClientData(const std::string& details = "");
    static BankException clientNotFound(int clientId);
    static BankException clientHasActiveAccounts(int clientId);

    static BankException invalidAccountData(const std::string& details = "");
    static BankException accountNotFound(const std::string& accountNumber);
    static BankException accountAlreadyClosed(const std::string& accountNumber);
    static BankException insufficientFunds(const std::string& accountNumber, double balance, double amount);
    static BankException accountNotActive(const std::string& accountNumber);

    static BankException invalidTransaction(const std::string& details = "");
    static BankException transactionLimitExceeded(double amount, double limit);
    static BankException sameAccountTransfer(const std::string& accountNumber);
    static BankException invalidAmount(double amount);

    static BankException systemError(const std::string& details = "");
    static BankException fileOperationError(const std::string& filename);

private:
    static std::string formatErrorCode(ErrorType type, int subCode = 0);
};
