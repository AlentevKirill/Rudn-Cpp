//
// Created by Anastasia.
//

#pragma once

#include "JsonStreamProcessor.h"
#include "../lib/sqlite3.h"
#include <string>
#include <vector>
#include <memory>

class JsonDatabase {
public:
    explicit JsonDatabase(const std::string& db_path);
    ~JsonDatabase();

    bool saveResult(const ParsingResult& result, const std::string& source_filename);
    std::vector<std::shared_ptr<JsonNode>> loadFromDatabase(const std::string& source_filename);
    std::vector<std::string> getSavedFiles();

private:
    sqlite3* m_db;

    void createTables();
    sqlite3_int64 saveFileInfo(const std::string& filename, const ParsingResult& result);
    void saveJsonObject(sqlite3_int64 file_id, size_t index, const std::shared_ptr<JsonNode>& node);
    void saveNodeRecursive(sqlite3_int64 object_id, const std::shared_ptr<JsonNode>& node,
                           const std::string& path, int depth);
    std::vector<std::shared_ptr<JsonNode>> loadJsonObjects(sqlite3_int64 file_id);
    std::shared_ptr<JsonNode> loadNodeTree(sqlite3_int64 object_id, sqlite3_int64 parent_id = 0,
                                           const std::string& parent_path = "");
};