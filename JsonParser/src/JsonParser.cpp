//
// Created by Kirill.
//

#include "../include/JsonParser.h"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <atomic>

std::shared_ptr<JsonNode> JsonParser::parse(const std::string& json) {
    static size_t parse_counter = 0;
    size_t current_count = ++parse_counter;

    // Логируем только первые 10 парсингов для отладки
    if (current_count <= 10) {
        std::cout << "[Parse #" << current_count << "] Начало парсинга, размер: "
                  << json.size() << " байт" << std::endl;

        // Показываем первый и последний символы
        if (!json.empty()) {
            char first = json[0];
            char last = json[json.size() - 1];
            std::cout << "[Parse #" << current_count << "] Первый символ: '"
                      << first << "', последний: '" << last << "'" << std::endl;

            // Показываем фрагмент для слишком длинных строк
            if (json.size() > 100) {
                std::string sample = json.substr(0, 50) + "..." + json.substr(json.size() - 50);
                std::cout << "[Parse #" << current_count << "] Фрагмент: " << sample << std::endl;
            }
        }
    }

    try {
        m_input = json;
        m_pos = 0;
        m_length = json.length();

        skipWhitespace();

        if (m_pos >= m_length) {
            if (current_count <= 10) {
                std::cout << "[Parse #" << current_count << "] Пустая строка после пробелов" << std::endl;
            }
            return std::make_shared<JsonNode>(nullptr);
        }

        auto result = parseValue();

        if (current_count <= 10) {
            std::cout << "[Parse #" << current_count << "] Успешно распарсено" << std::endl;
        }

        return result;
    } catch (const JsonParseException& e) {
        // Логируем ошибки
        std::cerr << "❌ [Parse #" << current_count << "] Ошибка парсинга: "
                  << e.getDetailedMessage() << std::endl;

        // Показываем фрагмент JSON для первых 5 ошибок
        if (current_count <= 5) {
            size_t sample_size = std::min(json.size(), size_t(200));
            std::string sample = json.substr(0, sample_size);
            if (json.size() > sample_size) sample += "...";
            std::cerr << "   JSON фрагмент: " << sample << std::endl;
        }

        throw;
    } catch (const std::exception& e) {
        std::cerr << "❌ [Parse #" << current_count << "] Неожиданная ошибка: "
                  << e.what() << std::endl;
        throw JsonParseException(e.what(), m_pos, getErrorContext(m_pos));
    }
}

void JsonParser::skipWhitespace() {
    while (m_pos < m_length && std::isspace(static_cast<unsigned char>(m_input[m_pos]))) {
        ++m_pos;
    }
}

char JsonParser::peek() const {
    return m_pos < m_length ? m_input[m_pos] : '\0';
}

char JsonParser::next() {
    if (m_pos >= m_length) {
        throwParseError("Unexpected end of JSON");
    }
    return m_input[m_pos++];
}

std::shared_ptr<JsonNode> JsonParser::parseValue() {
    skipWhitespace();
    char c = peek();

    switch (c) {
        case '{': return parseObject();
        case '[': return parseArray();
        case '"': return parseString();
        case 't': case 'f': return parseBoolean();
        case 'n': return parseNull();
        case '-': case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return parseNumber();
        default:
            throwParseError(std::string("Unexpected character: '") + c + "'");
    }

    return nullptr; // никогда не выполнится
}

std::shared_ptr<JsonNode> JsonParser::parseObject() {
    std::unordered_map<std::string, std::shared_ptr<JsonNode>> obj;

    size_t start_pos = m_pos;
    next(); // '{'
    skipWhitespace();

    if (peek() == '}') {
        next(); // '}'
        return std::make_shared<JsonNode>(std::move(obj));
    }

    while (true) {
        skipWhitespace();

        // Ключ должен быть строкой
        if (peek() != '"') {
            throwParseError("Expected string key in object");
        }

        auto key = parseStringValue();
        skipWhitespace();

        // Двоеточие после ключа
        if (next() != ':') {
            throwParseError("Expected ':' after key in object");
        }

        skipWhitespace();
        auto value = parseValue();
        obj.emplace(std::move(key), std::move(value));

        skipWhitespace();
        char c = next();

        if (c == '}') break;
        if (c != ',') {
            throwParseError("Expected ',' or '}' in object");
        }
    }

    return std::make_shared<JsonNode>(std::move(obj));
}

std::shared_ptr<JsonNode> JsonParser::parseArray() {
    std::vector<std::shared_ptr<JsonNode>> arr;

    size_t start_pos = m_pos;
    next(); // '['
    skipWhitespace();

    if (peek() == ']') {
        next(); // ']'
        return std::make_shared<JsonNode>(std::move(arr));
    }

    while (true) {
        skipWhitespace();
        arr.push_back(parseValue());
        skipWhitespace();

        char c = next();
        if (c == ']') break;
        if (c != ',') {
            throwParseError("Expected ',' or ']' in array");
        }
    }

    return std::make_shared<JsonNode>(std::move(arr));
}

