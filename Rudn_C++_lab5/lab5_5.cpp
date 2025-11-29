//
// Created by Kirill on 29.11.2025.
//

/*
Задача: Создать класс ThreadPool, который управляет пулом рабочих потоков.

Требования:
* Реализовать класс ThreadPool с фиксированным количеством потоков
* Добавить метод submit() для добавления задач
* Использовать std::packaged_task для возврата результатов
* Реализовать корректное завершение пула
* Продемонстрировать работу на примере вычисления факториалов
 */

#include <thread>
#include <vector>
#include <queue>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <chrono>

using namespace std;

class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex queue_mutex;
    condition_variable condition;
    bool stop = false;

public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });

                        if (this->stop && this->tasks.empty()) {
                            return;
                        }

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    template<class F, class... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using return_type = decltype(f(args...));

        auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("submit() called on stopped ThreadPool");
            }
            tasks.emplace([task]() { (*task)(); });
        }

        condition.notify_one();
        return result;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }

        condition.notify_all();

        for (std::thread& worker : workers) {
            worker.join();
        }
    }
};

// Функция для демонстрации - вычисление факториала
unsigned long long factorial(int n) {
    if (n < 0) return 0;
    if (n == 0 || n == 1) return 1;

    unsigned long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
        // Имитация сложных вычислений
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return result;
}

// Демонстрация работы
int main() {
    // Создаем пул из 4 потоков
    ThreadPool pool(4);
    vector<future<unsigned long long>> results;

    // Демонстрация параллельной работы
    cout << "\n=== Параллельный запуск ===\n";
    auto start = chrono::high_resolution_clock::now();

    vector<future<unsigned long long>> parallel_results;
    for (int i = 10; i <= 20; ++i) {
        parallel_results.push_back(pool.submit(factorial, i));
    }

    for (size_t i = 0; i < parallel_results.size(); ++i) {
        auto result = parallel_results[i].get();
        cout << "factorial(" << (i + 10) << ") = " << result << "\n";
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start);

    cout << "Время параллельной работы: " << duration.count() << " ms\n";

    // Сравнение с последовательным выполнением
    cout << "\n=== Последовательный запуск ===\n";
    start = chrono::high_resolution_clock::now();

    for (int i = 10; i <= 20; ++i) {
        cout << "factorial(" << (i) << ") = " << factorial(i) << "\n";
    }

    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Время последовательной работы: " << duration.count() << " ms\n";

}