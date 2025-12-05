//
// Created by Kirill on 05.12.2025.
//

/*
**Задача:**
Создайте класс `StudentRepository` со следующими методами:
- `addStudent(name, email, group)`
- `getStudent(id)`
- `updateStudent(id, newName, newEmail, newGroup)`
- `deleteStudent(id)`
- `getAllStudents()`
*/

#include "lib/sqlite3.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

class DatabaseManager {
private:
    sqlite3* db;

public:
    DatabaseManager() : db(nullptr) {}

    ~DatabaseManager() {
        if (db) {
            sqlite3_close(db);
        }
    }

    bool initialize(const std::string& filename) {
        if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        // Настройка базы данных
        optimizeDatabase();
        createTables();

        return true;
    }

    void optimizeDatabase() {
        execute("PRAGMA journal_mode = WAL;");
        execute("PRAGMA foreign_keys = ON;");
        execute("PRAGMA cache_size = -64000;");
        sqlite3_busy_timeout(db, 5000);
    }

    void createTables() {
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS students (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                email TEXT UNIQUE,
                group_name TEXT
            );

            CREATE TABLE IF NOT EXISTS grades (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                student_id INTEGER,
                subject TEXT,
                grade INTEGER,
                FOREIGN KEY(student_id) REFERENCES students(id) ON DELETE CASCADE
            );
        )";

        execute(sql);
    }

    bool execute(const std::string& sql) {
        char* errorMessage = nullptr;
        int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);

        if (result != SQLITE_OK) {
            std::cerr << "SQL error: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
            return false;
        }

        return true;
    }

    sqlite3* getHandle() const {
        return db;
    }
};

struct Student {
    int id;
    std::string name;
    std::string email;
    std::string group_name;

};

class PreparedStatement {
private:
    sqlite3_stmt* stmt;

public:
    PreparedStatement(sqlite3* db, const std::string& sql) : stmt(nullptr) {
        int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (result != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }
    }

    ~PreparedStatement() {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }

    // Привязка параметров
    void bindInt(int index, int value) {
        sqlite3_bind_int(stmt, index, value);
    }

    void bindText(int index, const std::string& value) {
        sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
    }

    void bindDouble(int index, double value) {
        sqlite3_bind_double(stmt, index, value);
    }

    void bindNull(int index) {
        sqlite3_bind_null(stmt, index);
    }

    // Выполнение
    bool execute() {
        int result = sqlite3_step(stmt);
        sqlite3_reset(stmt); // Сбрасываем для повторного использования

        return result == SQLITE_DONE;
    }

    // Для SELECT запросов
    bool next() {
        int result = sqlite3_step(stmt);
        return result == SQLITE_ROW;
    }

    // Получение данных
    int getInt(int column) {
        return sqlite3_column_int(stmt, column);
    }

    std::string getText(int column) {
        const unsigned char* text = sqlite3_column_text(stmt, column);
        return text ? reinterpret_cast<const char*>(text) : "";
    }

    void reset() {
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
};

class StudentRepository {
private:
    sqlite3* db;

public:
    StudentRepository(sqlite3* db) : db(db) {}

    bool addStudent(
            const std::string& name,
            const std::string& email,
            const std::string& group_name
            ) {
        const std::string sql = "INSERT INTO students (name, email, group_name) VALUES (?, ?, ?)";

        try {
            PreparedStatement stmt(db, sql);
            stmt.bindText(1, name);
            stmt.bindText(2, email);
            stmt.bindText(3, group_name);
            return stmt.execute();
        } catch (const std::exception& e) {
            std::cerr << "Error adding user: " << e.what() << std::endl;

            // Проверка уникальности email
            const char* errMsg = sqlite3_errmsg(db);
            if (strstr(errMsg, "UNIQUE constraint failed")) { // strstr == indexOf(str) - ищет первое вхождение подстроки в строке
                std::cerr << "Error: Email '" << email << "' already exists!" << std::endl;
            }

            return false;
        }
    }

