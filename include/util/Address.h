//
// Created by Kirill on 12.12.2025.
//

#pragma once
#include <string>
#include <iostream>

struct Address {
    std::string street;
    std::string city;
    std::string postalCode;
    std::string country;

    Address() = default;
    Address(const std::string& s, const std::string& c,
            const std::string& p, const std::string& cnt)
            : street(s), city(c), postalCode(p), country(cnt) {}

    void display() const {
        std::cout << street << ", " << city << ", " << postalCode << ", " << country;
    }
};