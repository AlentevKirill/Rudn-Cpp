//
// Created by Kirill on 01.11.2025.
//

#include <iostream>
#include <string>
using namespace std;

class Complex {
private:
    double real;
    double imaginary;

public:
    Complex(double r = 0, double i = 0) : real(r), imaginary(i) {}

    // TODO: Геттеры и сеттеры

    double getReal() const {
        return real;
    }

    void setReal(double real) {
        Complex::real = real;
    }

    double getImaginary() const {
        return imaginary;
    }

    void setImaginary(double imaginary) {
        Complex::imaginary = imaginary;
    }

    // TODO: Перегрузить операторы:
    // - Complex operator+(const Complex& other) const
    Complex operator+(const Complex& other) const {
        return {real + other.getReal(), imaginary + other.getImaginary()};
    }

    // - Complex operator-(const Complex& other) const
    Complex operator-(const Complex& other) const {
        return {real - other.getReal(), imaginary - other.getImaginary()};
    }

    // - Complex operator*(const Complex& other) const
    Complex operator*(const Complex& other) const {
        return {
            real * other.getReal() - imaginary * other.getImaginary(),
            real * other.getImaginary() + imaginary * other.getReal()
        };
    }

    // - bool operator==(const Complex& other) const
    bool operator==(const Complex& other) const {
        return real == other.getReal() && imaginary == other.getImaginary();
    }

    // TODO: Сделать дружественными:
    // - friend ostream& operator<<(ostream& os, const Complex& c)
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << c.real << " + " << c.imaginary << "i";
        return os;
    }
    // - friend istream& operator>>(istream& is, Complex& c)
    friend istream& operator>>(istream& is, Complex& c) {
        double real;
        double imaginary;
        is >> real >> imaginary;
        if (is) {
            c.real = real;
            c.imaginary = imaginary;
        }
        return is;
    }

    void display() const {
        cout << real << " + " << imaginary << "i" << endl;
    }


};

int main() {
    setlocale(LC_ALL, "rus");
    // TODO: Создать несколько комплексных чисел
    Complex c1 = Complex(1, 2);
    Complex c2 = Complex(2, 3);
    cout << "Создали 2 комплексных числа: " << c1 << "; " << c2 << endl;

    // Продемонстрировать работу перегруженных операторов
    // Показать ввод/вывод через перегруженные операторы << и >>
    cout << "Сложение: " << c1 + c2 << endl;
    cout << "Вычитание: " << c1 - c2 << endl;
    cout << "Произведение: " << c1 * c2 << endl;
    cout << "Равенство: " << (c1 == c2 ? "TRUE" : "False") << endl;

    return 0;
}