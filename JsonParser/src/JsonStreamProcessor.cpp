//
// Created by Kirill.
//

#include "../include/JsonStreamProcessor.h"
#include <iostream>
#include <algorithm>
#include <future>
#include <iomanip>
#include <mutex>

static std::mutex cout_mutex;

void ParsingResult::clear() {
    objects.clear();
    wasOriginalArray = false;
    parsingTime = std::chrono::milliseconds{0};
    totalObjects = 0;
    totalNodes = 0;
}

JsonStreamProcessor::JsonStreamProcessor() = default;

JsonStreamProcessor::~JsonStreamProcessor() {
    stopAllThreads();
    clear();
}

const ParsingResult& JsonStreamProcessor::processFile(const std::string& filename) {
    auto start_time = std::chrono::high_resolution_clock::now();

    clear();

    m_processing = true;
    m_shouldStop = false;

    try {
        JsonObjectExtractor extractor(filename);
        m_result.wasOriginalArray = extractor.isRootArray();

        if (m_result.wasOriginalArray) {
            processStreaming(extractor);
        } else {
            processSingleObject(extractor);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        m_result.parsingTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                end_time - start_time);

        m_result.totalObjects = m_result.objects.size();
        m_result.totalNodes = calculateTotalNodes();

        m_processing = false;

    } catch (const JsonParseException& e) {
        cleanupOnError();
        throw std::runtime_error("Ошибка парсинга JSON: " + std::string(e.what()));
    } catch (const std::runtime_error& e) {
        cleanupOnError();
        throw std::runtime_error("Ошибка чтения файла: " + std::string(e.what()));
    } catch (const std::exception& e) {
        cleanupOnError();
        throw std::runtime_error("Неожиданная ошибка: " + std::string(e.what()));
    } catch (...) {
        cleanupOnError();
        throw std::runtime_error("Неизвестная ошибка при обработке файла");
    }

    return m_result;
}

