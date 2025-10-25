//
// RUDN C++
//

/*
Напишите программу, которая работает со строкой C-style (char[]):

1. Пользователь вводит строку (можно использовать std::cin.getline).
2. Программа определяет длину строки без использования стандартной функции strlen.
3. Программа переворачивает строку на месте (in-place), т.е. не создавая новый массив.
4. Программа подсчитывает количество вхождений определенного символа (также введенного пользователем) в строку.
*/

#include <iostream>
#include <algorithm>

using namespace std;



int main() {
    setlocale(LC_ALL, "rus");
    const int SIZE = 10000;
    // Выделяем память под массив символов с помощью malloc без автозаполнения
    char* text = (char*) malloc(SIZE * sizeof(char));

    if (text == nullptr) {
        cerr << "Ошибка выделения памяти!" << endl;
        return 1;
    }

    cout << "Введите строку: ";
    cin.getline(text, SIZE);

    int length = 0;
    char* ptr = text;
    while (*ptr != '\0') {
        length++;
        ptr++;
    }
    cout << "Длина строки: " << length << endl;

    char temp;
    for (int i = 0; i < length / 2; i++) {
        // Меняем симметричные символы местами
        temp = text[i];
        text[i] = text[length - 1 - i];
        text[length - 1 - i] = temp;
    }
    cout << "Перевёрнутая строка: " << text << endl;

    char input_sym;
    int count_sym;
    while (true) {
        cout << "Введите символ, количество вхождений которого вам надо посчитать\nЕсли захотите завершить программу, введите символ '0': ";
        cin >> input_sym;
        if (input_sym == '0') {
            break;
        }
        count_sym = 0;
        for (int i = 0; i < length; i++) {
            if (text[i] == input_sym) {
                count_sym++;
            }
        }
        cout << "Количество вхождений символа " << input_sym << ": " << count_sym << endl;
    }

    // Освобождаем выделенную память
    free(text);
	free(ptr);
}