//
// Created by Kirill on 12.12.2025.
//

#include "../../include/bank/Bank.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

Bank::Bank(const std::string& name) : bankName(name) {
    Client::resetCounter();
    Account::resetCounter();
    Transaction::resetCounter();
}

std::shared_ptr<Client> Bank::addClient(const std::string& firstName,
                                        const std::string& lastName,
                                        const Address& address,
                                        bool isPremium,
                                        int premiumLevel,
                                        double discountRate) {
    try {
        validateClientData(firstName, lastName, address);

        Date regDate = Date();
        std::shared_ptr<Client> client;

        if (isPremium) {
            if (premiumLevel < 1 || premiumLevel > 3) {
                throw BankException::invalidClientData("Premium level must be between 1 and 3");
            }
            if (discountRate < 0.0 || discountRate > 0.3) {
                throw BankException::invalidClientData("Discount rate must be between 0% and 30%");
            }

            client = std::make_shared<VipClient>(firstName, lastName, address,
                                                     regDate, premiumLevel, discountRate);
        } else {
            client = std::make_shared<Client>(firstName, lastName, address, regDate);
        }

        clients.push_back(client);
        std::cout << "Клиент успешно добавлен! ID: " << client->getId() << std::endl;
        return client;

    } catch (const BankException& e) {
        throw;
    } catch (const std::exception& e) {
        throw BankException::systemError(std::string("Failed to add client: ") + e.what());
    }
}

bool Bank::removeClient(int clientId) {
    if (!clientExists(clientId)) {
        throw BankException::clientNotFound(clientId);
    }

    if (clientHasActiveAccounts(clientId)) {
        throw BankException::clientHasActiveAccounts(clientId);
    }

    auto it = std::remove_if(clients.begin(), clients.end(),
                             [clientId](const std::shared_ptr<Client>& client) {
                                 return client->getId() == clientId;
                             });

    if (it != clients.end()) {
        clients.erase(it, clients.end());
        std::cout << "Клиент ID " << clientId << " успешно удален." << std::endl;
        return true;
    }

    return false;
}

