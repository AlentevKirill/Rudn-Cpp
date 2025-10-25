//
// RUDN C++
//

/*
Создайте структуру Account с полями: номер счета (int) и баланс (double).
	1. Напишите функцию void deposit(Account* acc, double amount), которая увеличивает баланс счета на указанную сумму, используя указатель.
	2. Напишите функцию void withdraw(Account& acc, double amount), которая уменьшает баланс счета на указанную сумму, используя ссылку. Функция должна проверять, достаточно ли средств на счете.
	3. Создайте в main объект Account, продемонстрируйте работу обеих функций.
*/

#include <iostream>

using namespace std;

struct Account {
    int id;
    double balance;
};

void deposit(Account* acc, double amount) {
    if (amount < 0) {
        cout << "Ошибка! Невозможно оперировать отрицательными суммами" << endl;
    } else {
        (*acc).balance += amount;
    }
}

void withdraw(Account& acc, double amount) {
    if (amount < 0) {
        cout << "Ошибка! Невозможно оперировать отрицательными суммами" << endl;
        return;
    }
    if (acc.balance < amount) {
        cout << "Ошибка! Невозможно списать со счёта сумму " << amount << " руб." << endl << "Остаток средств на счёте: " << acc.balance << " руб." << endl;
    } else {
        acc.balance -= amount;
    }
}

int main() {
    setlocale(LC_ALL, "rus");
    Account account = {1, 1000};
    cout << "Инициализирован Аккаунт номер 1 с балансом 1000 руб." << endl;
    deposit(&account, 2000);
    cout << "После добавления 2000 руб. на счёте стало: " << account.balance << " руб." << endl;
    withdraw(account, 1500);
    cout << "После снятия 1500 руб. на счёте стало: " << account.balance << " руб." << endl;
    cout << "Попытка снять 3000 руб." << endl;
    withdraw(account, 3000);
}