//
// Created by Kirill.
//

#pragma once

#include <string>
#include <fstream>
#include <queue>
#include <atomic>
#include <memory>

class JsonStreamReader {
public:
    explicit JsonStreamReader(const std::string& filename);
    ~JsonStreamReader();

    bool getNextObject(std::string& json_str);

    size_t getBytesRead() const { return m_totalBytesRead; }
    size_t getObjectsFound() const { return m_objectsFound; }
    bool isRootArray() const { return m_isRootArray; }
    bool isDone() const { return m_done && m_bufferQueue.empty(); }

private:
    static constexpr size_t BUFFER_SIZE = 4 * 1024;

    enum class ParserState {
        ROOT,           // Начало или между объектами
        IN_OBJECT,      // Внутри объекта
        IN_STRING,      // Внутри строки
        ESCAPE,         // После обратного слеша в строке
        IN_NUMBER,      // Внутри числа
        IN_TRUE,        // Читаем "true"
        IN_FALSE,       // Читаем "false"
        IN_NULL,        // Читаем "null"
        IN_ARRAY        // Внутри массива (если не корневой)
    };

    std::ifstream m_file;
    std::string m_filename;
    std::atomic<size_t> m_totalBytesRead{0};
    std::atomic<size_t> m_objectsFound{0};
    std::atomic<bool> m_done{false};

    bool m_isRootArray = false;

    std::queue<std::string> m_bufferQueue;
    std::string m_partialObject;    // Частично собранный объект
    ParserState m_state = ParserState::ROOT;

    int m_braceDepth = 0;           // Глубина объектов {}
    int m_bracketDepth = 0;         // Глубина массивов []
    bool m_inRootArray = false;     // Находимся в корневом массиве

    // Для детектирования токенов
    size_t m_tokenPos = 0;
    char m_tokenBuffer[10];         // Для временного хранения токенов

    // Методы
    void initParser();
    void processChar(char c);
    void processBuffer(const char* data, size_t size);
    bool tryCompleteObject();
    void resetState();
    void savePartialForNextBuffer(const std::string& data);
};