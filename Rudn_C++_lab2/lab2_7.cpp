//
// RUDN C++
//

/*
Создайте программу для учета студентов в группе.

	1. Создайте структуру Date (день, месяц, год).
	2. Создайте структуру Student, которая содержит:
		-ФИО (std::string)
		-Дата рождения (Date)
		-Массив из 5 оценок (за сессию) (int[5])
	3. В функции main создайте массив из 3-5 студентов и инициализируйте его данными (можно "вшитыми" в код).
	4. Реализуйте функции:
		-void printStudent(const Student& s); // Вывод информации о студенте
		-double getAverageRating(const Student& s); // Подсчет среднего балла студента
		-Выведите список студентов, чей средний балл выше 4.0.
*/

#include <iostream>
#include <iomanip>
#include <bitset>

using namespace std;

const int MARKS_COUNT = 5;

struct Date {
    int day;
    int month;
    int year;
};

struct Student {
    string fio;
    Date birthDate;
    int marks[MARKS_COUNT];
};

// Функция для вычисления среднего балла
double getAverageRating(const Student& s) {
    double sum = 0;
    for (int mark : s.marks) {
        sum += mark;
    }
    return sum / MARKS_COUNT;
}

// Функция для вывода информации о студенте
void printStudent(const Student& student) {
    cout << "Имя: " << student.fio << endl;
    cout << "Дата рождения: " << student.birthDate.day << "."
         << student.birthDate.month << "." << student.birthDate.year << endl;
    cout << "Оценки: ";
    for (int mark : student.marks) {
        cout << mark << " ";
    }
    cout << endl;
    cout << "Средний балл: " << getAverageRating(student) << endl;
    cout << "-------------------" << endl;
}

int main() {
    setlocale(LC_ALL, "rus");
    const int N = 3;
    Student students[N];
    int i = 0;

    // Добавление студентов
    students[i++] = {"Анна Иванова", {15, 5, 2000}, {4, 5, 4, 3, 5}};

    students[i++] = {"Петр Сидоров", {20, 8, 1999}, {3, 4, 4, 3, 4}};

    students[i] = {"Ваня Казаков", {21, 3, 1995}, {1, 4, 2, 3, 5}};

    // Вывод всех студентов
    cout << "Список студентов:" << endl;
    for (int j = 0; j < N; j++) {
        printStudent(students[j]);
    }

    // Вывод студентов со средним баллом выше 4.0
    cout << "Список студентов со средним баллом выше 4.0:" << endl;
    for (int j = 0; j < N; j++) {
        if (getAverageRating(students[j]) > 4.0) {
            printStudent(students[j]);
        }
    }

}