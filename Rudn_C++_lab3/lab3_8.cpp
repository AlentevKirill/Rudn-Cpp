//
// Created by Kirill on 01.11.2025.
//

#include <iostream>
#include <string>

using namespace std;

class Person {
protected:
    string name;
    int age;

public:
    Person(string n, int a) : name(n), age(a) {}

    virtual void display() const {
        cout << "Имя: " << name << ", Возраст: " << age << endl;
    }
};

class Employee {

protected:
    string position;
    double salary;

public:
    Employee(string pos, double sal) : position(pos), salary(sal) {}

    virtual void work() const {
        cout << "Выполняет работу как сотрудник" << endl;
    }
};

// TODO: Создать класс Teacher, наследующий от Person и Employee
class Teacher : public Person, public Employee {

private:
    string subject;
    int experienceYears;

public:
    // TODO: Создать конструктор
    Teacher(
            const string &n,
            int a,
            const string &pos,
            double sal,
            const string &subject,
            int experienceYears
            ) : Person(n, a),
            Employee(pos, sal),
            subject(subject),
            experienceYears(experienceYears) {}

    // TODO: Переопределить методы display() и work()
    void display() const override {
        Person::display();
        cout << "Должность: " << position << ", Зарплата: " << salary << " руб." << endl;
        cout << "Предмет: " << subject << ", Стаж: " << experienceYears << endl;
    }

    void work() const override {
        cout << "Выполняет работу как учитель" << endl;
    }

    // TODO: Добавить специфичные методы для Teacher

    void pen() const {
        cout << "Часто пользуется мелом и красной ручкой" << endl;
    }

    void bas() const {
        cout << "Знает свой предмет: " << subject << " как Отче наш" << endl;
    }
};

class Researcher {
protected:
    string researchArea;
    int publicationsCount;

public:
    Researcher(string area, int publications)
            : researchArea(area), publicationsCount(publications) {}

    void conductResearch() const {
        cout << "Проводит исследования в области: " << researchArea << endl;
    }
};

// TODO: Создать класс Professor, наследующий от Teacher и Researcher
class Professor : public Teacher, public Researcher {
    // TODO: Создать конструктор
public:
    Professor(
            const string &n,
            int a,
            const string &pos,
            double sal,
            const string &subject,
            int experienceYears,
            const string &area,
            int publications
            ) : Teacher(n, a, pos, sal, subject, experienceYears),
            Researcher(area, publications) {}

    // TODO: Переопределить необходимые методы
    void display() const override {
        Teacher::display();
        cout << "Область исследования: " << researchArea << ", Публикации: " << publicationsCount << endl;
    }

    void work() const override {
        cout << "Выполняет работу как профессор" << endl;
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    // TODO: Создать объекты Teacher и Professor
    Teacher teacher = Teacher("Зинаида", 58, "учитель", 20000, "физика", 5);
    Professor professor = Professor("Катерина", 67, "профессор", 30000, "матемтика", 15, "Теория полей", 15);
    // Продемонстрировать множественное наследование
    cout << "Вызов методов учителя: " << endl;
    teacher.display();
    teacher.work();
    teacher.bas();
    teacher.pen();
    cout << endl;

    cout << "Вызов методов профессора: " << endl;
    professor.display();
    professor.work();
    professor.bas();
    professor.pen();
    professor.conductResearch();

    return 0;
}