//
// Created by Kirill on 31.10.2025.
//

#include <iostream>
#include <string>

using namespace std;

class Person {
protected:
    string name;
    int age;
    string address;

public:
    Person(string n, int a, string addr) : name(n), age(a), address(addr) {}
    virtual void displayInfo() const {
        cout << "Имя: " << name << ", Возраст: " << age << ", Адрес: " << address << endl;
    }
    virtual ~Person() {}
};

class Student : public Person {
private:
    string studentId;
    double averageGrade;

public:
    // TODO: Создать конструктор
    Student(
            string name,
            int age,
            string address,
            string studentId,
            double averageGrade
            ) : Person(name, age, address), studentId(studentId), averageGrade(averageGrade) {}

    // TODO: Переопределить displayInfo()
    void displayInfo() const override {
        Person::displayInfo();
        cout << "Номер студента: " << studentId << ", Средний балл: " << averageGrade << endl;
    }

    // TODO: Добавить методы для работы с оценками

    double getAverageGrade() const {
        return averageGrade;
    }

    void setAverageGrade(double averageGrade) {
        if (averageGrade > 5 || averageGrade < 2) {
            cout << "Обнаружена попытка ввести некорректное значение средней оценки averageGrade = " << averageGrade << endl;
            cout << "Значение оценки должно находится в диапазоне [2;5]" << endl;
            return;
        }
        Student::averageGrade = averageGrade;
    }

};

class Professor : public Person {
private:
    string department;
    double salary;
    int yearsOfExperience;

public:
    // TODO: Создать конструктор
    Professor(
            string name,
            int age,
            string address,
            string department,
            double salary,
            int yearsOfExperience
    ) : Person(name, age, address), department(department), salary(salary), yearsOfExperience(yearsOfExperience) {}
    // TODO: Переопределить displayInfo()

    void displayInfo() const override {
        Person::displayInfo();
        cout << "Факультет: " << department << ", Зарплата: " << salary << " руб." << ", Опыт работы: " << yearsOfExperience << " лет" endl;
    }

    // TODO: Добавить метод для расчета надбавки за стаж

    double dopSalary() {
        if (yearsOfExperience > 15) {
            return salary * 0.75;
        } else if (yearsOfExperience > 10) {
            return salary * 0.5;
        } else if (yearsOfExperience > 5) {
            return salary * 0.25;
        } else {
            return 0;
        }
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    // Тестирование иерархии классов
    Student student("Петр Петров", 20, "ул. Студенческая, 15", "S12345", 4.3);
    Professor prof("Доктор Иванов", 45, "ул. Академическая, 10", "Компьютерные науки", 50000, 15);

    cout << "Надбавка за стаж: " << prof.dopSalary() << " руб." << endl;

    student.displayInfo();
    prof.displayInfo();

    return 0;
}