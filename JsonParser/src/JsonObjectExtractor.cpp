//
// Created by Kirill.
//

#include "../include/JsonObjectExtractor.h"

JsonObjectExtractor::JsonObjectExtractor(const std::string& filename)
        : m_reader(filename) {}

bool JsonObjectExtractor::extractNextObject(std::string& json_str) {
    return m_reader.extractNextObject(json_str);
}