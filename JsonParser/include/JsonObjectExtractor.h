//
// Created by Kirill.
//

#pragma once
#include "BufferedJsonReader.h"
#include <string>
#include <memory>

#pragma once

#include "BufferedJsonReader.h"
#include <string>

class JsonObjectExtractor {
public:
    JsonObjectExtractor(const std::string& filename);
    ~JsonObjectExtractor() = default;

    bool extractNextObject(std::string& json_str);  // Объявление правильное
    bool isRootArray() const { return m_reader.isRootArray(); }
    size_t getBytesRead() const { return m_reader.getBytesRead(); }
    bool isDone() const { return !m_reader.hasMoreObjects(); }

private:
    BufferedJsonReader m_reader;
};