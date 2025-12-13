//
// Created by Kirill on 12.12.2025.
//

#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <stdexcept>

class Date {
private:
    int day;
    int month;
    int year;
    int hour;
    int minute;
    int second;

    bool validateDate() const;

public:
    Date();
    Date(int d, int m, int y);
    Date(int d, int m, int y, int h, int min, int sec);

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    void display() const;
    void displayFull() const;
    std::string toString() const;

    bool isValid() const { return validateDate(); }

    static Date parseFromString(const std::string& dateStr);
};