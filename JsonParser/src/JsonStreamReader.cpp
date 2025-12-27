//
// Created by Kirill.
//

#include "../include/JsonStreamReader.h"
#include <iostream>
#include <cctype>
#include <cstring>

JsonStreamReader::JsonStreamReader(const std::string& filename)
        : m_filename(filename) {

    m_file.open(filename, std::ios::binary);
    if (!m_file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    initParser();
}

JsonStreamReader::~JsonStreamReader() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

void JsonStreamReader::initParser() {
    char buffer[256];
    m_file.read(buffer, 256);
    size_t bytes_read = static_cast<size_t>(m_file.gcount());

    for (size_t i = 0; i < bytes_read; ++i) {
        if (!std::isspace(static_cast<unsigned char>(buffer[i]))) {
            m_isRootArray = (buffer[i] == '[');
            m_bracketDepth = m_isRootArray ? 1 : 0;
            m_inRootArray = m_isRootArray;

            m_file.seekg(0, std::ios::beg);
            m_totalBytesRead = 0;
            break;
        }
    }

    if (bytes_read == 0) {
        throw std::runtime_error("File is empty");
    }
}

bool JsonStreamReader::getNextObject(std::string& json_str) {
    // Проверяем очередь готовых объектов
    if (!m_bufferQueue.empty()) {
        json_str = std::move(m_bufferQueue.front());
        m_bufferQueue.pop();
        m_objectsFound++;
        return true;
    }

    // Читаем файл порциями
    char buffer[BUFFER_SIZE];

    while (!m_done) {
        m_file.read(buffer, BUFFER_SIZE);
        size_t bytes_read = static_cast<size_t>(m_file.gcount());

        if (bytes_read == 0) {
            m_done = true;

            // Проверяем, есть ли частичный объект в конце
            if (!m_partialObject.empty() && m_braceDepth == 0 &&
                (!m_inRootArray || (m_inRootArray && m_bracketDepth == 1))) {
                // Извлекаем объект из частичного буфера
                size_t start = m_partialObject.find_first_not_of(" \t\n\r,");
                if (start != std::string::npos) {
                    size_t end = m_partialObject.find_last_not_of(" \t\n\r,");
                    if (end != std::string::npos) {
                        std::string obj = m_partialObject.substr(start, end - start + 1);
                        if (!obj.empty() && obj != "[" && obj != "]") {
                            json_str = std::move(obj);
                            m_objectsFound++;
                            m_partialObject.clear();
                            resetState();
                            return true;
                        }
                    }
                }
            }
            break;
        }

        m_totalBytesRead += bytes_read;

        try {
            processBuffer(buffer, bytes_read);
        } catch (const std::exception& e) {
            std::cerr << "❌ Ошибка обработки буфера: " << e.what() << std::endl;
            resetState();
            m_partialObject.clear();
            continue;
        }

        // Проверяем, появились ли готовые объекты
        if (!m_bufferQueue.empty()) {
            json_str = std::move(m_bufferQueue.front());
            m_bufferQueue.pop();
            m_objectsFound++;
            return true;
        }
    }

    return false;
}

void JsonStreamReader::processBuffer(const char* data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        char c = data[i];
        m_partialObject += c;
        processChar(c);

        // Проверяем, завершился ли объект
        if (tryCompleteObject()) {
            savePartialForNextBuffer(m_partialObject);
        }
    }
}

