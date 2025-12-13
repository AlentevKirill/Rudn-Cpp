//
// Created by Kirill on 12.12.2025.
//

#include "../../include/client/VipClient.h"
#include <iostream>
#include <algorithm>

VipClient::VipClient()
        : Client(), premiumLevel(1), discountRate(0.05) {}

VipClient::VipClient(const std::string& fName, const std::string& lName,
                             const Address& addr, const Date& regDate,
                             int level, double discount)
        : Client(fName, lName, addr, regDate),
          premiumLevel(std::max(1, std::min(level, 3))),
          discountRate(std::max(0.0, std::min(discount, 0.2))) {}

void VipClient::displayInfo() const {
    Client::displayInfo();
    std::cout << "Premium Level: " << premiumLevel << std::endl;
    std::cout << "Discount Rate: " << (discountRate * 100) << "%" << std::endl;
}

double VipClient::applyDiscount(double amount) const {
    return amount * (1.0 - discountRate);
}

void VipClient::upgradeLevel() {
    if (premiumLevel < 3) {
        premiumLevel++;
        discountRate += 0.02;
    }
}

void VipClient::downgradeLevel() {
    if (premiumLevel > 1) {
        premiumLevel--;
        discountRate -= 0.02;
    }
}