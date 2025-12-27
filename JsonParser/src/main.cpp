//
// Created by Kirill.
//

#include "../include/ConsoleInterface.h"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <atomic>

std::atomic<bool> g_running{true};

void signalHandler(int signal) {
    std::cout << "\n⚠️  Получен сигнал " << signal << ". Завершение работы...\n";
    g_running = false;
}

int main() {
    // Устанавливаем обработчики сигналов
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "Запуск многопоточного JSON парсера...\n";
    std::cout << "Для выхода нажмите Ctrl+C или введите 'exit'\n\n";

    try {
        ConsoleInterface console;

        while (g_running) {
            try {
                console.run();
                break; // Выход по команде exit
            } catch (const std::exception& e) {
                std::cerr << "\n💥 Критическая ошибка в интерфейсе: " << e.what() << std::endl;
                std::cerr << "Перезапуск интерфейса...\n\n";

                // Небольшая пауза перед перезапуском
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "💥 Фатальная ошибка: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\nПрограмма завершена.\n";
    return EXIT_SUCCESS;
}