std::shared_ptr<Client> Bank::findClientById(int id) const {
    auto it = std::find_if(clients.begin(), clients.end(),
                           [id](const std::shared_ptr<Client>& client) {
                               return client->getId() == id;
                           });

    return (it != clients.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Client>> Bank::findClientsByName(const std::string& name) const {
    if (name.empty()) {
        throw BankException::invalidClientData("Search name cannot be empty");
    }

    std::vector<std::shared_ptr<Client>> result;
    std::string searchNameLower = name;
    std::transform(searchNameLower.begin(), searchNameLower.end(), searchNameLower.begin(), ::tolower);

    std::copy_if(clients.begin(), clients.end(), std::back_inserter(result),
                 [&searchNameLower](const std::shared_ptr<Client>& client) {
                     std::string fullName = client->getFullName();
                     std::transform(fullName.begin(), fullName.end(), fullName.begin(), ::tolower);

                     return fullName.find(searchNameLower) != std::string::npos;
                 });

    return result;
}

void Bank::displayAllClients() const {
    if (clients.empty()) {
        std::cout << "\nНет зарегистрированных клиентов." << std::endl;
        return;
    }

    std::cout << "\n=== Список всех клиентов (" << clients.size() << ") ===" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << std::left << std::setw(8) << "ID"
              << std::setw(15) << "Тип"
              << std::setw(25) << "ФИО"
              << std::setw(12) << "Дата регистр."
              << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    for (const auto& client : clients) {
        std::cout << std::left << std::setw(8) << client->getId()
                  << std::setw(15) << client->getType()
                  << std::setw(25) << client->getFullName()
                  << std::setw(12) << client->getRegistrationDate().toString()
                  << std::endl;
    }
    std::cout << std::string(60, '=') << std::endl;
}

std::shared_ptr<Account> Bank::openAccount(int clientId,
                                               double initialBalance,
                                               const std::string& accountType) {
    try {
        validateAccountData(clientId, initialBalance, accountType);

        Date openDate = Date();
        auto account = std::make_shared<Account>(clientId, initialBalance,
                                                     accountType, openDate);
        accounts.push_back(account);

        std::cout << "\nСчет успешно открыт!" << std::endl;
        std::cout << "Номер счета: " << account->getAccountNumber() << std::endl;
        std::cout << "Владелец ID: " << clientId << std::endl;
        std::cout << "Начальный баланс: " << std::fixed << std::setprecision(2)
                  << initialBalance << " руб." << std::endl;

        return account;

    } catch (const BankException& e) {
        throw;
    } catch (const std::exception& e) {
        throw BankException::systemError(std::string("Failed to open account: ") + e.what());
    }
}

bool Bank::closeAccount(const std::string& accountNumber) {
    if (!Account::validateAccountNumber(accountNumber)) {
        throw BankException::invalidAccountData("Invalid account number format");
    }

    auto account = findAccountByNumber(accountNumber);
    if (!account) {
        throw BankException::accountNotFound(accountNumber);
    }

    try {
        account->close();
        std::cout << "Счет " << accountNumber << " успешно закрыт." << std::endl;
        return true;
    } catch (const BankException& e) {
        throw;
    } catch (const std::exception& e) {
        throw BankException::systemError(std::string("Failed to close account: ") + e.what());
    }
}

std::shared_ptr<Account> Bank::findAccountByNumber(const std::string& number) const {
    auto it = std::find_if(accounts.begin(), accounts.end(),
                           [&number](const std::shared_ptr<Account>& account) {
                               return account->getAccountNumber() == number;
                           });

    return (it != accounts.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Account>> Bank::findAccountsByClientId(int clientId) const {
    std::vector<std::shared_ptr<Account>> result;
    std::copy_if(accounts.begin(), accounts.end(), std::back_inserter(result),
                 [clientId](const std::shared_ptr<Account>& account) {
                     return account->getClientId() == clientId && account->getIsActive();
                 });

    return result;
}

void Bank::displayAllAccounts() const {
    if (accounts.empty()) {
        std::cout << "\nНет открытых счетов." << std::endl;
        return;
    }

    std::cout << "\n=== Список всех счетов (" << accounts.size() << ") ===" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::left << std::setw(12) << "Номер счета"
              << std::setw(10) << "Владелец"
              << std::setw(15) << "Тип"
              << std::setw(15) << "Баланс"
              << std::setw(15) << "Дата открытия"
              << std::setw(10) << "Статус"
              << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    double totalBalance = 0.0;
    for (const auto& account : accounts) {
        std::cout << std::left << std::setw(12) << account->getAccountNumber()
                  << std::setw(10) << account->getClientId()
                  << std::setw(15) << account->getAccountType()
                  << std::setw(15) << std::fixed << std::setprecision(2)
                  << account->getBalance() << " руб."
                  << std::setw(15) << account->getOpeningDate().toString()
                  << std::setw(10) << (account->getIsActive() ? "Активен" : "Закрыт")
                  << std::endl;
        totalBalance += account->getBalance();
    }

    std::cout << std::string(80, '-') << std::endl;
    std::cout << std::right << std::setw(60) << "Общий баланс: "
              << std::fixed << std::setprecision(2) << totalBalance << " руб." << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void Bank::deposit(const std::string& accountNumber, double amount) {
    validateTransaction(accountNumber, amount, "deposit");

    auto account = findAccountByNumber(accountNumber);
    if (!account) {
        throw BankException::accountNotFound(accountNumber);
    }

    try {
        if (account->deposit(amount)) {
            recordTransaction("", accountNumber, amount, "deposit");
            std::cout << "\nПополнение успешно!" << std::endl;
            std::cout << "Счет: " << accountNumber << std::endl;
            std::cout << "Сумма: " << std::fixed << std::setprecision(2)
                      << amount << " руб." << std::endl;
            std::cout << "Новый баланс: " << account->getBalance() << " руб." << std::endl;
        }
    } catch (const BankException& e) {
        throw;
    } catch (const std::exception& e) {
        throw BankException::systemError(std::string("Deposit failed: ") + e.what());
    }
}

void Bank::withdraw(const std::string& accountNumber, double amount) {
    validateTransaction(accountNumber, amount, "withdrawal");

    auto account = findAccountByNumber(accountNumber);
    if (!account) {
        throw BankException::accountNotFound(accountNumber);
    }

    try {
        if (account->withdraw(amount)) {
            recordTransaction(accountNumber, "", amount, "withdrawal");
            std::cout << "\nСнятие успешно!" << std::endl;
            std::cout << "Счет: " << accountNumber << std::endl;
            std::cout << "Сумма: " << std::fixed << std::setprecision(2)
                      << amount << " руб." << std::endl;
            std::cout << "Новый баланс: " << account->getBalance() << " руб." << std::endl;
        }
    } catch (const BankException& e) {
        throw;
    } catch (const std::exception& e) {
        throw BankException::systemError(std::string("Withdrawal failed: ") + e.what());
    }
}

void Bank::transfer(const std::string& fromAccount,
                    const std::string& toAccount,
                    double amount) {
    if (fromAccount == toAccount) {
        throw BankException::sameAccountTransfer(fromAccount);
    }

    validateTransaction(fromAccount, amount, "transfer");

    auto from = findAccountByNumber(fromAccount);
    auto to = findAccountByNumber(toAccount);

    if (!from) {
        throw BankException::accountNotFound(fromAccount);
    }
    if (!to) {
        throw BankException::accountNotFound(toAccount);
    }

    try {
        if (from->transfer(*to, amount)) {
            recordTransaction(fromAccount, toAccount, amount, "transfer");
            std::cout << "\nПеревод успешен!" << std::endl;
            std::cout << "Со счета: " << fromAccount << std::endl;
            std::cout << "На счет: " << toAccount << std::endl;
            std::cout << "Сумма: " << std::fixed << std::setprecision(2)
                      << amount << " руб." << std::endl;
            std::cout << "Баланс отправителя: " << from->getBalance() << " руб." << std::endl;
            std::cout << "Баланс получателя: " << to->getBalance() << " руб." << std::endl;
        }
    } catch (const BankException& e) {
        throw;
    } catch (const std::exception& e) {
        throw BankException::systemError(std::string("Transfer failed: ") + e.what());
    }
}

void Bank::recordTransaction(const std::string& from,
                             const std::string& to,
                             double amount,
                             const std::string& type) {
    transactions.emplace_back(from, to, amount, type);
}

void Bank::displayAllTransactions() const {
    if (transactions.empty()) {
        std::cout << "\nНет проведенных транзакций." << std::endl;
        return;
    }

    std::cout << "\n=== История всех транзакций (" << transactions.size() << ") ===" << std::endl;
    std::cout << std::string(90, '=') << std::endl;
    std::cout << std::left << std::setw(5) << "ID"
              << std::setw(15) << "Тип операции"
              << std::setw(15) << "Со счета"
              << std::setw(15) << "На счет"
              << std::setw(15) << "Сумма"
              << std::setw(25) << "Дата и время"
              << std::endl;
    std::cout << std::string(90, '-') << std::endl;

    for (const auto& transaction : transactions) {
        std::string from = transaction.getFromAccount().empty() ? "N/A" : transaction.getFromAccount();
        std::string to = transaction.getToAccount().empty() ? "N/A" : transaction.getToAccount();

        std::cout << std::left << std::setw(5) << transaction.getId()
                  << std::setw(15) << transaction.getOperationType()
                  << std::setw(15) << from
                  << std::setw(15) << to
                  << std::setw(15) << std::fixed << std::setprecision(2)
                  << transaction.getAmount() << " руб."
                  << std::setw(25);
        transaction.getTransactionDate().displayFull();
        std::cout << std::endl;
    }
    std::cout << std::string(90, '=') << std::endl;
}

void Bank::displayClientTransactions(int clientId) const {
    if (!clientExists(clientId)) {
        throw BankException::clientNotFound(clientId);
    }

    auto clientAccounts = findAccountsByClientId(clientId);
    if (clientAccounts.empty()) {
        std::cout << "\nУ клиента нет активных счетов." << std::endl;
        return;
    }

    std::vector<std::string> accountNumbers;
    for (const auto& account : clientAccounts) {
        accountNumbers.push_back(account->getAccountNumber());
    }

    std::cout << "\n=== Транзакции клиента ID: " << clientId << " ===" << std::endl;
    int count = 0;

    for (const auto& transaction : transactions) {
        std::string from = transaction.getFromAccount();
        std::string to = transaction.getToAccount();

        auto fromIt = std::find(accountNumbers.begin(), accountNumbers.end(), from);
        auto toIt = std::find(accountNumbers.begin(), accountNumbers.end(), to);

        if (fromIt != accountNumbers.end() || toIt != accountNumbers.end()) {
            transaction.displayInfo();
            count++;
        }
    }

    if (count == 0) {
        std::cout << "Транзакции не найдены." << std::endl;
    } else {
        std::cout << "\nВсего транзакций: " << count << std::endl;
    }
}

void Bank::generateBankReport() const {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "          ОТЧЕТ БАНКА: " << bankName << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    std::cout << "\n📊 СТАТИСТИКА:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    std::cout << std::left << std::setw(30) << "Общее количество клиентов:"
              << std::right << std::setw(10) << getTotalClients() << std::endl;
    std::cout << std::left << std::setw(30) << "Общее количество счетов:"
              << std::right << std::setw(10) << getTotalAccounts() << std::endl;
    std::cout << std::left << std::setw(30) << "Активных счетов:"
              << std::right << std::setw(10) << getActiveAccountsCount() << std::endl;
    std::cout << std::left << std::setw(30) << "Всего транзакций:"
              << std::right << std::setw(10) << getTotalTransactions() << std::endl;

    double totalBalance = getTotalBankBalance();
    std::cout << std::left << std::setw(30) << "Общий баланс банка:"
              << std::right << std::setw(10) << std::fixed << std::setprecision(2)
              << totalBalance << " руб." << std::endl;

    std::cout << std::string(40, '-') << std::endl;

    int checkingCount = 0, savingsCount = 0;
    double checkingBalance = 0.0, savingsBalance = 0.0;

    for (const auto& account : accounts) {
        if (account->getIsActive()) {
            if (account->getAccountType() == "Checking" || account->getAccountType() == "Расчетный") {
                checkingCount++;
                checkingBalance += account->getBalance();
            } else {
                savingsCount++;
                savingsBalance += account->getBalance();
            }
        }
    }

    std::cout << "\n💳 РАСПРЕДЕЛЕНИЕ ПО ТИПАМ СЧЕТОВ:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    std::cout << "Расчетные счета: " << checkingCount << " ("
              << std::fixed << std::setprecision(1)
              << (checkingCount * 100.0 / std::max(1, getActiveAccountsCount()))
              << "%) - " << std::setprecision(2) << checkingBalance << " руб." << std::endl;
    std::cout << "Сберегательные счета: " << savingsCount << " ("
              << std::fixed << std::setprecision(1)
              << (savingsCount * 100.0 / std::max(1, getActiveAccountsCount()))
              << "%) - " << std::setprecision(2) << savingsBalance << " руб." << std::endl;

    std::cout << std::string(60, '=') << std::endl;
}

void Bank::generateClientReport(int clientId) const {
    auto client = findClientById(clientId);
    if (!client) {
        throw BankException::clientNotFound(clientId);
    }

    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "          ОТЧЕТ ПО КЛИЕНТУ" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    client->displayInfo();

    auto clientAccounts = findAccountsByClientId(clientId);
    if (clientAccounts.empty()) {
        std::cout << "\nУ клиента нет активных счетов." << std::endl;
    } else {
        std::cout << "\n📋 СЧЕТА КЛИЕНТА:" << std::endl;
        std::cout << std::string(60, '-') << std::endl;

        double totalBalance = 0.0;
        for (const auto& account : clientAccounts) {
            std::cout << "• " << account->getAccountNumber()
                      << " (" << account->getAccountType() << "): "
                      << std::fixed << std::setprecision(2)
                      << account->getBalance() << " руб." << std::endl;
            totalBalance += account->getBalance();
        }

        std::cout << std::string(60, '-') << std::endl;
        std::cout << "💰 ОБЩИЙ БАЛАНС: " << std::fixed << std::setprecision(2)
                  << totalBalance << " руб." << std::endl;
        std::cout << "📈 СРЕДНИЙ БАЛАНС НА СЧЕТ: "
                  << (totalBalance / clientAccounts.size()) << " руб." << std::endl;
    }

    std::cout << std::string(60, '=') << std::endl;
}

void Bank::generateFinancialSummary() const {
    double totalBalance = getTotalBankBalance();
    int activeAccounts = getActiveAccountsCount();
    int totalTransactions = getTotalTransactions();

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "     ФИНАНСОВАЯ СВОДКА" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::cout << "\n📈 ПОКАЗАТЕЛИ ЭФФЕКТИВНОСТИ:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    if (activeAccounts > 0) {
        double avgBalance = totalBalance / activeAccounts;
        double avgTransactions = static_cast<double>(totalTransactions) / activeAccounts;

        std::cout << std::left << std::setw(35) << "Средний баланс на счет:"
                  << std::right << std::setw(15) << std::fixed << std::setprecision(2)
                  << avgBalance << " руб." << std::endl;

        std::cout << std::left << std::setw(35) << "Среднее кол-во транзакций на счет:"
                  << std::right << std::setw(15) << std::fixed << std::setprecision(1)
                  << avgTransactions << std::endl;

        std::cout << "\n🏆 ОЦЕНКА АКТИВНОСТИ:" << std::endl;
        std::cout << std::string(40, '-') << std::endl;

        if (avgTransactions > 10) {
            std::cout << "✅ Высокая активность клиентов" << std::endl;
        } else if (avgTransactions > 5) {
            std::cout << "⚠️  Средняя активность клиентов" << std::endl;
        } else {
            std::cout << "❌ Низкая активность клиентов" << std::endl;
        }

        if (avgBalance > 50000) {
            std::cout << "💰 Высокий средний баланс" << std::endl;
        } else if (avgBalance > 10000) {
            std::cout << "💵 Средний баланс в норме" << std::endl;
        } else {
            std::cout << "💸 Низкий средний баланс" << std::endl;
        }
    } else {
        std::cout << "Нет активных счетов для анализа." << std::endl;
    }

    std::cout << std::string(50, '=') << std::endl;
}

double Bank::getTotalBankBalance() const {
    double total = 0.0;
    for (const auto& account : accounts) {
        if (account->getIsActive()) {
            total += account->getBalance();
        }
    }
    return total;
}

int Bank::getActiveAccountsCount() const {
    return std::count_if(accounts.begin(), accounts.end(),
                         [](const std::shared_ptr<Account>& account) {
                             return account->getIsActive();
                         });
}

bool Bank::clientExists(int clientId) const {
    return findClientById(clientId) != nullptr;
}

bool Bank::accountExists(const std::string& accountNumber) const {
    return findAccountByNumber(accountNumber) != nullptr;
}

bool Bank::validateClientForAccount(int clientId) const {
    return clientExists(clientId);
}

bool Bank::clientHasActiveAccounts(int clientId) const {
    auto clientAccounts = findAccountsByClientId(clientId);
    return !clientAccounts.empty();
}

void Bank::validateClientData(const std::string& firstName,
                              const std::string& lastName,
                              const Address& address) const {
    if (firstName.empty() || lastName.empty()) {
        throw BankException::invalidClientData("First and last names cannot be empty");
    }
    if (firstName.length() < 2 || lastName.length() < 2) {
        throw BankException::invalidClientData("Names must be at least 2 characters long");
    }
}

void Bank::validateAccountData(int clientId, double initialBalance,
                               const std::string& accountType) const {
    if (!clientExists(clientId)) {
        throw BankException::clientNotFound(clientId);
    }
    if (!Account::validateAmount(initialBalance)) {
        throw BankException::invalidAccountData("Invalid initial balance amount");
    }
    if (!Account::validateAccountType(accountType)) {
        throw BankException::invalidAccountData("Invalid account type. Use 'Checking' or 'Savings'");
    }
}

void Bank::validateTransaction(const std::string& accountNumber, double amount,
                               const std::string& operation) const {
    if (!Account::validateAccountNumber(accountNumber)) {
        throw BankException::invalidAccountData("Invalid account number format");
    }
    if (!accountExists(accountNumber)) {
        throw BankException::accountNotFound(accountNumber);
    }
    if (!Account::validateAmount(amount)) {
        throw BankException::invalidAmount(amount);
    }
    if (amount <= 0) {
        throw BankException::invalidTransaction("Transaction amount must be positive");
    }
}