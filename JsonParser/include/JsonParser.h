//
// Created by Kirill.
//

#pragma once
#include "JsonNode.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <sstream>

class JsonParseException : public std::runtime_error {
public:
    JsonParseException(const std::string& message, size_t position, const std::string& context)
            : std::runtime_error(message), m_position(position), m_context(context) {}

    size_t getPosition() const { return m_position; }
    const std::string& getContext() const { return m_context; }

    std::string getDetailedMessage() const {
        std::ostringstream oss;
        oss << what() << " at position " << m_position;
        if (!m_context.empty()) {
            oss << " (context: '" << m_context << "')";
        }
        return oss.str();
    }

private:
    size_t m_position;
    std::string m_context;
};

class JsonParser {
public:
    JsonParser() = default;
    std::shared_ptr<JsonNode> parse(const std::string& json);

private:
    std::string m_input;
    size_t m_pos = 0;
    size_t m_length = 0;

    void skipWhitespace();
    char peek() const;
    char next();

    std::shared_ptr<JsonNode> parseValue();
    std::shared_ptr<JsonNode> parseObject();
    std::shared_ptr<JsonNode> parseArray();
    std::shared_ptr<JsonNode> parseString();
    std::shared_ptr<JsonNode> parseBoolean();
    std::shared_ptr<JsonNode> parseNull();
    std::shared_ptr<JsonNode> parseNumber();

    std::string parseStringValue();

    // Методы для получения контекста ошибки
    std::string getErrorContext(size_t pos, size_t context_size = 20) const;
    void throwParseError(const std::string& message) const;
};