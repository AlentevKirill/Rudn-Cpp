//
// Created by Kirill on 13.12.2025.
//

#include "../../include/util/Date.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>

// Конструктор по умолчанию
Date::Date() {
    // Простая реализация - фиксированная дата
    day = 1;
    month = 1;
    year = 2024;
    hour = 0;
    minute = 0;
    second = 0;
}

Date::Date(int d, int m, int y)
        : day(d), month(m), year(y), hour(0), minute(0), second(0) {}

Date::Date(int d, int m, int y, int h, int min, int sec)
        : day(d), month(m), year(y), hour(h), minute(min), second(sec) {}

bool Date::validateDate() const {
    // Простая валидация
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if (hour < 0 || hour > 23) return false;
    if (minute < 0 || minute > 59) return false;
    if (second < 0 || second > 59) return false;
    return true;
}

void Date::display() const {
    if (validateDate()) {
        std::cout << std::setfill('0') << std::setw(2) << day << "/"
                  << std::setw(2) << month << "/"
                  << std::setw(4) << year;
    } else {
        std::cout << "Invalid Date";
    }
}

void Date::displayFull() const {
    if (validateDate()) {
        std::cout << std::setfill('0') << std::setw(2) << day << "/"
                  << std::setw(2) << month << "/"
                  << std::setw(4) << year << " "
                  << std::setw(2) << hour << ":"
                  << std::setw(2) << minute << ":"
                  << std::setw(2) << second;
    } else {
        std::cout << "Invalid Date";
    }
}

std::string Date::toString() const {
    if (!validateDate()) {
        return "Invalid Date";
    }

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << day << "/"
       << std::setw(2) << month << "/"
       << std::setw(4) << year;
    return ss.str();
}

Date Date::parseFromString(const std::string& dateStr) {
    // Простой парсинг формата DD/MM/YYYY
    // Для простоты предполагаем правильный формат

    if (dateStr.length() < 10) {
        return Date(); // Возвращаем дату по умолчанию
    }

    // Извлекаем DD/MM/YYYY
    std::string dayStr = dateStr.substr(0, 2);
    std::string monthStr = dateStr.substr(3, 2);
    std::string yearStr = dateStr.substr(6, 4);

    int d = std::atoi(dayStr.c_str());
    int m = std::atoi(monthStr.c_str());
    int y = std::atoi(yearStr.c_str());

    return Date(d, m, y);
}