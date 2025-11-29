//
// Created by Kirill on 29.11.2025.
//

/*
Задача: Реализовать параллельное вычисление числа π с использованием метода Монте-Карло.

Требования:
* Использовать метод Монте-Карло для вычисления π
* Разделить вычисления на несколько асинхронных задач
* Использовать std::async с политикой std::launch::async
* Собрать результаты с помощью std::future
* Обработать возможные исключения
 */

#include <future>
#include <random>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <chrono>

class PiCalculator {
private:
    // Генератор случайных чисел для каждого потока
    double monteCarloPi(int points) {
        thread_local std::random_device rd;
        thread_local std::mt19937 gen(rd());
        thread_local std::uniform_real_distribution<double> dist(-1.0, 1.0);

        int points_inside = 0;

        for (int i = 0; i < points; ++i) {
            double x = dist(gen);
            double y = dist(gen);

            // Проверяем, попадает ли точка в единичную окружность
            if (x * x + y * y <= 1.0) {
                points_inside++;
            }
        }

        // π ≈ 4 * (точки_внутри / общее_количество_точек)
        return 4.0 * static_cast<double>(points_inside) / points;
    }

public:
    // Вычисление π с использованием заданного количества точек
    double calculatePiPortion(int total_points) {
        if (total_points <= 0) {
            throw std::invalid_argument("Количество точек должно быть положительным");
        }

        return monteCarloPi(total_points);
    }

    // Параллельное вычисление π
    double calculatePiParallel(int total_points, int num_tasks) {
        if (total_points <= 0 || num_tasks <= 0) {
            throw std::invalid_argument("Количество точек и задач должно быть положительным");
        }

        std::vector<std::future<double>> futures;
        int points_per_task = total_points / num_tasks;
        int remaining_points = total_points % num_tasks;

        // Создаем асинхронные задачи
        for (int i = 0; i < num_tasks; ++i) {
            int points = points_per_task + (i < remaining_points ? 1 : 0);
            if (points > 0) {
                futures.push_back(
                        std::async(std::launch::async,
                                   &PiCalculator::calculatePiPortion, this, points)
                );
            }
        }

        // Собираем и усредняем результаты
        double sum = 0.0;
        int valid_results = 0;

        for (auto& future : futures) {
            try {
                double result = future.get(); // Может бросить исключение
                sum += result;
                valid_results++;
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка в задаче: " << e.what() << std::endl;
            }
        }

        if (valid_results == 0) {
            throw std::runtime_error("Все задачи завершились с ошибкой");
        }

        return sum / valid_results;
    }
};

// Пример использования
int main() {
    PiCalculator calculator;

    const int TOTAL_POINTS = 10000000;
    const int NUM_TASKS = 4;

    try {
        auto start = std::chrono::high_resolution_clock::now();

        double pi_parallel = calculator.calculatePiParallel(TOTAL_POINTS, NUM_TASKS);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "=== Вычисление π методом Монте-Карло ===" << std::endl;
        std::cout << "Общее количество точек: " << TOTAL_POINTS << std::endl;
        std::cout << "Количество задач: " << NUM_TASKS << std::endl;
        std::cout << "Вычисленное значение π: " << pi_parallel << std::endl;
        std::cout << "Ошибка: " << std::abs(pi_parallel - 3.141592653589793) << std::endl;
        std::cout << "Время выполнения: " << duration.count() << " мс" << std::endl;

        // Сравнение с однопоточным вычислением
        start = std::chrono::high_resolution_clock::now();
        double pi_single = calculator.calculatePiPortion(TOTAL_POINTS);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "\n=== Сравнение с однопоточным режимом ===" << std::endl;
        std::cout << "Однопоточное значение π: " << pi_single << std::endl;
        std::cout << "Ошибка: " << std::abs(pi_single - 3.141592653589793) << std::endl;
        std::cout << "Время выполнения: " << duration.count() << " мс" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}