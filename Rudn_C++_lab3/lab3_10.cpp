//
// Created by Kirill on 01.11.2025.
//

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// Абстрактный базовый класс
class UniversityMember {

protected:
    string name;
    string id;
    string email;

public:
    UniversityMember(string n, string i, string e) : name(n), id(i), email(e) {}
    virtual ~UniversityMember() {}

    // TODO: Чисто виртуальные методы:

    // - virtual void displayInfo() const = 0;
    virtual void displayInfo() const = 0;

    // - virtual void work() const = 0;
    virtual void work() const = 0;

    virtual string getRole() const = 0;

    virtual string getName() const = 0;

    virtual string getID() const = 0;
};

class Student : public UniversityMember {

private:
    string majorr;
    int year;
    double gpa;
    vector<string> courses;

public:

    // TODO: Конструктор
    Student(
            const string &n,
            const string &i,
            const string &e,
            const string &majorr,
            int year,
            double gpa
            ) : UniversityMember(n, i, e),
            majorr(majorr),
            year(year),
            gpa(gpa) {}

    // TODO: Реализовать виртуальные методы

    void displayInfo() const override {
        cout << "Информация о студенте:" << endl;
        cout << "Имя: " << name << endl;
        cout << "Идентификатор: " << id << endl;
        cout << "Почта: " << email << endl;
        cout << "Направление: " << majorr << endl;
        cout << "Год обучения: " << year << endl;
        cout << "Средний балл: " << gpa << endl;
        cout << "Список курсов:" << endl;
        for (const string& course : courses) {
            cout << "- " << course << endl;
        }
    }

    void work() const override {
        cout << "Не работает" << endl;
    }

    string getRole() const override {
        return "Студент";
    }

    // TODO: Добавить методы:

    // - enrollCourse(const string& course)
    void enrollCourse(const string& course) {
        courses.push_back(course);
    }

    // - dropCourse(const string& course)
    void dropCourse(const string& course) {
        for (vector<string>::iterator it = courses.begin(); it != courses.end(); it++) {
            if (*it == course) {
                courses.erase(it);
                return;
            }
        }
        cout << "Не найдено курса с названием: " << course << endl;
    }

    // - calculateGPA()
    double calculateGPA() const {
        return gpa;
    }

    string getID() const override {
        return id;
    }

    string getName() const override{
        return name;
    }

};

class Professor : public UniversityMember {

private:
    string department;
    string office;
    double salary;
    vector<string> teachingCourses;

public:

    // TODO: Конструктор
    Professor(
            const string &n,
            const string &i,
            const string &e,
            const string &department,
            const string &office,
            double salary
            ) : UniversityMember(n, i, e),
            department(department),
            office(office),
            salary(salary) {}

    // TODO: Реализовать виртуальные методы
    void displayInfo() const override {
        cout << "Информация о профессоре:" << endl;
        cout << "Имя: " << name << endl;
        cout << "Идентификатор: " << id << endl;
        cout << "Почта: " << email << endl;
        cout << "Факультет: " << department << endl;
        cout << "Кабинет: " << office << endl;
        cout << "Зарплата: " << salary << endl;
        cout << "Список препдаваемых предметов:" << endl;
        for (const string& course : teachingCourses) {
            cout << "- " << course << endl;
        }
    }

    void work() const override {
        cout << "Работает преподавателем" << endl;
    }

    string getRole() const override {
        return "Преподаватель";
    }

    string getName() const override{
        return name;
    }

    string getID() const override{
        return id;
    }

    // TODO: Добавить методы:

    // - assignCourse(const string& course)
    void assignCourse(const string& course) {
        teachingCourses.push_back(course);
    }

    // - removeCourse(const string& course)
    void removeCourse(const string& course) {
        for (vector<string>::iterator it = teachingCourses.begin(); it != teachingCourses.end(); it++) {
            if (*it == course) {
                teachingCourses.erase(it);
                return;
            }
        }
        cout << "Не найдено курса с названием: " << course << endl;
    }

    // - calculateSalary() const
    double calculateSalary() const {
        return salary;
    }
};

class Course {

private:
    string courseCode;
    string courseName;
    int credits;
    Professor* instructor;
    vector<Student*> enrolledStudents;

public:
    // TODO: Конструктор
    Course(
            const string &courseCode,
            const string &courseName,
            int credits
            ) : courseCode(courseCode),
            courseName(courseName),
            credits(credits) {}

    // TODO: Методы:

    // - addStudent(Student* student)
    void addStudent(Student* student) {
        enrolledStudents.push_back(student);
    }

    // - removeStudent(const string& studentId)
    void removeStudent(const string& studentId) {
        for (vector<Student*>::iterator it = enrolledStudents.begin(); it != enrolledStudents.end(); it++) {
            if ((*it)->getID() == studentId) {
                enrolledStudents.erase(it);
                return;
            }
        }
        cout << "Не найден студент с идентификатором: " << studentId << endl;
    }

