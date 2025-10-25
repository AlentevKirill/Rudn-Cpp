//
// RUDN C++
//

/*
Используйте структуру Book из Задачи 5.
	1. Создайте динамический массив книг (можно инициализировать данными напрямую в коде).
	2. Напишите функцию void sortBooksByYear(Book* books, int size), которая сортирует массив книг по году издания (по возрастанию) с использованием простого алгоритма (например, пузырьковой сортировки).
	3. Напишите функцию Book* findBookByAuthor(Book* books, int size, const std::string& author), которая возвращает указатель на первую найденную книгу указанного автора. Если книга не найдена, функция должна возвращать nullptr.
	4. Продемонстрируйте сортировку и поиск в функции main.
*/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <bitset>

using namespace std;

struct Book {
    string title;
    string author;
    int year;
    double price;
};

// Поиск минмума и постановка его в начало неотсортированной части O(n^2)
void sortBooksByYear(Book* books, int size) {
    for (int i = 0; i < size - 1; i++) {
        int minYear = 2026;
        int indMinYear = 0;
        for (Book* iter = books + i; iter < books + size; iter++) {
            if ((*iter).year < minYear) {
                minYear = (*iter).year;
                indMinYear = (int) (iter - books);
            }
        }
        Book temp = *(books + i);
        *(books + i) = *(books + indMinYear);
        *(books + indMinYear) = temp;
    }
}

Book* findBookByAuthor(Book* books, int size, const std::string& author) {
    for (Book* iter = books; iter < books + size; iter++) {
        if ((*books).author == author) {
            return iter;
        }
    }
    return nullptr;
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

int main() {
    setlocale(LC_ALL, "rus");
    // Динамическое выделение памяти под массив структур Book
    Book* books = new Book[5] {
        {"Война и мир", "Лев Толстой", 1869, 15.99},
        {"Преступление и наказание", "Федор Достоевский", 1866, 12.50},
        {"Мастер и Маргарита", "Михаил Булгаков", 1967, 14.75},
        {"Альфа-ноль", "Артём Каменистый", 2020, 270},
        {"Проект Альверон", "Райкар", 2017, 350}
    };

    cout << "Инциализирован массив книг:" << endl;
    printBooks(books, 5);

    cout << endl;

    cout << "Отсортированный по дате массив книг:" << endl;
    sortBooksByYear(books, 5);
    printBooks(books, 5);

    cout << endl;

    cout << "Поиск книги по имени автора Федор Достоевский:" << endl;
    Book* book = findBookByAuthor(books, 5, "Федор Достоевский");
    if (book == nullptr) {
        cout << "Книги с указанным автором не найдено в библиотеке." << endl;
    } else {
        printBook(*book);
    }

    cout << endl;

    cout << "Поиск книги по имени автора Гоголь:" << endl;
    book = findBookByAuthor(books, 5, "Гоголь");
    if (book == nullptr) {
        cout << "Книги с указанным автором не найдено в библиотеке." << endl;
    } else {
        printBook(*book);
    }

    delete[] books;
    free(book);
}