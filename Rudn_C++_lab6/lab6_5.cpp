//
// Created by Kirill on 06.12.2025.
//

/*
**Задача:**
1. Реализуйте пакетную вставку данных (`batchInsertStudents`)
2. Создайте индексы для часто используемых запросов
3. Напишите unit-тесты для всех методов репозитория
4. Протестируйте производительность с большим объемом данных (1000+ записей)
 */

#include "lib/sqlite3.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <chrono>

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
        createIndexes();

        return true;
    }

    // ---------------------------------------------- НОВОЕ
    void createIndexes() {
        std::cout << "Creating indexes..." << std::endl;

        // Индекс для быстрого поиска по email (уникальный)
        execute("CREATE INDEX IF NOT EXISTS idx_students_email ON students(email);");

        // Индекс для поиска студентов по группе
        execute("CREATE INDEX IF NOT EXISTS idx_students_group ON students(group_name);");

        // Индекс для быстрого поиска оценок по предмету
        execute("CREATE INDEX IF NOT EXISTS idx_grades_subject ON grades(subject);");

        // Индекс для связи студент-оценки
        execute("CREATE INDEX IF NOT EXISTS idx_grades_student_id ON grades(student_id);");

        // Составной индекс для частых запросов по группе и имени
        execute("CREATE INDEX IF NOT EXISTS idx_students_group_name ON students(group_name, name);");

        std::cout << "Indexes created successfully" << std::endl;
    }

    // Метод для очистки всех таблиц (для тестирования) ------------------- НОВОЕ
    void clearAllTables() {
        execute("DELETE FROM grades;");
        execute("DELETE FROM students;");
        execute("VACUUM;");  // Освобождаем место
    }

    // Метод для измерения времени выполнения запроса ----------------------- НОВОЕ
    template<typename Func>
    long long measureExecutionTime(Func&& func, const std::string& operationName = "") {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (!operationName.empty()) {
            std::cout << operationName << " took: " << duration.count() << " ms" << std::endl;
        }

        return duration.count();
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

struct Grade {
    std::string subject;
    int grade;
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

class InputValidator {
private:
    // Регулярное выражение для проверки email
    bool isValidEmailPattern(const std::string& email) {
        // Простая, но эффективная проверка формата email
        size_t atPos = email.find('@');
        if (atPos == std::string::npos || atPos == 0 || atPos == email.length() - 1) {
            return false;
        }

        size_t dotPos = email.find('.', atPos);
        if (dotPos == std::string::npos || dotPos == email.length() - 1) {
            return false;
        }

        return true;
    }

    // Проверка на опасные SQL-символы
    bool containsSQLInjection(const std::string& str) {
        const char* dangerousPatterns[] = {
                "'", "\"", ";", "--", "/*", "*/",
                "DROP ", "DELETE ", "INSERT ", "UPDATE ",
                "SELECT ", "UNION ", "OR ", "AND ", "="
        };

        std::string upperStr = str;
        std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);

        for (const char* pattern : dangerousPatterns) {
            if (upperStr.find(pattern) != std::string::npos) {
                return true;
            }
        }

        return false;
    }

public:
    struct ValidationResult {
        bool isValid;
        std::string errorMessage;
    };

    // Валидация имени студента
    ValidationResult validateName(const std::string& name) {
        if (name.empty()) {
            return {false, "Имя не может быть пустым"};
        }

        if (name.length() > 100) {
            return {false, "Имя слишком длинное (макс. 100 символов)"};
        }

        if (containsSQLInjection(name)) {
            return {false, "Имя содержит опасные символы"};
        }

        return {true, ""};
    }

    // Валидация email
    ValidationResult validateEmail(const std::string& email) {
        if (email.empty()) {
            return {false, "Email не может быть пустым"};
        }

        if (email.length() > 255) {
            return {false, "Email слишком длинный (макс. 255 символов)"};
        }

        if (!isValidEmailPattern(email)) {
            return {false, "Некорректный формат email"};
        }

        if (containsSQLInjection(email)) {
            return {false, "Email содержит опасные символы"};
        }

        return {true, ""};
    }

    // Валидация названия группы
    ValidationResult validateGroupName(const std::string& group) {
        if (group.empty()) {
            return {false, "Название группы не может быть пустым"};
        }

        if (group.length() > 50) {
            return {false, "Название группы слишком длинное (макс. 50 символов)"};
        }

        if (containsSQLInjection(group)) {
            return {false, "Название группы содержит опасные символы"};
        }

        return {true, ""};
    }

    // Валидация предмета
    ValidationResult validateSubject(const std::string& subject) {
        if (subject.empty()) {
            return {false, "Название предмета не может быть пустым"};
        }

        if (subject.length() > 100) {
            return {false, "Название предмета слишком длинное (макс. 100 символов)"};
        }

        if (containsSQLInjection(subject)) {
            return {false, "Название предмета содержит опасные символы"};
        }

        return {true, ""};
    }

    // Валидация оценки
    ValidationResult validateGrade(int grade) {
        if (grade < 0 || grade > 100) {
            return {false, "Оценка должна быть в диапазоне 0-100"};
        }

        return {true, ""};
    }

    // Валидация ID студента
    ValidationResult validateStudentId(int id) {
        if (id <= 0) {
            return {false, "ID студента должен быть положительным числом"};
        }

        return {true, ""};
    }

    // Комплексная валидация студента
    ValidationResult validateStudent(const std::string& name,
                                     const std::string& email,
                                     const std::string& group) {
        auto nameResult = validateName(name);
        if (!nameResult.isValid) return nameResult;

        auto emailResult = validateEmail(email);
        if (!emailResult.isValid) return emailResult;

        auto groupResult = validateGroupName(group);
        if (!groupResult.isValid) return groupResult;

        return {true, ""};
    }

    // Комплексная валидация оценки
    ValidationResult validateGradeData(int studentId,
                                       const std::string& subject,
                                       int grade) {
        auto idResult = validateStudentId(studentId);
        if (!idResult.isValid) return idResult;

        auto subjectResult = validateSubject(subject);
        if (!subjectResult.isValid) return subjectResult;

        auto gradeResult = validateGrade(grade);
        if (!gradeResult.isValid) return gradeResult;

        return {true, ""};
    }
};

class StudentRepository {
private:
    sqlite3* db;
    InputValidator validator;

    bool execute(const std::string& sql) {
        char* errorMessage = nullptr;
        int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);

        if (result != SQLITE_OK) {
            if (errorMessage) {
                std::cerr << "SQL error: " << errorMessage << std::endl;
                sqlite3_free(errorMessage);
            }
            return false;
        }
        return true;
    }

public:
    StudentRepository(sqlite3* db) : db(db) {}

    bool addStudent(
            const std::string& name,
            const std::string& email,
            const std::string& group_name
    ) {
        auto validation = validator.validateStudent(name, email, group_name);
        if (!validation.isValid) {
            std::cerr << "Validation error: " << validation.errorMessage << std::endl;
            return false;
        }

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

        // Валидация входных данных
        auto validation = validator.validateStudent(newName, newEmail, newGroup);
        if (!validation.isValid) {
            std::cerr << "Validation error: " << validation.errorMessage << std::endl;
            return false;
        }

        // Дополнительная проверка ID
        auto idValidation = validator.validateStudentId(id);
        if (!idValidation.isValid) {
            std::cerr << "Validation error: " << idValidation.errorMessage << std::endl;
            return false;
        }

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

        // Валидация ID
        auto validation = validator.validateStudentId(id);
        if (!validation.isValid) {
            std::cerr << "Validation error: " << validation.errorMessage << std::endl;
            return false;
        }

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

    bool addStudentWithGrades(
            const std::string& name,
            const std::string& email,
            const std::string& group_name,
            const std::vector<Grade>& grades
    ) {

        auto studentValidation = validator.validateStudent(name, email, group_name);
        if (!studentValidation.isValid) {
            std::cerr << "Student validation error: " << studentValidation.errorMessage << std::endl;
            return false;
        }

        // Валидация всех оценок
        for (const auto& grade : grades) {
            auto gradeValidation = validator.validateSubject(grade.subject);
            if (!gradeValidation.isValid) {
                std::cerr << "Subject validation error: " << gradeValidation.errorMessage << std::endl;
                return false;
            }
        }

        // Начинаем транзакцию
        if (!execute("BEGIN TRANSACTION;")) {
            std::cerr << "Failed to start transaction" << std::endl;
            return false;
        }

        try {
            // Добавляем студента
            const std::string insertStudentSQL =
                    "INSERT INTO students (name, email, group_name) VALUES (?, ?, ?)";

            PreparedStatement stmtStudent(db, insertStudentSQL);
            stmtStudent.bindText(1, name);
            stmtStudent.bindText(2, email);
            stmtStudent.bindText(3, group_name);

            if (!stmtStudent.execute()) {
                throw std::runtime_error("Failed to insert student");
            }

            // Получаем ID добавленного студента
            int studentId = static_cast<int>(sqlite3_last_insert_rowid(db));

            // Добавляем оценки
            const std::string insertGradeSQL =
                    "INSERT INTO grades (student_id, subject, grade) VALUES (?, ?, ?)";

            PreparedStatement stmtGrade(db, insertGradeSQL);

            for (const auto& grade : grades) {
                stmtGrade.bindInt(1, studentId);
                stmtGrade.bindText(2, grade.subject);
                stmtGrade.bindInt(3, grade.grade);

                if (!stmtGrade.execute()) {
                    throw std::runtime_error("Failed to insert grade");
                }
                stmtGrade.reset();
            }

            // Фиксируем транзакцию
            if (!execute("COMMIT;")) {
                throw std::runtime_error("Failed to commit transaction");
            }

            std::cout << "Student added with " << grades.size() << " grades" << std::endl;
            return true;

        } catch (const std::exception& e) {
            // Откатываем транзакцию при ошибке
            execute("ROLLBACK;");
            std::cerr << "Transaction failed: " << e.what() << std::endl;

            // Проверка уникальности email
            const char* errMsg = sqlite3_errmsg(db);
            if (strstr(errMsg, "UNIQUE constraint failed")) {
                std::cerr << "Error: Email already exists!" << std::endl;
            }
            return false;
        }
    }

    std::vector<Student> getStudentsByGroup(const std::string& group_name) {

        // Валидация названия группы
        auto validation = validator.validateGroupName(group_name);
        if (!validation.isValid) {
            std::cerr << "Validation error: " << validation.errorMessage << std::endl;
            return {};
        }
        std::vector<Student> students;
        const std::string sql =
                "SELECT id, name, email, group_name FROM students WHERE group_name = ?";

        try {
            PreparedStatement stmt(db, sql);
            stmt.bindText(1, group_name);

            while (stmt.next()) {
                Student student;
                student.id = stmt.getInt(0);
                student.name = stmt.getText(1);
                student.email = stmt.getText(2);
                student.group_name = stmt.getText(3);
                students.push_back(student);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error getting students by group: " << e.what() << std::endl;
        }

        return students;
    }


    double getAverageGradeBySubject(const std::string& subject) {

        // Валидация названия предмета
        auto validation = validator.validateSubject(subject);
        if (!validation.isValid) {
            std::cerr << "Validation error: " << validation.errorMessage << std::endl;
            return 0.0;
        }

        const std::string sql =
                "SELECT AVG(grade) FROM grades WHERE subject = ?";

        try {
            PreparedStatement stmt(db, sql);
            stmt.bindText(1, subject);

            if (stmt.next()) {
                return stmt.getInt(0); // SQLite возвращает целое для AVG
            }
        } catch (const std::exception& e) {
            std::cerr << "Error getting average grade: " << e.what() << std::endl;
        }

        return 0.0;
    }

    std::vector<Student> getTopStudents(int limit) {

        if (limit <= 0) {
            std::cerr << "Validation error: Limit must be positive" << std::endl;
            return {};
        }

        if (limit > 100) {
            std::cerr << "Warning: Limit too high, using maximum 100" << std::endl;
            limit = 100;
        }

        std::vector<Student> topStudents;
        const std::string sql = R"(
            SELECT students.id, students.name, students.email, students.group_name
            FROM students
            JOIN grades ON students.id = grades.student_id
            GROUP BY students.id
            HAVING COUNT(grades.id) > 0
            ORDER BY AVG(grades.grade) DESC
            LIMIT ?
        )";

        try {
            PreparedStatement stmt(db, sql);
            stmt.bindInt(1, limit);

            while (stmt.next()) {
                Student student;

                student.id = stmt.getInt(0);
                student.name = stmt.getText(1);
                student.email = stmt.getText(2);
                student.group_name = stmt.getText(3);

                topStudents.push_back(student);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error getting top students: " << e.what() << std::endl;
        }

        return topStudents;
    }

    // ---------------------------------------------------- НОВОЕ

    // Пакетная вставка студентов (с транзакцией)
    bool batchInsertStudents(const std::vector<std::tuple<std::string, std::string, std::string>>& students) {
        if (students.empty()) {
            std::cerr << "No students to insert" << std::endl;
            return false;
        }

        // Начинаем транзакцию
        if (!execute("BEGIN TRANSACTION;")) {
            std::cerr << "Failed to start transaction" << std::endl;
            return false;
        }

        const std::string sql = "INSERT INTO students (name, email, group_name) VALUES (?, ?, ?)";

        try {
            PreparedStatement stmt(db, sql);
            int insertedCount = 0;

            for (const auto& student : students) {
                const auto& [name, email, group] = student;

                // Валидация данных
                auto validation = validator.validateStudent(name, email, group);
                if (!validation.isValid) {
                    std::cerr << "Skipping invalid student: " << name
                              << " - " << validation.errorMessage << std::endl;
                    continue;
                }

                stmt.bindText(1, name);
                stmt.bindText(2, email);
                stmt.bindText(3, group);

                if (!stmt.execute()) {
                    std::cerr << "Failed to insert student: " << name << std::endl;
                    // Продолжаем вставлять остальных студентов
                } else {
                    insertedCount++;
                }

                stmt.reset();
            }

            // Фиксируем транзакцию
            if (!execute("COMMIT;")) {
                throw std::runtime_error("Failed to commit transaction");
            }

            std::cout << "Batch insert completed. Inserted " << insertedCount
                      << " out of " << students.size() << " students" << std::endl;
            return insertedCount > 0;

        } catch (const std::exception& e) {
            // Откатываем транзакцию при ошибке
            execute("ROLLBACK;");
            std::cerr << "Batch insert failed: " << e.what() << std::endl;
            return false;
        }
    }

    // Пакетная вставка оценок
    bool batchInsertGrades(const std::vector<std::tuple<int, std::string, int>>& grades) {
        if (grades.empty()) {
            std::cerr << "No grades to insert" << std::endl;
            return false;
        }

        // Начинаем транзакцию
        if (!execute("BEGIN TRANSACTION;")) {
            std::cerr << "Failed to start transaction" << std::endl;
            return false;
        }

        const std::string sql = "INSERT INTO grades (student_id, subject, grade) VALUES (?, ?, ?)";

        try {
            PreparedStatement stmt(db, sql);
            int insertedCount = 0;

            for (const auto& grade : grades) {
                const auto& [studentId, subject, gradeValue] = grade;

                // Валидация данных
                auto validation = validator.validateGradeData(studentId, subject, gradeValue);
                if (!validation.isValid) {
                    std::cerr << "Skipping invalid grade for student " << studentId
                              << " - " << validation.errorMessage << std::endl;
                    continue;
                }

                stmt.bindInt(1, studentId);
                stmt.bindText(2, subject);
                stmt.bindInt(3, gradeValue);

                if (!stmt.execute()) {
                    std::cerr << "Failed to insert grade for student: " << studentId << std::endl;
                } else {
                    insertedCount++;
                }

                stmt.reset();
            }

            // Фиксируем транзакцию
            if (!execute("COMMIT;")) {
                throw std::runtime_error("Failed to commit transaction");
            }

            std::cout << "Batch insert grades completed. Inserted " << insertedCount
                      << " out of " << grades.size() << " grades" << std::endl;
            return insertedCount > 0;

        } catch (const std::exception& e) {
            // Откатываем транзакцию при ошибке
            execute("ROLLBACK;");
            std::cerr << "Batch insert grades failed: " << e.what() << std::endl;
            return false;
        }
    }

    // Метод для генерации тестовых данных
    std::vector<std::tuple<std::string, std::string, std::string>> generateTestStudents(int count) {
        std::vector<std::tuple<std::string, std::string, std::string>> students;
        students.reserve(count);

        std::vector<std::string> groups = {"CS-101", "CS-102", "CS-103", "CS-201", "CS-202"};

        for (int i = 1; i <= count; i++) {
            std::string name = "Student_" + std::to_string(i);
            std::string email = "student" + std::to_string(i) + "@university.edu";
            std::string group = groups[i % groups.size()];

            students.emplace_back(name, email, group);
        }

        return students;
    }

    // Метод для генерации тестовых оценок
    std::vector<std::tuple<int, std::string, int>> generateTestGrades(int studentCount, int gradesPerStudent) {
        std::vector<std::tuple<int, std::string, int>> grades;
        grades.reserve(studentCount * gradesPerStudent);

        std::vector<std::string> subjects = {"Mathematics", "Physics", "Chemistry", "Computer Science", "English"};

        for (int studentId = 1; studentId <= studentCount; studentId++) {
            for (int j = 0; j < gradesPerStudent; j++) {
                std::string subject = subjects[j % subjects.size()];
                int grade = 50 + (rand() % 51); // Оценки от 50 до 100

                grades.emplace_back(studentId, subject, grade);
            }
        }

        return grades;
    }

    // Метод для тестирования производительности
    void performanceTest(int studentCount = 1000, int gradesPerStudent = 5) {
        std::cout << "\n=== Performance Test ===" << std::endl;
        std::cout << "Testing with " << studentCount << " students and "
                  << (studentCount * gradesPerStudent) << " grades" << std::endl;

        // Генерация тестовых данных
        auto testStudents = generateTestStudents(studentCount);
        auto testGrades = generateTestGrades(studentCount, gradesPerStudent);

        // Тест 1: Пакетная вставка студентов
        auto time1 = std::chrono::high_resolution_clock::now();
        bool batchResult = batchInsertStudents(testStudents);
        auto time2 = std::chrono::high_resolution_clock::now();
        auto batchTime = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);

        if (batchResult) {
            std::cout << "Batch insert students: " << batchTime.count() << " ms" << std::endl;
        }

        // Тест 2: Пакетная вставка оценок
        time1 = std::chrono::high_resolution_clock::now();
        batchResult = batchInsertGrades(testGrades);
        time2 = std::chrono::high_resolution_clock::now();
        batchTime = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);

        if (batchResult) {
            std::cout << "Batch insert grades: " << batchTime.count() << " ms" << std::endl;
        }

        // Тест 3: Поиск по группе (с индексом)
        time1 = std::chrono::high_resolution_clock::now();
        auto studentsByGroup = getStudentsByGroup("CS-101");
        time2 = std::chrono::high_resolution_clock::now();
        auto searchTime = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);
        std::cout << "Search by group: " << searchTime.count() << " ms, found: "
                  << studentsByGroup.size() << " students" << std::endl;

        // Тест 4: Получение средней оценки (с индексом)
        time1 = std::chrono::high_resolution_clock::now();
        double avg = getAverageGradeBySubject("Mathematics");
        time2 = std::chrono::high_resolution_clock::now();
        searchTime = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);
        std::cout << "Average grade calculation: " << searchTime.count() << " ms, average: " << avg << std::endl;

        // Тест 5: Получение лучших студентов
        time1 = std::chrono::high_resolution_clock::now();
        auto topStudents = getTopStudents(10);
        time2 = std::chrono::high_resolution_clock::now();
        searchTime = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);
        std::cout << "Get top students: " << searchTime.count() << " ms" << std::endl;

        // Тест 6: Получение всех студентов
        time1 = std::chrono::high_resolution_clock::now();
        auto allStudents = getAllStudents();
        time2 = std::chrono::high_resolution_clock::now();
        searchTime = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);
        std::cout << "Get all students: " << searchTime.count() << " ms, total: "
                  << allStudents.size() << " students" << std::endl;
    }

};

