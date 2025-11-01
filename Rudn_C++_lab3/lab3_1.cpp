//
// Created by Kirill on 31.10.2025.
//

#include <iostream>
#include <string>
#include <utility>

using namespace std;

const int N = 5;

class Student {
private:
    // TODO: Добавить приватные поля: имя, возраст, средний балл
    // и массив оценок (5 элементов)
    string name;
    int age;
    double averageGrade{};
    double grades[N]{};

    // TODO: Метод для расчета среднего балла
    void calculateAverage() {
        double sum = 0;
        for (double grade : grades) {
            sum += grade;
        }
        averageGrade = sum / 5;
    }

public:
    // TODO: Создать конструктор с параметрами
    Student(string name, int age) : name(name), age(age) {}

    // TODO: Реализовать геттеры и сеттеры с валидацией

    const string &getName() const {
        return name;
    }

    void setName(const string &name) {
        Student::name = name;
    }

    int getAge() const {
        return age;
    }

    void setAge(int age) {
        if (age > 100 || age < 10) {
            cout << "age = " << age << " Некорректное значение возраста, введите значение из диапазона [10;100]" << endl;
            return;
        }
        Student::age = age;
    }

    double getAverageGrade() const {
        return averageGrade;
    }

    const double *getGrads() const {
        return grades;
    }

    void setGrade(int i, double grade) {
        if (grade > 5 || grade < 2) {
            cout << "Обнаружена попытка ввести некорректное значение оценки grades[" << i << "] = " << grade << endl;
            cout << "Значение оценки должно находится в диапазоне [2;5]" << endl;
            return;
        }
        Student::grades[i] = grade;
        calculateAverage();
    }

    // TODO: Метод для вывода информации о студенте
    void displayInfo() {
        cout << "Информация о студенте " << name << ":" << endl;
        cout << "Возраст: " << age << endl;
        cout << "Средняя оценка по предметам: " << averageGrade << endl;
        cout << "Список оценок: ";
        for (double grade : grades) {
            cout << grade << " ";
        }
        cout << endl << endl;
    }

    // TODO: Метод для проверки стипендии (средний балл >= 4.5)
    bool hasScholarship() const {
        return averageGrade >= 4.5;
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    // Тестирование класса Student
    Student student("Иван Иванов", 20);

    // Установка оценок
    student.setGrade(0, 5);
    student.setGrade(1, 4);
    student.setGrade(2, 5);
    student.setGrade(3, 3);
    student.setGrade(4, 4);

    student.displayInfo();

    if (student.hasScholarship()) {
        cout << "Студент получает стипендию" << endl;
    } else {
        cout << "Стипендия не назначена" << endl;
    }

    return 0;
}