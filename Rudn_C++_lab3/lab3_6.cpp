//
// Created by Kirill on 01.11.2025.
//

#include <iostream>
#include <string>

using namespace std;

class BankAccount {

private:
    string accountNumber;
    string ownerName;
    double balance;
    static int totalAccounts;           // Общее количество счетов
    static double totalBankBalance;     // Общий баланс банка
    const double MIN_BALANCE = 10.0;    // Минимальный баланс

public:
    // TODO: Создать конструктор с параметрами
    // В конструкторе увеличивать totalAccounts и добавлять баланс к totalBankBalance
    BankAccount(
            const string &accountNumber,
            const string &ownerName,
            double balance
            ) : accountNumber(accountNumber),
            ownerName(ownerName),
            balance(balance) {
        totalAccounts++;
        totalBankBalance += balance;
    }


    // TODO: Создать деструктор
    // В деструкторе уменьшать totalAccounts и вычитать баланс из totalBankBalance
    ~BankAccount() {
        totalAccounts--;
        totalBankBalance -= balance;
    }

    // TODO: Реализовать методы:

    // - deposit(double amount)
    void deposit(double amount) {
        if (amount < 0) {
            cout << "Ошибка! Сумма пополнения должна быть больше 0" << endl;
            return;
        }
        balance += amount;
        totalBankBalance += amount;
    }

    // - withdraw(double amount) с проверкой минимального баланса
    void withdraw(double amount) {
        if (amount < 0) {
            cout << "Ошибка! Сумма снятия со счёта должна быть больше 0" << endl;
            return;
        }
        if (balance < amount) {
            cout << "Ошибка! Нехватка средств, остаточный баланс: " << balance << endl;
            return;
        }
        if (totalBankBalance - amount < MIN_BALANCE) {
            cout << "Ошибка! Невозможно вывести срества, банк обанкротился>: " << balance << endl;
            return;
        }
        balance -= amount;
        totalBankBalance -= amount;
    }

    // - displayAccountInfo() const
    void displayAccountInfo() const {
        cout << "Информация по аккаунту: " << endl;
        cout << "Номер аккаунта: " << accountNumber << endl;
        cout << "Имя пользователя: " << ownerName << endl;
        cout << "Баланс: " << balance << " руб." << endl;
    }

    // TODO: Статические методы:

    // - static int getTotalAccounts()
    static int getTotalAccounts() {
        return totalAccounts;
    }

    // - static double getTotalBankBalance()
    static double getTotalBankBalance() {
        return totalBankBalance;
    }

    // - static double getAverageBalance()
    static double getAverageBalance() {
        return totalBankBalance / totalAccounts;
    }
};

// TODO: Инициализировать статические переменные
int BankAccount::totalAccounts = 0;
double BankAccount::totalBankBalance = 0.0;

int main() {
    setlocale(LC_ALL, "rus");
    // TODO: Создать несколько счетов
    BankAccount ba1 = BankAccount("1", "Василий", 1);
    BankAccount ba2 = BankAccount("2", "Иван", 2);
    BankAccount ba3 = BankAccount("3", "Пётр", 3);

    // Продемонстрировать работу статических методов
    cout << "Всего аккаунтов: " << BankAccount::getTotalAccounts() << endl;
    cout << "Фонд банка: " << BankAccount::getTotalBankBalance() << " руб." << endl;
    cout << "Средняя сумма на счёте: " << BankAccount::getAverageBalance() << " руб." << endl;

    // Показать, что статические переменные общие для всех объектов
    string result = ba1.getTotalAccounts() == ba2.getTotalAccounts() ? "Статические переменные равны" : "Ошибка";
    cout << result << endl;

    return 0;
}