//
// Created by Kirill on 31.10.2025.
//

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Vehicle {

protected:
    string brand;
    string model;
    int year;
    double fuelLevel;

public:
    Vehicle(string b, string m, int y) : brand(b), model(m), year(y), fuelLevel(100) {}

    // TODO: Сделать класс абстрактным с чисто виртуальными методами:
    // - startEngine()
    // - stopEngine()
    // - refuel(double amount)
    virtual double startEngine() const = 0;
    virtual double stopEngine() const = 0;
    virtual double refuel(double amount) const = 0;

    virtual void displayInfo() const {
        cout << brand << " " << model << " (" << year << ")" << endl;
        cout << "Уровень топлива: " << fuelLevel << "%" << endl;
    }

    double getFuelLevel() const {
        return fuelLevel;
    }

    void setFuelLevel(double level) {
        if (level >= 0 && level <= 100) fuelLevel = level;
    }

    virtual ~Vehicle() {}
};

class Car : public Vehicle {
private:
    int doors;
    string transmission;

public:
    Car(string b, string m, int y, int d, string trans)
            : Vehicle(b, m, y), doors(d), transmission(trans) {}

    // TODO: Реализовать чисто виртуальные методы
    double startEngine() const override {
        cout << "Автомобиль " << brand << " заводится с ключа" << endl;
    }

    double stopEngine() const override {
        cout << "Автомобиль " << brand << " останавливается" << endl;
    }

    double refuel(double amount) const override {
        cout << "Автомобиль " << brand << " с уровнем топлива "
             << fuelLevel << " %" << " Заправился на: " << amount << " руб." << endl;
    }
};

class Motorcycle : public Vehicle {
private:
    bool hasFairing;

public:
    Motorcycle(string b, string m, int y, bool fairing)
            : Vehicle(b, m, y), hasFairing(fairing) {}

    // TODO: Реализовать чисто виртуальные методы

    double startEngine() const override {
        cout << "Мотоцикл " << brand << " заводится с ключа" << endl;
    }

    double stopEngine() const override {
        cout << "Мотоцикл " << brand << " останавливается" << endl;
    }

    double refuel(double amount) const override {
        cout << "Мотоцикл " << brand << " с уровнем топлива "
             << fuelLevel << " %" << " Заправился на: " << amount << " руб." << endl;
    }

    void wheelie() {
        cout << brand << " " << model << " делает вилли!" << endl;
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    vector<Vehicle*> vehicles;
    vehicles.push_back(new Car("BMW", "x6", 2023, 5, "Auto"));
    vehicles.push_back(new Motorcycle("Yamaha", "YBR 125", 2023, true));
    // TODO: Создать массив указателей на Vehicle
    // Добавить объекты Car и Motorcycle
    // Продемонстрировать полиморфное поведение
    for (int i = 0; i < 2; ++i) {
        vehicles[i]->displayInfo();
        cout << endl;
        vehicles[i]->startEngine();
        vehicles[i]->stopEngine();
        vehicles[i]->refuel(5000);
        cout << "---------------------------------------------------------------------------------" << endl;
    }

    return 0;
}