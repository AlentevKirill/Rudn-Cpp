//
// Created by Kirill.
//

#pragma once
#include "JsonNode.h"
#include "JsonParser.h"
#include "JsonObjectExtractor.h"
#include "ThreadSafeQueue.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

struct ParsingResult {
    bool wasOriginalArray = false;
    std::vector<std::shared_ptr<JsonNode>> objects;
    std::chrono::milliseconds parsingTime{0};
    size_t totalObjects = 0;
    size_t totalNodes = 0;

    void clear();
};

class JsonStreamProcessor {
public:
    JsonStreamProcessor();
    ~JsonStreamProcessor();

    const ParsingResult& processFile(const std::string& filename);
    std::shared_ptr<JsonNode> get(size_t index, const std::string& path = "") const;
    const ParsingResult& getResult() const;
    void clear();

private:
    ParsingResult m_result;

    void processSingleObject(JsonObjectExtractor& extractor);
    size_t calculateTotalNodes() const;
    void processStreaming(JsonObjectExtractor& extractor);

    // Обработка ошибок
    void cleanupOnError();
};
