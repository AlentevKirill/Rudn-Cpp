//
// Created by Anastasia.
//

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() : m_done(false) {}

    ~ThreadSafeQueue() {
        shutdown();
    }

    void push(T value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(value));
        m_cond.notify_one();
    }

    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);

        // ОЖИДАЕМ: пока есть задачи ИЛИ пока не сказали завершаться
        m_cond.wait(lock, [this]() {
            return !m_queue.empty() || m_done;
        });

        // ЕСЛИ: сказали завершаться И очередь пуста - выходим
        if (m_done && m_queue.empty()) {
            return false;
        }

        // БЕРЕМ ЗАДАЧУ: всегда, если дошли сюда
        value = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    bool is_active() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return !(m_done && m_queue.empty());
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_done = true;
        }
        m_cond.notify_all();
    }

    bool is_done() const {
        return m_done.load();
    }

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_cond;
    std::queue<T> m_queue;
    std::atomic<bool> m_done;
};