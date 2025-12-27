//
// Created by Kirill.
//

#include "../include/ConsoleInterface.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <limits>

namespace fs = std::filesystem;

ConsoleInterface::ConsoleInterface() : m_db("json_data.db") {
    printWelcome();
}

void ConsoleInterface::run() {
    while (true) {
        std::cout << "\njson> ";
        std::string input;

        if (!std::getline(std::cin, input)) {
            break;
        }

        if (input.empty()) {
            continue;
        }

        auto tokens = tokenize(input);
        if (tokens.empty()) {
            continue;
        }

        std::string command = tokens[0];

        try {
            if (command == "load" && tokens.size() > 1) {
                handleLoad(tokens[1]);
            } else if (command == "get") {
                handleGet(tokens);
            } else if (command == "stats") {
                handleStats();
            } else if (command == "save") {
                handleSave();
            } else if (command == "list") {
                handleList(tokens);
            } else if (command == "clear") {
                handleClear();
            } else if (command == "files") {
                handleFiles();
            } else if (command == "retry") {
                handleRetry();
            } else if (command == "help" || command == "?") {
                printHelp();
            } else if (command == "exit" || command == "quit") {
                std::cout << "Завершение работы.\n";
                break;
            } else {
                std::cout << "❌ Неизвестная команда. Введите 'help' для списка команд.\n";
            }
        } catch (const std::exception& e) {
            printError(e);
        }
    }
}

void ConsoleInterface::handleLoad(const std::string& filename) {
    if (!validateFilePath(filename)) {
        return;
    }

    std::cout << "📂 Загрузка файла: " << filename << " (потоковое чтение 4KB буферами)...\n";
    std::cout << "   Используется " << std::thread::hardware_concurrency() << " потоков\n";

    try {
        auto start_time = std::chrono::high_resolution_clock::now();

        const auto& result = m_processor.processFile(filename);
        m_currentFile = filename;

        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "✅ Загрузка завершена!\n";
        std::cout << "   Тип файла: "
                  << (result.wasOriginalArray ? "массив объектов" : "одиночный объект") << "\n";
        std::cout << "   Объектов загружено: " << result.totalObjects << "\n";
        std::cout << "   Всего узлов: " << result.totalNodes << "\n";
        std::cout << "   Время обработки: " << total_time.count() << " мс\n";
        std::cout << "   Скорость: "
                  << (static_cast<double>(result.totalNodes) / total_time.count() * 1000)
                  << " узлов/сек\n";

        // Выводим структуру только если есть объекты
        if (result.totalObjects > 0) {
            std::cout << "\n   Структура первого объекта:\n";
            auto firstObject = m_processor.get(0, "");
            if (firstObject) {
                // Ограничим глубину вывода для больших структур
                printStructureWithTypes(firstObject, "", 1);
            }
        } else {
            std::cout << "\n⚠️  Внимание: не удалось загрузить ни одного объекта\n";
            std::cout << "   Возможные причины:\n";
            std::cout << "   1. Файл пустой или содержит только пробелы\n";
            std::cout << "   2. Некорректный JSON формат\n";
            std::cout << "   3. Проблемы с кодировкой файла\n";
        }

    } catch (const JsonParseException& e) {
        printFileError(filename, e.getDetailedMessage());
        m_processor.clear();

        if (askRetry()) {
            handleLoad(filename);
        }
    } catch (const std::runtime_error& e) {
        printFileError(filename, e.what());
        m_processor.clear();

        if (askRetry()) {
            handleLoad(filename);
        }
    } catch (const std::exception& e) {
        printFileError(filename, e.what());
        m_processor.clear();
    }
}