    // - setInstructor(Professor* prof)
    void setInstructor(Professor* prof) {
        instructor = prof;
    }

    string getCourseCode() {
        return courseCode;
    }

    // - displayCourseInfo() const
    void displayCourseInfo() const {
        cout << "Информация о Курсе:" << endl;
        cout << "Код курса: " << courseName << endl;
        cout << "Название курса: " << courseCode << endl;
        cout << "Объём курса: " << credits << " з.е." << endl;
        cout << "Курс ведёт: " << instructor->getName() << endl;
        cout << "Список студентов:" << endl;
        for (Student* student : enrolledStudents) {
            cout << "- " << student->getName() << endl;
        }
    }
};

class University {
private:
    string name;
    vector<unique_ptr<UniversityMember>> members;
    vector<unique_ptr<Course>> courses;
    static int totalUniversities;

public:
    University(string n) : name(n) {
        totalUniversities++;
    }

    ~University() {
        totalUniversities--;
    }

    // TODO: Методы для управления членами университета и курсами

    void addMember(unique_ptr<UniversityMember> member) {
        members.push_back(move(member));
    }

    void removeMember(const string& id) {
        for (size_t i = 0; i < members.size(); i++) {
            if (members[i]->getID() == id) {
                members.erase(members.begin() + i);
                return;
            }
        }
    }

    UniversityMember* findMember(const string& id) {
        for (size_t i = 0; i < members.size(); i++) {
            if (members[i]->getID() == id) {
                return members[i].get();
            }
        }
        return nullptr;
    }

    void addCourse(unique_ptr<Course> course) {
        courses.push_back(move(course));
    }

    void removeCourse(const string& courseCode) {
        for (size_t i = 0; i < courses.size(); i++) {
            if (courses[i]->getCourseCode() == courseCode) {
                courses.erase(courses.begin() + i);
                return;
            }
        }
    }

    Course* findCourse(const string& courseCode) {
        for (size_t i = 0; i < courses.size(); i++) {
            if (courses[i]->getCourseCode() == courseCode) {
                return courses[i].get();
            }
        }
        return nullptr;
    }

    // TODO: Статические методы
    static int getTotalUniversities() {
        return totalUniversities;
    }

    void displayAllMembers() const {
        for (const auto& member : members) {
            member->displayInfo();
            cout << endl;
        }
    }
};

int University::totalUniversities = 0;

int main() {
    setlocale(LC_ALL, "rus");
    // TODO: Создать университет
    // Добавить студентов, преподавателей, курсы
    // Продемонстрировать все возможности системы
    // Создать университет
    University uni("Технический Университет");

    // Создать студентов
    auto student1 = make_unique<Student>("Иван Петров", "S001", "ivan@mail.ru", "Информатика", 2, 4.5);
    auto student2 = make_unique<Student>("Мария Сидорова", "S002", "maria@mail.ru", "Математика", 1, 4.2);

    // Создать преподавателей
    auto prof1 = make_unique<Professor>("Дмитрий Козлов", "P001", "kozlov@mail.ru", "Информатика", "Каб. 301", 50000);
    auto prof2 = make_unique<Professor>("Ольга Новикова", "P002", "novikova@mail.ru", "Математика", "Каб. 205", 45000);

    // Добавить членов в университет
    uni.addMember(move(student1));
    uni.addMember(move(student2));
    uni.addMember(move(prof1));
    uni.addMember(move(prof2));

    // Создать курсы
    auto course1 = make_unique<Course>("CS101", "Программирование", 5);
    auto course2 = make_unique<Course>("MATH201", "Высшая математика", 6);

    // Назначить преподавателей курсам
    course1->setInstructor(static_cast<Professor*>(uni.findMember("P001")));
    course2->setInstructor(static_cast<Professor*>(uni.findMember("P002")));

    // Добавить курсы в университет
    uni.addCourse(move(course1));
    uni.addCourse(move(course2));

    // Записать студентов на курсы
    uni.findCourse("CS101")->addStudent(static_cast<Student*>(uni.findMember("S001")));
    uni.findCourse("CS101")->addStudent(static_cast<Student*>(uni.findMember("S002")));
    uni.findCourse("MATH201")->addStudent(static_cast<Student*>(uni.findMember("S002")));

    // Продемонстрировать возможности системы
    cout << "=== Все члены университета ===" << endl;
    uni.displayAllMembers();

    cout << "=== Информация о курсах ===" << endl;
    uni.findCourse("CS101")->displayCourseInfo();
    cout << endl;
    uni.findCourse("MATH201")->displayCourseInfo();

    cout << "=== Работа членов университета ===" << endl;
    uni.findMember("S001")->work();
    uni.findMember("P001")->work();

    cout << "=== Всего университетов: " << University::getTotalUniversities() << " ===" << endl;

    return 0;
}