//
// RUDN C++
//

/*
Создайте структуру Complex для представления комплексного числа (действительная и мнимая части).

Напишите программу, которая демонстрирует разницу между передачей по значению, по указателю и по ссылке.

	1. Создайте функцию void swapValues(int a, int b), которая пытается обменять значения (не получится).
	2. Создайте функцию void swapValues(int* a, int* b), которая обменивает значения с помощью указателей.
	3. Создайте функцию void swapValues(int& a, int& b), которая обменивает значения с помощью ссылок.
	4. В main покажите работу всех трех функций на примере двух целых чисел.
*/

#include <iostream>

using namespace std;

void swapValuesBase(int a, int b) {
    int temp;
    temp = a;
    a = b;
    b = temp;
}

void swapValuesCursor(int* a, int* b) {
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void swapValuesLink(int& a, int& b) {
    int temp;
    temp = a;
    a = b;  
    b = temp;
}

int main() {
    setlocale(LC_ALL, "rus");
    cout << "Введены 2 целых числа: a = 1, b = 2" << endl;
    int a = 1;
    int b = 2;

    swapValuesBase(a, b);
    cout << "В результате работы void swapValues(int a, int b) a = " << a << " b = " << b << endl;

    swapValuesCursor(&a, &b);
    cout << "В результате работы void swapValues(int* a, int* b) a = " << a << " b = " << b << endl;

    swapValuesLink(a, b);
    cout << "В результате работы void swapValues(int& a, int& b) a = " << a << " b = " << b << endl;
}