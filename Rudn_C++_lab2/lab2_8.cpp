//
// RUDN C++
//

/*
Напишите одну универсальную функцию с именем arrayInfo, которая принимает указатель на массив целых чисел и его размер, и выводит на экран:
	- Размер массива.
	- Сумму элементов.
	- Первый и последний элемент.
Продемонстрируйте работу этой функции на двух разных массивах разной длины.
*/

#include <iostream>
#include <algorithm>
#include <climits>

using namespace std;

void arrayInfo(int* arr, int size) {
    cout << "Размер массива: " << size << endl;
    long long sum = 0;
    bool errFlag = false;
    for (int* iterator = arr; iterator < (arr + size); iterator++) {
        if (*iterator >= 0) {
            if (LLONG_MAX - sum < *iterator) {
                cout << "Сумма элементов массива вышла за границы типа long long (> 2^63 - 1)" << endl;
                errFlag = true;
                break;
            }
        } else {
            if (LLONG_MAX - abs(sum) < abs(*iterator)) {
                cout << "Сумма элементов массива вышла за границы типа long long (< -2^63)" << endl;
                errFlag = true;
                break;
            }
        }
        sum += *iterator;
    }
    if (!errFlag) {
        cout << "Сумма элементов массива: " << sum << endl;
    }
    cout << "Первый элемент массива: " << *arr << endl;
    cout << "Последний элемент массива: " << *(arr + size - 1) << endl;
}

int main() {
    setlocale(LC_ALL, "rus");
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {4, 6, 2, 7 ,3 ,7 ,2 ,1, 64, 6251, 535};

    cout << "Инициализирован массив 1 числами: ";
    for (int i : arr1) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Информация о массиве 1:" << endl;
    arrayInfo(arr1, sizeof(arr1) / sizeof(arr1[0]));

    cout << endl;

    cout << "Инициализирован массив 2 числами: ";
    for (int i : arr2) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Информация о массиве 2:" << endl;
    arrayInfo(arr2, sizeof(arr2) / sizeof(arr2[0]));

}