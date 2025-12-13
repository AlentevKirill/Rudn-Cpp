//
// Created by Kirill on 12.12.2025.
//

#pragma once
#include "Client.h"
#include "../../include/util/Address.h"
#include "../../include/util/Date.h"

class VipClient : public Client {
private:
    int premiumLevel;
    double discountRate;

public:
    VipClient();
    VipClient(const std::string& fName, const std::string& lName,
                  const Address& addr, const Date& regDate,
                  int level = 1, double discount = 0.05);

    int getPremiumLevel() const { return premiumLevel; }
    double getDiscountRate() const { return discountRate; }

    void setPremiumLevel(int level) { premiumLevel = level; }
    void setDiscountRate(double rate) { discountRate = rate; }

    void displayInfo() const override;
    std::string getType() const override { return "Premium"; }

    double applyDiscount(double amount) const;
    void upgradeLevel();
    void downgradeLevel();
};
