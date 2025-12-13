//
// Created by Kirill on 12.12.2025.
//

#pragma once
#include "../client/Client.h"
#include "../client/VipClient.h"
#include "../account/Account.h"
#include "../transaction/Transaction.h"
#include "../exception/BankException.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

class Bank {
private:
    std::vector<std::shared_ptr<Client>> clients;
    std::vector<std::shared_ptr<Account>> accounts;
    std::vector<Transaction> transactions;
    std::string bankName;

public:
    Bank(const std::string& name);

    // Управление клиентами с валидацией
    std::shared_ptr<Client> addClient(const std::string& firstName,
                                      const std::string& lastName,
                                      const Address& address,
                                      bool isPremium = false,
                                      int premiumLevel = 1,
                                      double discountRate = 0.05);

    bool removeClient(int clientId);
    std::shared_ptr<Client> findClientById(int id) const;
    std::vector<std::shared_ptr<Client>> findClientsByName(const std::string& name) const;
    void displayAllClients() const;

    // Управление счетами с валидацией
    std::shared_ptr<Account> openAccount(int clientId,
                                             double initialBalance,
                                             const std::string& accountType);

    bool closeAccount(const std::string& accountNumber);
    std::shared_ptr<Account> findAccountByNumber(const std::string& number) const;
    std::vector<std::shared_ptr<Account>> findAccountsByClientId(int clientId) const;
    void displayAllAccounts() const;

    // Банковские операции с обработкой исключений
    void deposit(const std::string& accountNumber, double amount);
    void withdraw(const std::string& accountNumber, double amount);
    void transfer(const std::string& fromAccount,
                  const std::string& toAccount,
                  double amount);

    // Транзакции
    void recordTransaction(const std::string& from,
                           const std::string& to,
                           double amount,
                           const std::string& type);
    void displayAllTransactions() const;
    void displayClientTransactions(int clientId) const;

    // Отчетность
    void generateBankReport() const;
    void generateClientReport(int clientId) const;
    void generateFinancialSummary() const;

    // Поисковые функции
    int getTotalClients() const { return clients.size(); }
    int getTotalAccounts() const { return accounts.size(); }
    int getTotalTransactions() const { return transactions.size(); }

    double getTotalBankBalance() const;
    int getActiveAccountsCount() const;

    // Валидация
    bool clientExists(int clientId) const;
    bool accountExists(const std::string& accountNumber) const;
    bool validateClientForAccount(int clientId) const;

private:
    bool clientHasActiveAccounts(int clientId) const;
    void validateClientData(const std::string& firstName,
                            const std::string& lastName,
                            const Address& address) const;
    void validateAccountData(int clientId, double initialBalance,
                             const std::string& accountType) const;
    void validateTransaction(const std::string& accountNumber, double amount,
                             const std::string& operation) const;
};