std::shared_ptr<JsonNode> JsonParser::parseString() {
    return std::make_shared<JsonNode>(parseStringValue());
}

std::string JsonParser::parseStringValue() {
    size_t start_pos = m_pos;
    std::string result;
    next(); // открывающая кавычка

    while (m_pos < m_length) {
        char c = m_input[m_pos++];

        if (c == '"') {
            return result;
        }

        if (c == '\\') {
            // Escape-последовательность
            if (m_pos >= m_length) {
                throwParseError("Unterminated escape sequence");
            }

            char escape_char = m_input[m_pos++];
            switch (escape_char) {
                case '"':  result += '"'; break;
                case '\\': result += '\\'; break;
                case '/':  result += '/'; break;
                case 'b':  result += '\b'; break;
                case 'f':  result += '\f'; break;
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                case 'u': {
                    // Unicode escape
                    if (m_length - m_pos < 4) {
                        throwParseError("Invalid Unicode escape sequence");
                    }

                    std::string hex_code = m_input.substr(m_pos, 4);
                    m_pos += 4;

                    // Проверяем, что это валидный hex
                    for (char hc : hex_code) {
                        if (!std::isxdigit(static_cast<unsigned char>(hc))) {
                            throwParseError("Invalid hex digit in Unicode escape");
                        }
                    }

                    try {
                        unsigned int code = std::stoul(hex_code, nullptr, 16);
                        if (code < 128) {
                            result += static_cast<char>(code);
                        } else {
                            // Упрощённая обработка UTF-8
                            result += '?';
                        }
                    } catch (...) {
                        throwParseError("Invalid Unicode code point");
                    }
                    break;
                }
                default:
                    throwParseError(std::string("Invalid escape sequence: \\") + escape_char);
            }
        } else {
            // Проверяем на управляющие символы
            if (static_cast<unsigned char>(c) < 32 && c != '\n' && c != '\r' && c != '\t') {
                throwParseError("Unescaped control character in string");
            }
            result += c;
        }
    }

    throwParseError("Unterminated string");
}

std::shared_ptr<JsonNode> JsonParser::parseBoolean() {
    size_t start_pos = m_pos;

    if (m_input.compare(m_pos, 4, "true") == 0) {
        m_pos += 4;
        return std::make_shared<JsonNode>(true);
    } else if (m_input.compare(m_pos, 5, "false") == 0) {
        m_pos += 5;
        return std::make_shared<JsonNode>(false);
    }

    throwParseError("Invalid boolean value");
}

std::shared_ptr<JsonNode> JsonParser::parseNull() {
    size_t start_pos = m_pos;

    if (m_input.compare(m_pos, 4, "null") == 0) {
        m_pos += 4;
        return std::make_shared<JsonNode>(nullptr);
    }

    throwParseError("Invalid null value");
}

std::shared_ptr<JsonNode> JsonParser::parseNumber() {
    size_t start_pos = m_pos;

    // Обрабатываем знак
    if (peek() == '-') {
        ++m_pos;
    }

    // Целая часть
    if (peek() == '0') {
        ++m_pos;
        if (std::isdigit(peek())) {
            throwParseError("Leading zeros are not allowed in numbers");
        }
    } else if (std::isdigit(peek())) {
        while (std::isdigit(peek())) {
            ++m_pos;
        }
    } else {
        throwParseError("Invalid number format");
    }

    // Дробная часть
    if (peek() == '.') {
        ++m_pos;
        if (!std::isdigit(peek())) {
            throwParseError("Invalid number format: expected digit after decimal point");
        }
        while (std::isdigit(peek())) {
            ++m_pos;
        }
    }

    // Экспоненциальная часть
    if (peek() == 'e' || peek() == 'E') {
        ++m_pos;
        if (peek() == '+' || peek() == '-') {
            ++m_pos;
        }
        if (!std::isdigit(peek())) {
            throwParseError("Invalid number format: expected digit in exponent");
        }
        while (std::isdigit(peek())) {
            ++m_pos;
        }
    }

    std::string num_str = m_input.substr(start_pos, m_pos - start_pos);

    try {
        double value = std::stod(num_str);
        return std::make_shared<JsonNode>(value);
    } catch (...) {
        throwParseError("Invalid number: " + num_str);
    }
}

std::string JsonParser::getErrorContext(size_t pos, size_t context_size) const {
    size_t start = (pos > context_size) ? pos - context_size : 0;
    size_t end = std::min(pos + context_size, m_length);

    std::string context = m_input.substr(start, end - start);

    // Заменяем непечатаемые символы
    for (char& c : context) {
        if (static_cast<unsigned char>(c) < 32) {
            c = '.';
        }
    }

    return context;
}

void JsonParser::throwParseError(const std::string& message) const {
    throw JsonParseException(message, m_pos, getErrorContext(m_pos));
}