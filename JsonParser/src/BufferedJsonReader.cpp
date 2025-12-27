//
// Created by Kirill.
//

#include "../include/BufferedJsonReader.h"
#include <iostream>
#include <cctype>
#include <algorithm>
#include <cstring>

BufferedJsonReader::BufferedJsonReader(const std::string& filename)
        : m_filename(filename) {

    m_file.open(filename, std::ios::binary);
    if (!m_file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    initialize();
}

BufferedJsonReader::~BufferedJsonReader() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

void BufferedJsonReader::initialize() const {
    // Читаем первые байты, чтобы определить, корневой ли это массив
    char buffer[256];
    m_file.read(buffer, 256);
    size_t bytes_read = static_cast<size_t>(m_file.gcount());

    for (size_t i = 0; i < bytes_read; ++i) {
        if (!std::isspace(static_cast<unsigned char>(buffer[i]))) {
            const_cast<bool&>(m_isRootArray) = (buffer[i] == '[');
            break;
        }
    }

    m_file.seekg(0, std::ios::beg);
    m_currentFilePos = 0;
    m_initialized = true;

    std::cout << "📄 Файл: " << m_filename
              << ", корневой массив: " << (m_isRootArray ? "да" : "нет") << std::endl;
}

bool BufferedJsonReader::hasMoreObjects() const {
    if (!m_initialized) return false;

    // Если в очереди есть готовые объекты
    if (!m_objectQueue.empty()) return true;

    // Пытаемся найти следующий объект
    return tryFindNextObject();
}

bool BufferedJsonReader::extractNextObject(std::string& json_str) {
    if (m_objectQueue.empty()) {
        // Пытаемся найти объект
        if (!tryFindNextObject()) {
            json_str.clear();
            return false;
        }
    }

    if (m_objectQueue.empty()) {
        json_str.clear();
        return false;
    }

    auto& obj = m_objectQueue.front();
    json_str = std::move(obj.data);
    m_objectQueue.pop();

    // Обновляем позицию для частичных данных
    if (obj.endPos > m_currentFilePos) {
        m_currentFilePos = obj.endPos;
    }

    return true;
}

bool BufferedJsonReader::tryFindNextObject() const {
    // Сначала проверяем, можно ли завершить объект из частичных данных
    if (tryCompleteObjectFromPartial()) {
        return true;
    }

    // Читаем файл по частям, пока не найдем объект
    size_t attempts = 0;
    while (!m_file.eof() && m_objectQueue.empty() && attempts < 1000) {
        std::string chunk = readFileChunk();
        if (chunk.empty()) break;

        processChunk(chunk);
        attempts++;
    }

    return !m_objectQueue.empty();
}

std::string BufferedJsonReader::readFileChunk() const {
    if (m_file.eof()) return "";

    std::string chunk(BUFFER_SIZE, '\0');
    m_file.read(&chunk[0], BUFFER_SIZE);
    size_t bytes_read = static_cast<size_t>(m_file.gcount());

    if (bytes_read > 0) {
        chunk.resize(bytes_read);
        m_bytesRead += bytes_read;

        // Отладочный вывод
        if (m_bytesRead % (10 * 1024 * 1024) < BUFFER_SIZE) { // Каждые 10MB
            std::cout << "📊 Прочитано: " << m_bytesRead / 1024 / 1024 << " MB" << std::endl;
        }
    }

    return chunk;
}

void BufferedJsonReader::processChunk(const std::string& chunk) const {
    // Быстрое добавление (резервируем память заранее)
    m_partialData.reserve(m_partialData.size() + chunk.size());
    m_partialData += chunk;

    // Быстрый поиск объектов
    const char* data = m_partialData.data();
    size_t dataSize = m_partialData.size();
    size_t searchPos = 0;
    size_t objectsFound = 0;
    const size_t MAX_OBJECTS_PER_CHUNK = 1000; // Ограничим, чтобы не блокировать

    while (searchPos < dataSize && objectsFound < MAX_OBJECTS_PER_CHUNK) {
        // Быстрый поиск начала объекта
        const char* startPtr = static_cast<const char*>(
                memchr(data + searchPos, '{', dataSize - searchPos));

        if (!startPtr) {
            break; // Больше нет объектов в этом чанке
        }

        size_t objectStart = startPtr - data;

        // Быстрый поиск конца объекта
        size_t objectEnd = findObjectEndFast(m_partialData, objectStart);

        if (objectEnd != std::string::npos) {
            // Извлекаем объект
            std::string objectData = m_partialData.substr(objectStart, objectEnd - objectStart + 1);

            // Быстрая проверка валидности
            if (objectData.size() >= 2 &&
                objectData.front() == '{' &&
                objectData.back() == '}') {

                // Вычисляем позиции в файле
                size_t fileStartPos = m_currentFilePos - dataSize + objectStart;
                size_t fileEndPos = fileStartPos + objectData.length() - 1;

                addCompleteObject(objectData, fileStartPos, fileEndPos);
                objectsFound++;
            }

            // Перескакиваем на позицию после объекта
            searchPos = objectEnd + 1;
        } else {
            // Объект неполный, останавливаемся
            break;
        }
    }

    // Оставляем необработанный хвост
    if (searchPos > 0 && searchPos < dataSize) {
        m_partialData = m_partialData.substr(searchPos);
    } else if (searchPos >= dataSize) {
        m_partialData.clear();
    }

    // Ограничиваем размер partialData
    if (m_partialData.size() > BUFFER_SIZE * 5) {
        // Оставляем только последние 20KB для поиска начала объектов
        size_t keepFrom = m_partialData.size() - BUFFER_SIZE;
        if (keepFrom < m_partialData.size()) {
            m_partialData = m_partialData.substr(keepFrom);
        }
    }
}

bool BufferedJsonReader::tryCompleteObjectFromPartial() const {
    if (m_partialData.empty()) return false;

    size_t searchPos = 0;

    // Пропускаем пробелы и запятые
    while (searchPos < m_partialData.length() &&
           (std::isspace(static_cast<unsigned char>(m_partialData[searchPos])) ||
            m_partialData[searchPos] == ',')) {
        searchPos++;
    }

    if (searchPos >= m_partialData.length()) return false;

    if (m_partialData[searchPos] == '{') {
        size_t objectEnd = findObjectEnd(m_partialData, searchPos);

        if (objectEnd != std::string::npos) {
            // Нашли полный объект в partialData
            std::string objectData = m_partialData.substr(searchPos, objectEnd - searchPos + 1);

            // Вычисляем позиции в файле
            size_t fileStartPos = m_currentFilePos - m_partialData.length() + searchPos;
            size_t fileEndPos = fileStartPos + objectData.length() - 1;

            addCompleteObject(objectData, fileStartPos, fileEndPos);

            // Удаляем обработанную часть
            m_partialData = m_partialData.substr(objectEnd + 1);
            return true;
        }
    }

    return false;
}


size_t BufferedJsonReader::findObjectEnd(const std::string& data, size_t startPos) const {
    int braceDepth = 0;
    int bracketDepth = 0;
    bool inString = false;
    bool escapeNext = false;

    // Быстрая проверка - если строка короткая
    if (startPos >= data.length()) return std::string::npos;

    // Оптимизация: используем прямой доступ к данным
    const char* ptr = data.data() + startPos;
    size_t length = data.length() - startPos;

    for (size_t i = 0; i < length; ++i) {
        char c = ptr[i];

        if (escapeNext) {
            escapeNext = false;
            continue;
        }

        if (inString) {
            if (c == '\\') {
                escapeNext = true;
            } else if (c == '"') {
                inString = false;
            }
            continue;
        }

        // Быстрый путь для наиболее частых случаев
        switch (c) {
            case '"':
                inString = true;
                break;

            case '{':
                braceDepth++;
                break;

            case '}':
                braceDepth--;
                if (braceDepth == 0) {
                    // Проверяем следующий символ
                    size_t pos = startPos + i;
                    if (pos + 1 < data.length()) {
                        char next = data[pos + 1];
                        if (next == ',' || next == ']' || std::isspace(next)) {
                            return pos;
                        }
                    } else {
                        return pos; // Конец данных
                    }
                }
                break;

            case '[':
                bracketDepth++;
                break;

            case ']':
                bracketDepth--;
                break;
        }
    }

    return std::string::npos;
}

void BufferedJsonReader::addCompleteObject(const std::string& data,
                                           size_t startPos,
                                           size_t endPos) const {
    // Очищаем от лишних пробелов в начале/конце
    size_t first = data.find_first_not_of(" \t\n\r");
    size_t last = data.find_last_not_of(" \t\n\r");

    if (first != std::string::npos && last != std::string::npos) {
        std::string cleaned = data.substr(first, last - first + 1);

        // Проверяем, что это действительно объект
        if (!cleaned.empty() && cleaned[0] == '{' && cleaned[cleaned.length() - 1] == '}') {
            m_objectQueue.push({startPos + first, endPos - (data.length() - last - 1), cleaned});

            // Отладочный вывод
            static size_t totalObjects = 0;
            totalObjects++;
            if (totalObjects % 1000 == 0) {
                std::cout << "🎯 Найдено объектов: " << totalObjects
                          << ", в очереди: " << m_objectQueue.size()
                          << ", прочитано: " << m_bytesRead / 1024 / 1024 << " MB" << std::endl;
            }
        } else {
            std::cout << "⚠️  Отброшен некорректный объект (не начинается/заканчивается скобками)" << std::endl;
        }
    } else {
        std::cout << "⚠️  Отброшен пустой объект" << std::endl;
    }
}

size_t BufferedJsonReader::findObjectEndFast(const std::string& data, size_t startPos) const {
    int braceDepth = 0;
    bool inString = false;
    bool escapeNext = false;

    const char* ptr = data.data() + startPos;
    size_t length = data.length() - startPos;

    for (size_t i = 0; i < length; ++i) {
        char c = ptr[i];

        if (escapeNext) {
            escapeNext = false;
            continue;
        }

        if (inString) {
            if (c == '\\') {
                escapeNext = true;
            } else if (c == '"') {
                inString = false;
            }
            continue;
        }

        if (c == '"') {
            inString = true;
        } else if (c == '{') {
            braceDepth++;
        } else if (c == '}') {
            braceDepth--;
            if (braceDepth == 0) {
                return startPos + i;
            }
        }
        // Игнорируем скобки массивов для скорости
    }

    return std::string::npos;
}