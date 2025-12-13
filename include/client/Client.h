//
// Created by Kirill on 12.12.2025.
//

#pragma once
#include "../util/Address.h"
#include "../util/Date.h"
#include <string>
#include <memory>
#include <stdexcept>

class Client {
protected:
    int id;
    std::string firstName;
    std::string lastName;
    Address address;
    Date registrationDate;

    static int clientCounter;

    bool validateName(const std::string& name) const;

public:
    Client();
    Client(const std::string& fName, const std::string& lName,
           const Address& addr, const Date& regDate);
    virtual ~Client();

    int getId() const { return id; }
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getFullName() const { return firstName + " " + lastName; }
    Address getAddress() const { return address; }
    Date getRegistrationDate() const { return registrationDate; }

    void setFirstName(const std::string& name);
    void setLastName(const std::string& name);
    void setAddress(const Address& addr);

    virtual void displayInfo() const;
    virtual std::string getType() const { return "Regular"; }

    bool isValid() const;

    static int getTotalClients() { return clientCounter; }
    static void resetCounter() { clientCounter = 0; }
    static bool validateClientId(int id) { return id > 0; }
};