void ConsoleInterface::handleGet(const std::vector<std::string>& tokens) {
    const auto& result = m_processor.getResult();

    if (result.objects.empty()) {
        std::cout << "❌ Нет загруженных данных. Сначала загрузите файл.\n";
        return;
    }

    if (tokens.size() == 1) {
        std::cout << "Доступно " << result.totalObjects << " объектов.\n";
        std::cout << "Используйте: get <индекс> <путь>\n";
        std::cout << "Пример: get 0 metadata.purpose\n";
        std::cout << "Пример: get 0 applicant_identity.applicant_identity.full_name.first_name\n";
        return;
    }

    try {
        size_t index = 0;
        std::string path;

        if (tokens.size() == 2) {
            index = 0;
            path = tokens[1];
        } else if (tokens.size() >= 3) {
            index = std::stoul(tokens[1]);
            path = tokens[2];
        }

        if (index >= result.totalObjects) {
            std::cout << "❌ Индекс выходит за пределы (0-"
                      << result.totalObjects - 1 << ")\n";
            return;
        }

        auto node = m_processor.get(index, path);

        if (node) {
            std::cout << "✅ Найдено!\n";
            std::cout << "Результат:\n";

            if (node->isNull()) {
                std::cout << "null\n";
            } else if (node->isBoolean()) {
                std::cout << (node->asBoolean() ? "true" : "false") << "\n";
            } else if (node->isNumber()) {
                std::cout << node->asNumber() << "\n";
            } else if (node->isString()) {
                std::cout << "\"" << node->asString() << "\"\n";
            } else if (node->isObject()) {
                std::cout << "Объект с " << node->asObject().size() << " полями\n";
            } else if (node->isArray()) {
                std::cout << "Массив с " << node->asArray().size() << " элементами\n";
            }
        } else {
            std::cout << "❌ Не найден\n";
            std::cout << "\nВозможные причины:\n";
            std::cout << "1. Неправильный путь\n";
            std::cout << "2. Объект по индексу " << index << " - не объект, а "
                      << nodeTypeToString(m_processor.get(index, "")) << "\n";

            auto root = m_processor.get(index, "");
            if (root && root->isObject()) {
                std::cout << "\nДоступные поля:\n";
                for (const auto& [key, val] : root->asObject()) {
                    std::cout << "  - " << key << "\n";
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "❌ Ошибка: " << e.what() << "\n";
    }
}

void ConsoleInterface::handleStats() {
    const auto& result = m_processor.getResult();

    if (result.objects.empty()) {
        std::cout << "Нет загруженных данных.\n";
        return;
    }

    std::cout << "=== Статистика ===\n";
    std::cout << "Текущий файл: "
              << (m_currentFile.empty() ? "не загружен" : m_currentFile) << "\n";
    std::cout << "Тип файла: JSON массив объектов\n";
    std::cout << "Загружено объектов: " << result.totalObjects << "\n";
    std::cout << "Всего узлов в памяти: " << result.totalNodes << "\n";
    std::cout << "Время парсинга: " << result.parsingTime.count() << " мс\n";

    size_t estimated_memory = result.totalNodes * 128;
    std::cout << "Примерное использование памяти: "
              << formatBytes(estimated_memory) << "\n";

    if (!result.objects.empty()) {
        std::cout << "\nСтруктура первого объекта (#0):\n";
        auto node = m_processor.get(0, "");
        if (node && node->isObject()) {
            auto& obj = node->asObject();
            std::cout << "  Корневые поля: " << obj.size() << "\n";

            std::cout << "  Типы полей:\n";
            for (const auto& [key, val] : obj) {
                std::cout << "    - " << key << ": ";
                if (val->isNull()) std::cout << "null";
                else if (val->isBoolean()) std::cout << "boolean";
                else if (val->isNumber()) std::cout << "number";
                else if (val->isString()) std::cout << "string";
                else if (val->isObject()) std::cout << "object";
                else if (val->isArray()) std::cout << "array";

                if (val->isArray()) {
                    std::cout << " [" << val->asArray().size() << "]";
                }
                std::cout << "\n";
            }
        }
    }
}

void ConsoleInterface::handleSave() {
    const auto& result = m_processor.getResult();

    if (result.objects.empty()) {
        std::cout << "❌ Нет данных для сохранения.\n";
        return;
    }

    if (m_currentFile.empty()) {
        std::cout << "❌ Неизвестный исходный файл.\n";
        return;
    }

    std::cout << "💾 Сохранение в базу данных...\n";

    try {
        if (m_db.saveResult(result, m_currentFile)) {
            std::cout << "✅ Данные успешно сохранены в базу.\n";
        } else {
            std::cout << "❌ Ошибка при сохранении в базу.\n";
        }
    } catch (const std::exception& e) {
        std::cout << "❌ Ошибка сохранения: " << e.what() << "\n";
    }
}

void ConsoleInterface::handleList(const std::vector<std::string>& tokens) {
    const auto& result = m_processor.getResult();

    if (result.objects.empty()) {
        std::cout << "Нет загруженных объектов.\n";
        return;
    }

    size_t start = 0;
    size_t count = 5;

    if (tokens.size() > 1) {
        try {
            start = std::stoul(tokens[1]);
            if (tokens.size() > 2) {
                count = std::stoul(tokens[2]);
            }
        } catch (...) {
            std::cout << "❌ Неверные аргументы. Используйте: list [начало] [количество]\n";
            return;
        }
    }

    if (start >= result.totalObjects) {
        std::cout << "❌ Начальный индекс выходит за пределы.\n";
        return;
    }

    size_t end = std::min(start + count, result.totalObjects);

    std::cout << "Объекты " << start << "-" << (end - 1) << " из "
              << result.totalObjects << ":\n";

    for (size_t i = start; i < end; ++i) {
        auto node = m_processor.get(i, "");
        std::cout << "\n  [" << i << "] ";

        if (node->isObject()) {
            auto& obj = node->asObject();
            std::cout << "Объект с " << obj.size() << " ключами\n";

            if (i == start) {
                std::cout << "  Структура (одинакова для всех объектов):\n";
                printStructureWithTypes(node, "", 2);
            }
        } else {
            std::cout << "Тип: ";
            if (node->isNull()) std::cout << "null";
            else if (node->isBoolean()) std::cout << "boolean";
            else if (node->isNumber()) std::cout << "number";
            else if (node->isString()) std::cout << "string";
            else if (node->isArray()) std::cout << "array";
            std::cout << "\n";
        }
    }

    if (result.totalObjects > 1) {
        std::cout << "\nℹ️  Все объекты в массиве имеют одинаковую структуру.\n";
    }
}

void ConsoleInterface::handleClear() {
    m_processor.clear();
    m_currentFile.clear();
    std::cout << "🗑️  Все данные очищены из памяти.\n";
}

void ConsoleInterface::handleFiles() {
    auto files = m_db.getSavedFiles();

    if (files.empty()) {
        std::cout << "В базе данных нет сохранённых файлов.\n";
        return;
    }

    std::cout << "Сохранённые файлы:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << "  " << i << ". " << files[i] << "\n";
    }
}

void ConsoleInterface::handleRetry() {
    if (m_currentFile.empty()) {
        std::cout << "❌ Нет предыдущего файла для повторной загрузки.\n";
        return;
    }

    std::cout << "🔄 Повторная загрузка файла: " << m_currentFile << "...\n";
    handleLoad(m_currentFile);
}

void ConsoleInterface::printWelcome() {
    std::cout << "========================================\n";
    std::cout << "     Многопоточный JSON парсер v1.0     \n";
    std::cout << "========================================\n";
    std::cout << "Особенности:\n";
    std::cout << "  • Потоковое чтение больших файлов (4KB буферы)\n";
    std::cout << "  • Многопоточный парсинг массивов\n";
    std::cout << "  • Древовидная структура в памяти\n";
    std::cout << "  • Сохранение в SQLite\n";
    std::cout << "  • Доступ по пути: user.address.city\n";
    std::cout << "  • Обработка ошибок с автоматическим восстановлением\n";
    std::cout << "========================================\n";
}

void ConsoleInterface::printHelp() {
    std::cout << "Доступные команды:\n";
    std::cout << "  load <file.json>    - загрузить JSON файл\n";
    std::cout << "  get [index] [path]  - получить данные\n";
    std::cout << "  list [start] [cnt]  - список объектов\n";
    std::cout << "  stats               - статистика\n";
    std::cout << "  save                - сохранить в SQLite\n";
    std::cout << "  files               - список сохранённых файлов\n";
    std::cout << "  clear               - очистить память\n";
    std::cout << "  retry               - повторно загрузить последний файл\n";
    std::cout << "  help, ?             - эта справка\n";
    std::cout << "  exit, quit          - выход\n";
    std::cout << "\nПримеры:\n";
    std::cout << "  load data.json\n";
    std::cout << "  get 0 user.name\n";
    std::cout << "  get user.address.city\n";
    std::cout << "  list 0 5\n";
}

void ConsoleInterface::printError(const std::exception& e) {
    std::cout << "❌ Ошибка: " << e.what() << "\n";
}

void ConsoleInterface::printFileError(const std::string& filename, const std::string& error) {
    std::cout << "❌ Ошибка при обработке файла '" << filename << "':\n";
    std::cout << "   " << error << "\n";
    std::cout << "   Все структуры очищены. Используйте команду 'retry' для повторной загрузки.\n";
}

std::vector<std::string> ConsoleInterface::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_quotes = false;

    for (char c : input) {
        if (c == '"') {
            in_quotes = !in_quotes;
            token += c;
        } else if (std::isspace(c) && !in_quotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string ConsoleInterface::formatBytes(size_t bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB"};
    size_t i = 0;
    double dbl_bytes = static_cast<double>(bytes);

    while (dbl_bytes >= 1024.0 && i < 3) {
        dbl_bytes /= 1024.0;
        i++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << dbl_bytes << " " << suffixes[i];
    return oss.str();
}

std::string ConsoleInterface::nodeTypeToString(const std::shared_ptr<JsonNode>& node) {
    if (!node) return "nullptr";

    if (node->isNull()) return "null";
    if (node->isBoolean()) return "boolean";
    if (node->isNumber()) return "number";
    if (node->isString()) return "string";
    if (node->isObject()) return "object";
    if (node->isArray()) return "array";

    return "unknown";
}

void ConsoleInterface::printStructureWithTypes(const std::shared_ptr<JsonNode>& node,
                                               const std::string& prefix,
                                               int depth) {
    if (!node) {
        std::cout << std::string(depth * 2, ' ') << prefix << ": null\n";
        return;
    }

    std::string indent(depth * 2, ' ');

    if (node->isNull()) {
        std::cout << indent << prefix << ": null\n";
    } else if (node->isBoolean()) {
        std::cout << indent << prefix << ": boolean\n";
    } else if (node->isNumber()) {
        std::cout << indent << prefix << ": number\n";
    } else if (node->isString()) {
        std::cout << indent << prefix << ": string\n";
    } else if (node->isObject()) {
        std::cout << indent << prefix << (prefix.empty() ? "" : ": ") << "object {\n";
        auto& obj = node->asObject();
        for (const auto& [key, child] : obj) {
            printStructureWithTypes(child, key, depth + 1);
        }
        std::cout << indent << "}\n";
    } else if (node->isArray()) {
        std::cout << indent << prefix << (prefix.empty() ? "" : ": ") << "array [\n";
        auto& arr = node->asArray();

        if (!arr.empty()) {
            for (size_t i = 0; i < arr.size(); ++i) {
                printStructureWithTypes(arr[i], "[" + std::to_string(i) + "]", depth + 1);
            }
        } else {
            std::cout << std::string((depth + 1) * 2, ' ') << "пустой массив\n";
        }
        std::cout << indent << "]\n";
    }
}

bool ConsoleInterface::validateFilePath(const std::string& path) {
    try {
        if (!fs::exists(path)) {
            std::cout << "❌ Файл не существует: " << path << "\n";
            return false;
        }

        if (!fs::is_regular_file(path)) {
            std::cout << "❌ Путь не является файлом: " << path << "\n";
            return false;
        }

        size_t file_size = fs::file_size(path);
        if (file_size == 0) {
            std::cout << "❌ Файл пустой: " << path << "\n";
            return false;
        }

        if (file_size > 10ULL * 1024 * 1024 * 1024) {
            std::cout << "❌ Файл слишком большой (максимум 10GB): " << path << "\n";
            return false;
        }

        return true;
    } catch (const fs::filesystem_error& e) {
        std::cout << "❌ Ошибка доступа к файлу: " << e.what() << "\n";
        return false;
    }
}

bool ConsoleInterface::askRetry() {
    std::cout << "\nПовторить загрузку файла? (y/n): ";
    std::string answer;
    std::getline(std::cin, answer);

    std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
    return answer == "y" || answer == "yes" || answer == "да";
}