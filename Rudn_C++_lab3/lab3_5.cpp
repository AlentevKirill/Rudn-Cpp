//
// Created by Kirill on 31.10.2025.
//

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Book {

private:
    string title;
    string author;
    string isbn;
    int year;
    bool isAvailable;

public:
    // TODO: Создать конструктор, геттеры, сеттеры
    Book(
            const string &title,
            const string &author,
            const string &isbn,
            int year
    ) : title(title),
        author(author),
        isbn(isbn),
        year(year),
        isAvailable(true) {}

    const string &getTitle() const {
        return title;
    }


    void setTitle(const string &title) {
        Book::title = title;
    }

    const string &getAuthor() const {
        return author;
    }

    void setAuthor(const string &author) {
        Book::author = author;
    }

    const string &getIsbn() const {
        return isbn;
    }

    void setIsbn(const string &isbn) {
        if (isbn.find_first_not_of("0123456789-") != string::npos) {
            cout << "Введён некоректный идентификатор книги " << isbn << endl;
            cout << "Код ISBN должен состоять только из цифр и знака '-'" << endl;
            return;
        }
        Book::isbn = isbn;
    }

    int getYear() const {
        return year;
    }

    void setYear(int year) {
        if (year > 2025 || year < 0) {
            cout << "Обнаружена попытка ввести некорректное значение года издания книги = " << year << endl;
            cout << "Значение года должно находится в диапазоне [0;2025]" << endl;
            return;
        }
        Book::year = year;
    }

    bool getIsAvailable() const {
        return isAvailable;
    }

    void setIsAvailable(bool isAvailable) {
        Book::isAvailable = isAvailable;
    }

    // TODO: Методы borrow() и returnBook() для изменения статуса доступности

    void borrow() {
        if (isAvailable) {
            cout << "Книга " << title << " отдана читателю" << endl;
            isAvailable = false;
        } else {
            cout << "Книги " << title << " нет в библиотеке" << endl;
        }
    }

    void returnBook() {
        if (isAvailable) {
            cout << "Ошибка! Книга " << title << " уже присутствует в библиотеке" << endl;
        } else {
            cout << "Книга " << title << " возвращена в библиотеку" << endl;
            isAvailable = true;
        }
    }

    // TODO: Метод displayInfo() для вывода информации о книге
    void displayInfo() const {
        cout << "Описание книги: " << endl;
        cout << "Название: " << title << endl;
        cout << "Автор: " << author << endl;
        cout << "Номер ISBN: " << isbn << endl;
        cout << "Год издания: " << year << endl;
        if (isAvailable) {
            cout << "Присутствует в библиотеке" << endl;
        } else {
            cout << "Отсутствует в библиотеке" << endl;
        }
    }
};

class Library {
private:
    string name;
    string address;
    vector<Book> books;

    Book* findBookByISBN(const string& isbn) {
        for (auto & book : books) {
            if (book.getIsbn() == isbn) {
                return &book;
            }
        }
        cout << "В библиотеке не найдена книга с номером: " << isbn << endl;
        return nullptr;
    }

public:
    Library(string n, string addr) : name(n), address(addr) {}

    // TODO: Добавить методы:

    // - addBook(const Book& book)
    void addBook(const Book& book) {
        books.push_back(book);
    }

    // - removeBook(const string& isbn)
    void removeBook(const string& isbn) {
        for (vector<Book>::iterator it = books.begin(); it != books.end(); it++) {
            if (it->getIsbn() == isbn) {
                books.erase(it);
                return;
            }
        }
        cout << "В библиотеке не найдена книга по номеру: " << isbn << endl;
    }

    // - findBook(const string& title) const
    Book* findBook(const string& title) {
        for (auto & book : books) {
            if (book.getTitle() == title) {
                return &book;
            }
        }
        cout << "В библиотеке не найдена книга с названием: " << title << endl;
        return nullptr;
    }

    // - borrowBook(const string& isbn)
    void borrowBook(const string& isbn) {
        Book* book = findBookByISBN(isbn);
        if (book != nullptr) {
            book->borrow();
        }
    }

    // - returnBook(const string& isbn)
    void returnBook(const string& isbn) {
        Book* book = findBookByISBN(isbn);
        if (book != nullptr) {
            book->returnBook();
        }
    }

    // - displayAllBooks() const
    void displayAllBooks() const {
        cout << "Все книги библиотеки: " << endl;
        for (const Book & book : books) {
            book.displayInfo();
            cout << endl;
        }
    }

    // - displayAvailableBooks() const
    void displayAvailableBooks() const {
        cout << "Все доступные книги библиотеки: " << endl;
        for (const Book & book : books) {
            if (book.getIsAvailable()) {
                book.displayInfo();
                cout << endl;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    // TODO: Создать библиотеку
    Library library = Library("Библиотека номер 1", "Улица Партизанов, дом 2");

    // Добавить несколько книг
    cout << "Добавили 2 книги" << endl;
    library.addBook(Book("Война и Мир", "Лев Толстой", "12-23-45-6", 1857));
    library.addBook(Book("Преступление и наказание", "Достоевский", "12-27-45-6", 1866));

    // Продемонстрировать работу всех методов

    library.displayAllBooks();

    library.displayAvailableBooks();

    cout << "Удалили книгу по номеру 12-27-45-6" << endl;
    library.removeBook("12-27-45-6");
    library.displayAllBooks();

    cout << "Добавили книгу Преступление и наказание" << endl;
    library.addBook(Book("Преступление и наказание", "Достоевский", "12-27-45-6", 1866));
    library.displayAllBooks();


    cout << "Взяли на прокат книгу Преступление и наказание" << endl;
    library.borrowBook(library.findBook("Преступление и наказание")->getIsbn());
    library.displayAvailableBooks();

    cout << "Вернули в библиотеку книгу Преступление и наказание" << endl;
    library.returnBook(library.findBook("Преступление и наказание")->getIsbn());
    library.displayAvailableBooks();

    return 0;
}