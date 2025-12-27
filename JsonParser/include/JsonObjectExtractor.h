//
// Created by Kirill.
//

#pragma once
#include "BufferedJsonReader.h"
#include <string>
#include <memory>

class JsonObjectExtractor {
public:
    JsonObjectExtractor(const std::string& filename);
    ~JsonObjectExtractor() = default;

    bool extractNextObject(std::string& json_str);
    bool isRootArray() const { return m_reader.isRootArray(); }
    size_t getBytesRead() const { return m_reader.getBytesRead(); }
    bool isDone() const { return !m_reader.hasMoreObjects(); }

private:
    BufferedJsonReader m_reader;
};