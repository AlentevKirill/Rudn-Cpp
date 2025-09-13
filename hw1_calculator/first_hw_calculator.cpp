//
// RUDN C++
//

#include <iostream>
#include <vector>
#include <cmath>
#include <windows.h>
#include <iomanip>
#include <limits>
#include <map>
#include <algorithm>
#include <bitset>
#include <cstdint>

using namespace std;

/*bool searchMark(const string& str, char mark) {
    return str.find(mark) != string::npos;
}*/

/*double tryCalculate(string str, char mark) {
    int
}*/

int main() {
    /*double eps = 1e-6;
    string inputString;
    getline(cin, inputString);
    inputString.erase(remove_if(
            inputString.begin(),
            inputString.end(),
            [](unsigned char x) { return std::isspace(x); }), inputString.end());
    if (searchMark(inputString, '+')) {
        int pos = inputString.find('+');
        double first = stod(inputString.substr(0, pos));
        double second = stod(inputString.substr(pos + 2));
        cout << "Result: " << first + second;
    }
    if (searchMark(inputString, '*')) {
        int pos = inputString.find('*');
        double first = stod(inputString.substr(0, pos));
        double second = stod(inputString.substr(pos + 2));
        cout << "Result: " << first * second;
    }
    if (searchMark(inputString, '-')) {
        int pos = inputString.find('-');
        double first = stod(inputString.substr(0, pos));
        double second = stod(inputString.substr(pos + 2));
        cout << "Result: " << first - second;
    }
    if (searchMark(inputString, '/')) {
        int pos = inputString.find('/');
        double first = stod(inputString.substr(0, pos));
        double second = stod(inputString.substr(pos + 2));
        if (second < eps) {
            cout << "Division by zero";
        } else {
            cout << "Result: " << first / second;
        }
    }
    if (searchMark(inputString, '^')) {
        int pos = inputString.find('^');
        double first = stod(inputString.substr(0, pos));
        double second = stod(inputString.substr(pos + 2));
        cout << "Result: " << pow(first, second);
    }
    if (inputString.find("sin") != string::npos) {
        int pos = inputString.find("sin");
        double num = stod(inputString.substr(pos + 4, inputString.length() - 5));
        cout << "Result: " << sin(num);
    }
    if (inputString.find("cos") != string::npos) {
        int pos = inputString.find("cos");
        double num = stod(inputString.substr(pos + 4, inputString.length() - 5));
        cout << "Result: " << cos(num);
    }
    if (inputString.find("tg") != string::npos) {
        int pos = inputString.find("tg");
        double num = stod(inputString.substr(pos + 3, inputString.length() - 4));
        cout << "Result: " << tan(num);
    }
    if (inputString.find("ctg") != string::npos) {
        int pos = inputString.find("ctg");
        double num = stod(inputString.substr(pos + 4, inputString.length() - 5));
        cout << "Result: " << 1/tan(num);
    }
    if (inputString.find("log") != string::npos) {
        int pos = inputString.find("log");
        double num = stod(inputString.substr(pos + 4, inputString.length() - 5));
        cout << "Result: " << log10(num);
    }
    if (inputString.find("ln") != string::npos) {
        int pos = inputString.find("ln");
        double num = stod(inputString.substr(pos + 3, inputString.length() - 4));
        cout << "Result: " << log(num);
    }
    if (inputString.find("sqrt") != string::npos) {
        int pos = inputString.find("sqrt");
        double num = stod(inputString.substr(pos + 5, inputString.length() - 6));
        cout << "Result: " << sqrt(num);
    }*/
    setlocale(LC_ALL, "rus");
    double eps = 1e-6;
    string operand;
    double x, y;
    vector<string> libUnaryOperation = {
            "sin",
            "cos",
            "tg",
            "ctg",
            "log10",
            "ln",
            "sqrt",
            "2to10",
            "10to2",
    };
    vector<string> libBinaryOperation = {
            "+",
            "-",
            "/",
            "*",
            "^"
    };
    cout << "В калькуляторе доступен следующий список команд:" << endl;
    for (const string& operation : libUnaryOperation) {
        cout << operation << endl;
    }
    for (const string& operation : libBinaryOperation) {
        cout << operation << endl;
    }
    cout << "Введите идентификатор операции из списка выше:" << endl;
    cin >> operand;
    if (find(libUnaryOperation.begin(), libUnaryOperation.end(), operand) != libUnaryOperation.end()) {
        if (operand == "2to10") {
            cout << "Введите двоичное представление целого числа: " << endl;
            string bin;
            cin >> bin;
            cout << "Результат: " << stoi(bin, nullptr, 2) << endl;
        } else if (operand == "10to2") {
            cout << "Введите целое число для преобразования: " << endl;
            long long x1;
            if (!(cin >> x1)) {
                cerr << "Введено некорректное целое число!";
            } else {
                size_t temp = x1 < 0 ? 1 : 0;
                x1 = abs(x1);
                string result = bitset<64>(x1).to_string();
                size_t pos = result.find_first_not_of('0', temp);
                cout << "Результат: ";
                if (pos == string::npos) {
                    cout << "0";
                } else {
                    result.erase(temp, pos - temp);
                    if (temp == 1) {
                        result[0] = '-';
                    }
                    cout << result;
                }
                cout << endl;
            }
        } else {
            cout << "Введите вещественное число: " << endl;
            if (!(cin >> x)) {
                cerr << "Введено некорректное вещественное число!";
            } else {
                if (operand == "sin") {
                    cout << "Результат: " << sin(x) << endl;
                } else if (operand == "cos") {
                    cout << "Результат: " << cos(x) << endl;
                } else if (operand == "tg") {
                    cout << "Результат: " << tan(x) << endl;
                } else if (operand == "ctg") {
                    cout << "Результат: " << 1 / tan(x) << endl;
                } else if (operand == "log10") {
                    cout << "Результат: " << log10(x) << endl;
                } else if (operand == "ln") {
                    cout << "Результат: " << log(x) << endl;
                } else if (operand == "sqrt") {
                    cout << "Результат: " << sqrt(x) << endl;
                }
            }
        }
    } else if (find(libBinaryOperation.begin(), libBinaryOperation.end(), operand) != libBinaryOperation.end()){
        cout << "Введите первое вещественное число: " << endl;
        if (!(cin >> x)) {
            cerr << "Введено некорректное вещественное число!";
        } else {
            cout << "Введите второе вещественное число: " << endl;
            if (!(cin >> y)) {
                cerr << "Введено некорректное вещественное число!";
            } else {
                if (operand == "+") {
                    cout << "Результат: " << x + y << endl;
                } else if (operand == "-") {
                    cout << "Результат: " << x - y << endl;
                } else if (operand == "*") {
                    cout << "Результат: " << x * y << endl;
                } else if (operand == "/") {
                    if (y < eps) {
                        cout << "На ноль делить нельзя";
                    } else {
                        cout << "Результат: " << x / y << endl;
                    }
                } else if (operand == "^") {
                    cout << "Результат: " << pow(x, y) << endl;
                }
            }
        }
    } else {
        cerr << "Был введен некорректный идентификатор операции!";
    }
}