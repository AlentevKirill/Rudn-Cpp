//
// Created by Kirill on 29.11.2025.
//

/*
Задача: Реализовать параллельное применение фильтра к "изображению" (матрице).

Требования:
* Представить изображение как матрицу чисел (яркостей пикселей)
* Реализовать фильтр размытия (усреднение соседних пикселей)
* Разделить изображение на горизонтальные полосы для обработки в разных потоках
* Собрать результаты и вывести статистику
 */

#include <vector>
#include <thread>
#include <algorithm>
#include <random>
#include <iostream>
#include <mutex>

using namespace std;

class ImageProcessor {
private:
    std::vector<std::vector<int>> image;
    vector<vector<int>> result_image;
    int width, height;
    mutex stats_mutex;
    int processed_pixels = 0;

public:
    ImageProcessor(int w, int h) : width(w), height(h) {
        // TODO: Инициализировать "изображение" случайными значениями
        random_device rd;  // случайное seed
        mt19937 gen(rd()); // генератор Mersenne Twister
        uniform_int_distribution<int> dist(0, 255);

        image.resize(height, vector<int>(width));
        result_image.resize(height, vector<int>(width));

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                image[i][j] = dist(gen);
            }
        }
    }

    // Однопоточное применение фильтра
    void applyFilterSingleThread() {
        // TODO: Реализовать последовательное применение фильтра

        auto start = chrono::high_resolution_clock::now();

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                result_image[i][j] = blurPixel(i, j);
            }
        }

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Single thread time: " << duration.count() << " ms\n";

    }

    // Многопоточное применение фильтра
    void applyFilterMultiThread(int num_threads) {
        // TODO: Разделить изображение на части,
        // создать потоки для обработки каждой части

        auto start = chrono::high_resolution_clock::now();

        vector<thread> threads;
        threads.reserve(num_threads);
        processed_pixels = 0;

        int strip_height = height / num_threads;

        for (int i = 0; i < num_threads; ++i) {
            int start_row = i * strip_height;
            int end_row = (i == num_threads - 1) ? height : (i + 1) * strip_height;

            threads.emplace_back([this, start_row, end_row]() {
                processStrip(start_row, end_row);
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Multi-thread (" << num_threads << " threads) time: "
             << duration.count() << " ms\n";
        cout << "Processed pixels: " << processed_pixels << "\n";
    }

    // Обработка полосы изображения
    void processStrip(int start_row, int end_row) {
        int local_processed = 0;

        for (int i = start_row; i < end_row; ++i) {
            for (int j = 0; j < width; ++j) {
                result_image[i][j] = blurPixel(i, j);
                local_processed++;
            }
        }


        lock_guard<mutex> lock(stats_mutex);
        processed_pixels += local_processed;
    }

    // Фильтр размытия (усреднение 3x3)
    int blurPixel(int x, int y) {
        // TODO: Реализовать фильтр усреднения
        int sum = 0;
        int count = 0;

        for (int i = max(0, x - 1); i <= min(height - 1, x + 1); ++i) {
            for (int j = max(0, y - 1); j <= min(width - 1, y + 1); ++j) {
                sum += image[i][j];
                count++;
            }
        }

        return sum / count;
    }

    void printStats() {
        int min_val = 255, max_val = 0;
        long long total = 0;

        for (const auto& row : result_image) {
            for (int val : row) {
                min_val = min(min_val, val);
                max_val = max(max_val, val);
                total += val;
            }
        }

        cout << "Image stats - Min: " << min_val
             << ", Max: " << max_val
             << ", Avg: " << total / (width * height) << "\n";
    }
};

int main() {
    const int WIDTH = 1000;
    const int HEIGHT = 1000;

    ImageProcessor processor(WIDTH, HEIGHT);

    cout << "=== Image Processing Benchmark ===\n";
    cout << "Image size: " << WIDTH << "x" << HEIGHT << "\n\n";

    // Однопоточная обработка
    processor.applyFilterSingleThread();
    processor.printStats();
    cout << "\n";

    // Многопоточная обработка

    processor.applyFilterMultiThread(4);
    processor.printStats();
    cout << "\n";
}