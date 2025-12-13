//
// Created by Kirill on 12.12.2025.
//

#include "../../include/client/Client.h"
#include <iostream>

int Client::clientCounter = 0;

Client::Client() : id(++clientCounter), registrationDate(Date()) {}

Client::Client(const std::string& fName, const std::string& lName,
               const Address& addr, const Date& regDate)
        : id(++clientCounter), firstName(fName), lastName(lName),
          address(addr), registrationDate(regDate) {}

Client::~Client() {}

void Client::displayInfo() const {
    std::cout << "\n=== Client Information ===" << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "Type: " << getType() << std::endl;
    std::cout << "Name: " << getFullName() << std::endl;
    std::cout << "Address: ";
    address.display();
    std::cout << std::endl;
    std::cout << "Registration Date: ";
    registrationDate.display();
    std::cout << std::endl;
}