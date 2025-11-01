//
// Created by Kirill on 31.10.2025.
//

#include <iostream>
#include <vector>
#include <cmath>
#include <math.h>

using namespace std;

class Shape {
protected:
    string color;
    string name;

public:
    Shape(string n, string c) : name(n), color(c) {}

    // TODO: Объявить чисто виртуальные методы:
    // - calculateArea()
    // - calculatePerimeter()
    // - draw()
    virtual double calculateArea() const = 0;
    virtual double calculatePerimeter() const = 0;
    virtual void draw() const = 0;

    virtual void displayInfo() const {
        cout << "Фигура: " << name << ", Цвет: " << color << endl;
        cout << "Площадь фигуры: " << calculateArea() << endl;
        cout << "Периметр фигуры: " << calculatePerimeter() << endl;
    }

    virtual ~Shape() {}
};

class Circle : public Shape {
private:
    double radius;

public:
    Circle(string c, double r) : Shape("Круг", c), radius(r) {}

    // TODO: Реализовать виртуальные методы

    // Площадь круга: π * r²
    double calculateArea() const override {
        return M_PI * pow(radius, 2);
    }

    // Периметр круга: 2 * π * r
    double calculatePerimeter() const override {
        return 2 * M_PI * radius;
    }

    void draw() const override {
        cout << "Рисую круг радиуса " << radius
             << " цвета " << color << endl;
    }

    void displayInfo() const override {
        Shape::displayInfo();
        cout << "Радиус окружности: " << radius << endl;
    }
};

class Rectangle : public Shape {
private:
    double width, height;

public:
    Rectangle(string c, double w, double h) : Shape("Прямоугольник", c), width(w), height(h) {}

    // TODO: Реализовать виртуальные методы

    double calculateArea() const override {
        return width * height;
    }

    double calculatePerimeter() const override {
        return 2 * (width + height);
    }

    void draw() const override {
        cout << "Рисую прямоугольник " << width << "x" << height
             << " цвета " << color << endl;
    }

    void displayInfo() const override {
        Shape::displayInfo();
        cout << "Размер прямоугольника: " << width << "x" << height << endl;
    }

};

class Triangle : public Shape {
private:
    double sideA, sideB, sideC;

public:
    Triangle(
            string color,
            double a,
            double b,
            double c
            ) : Shape("Треугольник", color), sideA(a), sideB(b), sideC(c) {}

    // TODO: Реализовать виртуальные методы

    // Площадь по формуле Герона
    double calculateArea() const override {
        double p = calculatePerimeter() / 2;
        return sqrt(p * (p - sideA) * (p - sideB) * (p - sideC));
    }

    double calculatePerimeter() const override {
        return sideA + sideB + sideC;
    }

    void draw() const override {
        cout << "Рисую треугольник " << sideA << "x" << sideB << "x" << sideC
             << " цвета " << color << endl;
    }

    void displayInfo() const override {
        Shape::displayInfo();
        cout << "Размер трекгольника: " << sideA << "x" << sideB << "x" << sideC << endl;
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    vector<Shape*> shapes;
    shapes.push_back(new Circle("Красный", 5.0));
    shapes.push_back(new Rectangle("Синий", 4.0, 6.0));
    shapes.push_back(new Triangle("Зеленый", 3.0, 4.0, 5.0));

    // TODO: Продемонстрировать полиморфизм - вывести информацию
    // о всех фигурах через указатель на базовый класс
    for (int i = 0; i < 3; ++i) {
        shapes[i]->displayInfo();
        cout << endl;
        shapes[i]->draw();
        cout << "---------------------------------------------------------------------------------" << endl;
    }

    // Очистка памяти
    for (auto shape : shapes) {
        delete shape;
    }

    return 0;
}