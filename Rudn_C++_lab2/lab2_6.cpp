//
// RUDN C++
//

/*
Напишите программу, которая выполняет следующие операции с целочисленным массивом, используя только указатели и арифметику указателей (без использования оператора индексации []).

	1. Заполните массив из 10 чисел последовательностью Фибоначчи.
	2. Найдите сумму всех элементов массива.
	3. Найдите минимальный элемент в массиве.
	4. Скопируйте содержимое первого массива во второй массив в обратном порядке.
*/

#include <iostream>
#include <algorithm>

using namespace std;

int main() {
    setlocale(LC_ALL, "rus");
    const int SIZE = 10;
    int* arr = (int*) malloc(SIZE * sizeof(int));
    int* iterator = arr;
    *iterator++ = 0;
    *iterator++ = 1;
    int sum = 1;
    int min = *arr; // Для нашего примера очевидно что минмум будет 0
    cout << "Инициализированна последовательность Фибоначчи из " << SIZE << " элементов:" << endl;
    cout << *arr << endl;
    cout << *(arr + 1) << endl;
    for (int i = 2; i < SIZE; i++) {
        *iterator = *(iterator - 1) + *(iterator - 2);
        sum += *iterator;
        if (*iterator < min) {
            min = *iterator;
        }
        cout << *iterator << endl;
        iterator++;
    }
    cout << "Сумма элементов последовательности: " << sum << endl;
    cout << "Минимальный элемент последовательности: " << min << endl;

    int* arr2 = (int*) malloc(SIZE * sizeof(int));
    int* iterator2 = arr2;
    iterator = arr;
    iterator2 += 9;
    for (int i = 0; i < SIZE; i++) {
        *iterator2-- = *iterator++;
    }
    cout << "Элементы последовательности в обратном порядке: " << endl;
    iterator2 = arr2;
    for (int i = 0; i < SIZE; i++) {
        cout << *iterator2++ << endl;
    }

    free(arr);
    free(arr2);
    free(iterator);
    free(iterator2);
}