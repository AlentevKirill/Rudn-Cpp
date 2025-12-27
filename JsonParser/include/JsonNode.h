//
// Created by Kirill.
//

#pragma once
#include <memory>
#include <variant>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

class JsonNode : public std::enable_shared_from_this<JsonNode> {
public:
    enum class Type {
        Null,
        Boolean,
        Number,
        String,
        Object,
        Array
    };

    // Конструкторы
    JsonNode();
    explicit JsonNode(std::nullptr_t);
    explicit JsonNode(bool b);
    explicit JsonNode(int i);
    explicit JsonNode(double d);
    explicit JsonNode(const char* s);
    explicit JsonNode(const std::string& s);
    explicit JsonNode(std::string&& s);
    explicit JsonNode(std::unordered_map<std::string, std::shared_ptr<JsonNode>>&& obj);
    explicit JsonNode(std::vector<std::shared_ptr<JsonNode>>&& arr);

    // Проверка типа
    bool isNull() const noexcept;
    bool isBoolean() const noexcept;
    bool isNumber() const noexcept;
    bool isString() const noexcept;
    bool isObject() const noexcept;
    bool isArray() const noexcept;

    // Получение значений
    bool asBoolean() const;
    double asNumber() const;
    const std::string& asString() const;
    std::unordered_map<std::string, std::shared_ptr<JsonNode>>& asObject();
    const std::unordered_map<std::string, std::shared_ptr<JsonNode>>& asObject() const;
    std::vector<std::shared_ptr<JsonNode>>& asArray();
    const std::vector<std::shared_ptr<JsonNode>>& asArray() const;

    // Доступ
    std::shared_ptr<JsonNode> operator[](const std::string& key);
    const std::shared_ptr<JsonNode> operator[](const std::string& key) const;
    std::shared_ptr<JsonNode> operator[](size_t index);
    const std::shared_ptr<JsonNode> operator[](size_t index) const;

    // Рекурсивный доступ по пути
    std::shared_ptr<JsonNode> getByPath(const std::string& path);
    const std::shared_ptr<JsonNode> getByPath(const std::string& path) const;

    // Вывод
    void print(std::ostream& os = std::cout, int indent = 0, bool compact = false) const;
    std::string toString(bool pretty = false) const;

    // Подсчёт узлов
    size_t countNodes() const;

    // Сериализация
    static std::string escapeString(const std::string& str);

private:
    Type type;
    std::variant<
            std::nullptr_t,
            bool,
            double,
            std::string,
            std::unordered_map<std::string, std::shared_ptr<JsonNode>>,
    std::vector<std::shared_ptr<JsonNode>>
    > value;
};