void JsonStreamReader::processChar(char c) {
    switch (m_state) {
        case ParserState::ROOT:
            if (m_inRootArray) {
                if (c == '{' && m_bracketDepth == 1) {
                    m_state = ParserState::IN_OBJECT;
                    m_braceDepth = 1;
                } else if (c == '[') {
                    m_bracketDepth++;
                } else if (c == ']') {
                    m_bracketDepth--;
                }
            } else {
                if (c == '{') {
                    m_state = ParserState::IN_OBJECT;
                    m_braceDepth = 1;
                }
            }
            break;

        case ParserState::IN_OBJECT:
            if (c == '"') {
                m_state = ParserState::IN_STRING;
            } else if (c == '{') {
                m_braceDepth++;
            } else if (c == '}') {
                m_braceDepth--;
                if (m_braceDepth == 0) {
                    m_state = m_inRootArray ? ParserState::ROOT : ParserState::ROOT;
                }
            } else if (c == '[') {
                m_state = ParserState::IN_ARRAY;
                m_bracketDepth++;
            } else if (std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '.') {
                m_state = ParserState::IN_NUMBER;
            } else if (c == 't') {
                m_state = ParserState::IN_TRUE;
                m_tokenPos = 1;
                m_tokenBuffer[0] = 't';
            } else if (c == 'f') {
                m_state = ParserState::IN_FALSE;
                m_tokenPos = 1;
                m_tokenBuffer[0] = 'f';
            } else if (c == 'n') {
                m_state = ParserState::IN_NULL;
                m_tokenPos = 1;
                m_tokenBuffer[0] = 'n';
            }
            break;

        case ParserState::IN_STRING:
            if (c == '\\') {
                m_state = ParserState::ESCAPE;
            } else if (c == '"') {
                m_state = ParserState::IN_OBJECT;
            }
            break;

        case ParserState::ESCAPE:
            m_state = ParserState::IN_STRING;
            break;

        case ParserState::IN_NUMBER:
            if (!std::isdigit(static_cast<unsigned char>(c)) && c != '.' &&
                c != 'e' && c != 'E' && c != '+' && c != '-') {
                m_state = ParserState::IN_OBJECT;
                // Обрабатываем символ снова, так как он не часть числа
                processChar(c);
            }
            break;

        case ParserState::IN_TRUE:
            if (m_tokenPos < 4) {
                m_tokenBuffer[m_tokenPos++] = c;
                if (m_tokenPos == 4) {
                    if (std::strncmp(m_tokenBuffer, "true", 4) == 0) {
                        m_state = ParserState::IN_OBJECT;
                    } else {
                        throw std::runtime_error("Invalid token: expected 'true'");
                    }
                }
            }
            break;

        case ParserState::IN_FALSE:
            if (m_tokenPos < 5) {
                m_tokenBuffer[m_tokenPos++] = c;
                if (m_tokenPos == 5) {
                    if (std::strncmp(m_tokenBuffer, "false", 5) == 0) {
                        m_state = ParserState::IN_OBJECT;
                    } else {
                        throw std::runtime_error("Invalid token: expected 'false'");
                    }
                }
            }
            break;

        case ParserState::IN_NULL:
            if (m_tokenPos < 4) {
                m_tokenBuffer[m_tokenPos++] = c;
                if (m_tokenPos == 4) {
                    if (std::strncmp(m_tokenBuffer, "null", 4) == 0) {
                        m_state = ParserState::IN_OBJECT;
                    } else {
                        throw std::runtime_error("Invalid token: expected 'null'");
                    }
                }
            }
            break;

        case ParserState::IN_ARRAY:
            if (c == ']') {
                m_bracketDepth--;
                if (m_bracketDepth == 0) {
                    m_state = ParserState::IN_OBJECT;
                }
            } else if (c == '[') {
                m_bracketDepth++;
            } else if (c == '"') {
                m_state = ParserState::IN_STRING;
            } else if (c == '{') {
                m_state = ParserState::IN_OBJECT;
                m_braceDepth++;
            }
            break;
    }
}

bool JsonStreamReader::tryCompleteObject() {
    if (m_inRootArray) {
        // В корневом массиве объект завершён, когда:
        // 1. Мы внутри объекта и глубина объектов = 0
        // 2. Глубина массивов = 1 (всё ещё в корневом массиве)
        // 3. Следующий символ - запятая, пробел или закрывающая скобка
        if (m_state == ParserState::ROOT && m_braceDepth == 0 && m_bracketDepth == 1) {
            // Нашли полный объект в корневом массиве
            return true;
        }
    } else {
        // Одиночный объект завершён, когда глубина объектов = 0
        if (m_braceDepth == 0 && m_state == ParserState::ROOT) {
            return true;
        }
    }
    return false;
}

void JsonStreamReader::savePartialForNextBuffer(const std::string& data) {
    // Находим последний завершённый объект
    size_t object_end = 0;

    if (m_inRootArray) {
        // Ищем позицию, где глубина объектов стала 0
        int temp_braces = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == '{') temp_braces++;
            else if (data[i] == '}') temp_braces--;

            if (temp_braces == 0 && i > 0) {
                // Нашли конец объекта
                object_end = i + 1;

                // Пропускаем запятые и пробелы после объекта
                while (object_end < data.size() &&
                       (std::isspace(static_cast<unsigned char>(data[object_end])) ||
                        data[object_end] == ',')) {
                    object_end++;
                }
                break;
            }
        }
    } else {
        // Для одиночного объекта - весь буфер
        object_end = data.size();
    }

    if (object_end > 0 && object_end <= data.size()) {
        // Извлекаем объект
        std::string object = data.substr(0, object_end);

        // Очищаем от лишних пробелов
        size_t first = object.find_first_not_of(" \t\n\r,");
        size_t last = object.find_last_not_of(" \t\n\r,");

        if (first != std::string::npos && last != std::string::npos) {
            object = object.substr(first, last - first + 1);
        }

        if (!object.empty() && object != "[" && object != "]") {
            m_bufferQueue.push(std::move(object));
        }

        // Сохраняем остаток для следующего буфера
        if (object_end < data.size()) {
            m_partialObject = data.substr(object_end);
        } else {
            m_partialObject.clear();
        }

        resetState();
    }
}

void JsonStreamReader::resetState() {
    if (m_inRootArray) {
        m_state = ParserState::ROOT;
        m_braceDepth = 0;
        // m_bracketDepth остаётся 1, так как мы всё ещё в корневом массиве
    } else {
        m_state = ParserState::ROOT;
        m_braceDepth = 0;
        m_bracketDepth = 0;
    }

    m_tokenPos = 0;
    std::memset(m_tokenBuffer, 0, sizeof(m_tokenBuffer));
}