int main() {
    DatabaseManager dbManager;

    // Инициализация базы данных
    if (!dbManager.initialize("university.db")) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }

    // Создаем индексы для оптимизации
    dbManager.createIndexes();

    StudentRepository repo(dbManager.getHandle());

    // Очищаем таблицы перед тестами
    dbManager.clearAllTables();

    std::cout << "=== Testing Student Repository ===" << std::endl;

    // Тест 1: Базовые операции
    std::cout << "\n1. Testing basic operations..." << std::endl;
    repo.addStudent("Иван Иванов", "ivan@university.edu", "ИТ-101");
    repo.addStudent("Мария Петрова", "maria@university.edu", "ИТ-102");

    auto allStudents = repo.getAllStudents();
    std::cout << "Total students: " << allStudents.size() << std::endl;

    // Тест 2: Добавление с оценками
    std::cout << "\n2. Testing student with grades..." << std::endl;
    std::vector<Grade> grades = {{"Математика", 85},
                                 {"Физика",     90}};
    repo.addStudentWithGrades("Алексей Сидоров", "alex@university.edu", "ИТ-101", grades);

    // Тест 3: Пакетная вставка
    std::cout << "\n3. Testing batch insert..." << std::endl;
    std::vector<std::tuple<std::string, std::string, std::string>> batchStudents = {
            {"Студент 1", "batch1@test.com", "Группа А"},
            {"Студент 2", "batch2@test.com", "Группа Б"},
            {"Студент 3", "batch3@test.com", "Группа А"},
            {"Студент 4", "batch4@test.com", "Группа Б"}
    };

    repo.batchInsertStudents(batchStudents);

    // Тест 4: Поиск по группе
    std::cout << "\n4. Testing search by group..." << std::endl;
    auto groupAStudents = repo.getStudentsByGroup("Группа А");
    std::cout << "Students in Группа А: " << groupAStudents.size() << std::endl;

    // Тест 5: Производительность с большим объемом данных
    std::cout << "\n5. Performance test with large dataset..." << std::endl;
    repo.performanceTest(1000, 3); // 1000 студентов, по 3 оценки каждый

    // Тест 6: Индексы и статистика
    std::cout << "\n6. Testing indexes and statistics..." << std::endl;
    double avgMath = repo.getAverageGradeBySubject("Математика");
    std::cout << "Average grade in Математика: " << avgMath << std::endl;

    auto topStudents = repo.getTopStudents(5);
    std::cout << "Top 5 students:" << std::endl;
    for (const auto &student : topStudents) {
        std::cout << "  - " << student.name << " (" << student.group_name << ")" << std::endl;
    }

    // Тест 7: Обновление и удаление
    std::cout << "\n7. Testing update and delete..." << std::endl;
    if (!allStudents.empty()) {
        bool updated = repo.updateStudent(allStudents[0].id, "Новое Имя", "new@email.com", "Новая Группа");
        std::cout << "Update result: " << (updated ? "success" : "failed") << std::endl;

        bool deleted = repo.deleteStudent(allStudents[0].id);
        std::cout << "Delete result: " << (deleted ? "success" : "failed") << std::endl;
    }

    std::cout << "\n=== All tests completed ===" << std::endl;
}