    Student getStudent(int id) {
        const std::string sql = "SELECT id, name, email, group_name FROM students WHERE id = ?";
        Student student;

        try {
            PreparedStatement stmt(db, sql);
            stmt.bindInt(1, id);

            if (stmt.next()) {
                student.id = stmt.getInt(0);
                student.name = stmt.getText(1);
                student.email = stmt.getText(2);
                student.group_name = stmt.getText(3);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error getting student: " << e.what() << std::endl;
        }

        return student;
    }

    bool updateStudent(int id, const std::string& newName, const std::string& newEmail, const std::string& newGroup) {
        const std::string sql = "UPDATE students SET name = ?, email = ?, group_name = ? WHERE id = ?";

        try {
            PreparedStatement stmt(db, sql);
            stmt.bindText(1, newName);
            stmt.bindText(2, newEmail);
            stmt.bindText(3, newGroup);
            stmt.bindInt(4, id);

            return stmt.execute();
        } catch (const std::exception& e) {
            std::cerr << "Error updating student: " << e.what() << std::endl;

            // Проверка уникальности email
            const char* errMsg = sqlite3_errmsg(db);
            if (strstr(errMsg, "UNIQUE constraint failed")) {
                std::cerr << "Error: Email '" << newEmail << "' already exists!" << std::endl;
            }
            return false;
        }
    }

    bool deleteStudent(int id) {
        // Проверяем, есть ли у студента оценки
        const std::string checkGradesSql = "SELECT COUNT(*) FROM grades WHERE student_id = ?";

        try {
            PreparedStatement checkStmt(db, checkGradesSql);
            checkStmt.bindInt(1, id);

            if (checkStmt.next() && checkStmt.getInt(0) > 0) {
                std::cout << "Warning: Student has " << checkStmt.getInt(0)
                          << " grade(s). They will be deleted due to CASCADE." << std::endl;
            }
        } catch (...) {
            // Игнорируем ошибки проверки, продолжаем удаление
        }

        // Удаляем студента
        const std::string sql = "DELETE FROM students WHERE id = ?";

        try {
            PreparedStatement stmt(db, sql);
            stmt.bindInt(1, id);

            bool result = stmt.execute();

            if (result) {
                int changes = sqlite3_changes(db);
                if (changes == 0) {
                    std::cerr << "Warning: No student found with ID " << id << std::endl;
                }
            }

            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error deleting student: " << e.what() << std::endl;

            // Проверка ошибок внешних ключей
            const char* errMsg = sqlite3_errmsg(db);
            if (strstr(errMsg, "FOREIGN KEY constraint failed")) {
                std::cerr << "Error: Cannot delete student due to foreign key constraint!" << std::endl;
            }
            return false;
        }
    }

    std::vector<Student> getAllStudents() {
        const std::string sql = "SELECT id, name, email, group_name FROM students";
        std::vector<Student> students;

        try {
            PreparedStatement stmt(db, sql);

            while (stmt.next()) {
                Student student;
                student.id = stmt.getInt(0);
                student.name = stmt.getText(1);
                student.email = stmt.getText(2);
                student.group_name = stmt.getText(3);
                students.push_back(student);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error getting students: " << e.what() << std::endl;
        }

        return students;
    }
};

int main() {
    DatabaseManager dbManager;
    if (!dbManager.initialize("students.db")){
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }

    StudentRepository repo(dbManager.getHandle());

    // 1. Добавление студента
    repo.addStudent("Иван Иванов", "ivan@mail.ru", "ИТ-101");

    // 2. Получение студента по ID
    Student student = repo.getStudent(2);
    std::cout << "Найден студент: " << student.name << std::endl;

    // 3. Обновление студента
    repo.updateStudent(2, "Иван Петров", "ivan.new@mail.ru", "ИТ-102");
    student = repo.getStudent(2);
    std::cout << "Найден студент: " << student.name << std::endl;

    // 4. Удаление студента
    repo.deleteStudent(2);

    // 5. Получение всех студентов
    std::vector<Student> allStudents = repo.getAllStudents();
    std::cout << "Всего студентов: " << allStudents.size() << std::endl;

}