void JsonStreamProcessor::processStreaming(JsonObjectExtractor& extractor) {
    ThreadSafeQueue<std::pair<size_t, std::string>> task_queue;
    std::atomic<size_t> object_counter{0};
    std::atomic<bool> producer_done{false};
    std::atomic<bool> should_stop{false};
    std::atomic<size_t> total_processed{0};

    // PRODUCER
    std::thread producer([&extractor, &task_queue, &object_counter, &producer_done, &should_stop, this]() {
        try {
            std::string json_str;
            size_t objects_extracted = 0;

            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "🚀 Producer: начало чтения файла..." << std::endl;
            }

            while (!should_stop.load()) {
                if (extractor.extractNextObject(json_str)) {
                    if (!json_str.empty()) {
                        size_t index = object_counter++;
                        task_queue.push({index, std::move(json_str)});
                        objects_extracted++;

                        if (objects_extracted % 1000 == 0) {
                            std::lock_guard<std::mutex> lock(cout_mutex);
                            std::cout << "📖 Извлечено объектов: " << objects_extracted
                                      << " (" << extractor.getBytesRead() / 1024 / 1024 << " MB)" << std::endl;
                        }
                    }
                } else {
                    if (extractor.isDone()) {
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            producer_done = true;
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "✅ Чтение файла завершено. Всего объектов: " << object_counter.load()
                          << ", всего прочитано: " << extractor.getBytesRead() / 1024 / 1024 << " MB" << std::endl;
            }

        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cerr << "❌ Ошибка в producer: " << e.what() << std::endl;
            producer_done = true;
            should_stop = true;
            task_queue.shutdown();
        }
    });

    // CONSUMERS - НИКОГДА НЕ ЗАВЕРШАЮТСЯ САМИ!
    unsigned int num_threads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> workers;
    std::vector<std::vector<std::pair<size_t, std::shared_ptr<JsonNode>>>> thread_results(num_threads);

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "⚡ Запуск " << num_threads << " потоков для парсинга..." << std::endl;
    }

    for (unsigned int i = 0; i < num_threads; ++i) {
        workers.emplace_back([i, &task_queue, &thread_results, &total_processed, &should_stop, this]() {
            JsonParser parser;
            size_t local_processed = 0;
            size_t local_errors = 0;

            // ПРОСТО: работаем пока НЕ СКАЗАЛИ ОСТАНОВИТЬСЯ
            while (!should_stop.load()) {
                std::pair<size_t, std::string> task;

                // Пытаемся получить задачу БЕЗ ВЫХОДА
                if (task_queue.pop(task)) {
                    // Есть задача - обрабатываем
                    try {
                        auto [index, json_str] = task;

                        if (json_str.empty()) {
                            continue;
                        }

                        auto node = parser.parse(json_str);
                        if (node) {
                            thread_results[i].emplace_back(index, std::move(node));
                            local_processed++;
                            total_processed++;

                            if (local_processed % 1000 == 0) {
                                std::lock_guard<std::mutex> lock(cout_mutex);
                                std::cout << "⚡ Поток " << i << " обработал: " << local_processed
                                          << " объектов" << std::endl;
                            }
                        }
                    } catch (const JsonParseException& e) {
                        local_errors++;
                        if (local_errors <= 3) {
                            std::lock_guard<std::mutex> lock(cout_mutex);
                            std::cerr << "❌ Ошибка парсинга в потоке " << i
                                      << " (объект #" << (local_processed + local_errors) << "): "
                                      << e.getDetailedMessage() << std::endl;
                        }
                    } catch (const std::exception& e) {
                        local_errors++;
                        std::lock_guard<std::mutex> lock(cout_mutex);
                        std::cerr << "❌ Неизвестная ошибка в потоке " << i << ": "
                                  << e.what() << std::endl;
                    }
                } else {
                    // Задачи нет - просто ждем немного
                    // НЕ ПЫТАЕМСЯ ВЫЙТИ!
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "🎯 Поток " << i << " завершил работу. Обработано: "
                          << local_processed << " объектов, ошибок: " << local_errors << std::endl;
            }
        });
    }

    // Ждём завершения producer
    if (producer.joinable()) {
        producer.join();
    }

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "⏳ Ожидание обработки оставшихся задач..." << std::endl;
        std::cout << "   Всего объектов в очереди: " << task_queue.size() << std::endl;
    }

    // Ждем ПОЛНОСТЬЮ, пока не обработаются ВСЕ задачи
    size_t expected_objects = object_counter.load();
    int wait_seconds = 0;
    bool all_tasks_done = false;

    while (!all_tasks_done) {
        size_t queue_size = task_queue.size();
        size_t processed = total_processed.load();

        // Условие остановки: очередь пуста И обработаны все объекты
        all_tasks_done = (queue_size == 0 && processed >= expected_objects);

        if (all_tasks_done) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "✅ Все задачи обработаны! Останавливаем потоки." << std::endl;
            break;
        }

        // Периодически выводим прогресс (каждые 5 секунд)
        if (wait_seconds % 5 == 0) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "⏱️  Прогресс (" << wait_seconds << " сек): "
                      << "обработано " << processed << "/" << expected_objects
                      << " (" << (expected_objects > 0 ? processed * 100 / expected_objects : 0) << "%), "
                      << "очередь: " << queue_size << std::endl;
        }

        // Проверяем deadlock: если 30 секунд нет прогресса
        static size_t last_processed = 0;
        static int no_progress_count = 0;

        if (processed == last_processed) {
            no_progress_count++;
            if (no_progress_count > 6) { // 30 секунд без прогресса
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "⚠️  Нет прогресса 30 секунд. Возможный deadlock." << std::endl;
                std::cout << "   Обработано: " << processed << "/" << expected_objects
                          << ", очередь: " << queue_size << std::endl;
                std::cout << "   Принудительная остановка." << std::endl;
                break;
            }
        } else {
            last_processed = processed;
            no_progress_count = 0;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
        wait_seconds += 5;
    }

    // ОСТАНАВЛИВАЕМ потоки явно
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "🛑 Остановка потоков..." << std::endl;
    }
    should_stop = true;
    task_queue.shutdown();

    // Даем время потокам завершиться
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Ждем завершения всех потоков
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    // Собираем результаты
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "📦 Сборка результатов..." << std::endl;
    }

    std::vector<std::shared_ptr<JsonNode>> ordered_results(expected_objects);

    for (unsigned int i = 0; i < num_threads; ++i) {
        for (const auto& [index, node] : thread_results[i]) {
            if (index < ordered_results.size()) {
                ordered_results[index] = std::move(node);
            }
        }
    }

    // Удаляем null-элементы
    ordered_results.erase(
            std::remove_if(ordered_results.begin(), ordered_results.end(),
                           [](const auto& ptr) { return ptr == nullptr; }),
            ordered_results.end()
    );

    m_result.objects = std::move(ordered_results);
    m_result.totalObjects = m_result.objects.size();
    m_result.totalNodes = calculateTotalNodes();

    // Выводим итоговую статистику
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "\n📊 ИТОГОВАЯ СТАТИСТИКА:" << std::endl;
        std::cout << "   Извлечено объектов из файла: " << expected_objects << std::endl;
        std::cout << "   Успешно распаршено: " << m_result.objects.size() << std::endl;
        std::cout << "   Всего узлов: " << m_result.totalNodes << std::endl;

        if (m_result.objects.size() < expected_objects) {
            std::cout << "⚠️  Внимание: не удалось распарсить "
                      << (expected_objects - m_result.objects.size())
                      << " объектов!" << std::endl;

            // Проверяем, сколько было реально обработано
            size_t actually_processed = 0;
            for (unsigned int i = 0; i < num_threads; ++i) {
                actually_processed += thread_results[i].size();
            }

            std::cout << "   Фактически обработано потоками: " << actually_processed << std::endl;
            std::cout << "   Разница: " << (m_result.objects.size() - actually_processed)
                      << " (возможно, дубликаты индексов)" << std::endl;
        } else {
            std::cout << "✅ Все объекты успешно обработаны!" << std::endl;
        }
    }
}

