//
// RUDN C++
//

/*
Создайте структуру Book, которая содержит поля: название (std::string), автор (std::string), год издания (int), цена (double).

	1. Запросите у пользователя количество книг N в библиотеке.
	2. Динамически выделите память под массив из N структур Book.
	3. Заполните массив данными, введенными пользователем.
	4. Реализуйте функцию void printBooks(Book* books, int size), которая выводит информацию о всех книгах.
	5. Напишите функцию, которая находит и выводит самую дорогую книгу в массиве.
	6. Не забудьте освободить динамически выделенную память.
*/

#include <iostream>
#include <iomanip>
#include <limits>
#include <bitset>

using namespace std;

struct Book {
    string title;
    string author;
    int year;
    double price;
};

// Функция для безопасного ввода целых чисел
int getPositiveInteger() {
    int value;
    while (true) {
        cout << "Введите количество книг в библиотеке: ";
        cin >> value;
        if (cin.fail()) {
            cout << "Ошибка! Введите корректное натуральное число." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (value < 0) {
                cout << "Ошибка! Введённое число должно быть натуральным.\n";
            } else {
                return value;
            }
        }
    }
}

// Функция для безопасного ввода года
int getYear() {
    int value;
    while (true) {
        cout << "Введите год издания книги: ";
        cin >> value;
        if (cin.fail()) {
            cout << "Ошибка! Введите корректный год." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (value < 0 || value > 2025) {
                cout << "Ошибка! Год должен быть в диапазоне 0-2025.\n";
            } else {
                return value;
            }
        }
    }
}

// Функция для безопасного ввода цены
double getPrice() {
    double value;
    while (true) {
        cout << "Введите цену книги в рублях: ";
        cin >> value;
        if (cin.fail()) {
            cout << "Ошибка! Введите корректную цену." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (value < 0) {
                cout << "Ошибка! Цена не может быть отрицательной.\n";
            } else {
                return value;
            }
        }
    }
}

void printBook(const Book& book) {
    cout << "  Название: " << book.title << endl;
    cout << "  Автор: " << book.author << endl;
    cout << "  Год: " << book.year << endl;
    cout << "  Цена: " << book.price << " руб." << endl;
}

void printBooks(Book* books, int size) {
    for (int i = 0; i < size; i++) {
        cout << "Книга " << i + 1 << ":" << endl;
        printBook(books[i]);
    }
}

void printMostExpensiveBook(Book* books, int N) {
    if (N == 0) {
        return;
    }
    int iMax = 0;
    for (int i = 1; i < N; i++) {
        if (books[i].price > books[iMax].price) {
            iMax = i;
        }
    }
    printBook(books[iMax]);
}

int main() {
    setlocale(LC_ALL, "rus");
    int N = getPositiveInteger();

    // Динамическое выделение памяти под массив структур Book
    Book* books = new Book[N];

    for (int i = 0; i < N; i++) {
        cout << "--- Книга " << i + 1 << " ---" << endl;
        cout << "Введите название: ";
        getline(cin, books[i].title);
        cout << "Введите автора: ";
        getline(cin, books[i].author);
        books[i].year = getYear();
        books[i].price = getPrice();
    }

    cout << "Информация о всех книгах:" << endl;
    printBooks(books, N);

    cout << "Самая дорогая книга в библиотеке:" << endl;
    printMostExpensiveBook(books, N);
	
	// Освобождение динамически выделенной памяти
    delete[] books;

}