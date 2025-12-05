//
// Created by Kirill on 04.12.2025.
//

/*
**Задача:**
1. Создайте класс `DatabaseManager` с методами для подключения к базе данных
2. Реализуйте создание таблиц `students` и `grades`
3. Добавьте базовую оптимизацию базы данных
 */


#include "lib/sqlite3.h"
#include <iostream>
#include <string>

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

int main() {
    DatabaseManager databaseManager;

    if (!databaseManager.initialize("test.db")) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }

    // Создаем студента
    databaseManager.execute(
            "INSERT INTO students (name, email, group_name) "
            "VALUES ('Иван Петров', 'ivan@example.com', 'CS-101');"
    );

    // Создаем оценку для студента
    databaseManager.execute(
            "INSERT INTO grades (student_id, subject, grade) "
            "VALUES (1, 'Математика', 5);"
    );

}