void JsonStreamProcessor::processSingleObject(JsonObjectExtractor& extractor) {
    std::string json_str;
    if (extractor.extractNextObject(json_str)) {
        try {
            JsonParser parser;
            auto node = parser.parse(json_str);
            m_result.objects.push_back(std::move(node));
        } catch (const JsonParseException& e) {
            std::cerr << "❌ Ошибка парсинга одиночного объекта: "
                      << e.getDetailedMessage() << std::endl;
            throw;
        }
    } else {
        throw std::runtime_error("Не удалось извлечь JSON объект из файла");
    }
}

std::shared_ptr<JsonNode> JsonStreamProcessor::get(size_t index, const std::string& path) const {
    if (index >= m_result.objects.size()) {
        return nullptr;
    }

    if (path.empty()) {
        return m_result.objects[index];
    }

    return m_result.objects[index]->getByPath(path);
}

std::shared_ptr<JsonNode> JsonStreamProcessor::getFirst(const std::string& path) const {
    return get(0, path);
}

const ParsingResult& JsonStreamProcessor::getResult() const {
    return m_result;
}

size_t JsonStreamProcessor::getObjectCount() const {
    return m_result.objects.size();
}

void JsonStreamProcessor::clear() {
    m_result.clear();
    m_processing = false;
    m_shouldStop = false;
}

size_t JsonStreamProcessor::calculateTotalNodes() const {
    size_t total = 0;
    for (const auto& node : m_result.objects) {
        if (node) {
            total += node->countNodes();
        }
    }
    return total;
}

void JsonStreamProcessor::cleanupOnError() {
    stopAllThreads();
    clear();
}

void JsonStreamProcessor::stopAllThreads() {
    m_shouldStop = true;
    m_processing = false;
}