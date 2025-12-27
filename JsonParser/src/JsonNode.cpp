//
// Created by Kirill.
//

#include "../include/JsonNode.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

JsonNode::JsonNode() : type(Type::Null), value(nullptr) {}
JsonNode::JsonNode(std::nullptr_t) : type(Type::Null), value(nullptr) {}
JsonNode::JsonNode(bool b) : type(Type::Boolean), value(b) {}
JsonNode::JsonNode(int i) : type(Type::Number), value(static_cast<double>(i)) {}
JsonNode::JsonNode(double d) : type(Type::Number), value(d) {}
JsonNode::JsonNode(const char* s) : type(Type::String), value(std::string(s)) {}
JsonNode::JsonNode(const std::string& s) : type(Type::String), value(s) {}
JsonNode::JsonNode(std::string&& s) : type(Type::String), value(std::move(s)) {}
JsonNode::JsonNode(std::unordered_map<std::string, std::shared_ptr<JsonNode>>&& obj)
        : type(Type::Object), value(std::move(obj)) {}
JsonNode::JsonNode(std::vector<std::shared_ptr<JsonNode>>&& arr)
        : type(Type::Array), value(std::move(arr)) {}

bool JsonNode::isNull() const noexcept { return type == Type::Null; }
bool JsonNode::isBoolean() const noexcept { return type == Type::Boolean; }
bool JsonNode::isNumber() const noexcept { return type == Type::Number; }
bool JsonNode::isString() const noexcept { return type == Type::String; }
bool JsonNode::isObject() const noexcept { return type == Type::Object; }
bool JsonNode::isArray() const noexcept { return type == Type::Array; }

bool JsonNode::asBoolean() const {
    if (!isBoolean()) throw std::bad_variant_access();
    return std::get<bool>(value);
}

double JsonNode::asNumber() const {
    if (!isNumber()) throw std::bad_variant_access();
    return std::get<double>(value);
}

const std::string& JsonNode::asString() const {
    if (!isString()) throw std::bad_variant_access();
    return std::get<std::string>(value);
}

std::unordered_map<std::string, std::shared_ptr<JsonNode>>& JsonNode::asObject() {
    if (!isObject()) throw std::bad_variant_access();
    return std::get<std::unordered_map<std::string, std::shared_ptr<JsonNode>>>(value);
}

const std::unordered_map<std::string, std::shared_ptr<JsonNode>>& JsonNode::asObject() const {
    if (!isObject()) throw std::bad_variant_access();
    return std::get<std::unordered_map<std::string, std::shared_ptr<JsonNode>>>(value);
}

std::vector<std::shared_ptr<JsonNode>>& JsonNode::asArray() {
    if (!isArray()) throw std::bad_variant_access();
    return std::get<std::vector<std::shared_ptr<JsonNode>>>(value);
}

const std::vector<std::shared_ptr<JsonNode>>& JsonNode::asArray() const {
    if (!isArray()) throw std::bad_variant_access();
    return std::get<std::vector<std::shared_ptr<JsonNode>>>(value);
}

std::shared_ptr<JsonNode> JsonNode::operator[](const std::string& key) {
    if (!isObject()) {
        return nullptr;
    }
    auto& obj = asObject();
    auto it = obj.find(key);
    return it != obj.end() ? it->second : nullptr;
}

const std::shared_ptr<JsonNode> JsonNode::operator[](const std::string& key) const {
    if (!isObject()) {
        return nullptr;
    }
    auto& obj = asObject();
    auto it = obj.find(key);
    return it != obj.end() ? it->second : nullptr;
}

std::shared_ptr<JsonNode> JsonNode::operator[](size_t index) {
    if (!isArray() || index >= asArray().size()) return nullptr;
    return asArray()[index];
}

const std::shared_ptr<JsonNode> JsonNode::operator[](size_t index) const {
    if (!isArray() || index >= asArray().size()) return nullptr;
    return asArray()[index];
}

std::shared_ptr<JsonNode> JsonNode::getByPath(const std::string& path) {
    std::istringstream iss(path);
    std::string token;
    std::shared_ptr<JsonNode> current = shared_from_this();

    while (std::getline(iss, token, '.')) {
        if (!current) {
            return nullptr;
        }

        // Проверяем, есть ли массив в токене (например: handling_instructions[0])
        size_t bracket_pos = token.find('[');

        if (bracket_pos != std::string::npos) {
            // Обработка массива: phones[0] или handling_instructions[0]
            std::string key_part = token.substr(0, bracket_pos);
            std::string index_str = token.substr(bracket_pos + 1);

            // Удаляем закрывающую скобку
            if (!index_str.empty() && index_str.back() == ']') {
                index_str.pop_back();
            }

            if (!key_part.empty()) {
                // Есть ключ до скобки: phones[0]
                current = (*current)[key_part];
            }
            // Если key_part пустой, то это случай [0] - уже обрабатываем массив

            if (current && current->isArray()) {
                try {
                    size_t index = std::stoul(index_str);
                    if (index < current->asArray().size()) {
                        current = (*current)[index];
                    } else {
                        return nullptr;
                    }
                } catch (...) {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
        } else {
            // Обычное поле объекта
            current = (*current)[token];
        }

        if (!current) {
            return nullptr;
        }
    }

    return current;
}

const std::shared_ptr<JsonNode> JsonNode::getByPath(const std::string& path) const {
    return const_cast<JsonNode*>(this)->getByPath(path);
}

void JsonNode::print(std::ostream& os, int indent, bool compact) const {
    std::string ind(indent * 2, ' ');

    switch (type) {
        case Type::Null:
            os << ind << "null";
            break;
        case Type::Boolean:
            os << ind << (asBoolean() ? "true" : "false");
            break;
        case Type::Number:
            os << ind << asNumber();
            break;
        case Type::String:
            os << ind << '"' << escapeString(asString()) << '"';
            break;
        case Type::Object: {
            auto& obj = asObject();
            if (obj.empty()) {
                os << ind << "{}";
                break;
            }
            os << ind << "{\n";
            bool first = true;
            for (const auto& [key, val] : obj) {
                if (!first) os << ",\n";
                os << ind << "  \"" << key << "\": ";
                if (compact) val->print(os, 0, true);
                else val->print(os, indent + 1, false);
                first = false;
            }
            os << "\n" << ind << "}";
            break;
        }
        case Type::Array: {
            auto& arr = asArray();
            if (arr.empty()) {
                os << ind << "[]";
                break;
            }
            os << ind << "[\n";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) os << ",\n";
                arr[i]->print(os, indent + 1, compact);
            }
            os << "\n" << ind << "]";
            break;
        }
    }
}

std::string JsonNode::toString(bool pretty) const {
    std::ostringstream oss;
    print(oss, 0, !pretty);
    return oss.str();
}

size_t JsonNode::countNodes() const {
    size_t count = 1;

    if (isObject()) {
        for (const auto& [key, child] : asObject()) {
            if (child) count += child->countNodes();
        }
    } else if (isArray()) {
        for (const auto& child : asArray()) {
            if (child) count += child->countNodes();
        }
    }

    return count;
}

std::string JsonNode::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    std::ostringstream oss;
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                        << static_cast<int>(c);
                    result += oss.str();
                } else {
                    result += c;
                }
        }
    }
    return result;
}