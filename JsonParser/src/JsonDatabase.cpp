//
// Created by Anastasia.
//

#include "../include/JsonDatabase.h"
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstring>

JsonDatabase::JsonDatabase(const std::string& db_path) : m_db(nullptr) {
    int rc = sqlite3_open(db_path.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Cannot open database: " + std::string(sqlite3_errmsg(m_db)));
    }
    createTables();
}

JsonDatabase::~JsonDatabase() {
    if (m_db) {
        sqlite3_close(m_db);
    }
}

void JsonDatabase::createTables() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS files (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            filename TEXT NOT NULL,
            file_size INTEGER,
            was_array INTEGER,
            object_count INTEGER,
            total_nodes INTEGER,
            parsing_time_ms INTEGER,
            loaded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS json_objects (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_id INTEGER NOT NULL,
            object_index INTEGER NOT NULL,
            original_json TEXT,
            FOREIGN KEY (file_id) REFERENCES files(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS json_nodes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            object_id INTEGER NOT NULL,
            path TEXT NOT NULL,
            node_type TEXT NOT NULL,
            string_value TEXT,
            numeric_value REAL,
            boolean_value INTEGER,
            is_null INTEGER DEFAULT 0,
            depth INTEGER NOT NULL,
            FOREIGN KEY (object_id) REFERENCES json_objects(id) ON DELETE CASCADE
        );

        CREATE INDEX IF NOT EXISTS idx_files_filename ON files(filename);
        CREATE INDEX IF NOT EXISTS idx_objects_file ON json_objects(file_id);
        CREATE INDEX IF NOT EXISTS idx_nodes_object ON json_nodes(object_id);
        CREATE INDEX IF NOT EXISTS idx_nodes_path ON json_nodes(path);
    )";

    char* err_msg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::string error = err_msg ? err_msg : "Unknown error";
        sqlite3_free(err_msg);
        throw std::runtime_error("Failed to create tables: " + error);
    }
}

bool JsonDatabase::saveResult(const ParsingResult& result, const std::string& source_filename) {
    if (!m_db || result.objects.empty()) {
        return false;
    }

    sqlite3_exec(m_db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    try {
        sqlite3_int64 file_id = saveFileInfo(source_filename, result);

        for (size_t i = 0; i < result.objects.size(); ++i) {
            saveJsonObject(file_id, i, result.objects[i]);
        }

        sqlite3_exec(m_db, "COMMIT", nullptr, nullptr, nullptr);
        return true;

    } catch (const std::exception& e) {
        sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
        std::cerr << "Error saving to database: " << e.what() << std::endl;
        return false;
    }
}

sqlite3_int64 JsonDatabase::saveFileInfo(const std::string& filename, const ParsingResult& result) {
    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO files (filename, was_array, object_count, total_nodes, parsing_time_ms)
        VALUES (?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, filename.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, result.wasOriginalArray ? 1 : 0);
    sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(result.totalObjects));
    sqlite3_bind_int64(stmt, 4, static_cast<sqlite3_int64>(result.totalNodes));
    sqlite3_bind_int64(stmt, 5, static_cast<sqlite3_int64>(result.parsingTime.count()));

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert file info");
    }

    sqlite3_int64 file_id = sqlite3_last_insert_rowid(m_db);
    sqlite3_finalize(stmt);

    return file_id;
}

void JsonDatabase::saveJsonObject(sqlite3_int64 file_id, size_t index, const std::shared_ptr<JsonNode>& node) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO json_objects (file_id, object_index) VALUES (?, ?)";

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare object statement");
    }

    sqlite3_bind_int64(stmt, 1, file_id);
    sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(index));

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert json object");
    }

    sqlite3_int64 object_id = sqlite3_last_insert_rowid(m_db);
    sqlite3_finalize(stmt);

    saveNodeRecursive(object_id, node, "", 0);
}

void JsonDatabase::saveNodeRecursive(sqlite3_int64 object_id, const std::shared_ptr<JsonNode>& node,
                                     const std::string& path, int depth) {
    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO json_nodes (object_id, path, node_type, string_value,
                               numeric_value, boolean_value, is_null, depth)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare node statement");
    }

    sqlite3_bind_int64(stmt, 1, object_id);
    sqlite3_bind_text(stmt, 2, path.c_str(), -1, SQLITE_STATIC);

    std::string node_type;
    std::string string_value;
    double numeric_value = 0.0;
    int boolean_value = 0;
    int is_null = 0;

    if (node->isNull()) {
        node_type = "null";
        is_null = 1;
    } else if (node->isBoolean()) {
        node_type = "boolean";
        boolean_value = node->asBoolean() ? 1 : 0;
    } else if (node->isNumber()) {
        node_type = "number";
        numeric_value = node->asNumber();
    } else if (node->isString()) {
        node_type = "string";
        string_value = node->asString();
    } else if (node->isObject()) {
        node_type = "object";
    } else if (node->isArray()) {
        node_type = "array";
    }

    sqlite3_bind_text(stmt, 3, node_type.c_str(), -1, SQLITE_STATIC);

    if (node_type == "string") {
        sqlite3_bind_text(stmt, 4, string_value.c_str(), -1, SQLITE_STATIC);
    } else {
        sqlite3_bind_null(stmt, 4);
    }

    if (node_type == "number") {
        sqlite3_bind_double(stmt, 5, numeric_value);
    } else {
        sqlite3_bind_null(stmt, 5);
    }

    if (node_type == "boolean") {
        sqlite3_bind_int(stmt, 6, boolean_value);
    } else {
        sqlite3_bind_null(stmt, 6);
    }

    sqlite3_bind_int(stmt, 7, is_null);
    sqlite3_bind_int(stmt, 8, depth);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert node");
    }

    sqlite3_finalize(stmt);

    if (node->isObject()) {
        auto& obj = node->asObject();
        for (const auto& [key, child] : obj) {
            std::string child_path = path.empty() ? key : path + "." + key;
            saveNodeRecursive(object_id, child, child_path, depth + 1);
        }
    } else if (node->isArray()) {
        auto& arr = node->asArray();
        for (size_t i = 0; i < arr.size(); ++i) {
            std::string child_path = path.empty() ? std::to_string(i) : path + "[" + std::to_string(i) + "]";
            saveNodeRecursive(object_id, arr[i], child_path, depth + 1);
        }
    }
}

std::vector<std::shared_ptr<JsonNode>> JsonDatabase::loadFromDatabase([[maybe_unused]] const std::string& source_filename) {
    return std::vector<std::shared_ptr<JsonNode>>(); // Упрощённая реализация
}

std::vector<std::string> JsonDatabase::getSavedFiles() {
    std::vector<std::string> files;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT filename, loaded_at FROM files ORDER BY loaded_at DESC";

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* filename = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* loaded_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            files.push_back(std::string(filename) + " (загружен: " + loaded_at + ")");
        }
    }
    sqlite3_finalize(stmt);
    return files;
}