//
// Created by Kirill.
//

#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>

class BufferedJsonReader {
public:
    explicit BufferedJsonReader(const std::string& filename);
    ~BufferedJsonReader();

    bool isRootArray() const { return m_isRootArray; }
    bool hasMoreObjects() const;
    bool extractNextObject(std::string& json_str);
    size_t getBytesRead() const { return m_bytesRead; }

private:
    static constexpr size_t BUFFER_SIZE = 4 * 1024; // 4KB буфер

    struct ObjectBoundary {
        size_t startPos;    // Позиция начала объекта в файле
        size_t endPos;      // Позиция конца объекта в файле
        std::string data;   // Собранные данные объекта
    };

    mutable std::ifstream m_file;  // Делаем mutable
    std::string m_filename;
    mutable std::atomic<size_t> m_bytesRead{0};  // Делаем mutable
    bool m_isRootArray = false;
    mutable bool m_initialized = false;  // Делаем mutable

    // Очередь готовых объектов - делаем mutable, так как может изменяться в const методах
    mutable std::queue<ObjectBoundary> m_objectQueue;
    mutable std::string m_partialData; // Данные после последнего полного объекта
    mutable size_t m_currentFilePos = 0;

    void initialize() const;  // Делаем const
    bool tryFindNextObject() const;
    std::string readFileChunk() const;
    bool tryCompleteObjectFromPartial() const;
    bool isStartOfObject(const std::string& data, size_t offset = 0) const;
    size_t findObjectEnd(const std::string& data, size_t startPos) const;
    size_t findObjectEndFast(const std::string& data, size_t startPos) const;

    void processChunk(const std::string& chunk) const;
    void addCompleteObject(const std::string& data, size_t startPos, size_t endPos) const;
};