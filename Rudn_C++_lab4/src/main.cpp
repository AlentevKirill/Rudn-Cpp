//
// Created by Kirill on 12.12.2025.
//

#include "../include/bank/Bank.h"
#include "../include/exception/BankException.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <cctype>
#include <climits>
#include <cfloat>

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

// Функция для очистки экрана
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Функция для паузы
void pauseScreen() {
    std::cout << "\n\033[1;36mНажмите Enter для продолжения...\033[0m";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// Функция для вывода успешного сообщения
void printSuccess(const std::string& message) {
    std::cout << "\n\033[1;32m✓ " << message << "\033[0m" << std::endl;
}

// Функция для вывода сообщения об ошибке
void printError(const std::string& message) {
    std::cout << "\n\033[1;31m✗ ОШИБКА: " << message << "\033[0m" << std::endl;
}

// Функция для вывода предупреждения
void printWarning(const std::string& message) {
    std::cout << "\n\033[1;33m⚠ " << message << "\033[0m" << std::endl;
}

// Функция для вывода заголовка
void printHeader(const std::string& title) {
    std::cout << "\n\033[1;34m" << std::string(50, '=') << "\033[0m" << std::endl;
    std::cout << "\033[1;34m" << title << "\033[0m" << std::endl;
    std::cout << "\033[1;34m" << std::string(50, '=') << "\033[0m" << std::endl;
}

// ==================== ФУНКЦИИ ВАЛИДАЦИИ ВВОДА ====================

// Проверка, содержит ли строка только буквы и пробелы
bool isValidName(const std::string& name) {
    if (name.empty() || name.length() < 2) return false;

    for (char c : name) {
        if (!std::isalpha(c) && c != ' ' && c != '-' && c != '\'') {
            return false;
        }
    }
    return true;
}

// Проверка, является ли строка числом
bool isNumber(const std::string& str) {
    if (str.empty()) return false;

    bool hasDecimal = false;
    for (size_t i = 0; i < str.length(); i++) {
        if (i == 0 && str[i] == '-') continue; // Отрицательные числа
        if (str[i] == '.' && !hasDecimal) {
            hasDecimal = true;
            continue;
        }
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

// Проверка почтового индекса (6 цифр)
bool isValidPostalCode(const std::string& postal) {
    if (postal.length() != 6) return false;

    for (char c : postal) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

// ==================== ФУНКЦИИ ВВОДА С ВАЛИДАЦИЕЙ ====================

// Безопасный ввод строки с валидацией
std::string getStringInput(const std::string& prompt, bool (*validator)(const std::string&) = nullptr) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);

        // Удаляем начальные и конечные пробелы
        value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
        value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

        if (value.empty()) {
            printError("Поле не может быть пустым. Попробуйте снова.");
            continue;
        }

        if (validator && !validator(value)) {
            printError("Неверный формат. Попробуйте снова.");
            continue;
        }

        return value;
    }
}

// Безопасный ввод целого числа с диапазоном
int getIntInput(const std::string& prompt, int min = INT_MIN, int max = INT_MAX) {
    std::string input;
    int value;

    while (true) {
        std::cout << prompt << " [" << min << "-" << max << "]: ";
        std::getline(std::cin, input);

        // Проверяем, является ли ввод числом
        if (!isNumber(input) || input.find('.') != std::string::npos) {
            printError("Пожалуйста, введите целое число.");
            continue;
        }

        try {
            value = std::stoi(input);
        } catch (...) {
            printError("Некорректное число. Попробуйте снова.");
            continue;
        }

        if (value < min || value > max) {
            printError("Число должно быть в диапазоне от " + std::to_string(min) +
                       " до " + std::to_string(max));
            continue;
        }

        return value;
    }
}

// Безопасный ввод числа с плавающей точкой
double getDoubleInput(const std::string& prompt, double min = 0.0, double max = DBL_MAX) {
    std::string input;
    double value;

    while (true) {
        std::cout << prompt;
        if (min != DBL_MIN && max != DBL_MAX) {
            std::cout << " [" << min << "-" << max << "]";
        }
        std::cout << ": ";

        std::getline(std::cin, input);

        // Проверяем, является ли ввод числом
        if (!isNumber(input)) {
            printError("Пожалуйста, введите число.");
            continue;
        }

        try {
            value = std::stod(input);
        } catch (...) {
            printError("Некорректное число. Попробуйте снова.");
            continue;
        }

        if (value < min) {
            printError("Значение должно быть не меньше " + std::to_string(min));
            continue;
        }

        if (value > max) {
            printError("Значение должно быть не больше " + std::to_string(max));
            continue;
        }

        return value;
    }
}

// Ввод адреса с валидацией
Address getAddressInput() {
    printHeader("ВВОД АДРЕСА");

    std::string street = getStringInput("Улица и дом: ");
    std::string city = getStringInput("Город: ", isValidName);

    std::string postal = getStringInput("Почтовый индекс (6 цифр): ");
    while (!isValidPostalCode(postal)) {
        printError("Почтовый индекс должен содержать ровно 6 цифр.");
        postal = getStringInput("Почтовый индекс (6 цифр): ");
    }

    std::string country = getStringInput("Страна: ", isValidName);

    return Address(street, city, postal, country);
}

// ==================== ФУНКЦИИ МЕНЮ ====================

// Основное меню
void showMainMenu() {
    clearScreen();
    std::cout << "\033[1;35m" << std::string(50, '=') << "\033[0m" << std::endl;
    std::cout << "\033[1;35m          БАНКОВСКАЯ СИСТЕМА          \033[0m" << std::endl;
    std::cout << "\033[1;35m" << std::string(50, '=') << "\033[0m" << std::endl;
    std::cout << "\033[1;36m1. Управление клиентами\033[0m" << std::endl;
    std::cout << "\033[1;36m2. Управление счетами\033[0m" << std::endl;
    std::cout << "\033[1;36m3. Банковские операции\033[0m" << std::endl;
    std::cout << "\033[1;36m4. Отчетность\033[0m" << std::endl;
    std::cout << "\033[1;36m5. Поиск информации\033[0m" << std::endl;
    std::cout << "\033[1;31m0. Выход\033[0m" << std::endl;
    std::cout << "\033[1;35m" << std::string(50, '=') << "\033[0m" << std::endl;
}

// Меню управления клиентами
void showClientMenu() {
    clearScreen();
    printHeader("УПРАВЛЕНИЕ КЛИЕНТАМИ");
    std::cout << "\033[1;33m1. Добавить клиента\033[0m" << std::endl;
    std::cout << "\033[1;33m2. Удалить клиента\033[0m" << std::endl;
    std::cout << "\033[1;33m3. Найти клиента по ID\033[0m" << std::endl;
    std::cout << "\033[1;33m4. Найти клиента по имени\033[0m" << std::endl;
    std::cout << "\033[1;33m5. Показать всех клиентов\033[0m" << std::endl;
    std::cout << "\033[1;31m0. Назад\033[0m" << std::endl;
}

// Меню управления счетами
void showAccountMenu() {
    clearScreen();
    printHeader("УПРАВЛЕНИЕ СЧЕТАМИ");
    std::cout << "\033[1;33m1. Открыть счет\033[0m" << std::endl;
    std::cout << "\033[1;33m2. Закрыть счет\033[0m" << std::endl;
    std::cout << "\033[1;33m3. Найти счет по номеру\033[0m" << std::endl;
    std::cout << "\033[1;33m4. Показать счета клиента\033[0m" << std::endl;
    std::cout << "\033[1;33m5. Показать все счета\033[0m" << std::endl;
    std::cout << "\033[1;31m0. Назад\033[0m" << std::endl;
}

// Меню банковских операций
void showTransactionMenu() {
    clearScreen();
    printHeader("БАНКОВСКИЕ ОПЕРАЦИИ");
    std::cout << "\033[1;33m1. Пополнить счет\033[0m" << std::endl;
    std::cout << "\033[1;33m2. Снять деньги\033[0m" << std::endl;
    std::cout << "\033[1;33m3. Перевести деньги\033[0m" << std::endl;
    std::cout << "\033[1;33m4. Показать историю операций\033[0m" << std::endl;
    std::cout << "\033[1;31m0. Назад\033[0m" << std::endl;
}

// ==================== ОБРАБОТЧИКИ МЕНЮ ====================

// Обработка меню клиентов
void handleClientMenu(Bank& bank) {
    bool back = false;

    while (!back) {
        showClientMenu();
        int choice = getIntInput("Выберите действие", 0, 5);

        switch (choice) {
            case 0:
                back = true;
                break;

            case 1: {
                // Добавление клиента
                clearScreen();
                printHeader("ДОБАВЛЕНИЕ КЛИЕНТА");

                std::string firstName = getStringInput("Имя: ", isValidName);
                std::string lastName = getStringInput("Фамилия: ", isValidName);
                Address address = getAddressInput();

                std::cout << "\n\033[1;36mВыберите тип клиента:\033[0m" << std::endl;
                std::cout << "1. Обычный клиент" << std::endl;
                std::cout << "2. Премиум клиент" << std::endl;

                int typeChoice = getIntInput("Ваш выбор", 1, 2);

                try {
                    if (typeChoice == 1) {
                        auto client = bank.addClient(firstName, lastName, address, false);
                        printSuccess("Обычный клиент успешно добавлен!");
                        std::cout << "ID клиента: " << client->getId() << std::endl;
                    } else {
                        int level = getIntInput("Уровень премиум (1-3)", 1, 3);
                        double discount = getDoubleInput("Процент скидки (например: 0.05 для 5%)", 0.0, 0.3);
                        auto client = bank.addClient(firstName, lastName, address, true, level, discount);
                        printSuccess("Премиум-клиент успешно добавлен!");
                        std::cout << "ID клиента: " << client->getId() << std::endl;
                        std::cout << "Уровень: " << level << std::endl;
                        std::cout << "Скидка: " << (discount * 100) << "%" << std::endl;
                    }
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 2: {
                // Удаление клиента
                clearScreen();
                printHeader("УДАЛЕНИЕ КЛИЕНТА");

                int clientId = getIntInput("Введите ID клиента", 1);

                try {
                    if (bank.removeClient(clientId)) {
                        printSuccess("Клиент успешно удален!");
                    }
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 3: {
                // Поиск клиента по ID
                clearScreen();
                printHeader("ПОИСК КЛИЕНТА ПО ID");

                int clientId = getIntInput("Введите ID клиента", 1);

                try {
                    auto client = bank.findClientById(clientId);
                    if (client) {
                        client->displayInfo();
                    } else {
                        printWarning("Клиент с ID " + std::to_string(clientId) + " не найден.");
                    }
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 4: {
                // Поиск клиента по имени
                clearScreen();
                printHeader("ПОИСК КЛИЕНТА ПО ИМЕНИ");

                std::string name = getStringInput("Введите имя или фамилию для поиска");

                try {
                    auto clients = bank.findClientsByName(name);
                    if (!clients.empty()) {
                        std::cout << "\n\033[1;32mНайдено клиентов: " << clients.size() << "\033[0m" << std::endl;
                        for (const auto& client : clients) {
                            client->displayInfo();
                        }
                    } else {
                        printWarning("Клиенты не найдены.");
                    }
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 5: {
                // Показать всех клиентов
                clearScreen();
                printHeader("СПИСОК ВСЕХ КЛИЕНТОВ");

                try {
                    bank.displayAllClients();
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }
        }
    }
}

// Обработка меню счетов
void handleAccountMenu(Bank& bank) {
    bool back = false;

    while (!back) {
        showAccountMenu();
        int choice = getIntInput("Выберите действие", 0, 5);

        switch (choice) {
            case 0:
                back = true;
                break;

            case 1: {
                // Открытие счета
                clearScreen();
                printHeader("ОТКРЫТИЕ СЧЕТА");

                int clientId = getIntInput("ID клиента", 1);
                double balance = getDoubleInput("Начальный баланс", 0.0);

                std::cout << "\n\033[1;36mВыберите тип счета:\033[0m" << std::endl;
                std::cout << "1. Расчетный счет" << std::endl;
                std::cout << "2. Сберегательный счет" << std::endl;

                int typeChoice = getIntInput("Ваш выбор", 1, 2);
                std::string type = (typeChoice == 1) ? "Расчетный" : "Сберегательный";

                try {
                    auto account = bank.openAccount(clientId, balance, type);
                    printSuccess("Счет успешно открыт!");
                    std::cout << "Номер счета: " << account->getAccountNumber() << std::endl;
                    std::cout << "Владелец ID: " << clientId << std::endl;
                    std::cout << "Баланс: " << std::fixed << std::setprecision(2)
                              << balance << " руб." << std::endl;
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 2: {
                // Закрытие счета
                clearScreen();
                printHeader("ЗАКРЫТИЕ СЧЕТА");

                std::string accNum = getStringInput("Введите номер счета для закрытия");

                try {
                    if (bank.closeAccount(accNum)) {
                        printSuccess("Счет успешно закрыт!");
                    }
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 3: {
                // Поиск счета по номеру
                clearScreen();
                printHeader("ПОИСК СЧЕТА");

                std::string accNum = getStringInput("Введите номер счета");

                try {
                    auto account = bank.findAccountByNumber(accNum);
                    if (account) {
                        account->displayInfo();
                    } else {
                        printWarning("Счет " + accNum + " не найден.");
                    }
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 4: {
                // Показать счета клиента
                clearScreen();
                printHeader("СЧЕТА КЛИЕНТА");

                int clientId = getIntInput("Введите ID клиента", 1);

                try {
                    auto accounts = bank.findAccountsByClientId(clientId);
                    if (!accounts.empty()) {
                        std::cout << "\n\033[1;32mСчета клиента ID " << clientId << ":\033[0m" << std::endl;
                        double totalBalance = 0.0;

                        for (const auto& account : accounts) {
                            std::cout << "\n\033[1;36mНомер счета:\033[0m " << account->getAccountNumber() << std::endl;
                            std::cout << "\033[1;36mТип:\033[0m " << account->getAccountType() << std::endl;
                            std::cout << "\033[1;36mБаланс:\033[0m " << std::fixed << std::setprecision(2)
                                      << account->getBalance() << " руб." << std::endl;
                            std::cout << "\033[1;36mСтатус:\033[0m "
                                      << (account->getIsActive() ? "Активен" : "Закрыт") << std::endl;
                            totalBalance += account->getBalance();
                        }

                        std::cout << "\n\033[1;32mОбщий баланс: " << totalBalance << " руб.\033[0m" << std::endl;
                    } else {
                        printWarning("У клиента нет счетов.");
                    }
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 5: {
                // Показать все счета
                clearScreen();
                printHeader("ВСЕ СЧЕТА БАНКА");

                try {
                    bank.displayAllAccounts();
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }
        }
    }
}

// Обработка меню операций
void handleTransactionMenu(Bank& bank) {
    bool back = false;

    while (!back) {
        showTransactionMenu();
        int choice = getIntInput("Выберите действие", 0, 4);

        switch (choice) {
            case 0:
                back = true;
                break;

            case 1: {
                // Пополнение счета
                clearScreen();
                printHeader("ПОПОЛНЕНИЕ СЧЕТА");

                std::string accNum = getStringInput("Номер счета");
                double amount = getDoubleInput("Сумма пополнения", 0.01);

                try {
                    bank.deposit(accNum, amount);
                    printSuccess("Счет успешно пополнен!");
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 2: {
                // Снятие денег
                clearScreen();
                printHeader("СНЯТИЕ ДЕНЕГ");

                std::string accNum = getStringInput("Номер счета");
                double amount = getDoubleInput("Сумма снятия", 0.01);

                try {
                    bank.withdraw(accNum, amount);
                    printSuccess("Снятие успешно выполнено!");
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 3: {
                // Перевод денег
                clearScreen();
                printHeader("ПЕРЕВОД ДЕНЕГ");

                std::string fromAcc = getStringInput("Счет отправителя");
                std::string toAcc = getStringInput("Счет получателя");
                double amount = getDoubleInput("Сумма перевода", 0.01);

                try {
                    bank.transfer(fromAcc, toAcc, amount);
                    printSuccess("Перевод успешно выполнен!");
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }

            case 4: {
                // История операций
                clearScreen();
                printHeader("ИСТОРИЯ ОПЕРАЦИЙ");

                try {
                    bank.displayAllTransactions();
                } catch (const BankException& e) {
                    printError(e.what());
                }
                pauseScreen();
                break;
            }
        }
    }
}

// ==================== ГЛАВНАЯ ФУНКЦИЯ ====================

int main() {
    Bank bank("Мой Банк");
    bool exitProgram = false;

    // Приветственное сообщение
    clearScreen();
    std::cout << "\033[1;35m" << std::string(60, '=') << "\033[0m" << std::endl;
    std::cout << "\033[1;35m     ДОБРО ПОЖАЛОВАТЬ В БАНКОВСКУЮ СИСТЕМУ     \033[0m" << std::endl;
    std::cout << "\033[1;35m" << std::string(60, '=') << "\033[0m" << std::endl;
    std::cout << "\n\033[1;36mСистема обеспечивает:\033[0m" << std::endl;
    std::cout << "• Управление клиентами и счетами" << std::endl;
    std::cout << "• Банковские операции (пополнение, снятие, перевод)" << std::endl;
    std::cout << "• Полную валидацию всех операций" << std::endl;
    std::cout << "• Подробную отчетность" << std::endl;
    std::cout << "\n\033[1;33mДля продолжения нажмите Enter...\033[0m";
    std::cin.get();

    while (!exitProgram) {
        showMainMenu();
        int choice = getIntInput("Выберите действие", 0, 5);

        try {
            switch (choice) {
                case 0:
                    exitProgram = true;
                    clearScreen();
                    std::cout << "\033[1;32m" << std::string(50, '=') << "\033[0m" << std::endl;
                    std::cout << "\033[1;32m     СПАСИБО ЗА ИСПОЛЬЗОВАНИЕ СИСТЕМЫ!     \033[0m" << std::endl;
                    std::cout << "\033[1;32m" << std::string(50, '=') << "\033[0m" << std::endl;
                    break;

                case 1:
                    handleClientMenu(bank);
                    break;

                case 2:
                    handleAccountMenu(bank);
                    break;

                case 3:
                    handleTransactionMenu(bank);
                    break;

                case 4: {
                    // Отчетность
                    clearScreen();
                    printHeader("ОТЧЕТНОСТЬ");
                    std::cout << "\033[1;33m1. Отчет по банку\033[0m" << std::endl;
                    std::cout << "\033[1;33m2. Отчет по клиенту\033[0m" << std::endl;
                    std::cout << "\033[1;33m3. Финансовая сводка\033[0m" << std::endl;
                    std::cout << "\033[1;31m0. Назад\033[0m" << std::endl;

                    int reportChoice = getIntInput("Выберите отчет", 0, 3);

                    if (reportChoice == 1) {
                        clearScreen();
                        printHeader("ОТЧЕТ ПО БАНКУ");
                        bank.generateBankReport();
                    } else if (reportChoice == 2) {
                        clearScreen();
                        printHeader("ОТЧЕТ ПО КЛИЕНТУ");
                        int clientId = getIntInput("Введите ID клиента", 1);
                        bank.generateClientReport(clientId);
                    } else if (reportChoice == 3) {
                        clearScreen();
                        printHeader("ФИНАНСОВАЯ СВОДКА");
                        bank.generateFinancialSummary();
                    }

                    if (reportChoice != 0) {
                        pauseScreen();
                    }
                    break;
                }

                case 5: {
                    // Поиск информации
                    clearScreen();
                    printHeader("ПОИСК ИНФОРМАЦИИ");
                    std::cout << "\033[1;33m1. Найти клиента по ID\033[0m" << std::endl;
                    std::cout << "\033[1;33m2. Найти клиента по имени\033[0m" << std::endl;
                    std::cout << "\033[1;33m3. Найти счет по номеру\033[0m" << std::endl;
                    std::cout << "\033[1;33m4. Найти счета клиента\033[0m" << std::endl;
                    std::cout << "\033[1;31m0. Назад\033[0m" << std::endl;

                    int searchChoice = getIntInput("Выберите тип поиска", 0, 4);

                    if (searchChoice == 1) {
                        clearScreen();
                        printHeader("ПОИСК КЛИЕНТА ПО ID");
                        int clientId = getIntInput("Введите ID клиента", 1);
                        auto client = bank.findClientById(clientId);
                        if (client) {
                            client->displayInfo();
                        } else {
                            printWarning("Клиент не найден.");
                        }
                    } else if (searchChoice == 2) {
                        clearScreen();
                        printHeader("ПОИСК КЛИЕНТА ПО ИМЕНИ");
                        std::string name = getStringInput("Введите имя или фамилию");
                        auto clients = bank.findClientsByName(name);
                        if (!clients.empty()) {
                            std::cout << "\n\033[1;32mНайдено клиентов: " << clients.size() << "\033[0m" << std::endl;
                            for (const auto& client : clients) {
                                client->displayInfo();
                            }
                        } else {
                            printWarning("Клиенты не найдены.");
                        }
                    } else if (searchChoice == 3) {
                        clearScreen();
                        printHeader("ПОИСК СЧЕТА");
                        std::string accNum = getStringInput("Введите номер счета");
                        auto account = bank.findAccountByNumber(accNum);
                        if (account) {
                            account->displayInfo();
                        } else {
                            printWarning("Счет не найден.");
                        }
                    } else if (searchChoice == 4) {
                        clearScreen();
                        printHeader("СЧЕТА КЛИЕНТА");
                        int clientId = getIntInput("Введите ID клиента", 1);
                        auto accounts = bank.findAccountsByClientId(clientId);
                        if (!accounts.empty()) {
                            std::cout << "\n\033[1;32mСчета клиента ID " << clientId << ":\033[0m" << std::endl;
                            for (const auto& account : accounts) {
                                std::cout << "\nНомер: " << account->getAccountNumber()
                                          << "\nТип: " << account->getAccountType()
                                          << "\nБаланс: " << std::fixed << std::setprecision(2)
                                          << account->getBalance() << " руб." << std::endl;
                            }
                        } else {
                            printWarning("У клиента нет счетов.");
                        }
                    }

                    if (searchChoice != 0) {
                        pauseScreen();
                    }
                    break;
                }

                default:
                    printError("Неверный выбор. Попробуйте снова.");
                    pauseScreen();
                    break;
            }
        } catch (const BankException& e) {
            printError("Банковская ошибка: " + std::string(e.what()));
            pauseScreen();
        } catch (const std::exception& e) {
            printError("Системная ошибка: " + std::string(e.what()));
            pauseScreen();
        } catch (...) {
            printError("Неизвестная ошибка");
            pauseScreen();
        }
    }
}
