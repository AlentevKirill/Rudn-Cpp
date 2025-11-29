//
// Created by Kirill on 29.11.2025.
//

/*
Задача: Реализовать систему "Производитель-Потребитель" с использованием std::condition_variable.

Требования:
* Создать класс ThreadSafeQueue с методами push() и pop()
* Реализовать один поток-производитель, который генерирует числа
* Реализовать три потока-потребителя, которые обрабатывают числа
* Использовать std::condition_variable для эффективного ожидания
* Обеспечить корректное завершение всех потоков
 */

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <random>

using namespace std;

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cond;
    bool shutdown_flag = false;

public:
    void push(T value) {
        // TODO: Реализовать потокобезопасное добавление
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (shutdown_flag) {
                return;
            }
            queue.push(move(value));
        }
        // Уведомляем один ожидающий потребитель
        cond.notify_one();
    }

    bool pop(T &value) {
        // TODO: Реализовать потокобезопасное извлечение
        // Возвращать false если очередь закрыта и пуста
        unique_lock<std::mutex> lock(mutex);

        cond.wait(lock, [this](){
            return !(this->queue).empty() || shutdown_flag;
        });

        if (queue.empty() && shutdown_flag) {
            return false;
        }

        // Извлекаем данные из очереди
        if (!queue.empty()) {
            value = move(queue.front());
            queue.pop();
            lock.unlock();  // Освобождаем мьютекс как можно раньше
            return true;
        } else {
            lock.unlock();
        }

        return false;
    }

    void shutdown() {
        // TODO: Реализовать корректное закрытие очереди
        {
            std::lock_guard<std::mutex> lock(mutex);
            shutdown_flag = true;
        }
        cond.notify_all(); // Будим все ожидающие потоки
    }
};

void producer(ThreadSafeQueue<int>& queue, int count) {
    // TODO: Генерировать числа и помещать в очередь
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100);

    for (int i = 0; i < count; ++i) {
        int value = dist(gen);
        queue.push(value);
        std::cout << "Producer: pushed " << value << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // имитация работы
    }
    std::cout << "Producer finished\n";
}

void consumer(ThreadSafeQueue<int>& queue, int id) {
    // TODO: Извлекать и обрабатывать числа из очереди
    int value;
    while (queue.pop(value)) {
        std::cout << "Consumer " << id << ": processed " << value
                  << " (result: " << value * 2 << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // имитация обработки
    }
    std::cout << "Consumer " << id << " finished\n";
}

int main() {
    ThreadSafeQueue<int> q;
    int n = 10;

    thread t1(producer, ref(q), n);
    thread t2(consumer, ref(q), n);
    thread t3(consumer, ref(q), n);
    thread t4(consumer, ref(q), n);

    t1.join();

    q.shutdown();

    t2.join();
    t3.join();
    t4.join();
}