//
// RUDN C++
//

/*
Напишите программу, которая:

1. Запрашивает у пользователя количество дней N для анализа.
2. Создает массив для хранения дневных температур (тип double).
3. Заполняет массив значениями, введенными пользователем.
4. Вычисляет и выводит:
	-Среднюю температуру за период.
	-Минимальную и максимальную температуру.
	-Количество дней, когда температура была ниже средней.
*/


#include <iostream>
#include <iomanip>
#include <limits>
#include <cfloat>

using namespace std;



int main() {
    setlocale(LC_ALL, "rus");
    int N;
    while (true) {
        cout << "Введите количество дней отслеживания температуры: ";
        cin >> N;
        if (cin.fail()) {
            cout << "Ошибка! Введите корректное натуральное число." << endl;
            cin.clear(); // Сбрасываем флаги ошибок
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищаем буфер
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищаем буфер
            if (N < 0) {
                cout << "Ошибка! Введённое число должно быть натуральным.\n";
            } else {
                break;
            }
        }
    }

    double temperature[N];
    double sum = 0;
    double min = DBL_MAX;
    double max = DBL_MIN;
    for (int i = 0; i != N; i++){
        while (true) {
            cout << "Введите показатель температуры в день " << i + 1 << ": ";
            cin >> temperature[i];
            if (cin.fail()) {
                cout << "Ошибка! Введите корректное рациональное число." << endl;
                cin.clear(); // Сбрасываем флаги ошибок
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищаем буфер
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищаем буфер
                break;
            }
        }
        sum += temperature[i];
        if (temperature[i] < min) {
            min = temperature[i];
        }
        if (temperature[i] > max) {
            max = temperature[i];
        }
    }
    double avg = sum / N;
    int k_low = 0;
    for (int i = 0; i != N; i++){
        if (temperature[i] < avg) {
            k_low++;
        }
    }
    cout << "Результат:" << endl;
    cout << "Средняя температура за период: " << avg << endl;
    cout << "Минимальное значение температура: " << min << endl;
    cout << "Максимальное значение температура: " << max << endl;
    cout << "Количество дней, когда температура была ниже средней: " << k_low << endl;
}