//
// Created by Kirill on 15.11.2025.
//

/*
Задача: Реализовать программу, которая создает несколько потоков для параллельного вычисления суммы элементов большого вектора.
Сравнить производительность однопоточного и многопоточного подходов.

Требования:
* Создать вектор из 10 000 000 случайных чисел от 1 до 100
* Реализовать однопоточное вычисление суммы
* Реализовать многопоточное вычисление суммы с использованием 4 потоков
* Замерить время выполнения обоих подходов
* Использовать std::mutex для защиты общей суммы
 */

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

using namespace std;

class VectorSumCalculator {
private:
    vector<int> data;
    mutex sum_mutex;

public:
    VectorSumCalculator(size_t size) {
        // TODO: Заполнить вектор случайными числами
        random_device rd;  // случайное seed
        mt19937 gen(rd()); // генератор Mersenne Twister
        uniform_int_distribution<int> dist(1, 100); // распределение [1, 100]

        data.reserve(size);

        for(size_t i = 0; i < size; ++i) {
            data.emplace_back(dist(gen));
        }
    }

    // Однопоточное вычисление суммы
    long long calculateSingleThreaded() {
        // TODO: Реализовать однопоточное вычисление
        long long sum = 0;
        for (int d: data) {
            sum += d;
        }
        return sum;
    }

    // Многопоточное вычисление суммы
    long long calculateMultiThreaded(int num_threads) {
        // TODO: Реализовать многопоточное вычисление
        long long sum = 0;

        // Разделить вектор на части, создать потоки,
        vector<pair<vector<int>::iterator, vector<int>::iterator>> ranges;
        size_t total = distance(data.begin(), data.end());
        size_t chunk_size = total / num_threads;
        size_t remainder = total % num_threads;

        auto it = data.begin();
        for (size_t i = 0; i < num_threads && it != data.end(); ++i) {
            auto next = it + chunk_size + (i < remainder ? 1 : 0);
            ranges.emplace_back(it, next);
            it = next;
        }

        // использовать мьютекс для защиты общей суммы
        vector<thread> threads(num_threads);
        for (int i = 0; i < num_threads; i++) {
            threads.emplace_back([this, &sum](vector<int>::iterator b, vector<int>::iterator e){
                for (auto it = b; it < e; it++) {
                    this->sum_mutex.lock();
                    sum += *it;
                    this->sum_mutex.unlock();
                }
                },
                                 ranges[i].first,
                                 ranges[i].second
                                 );
        }
        for (int i = 0; i < num_threads; i++) {
            threads[i].join();
        }

        return sum;
    }
};

int main() {
    // TODO: Создать объект, выполнить оба расчета,
    VectorSumCalculator vec = VectorSumCalculator(10000000);
    int num_threads = 4;

    // замерить время и сравнить результаты
    auto start = chrono::high_resolution_clock::now();
    long long singleSum = vec.calculateSingleThreaded();
    auto end = chrono::high_resolution_clock::now();

    auto duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start);
    auto duration_ms = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Для однного потока:" << endl;
    cout << "Результат суммирования: " << singleSum << endl;
    cout << "Время выполнения: " << duration_ns.count() << " ns\n";
    cout << "Время выполнения: " << duration_ms.count() << " ms\n";

    cout << endl;

    start = chrono::high_resolution_clock::now();
    long long multyThreadSum = vec.calculateMultiThreaded(4);
    end = chrono::high_resolution_clock::now();

    duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start);
    duration_ms = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Для " << num_threads << " потоков:" << endl;
    cout << "Результат суммирования: " << multyThreadSum << endl;
    cout << "Время выполнения: " << duration_ns.count() << " ns\n";
    cout << "Время выполнения: " << duration_ms.count() << " ms\n";
}
