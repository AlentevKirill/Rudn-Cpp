//
// RUDN C++
//

/*
Создайте структуру Complex для представления комплексного числа (действительная и мнимая части).

1. Напишите функции для основных операций:
	Complex add(Complex a, Complex b); // Сложение
	Complex subtract(Complex a, Complex b); // Вычитание
	Complex multiply(Complex a, Complex b); // Умножение
	void print(Complex c); // Вывод в форме (a + bi)
2. В функции main продемонстрируйте работу всех функций, создав несколько комплексных чисел и выполнив с ними операции.
*/

#include <iostream>

using namespace std;

struct Complex {
    double Re;
    double Im;
};

Complex add(Complex a, Complex b) {
    Complex result{};
    result.Re = a.Re + b.Re;
    result.Im = a.Im + b.Im;
    return result;
}

Complex subtract(Complex a, Complex b) {
    Complex result{};
    result.Re = a.Re - b.Re;
    result.Im = a.Im - b.Im;
    return result;
}

Complex multiply(Complex a, Complex b) {
    Complex result{};
    result.Re = a.Re * b.Re - a.Im * b.Im;
    result.Im = a.Re * b.Im + a.Im * b.Re;
    return result;
}

void print(Complex c) {
    cout << "(" << c.Re << " + " << c.Im << "i" << ")" << endl;
}

int main() {
    setlocale(LC_ALL, "rus");
    cout << "Инициализируем пару комплексных чисел: a = (1 + 2i) и b = (3.1 + 5.5i)" << endl;
    Complex a{}, b{};
    a.Re = 1;
    a.Im = 2;
    b.Re = 3.1;
    b.Im = 5.5;
    cout << "Результат сложения a + b: ";
    print(add(a, b));
    cout << "Результат вычитания a - b: ";
    print(subtract(a, b));
    cout << "Результат умножения a * b: ";
    print(multiply(a, b));
}