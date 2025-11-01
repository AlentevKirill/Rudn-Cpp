//
// Created by Kirill on 01.11.2025.
//

#include <iostream>
#include <vector>
#include <stdexcept>

using namespace std;

template<typename T>

class Stack {

private:
    vector<T> elements;
    int capacity;

public:

    Stack(int size = 10) : capacity(size) {}

    // TODO: Реализовать методы:

    // - void push(const T& element)
    void push(const T& element) {
        elements.push_back(element);
    }

    // - T pop()
    T pop() {
        if (elements.empty()) {
            throw out_of_range("Стэк пуст");
        }
        T last = elements.back();
        elements.pop_back();
        return last;
    }

    // - T top() const
    T top() const {
        return elements.back();
    }

    // - bool isEmpty() const
    bool isEmpty() const {
        return size() == 0;
    }

    // - bool isFull() const
    bool isFull() const {
        return size() == capacity;
    }

    // - int size() const
    int size() const {
        return elements.size();
    }

    void display() const {
        cout << "Стек: ";
        for (const auto& elem : elements) {
            cout << elem << " ";
        }
        cout << endl;
    }
};

template<typename T>
class Queue {
private:
    vector<T> elements;
    int capacity;

public:
    Queue(int size = 10) : capacity(size) {}

    // TODO: Реализовать методы:

    // - void enqueue(const T& element)
    void enqueue(const T& element) {
        elements.push_back(element);
    }

    // - T dequeue()
    T dequeue() {
        if (elements.empty()) {
            throw out_of_range("Очередь пуста");
        }
        T first = front();
        elements.erase(elements.begin());;
        return first;
    }

    // - T front() const
    T front() const {
        return elements.front();
    }

    // - bool isEmpty() const
    bool isEmpty() const {
        return size() == 0;
    }

    // - bool isFull() const
    bool isFull() const {
        return size() == capacity;
    }

    // - int size() const
    int size() const {
        return elements.size();
    }

    void display() const {
        cout << "Очередь: ";
        for (const auto& elem : elements) {
            cout << elem << " ";
        }
        cout << endl;
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    // TODO: Протестировать Stack и Queue с разными типами данных:

    // - int
    cout << "Тест int:" << endl;

    Stack<int> st1 = Stack<int>();
    st1.push(1);
    st1.push(2);
    st1.push(3);
    st1.display();
    cout << "Размер: " << st1.size() << endl;
    cout << "Пустой? - " << (st1.isEmpty() ? "Да" : "Нет") << endl;
    cout << "Полный? - " << (st1.isFull() ? "Да" : "Нет") << endl;
    cout << "Последний элемент: " << st1.top() << endl;
    cout << "Удалили: " << st1.pop() << " элемент" << endl;
    st1.display();

    cout << endl;

    Queue<int> q1 = Queue<int>();
    q1.enqueue(1);
    q1.enqueue(2);
    q1.enqueue(3);
    q1.display();
    cout << "Размер: " << q1.size() << endl;
    cout << "Пустой? - " << (q1.isEmpty() ? "Да" : "Нет") << endl;
    cout << "Полный? - " << (q1.isFull() ? "Да" : "Нет") << endl;
    cout << "Последний элемент: " << q1.front() << endl;
    cout << "Удалили: " << q1.dequeue() << " элемент" << endl;
    q1.display();

    cout << endl << "----------------------------------------------------------------" << endl << endl;

    // - double
    cout << "Тест double:" << endl;

    Stack<double> st2 = Stack<double>();
    st2.push(1.1);
    st2.push(2.2);
    st2.push(3.3);
    st2.display();
    cout << "Размер: " << st2.size() << endl;
    cout << "Пустой? - " << (st2.isEmpty() ? "Да" : "Нет") << endl;
    cout << "Полный? - " << (st2.isFull() ? "Да" : "Нет") << endl;
    cout << "Последний элемент: " << st2.top() << endl;
    cout << "Удалили: " << st2.pop() << " элемент" << endl;
    st2.display();

    cout << endl;

    Queue<double> q2 = Queue<double>();
    q2.enqueue(1.1);
    q2.enqueue(2.2);
    q2.enqueue(3.3);
    q2.display();
    cout << "Размер: " << q2.size() << endl;
    cout << "Пустой? - " << (q2.isEmpty() ? "Да" : "Нет") << endl;
    cout << "Полный? - " << (q2.isFull() ? "Да" : "Нет") << endl;
    cout << "Последний элемент: " << q2.front() << endl;
    cout << "Удалили: " << q2.dequeue() << " элемент" << endl;
    q2.display();

    cout << "----------------------------------------------------------------" << endl << endl;

    // - string
    cout << "Тест string:" << endl;

    Stack<string> st3 = Stack<string>();
    st3.push("a");
    st3.push("b");
    st3.push("c");
    st3.display();
    cout << "Размер: " << st3.size() << endl;
    cout << "Пустой? - " << (st3.isEmpty() ? "Да" : "Нет") << endl;
    cout << "Полный? - " << (st3.isFull() ? "Да" : "Нет") << endl;
    cout << "Последний элемент: " << st3.top() << endl;
    cout << "Удалили: " << st3.pop() << " элемент" << endl;
    st3.display();

    cout << endl;

    Queue<string> q3 = Queue<string>();
    q3.enqueue("a");
    q3.enqueue("b");
    q3.enqueue("c");
    q3.display();
    cout << "Размер: " << q3.size() << endl;
    cout << "Пустой? - " << (q3.isEmpty() ? "Да" : "Нет") << endl;
    cout << "Полный? - " << (q3.isFull() ? "Да" : "Нет") << endl;
    cout << "Последний элемент: " << q3.front() << endl;
    cout << "Удалили: " << q3.dequeue() << " элемент" << endl;
    q3.display();

    return 0;
}