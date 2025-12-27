//
// Created by Kirill.
//

#pragma once
#include "JsonStreamProcessor.h"
#include "JsonDatabase.h"
#include <string>
#include <vector>
#include <functional>

class ConsoleInterface {
public:
    ConsoleInterface();
    void run();
    std::string nodeTypeToString(const std::shared_ptr<JsonNode>& node);

private:
    JsonStreamProcessor m_processor;
    JsonDatabase m_db;
    std::string m_currentFile;

    void handleLoad(const std::string& filename);
    void handleGet(const std::vector<std::string>& tokens);
    void handleStats();
    void handleSave();
    void handleList(const std::vector<std::string>& tokens);
    void handleClear();
    void handleFiles();
    void handleRetry();

    void printWelcome();
    void printHelp();
    void printError(const std::exception& e);
    void printFileError(const std::string& filename, const std::string& error);

    std::vector<std::string> tokenize(const std::string& input);
    std::string formatBytes(size_t bytes);

    bool validateFilePath(const std::string& path);
    bool askRetry();
    void printNodeStructure(
            const std::shared_ptr<JsonNode>& node,
            const std::string& path,
            int indent
            );

    void printStructureWithTypes(const std::shared_ptr<JsonNode>& node,
                                 const std::string& prefix = "",
                                 int depth = 0);
};