#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <fstream>

using namespace std;
using namespace chrono;

//////////////////////////////////////////////////
// TABLICA DYNAMICZNA
//////////////////////////////////////////////////

class DynamicArray {

private:

    int* arr;
    int size;
    int capacity;

    void resize() {

        capacity *= 2;

        int* newArr = new int[capacity];

        for(int i=0;i<size;i++)
            newArr[i] = arr[i];

        delete[] arr;

        arr = newArr;
    }

public:

    DynamicArray(int cap = 4) {

        capacity = cap;
        size = 0;

        arr = new int[capacity];
    }

    // COPY CONSTRUCTOR

    DynamicArray(const DynamicArray& other) {

        size = other.size;
        capacity = other.capacity;

        arr = new int[capacity];

        for(int i=0;i<size;i++)
            arr[i] = other.arr[i];
    }

    void addFront(int val) {

        if(size == capacity) resize();

        for(int i=size;i>0;i--)
            arr[i] = arr[i-1];

        arr[0] = val;

        size++;
    }

    void addBack(int val) {

        if(size == capacity) resize();

        arr[size++] = val;
    }

    void addAt(int index,int val) {

        if(size == capacity) resize();

        for(int i=size;i>index;i--)
            arr[i] = arr[i-1];

        arr[index] = val;

        size++;
    }

    void removeFront() {

        for(int i=0;i<size-1;i++)
            arr[i] = arr[i+1];

        size--;
    }

    void removeBack() {

        size--;
    }

    void removeAt(int index) {

        for(int i=index;i<size-1;i++)
            arr[i] = arr[i+1];

        size--;
    }

    int search(int val) {

        for(int i=0;i<size;i++)
            if(arr[i] == val)
                return i;

        return -1;
    }

};

//////////////////////////////////////////////////
// LISTA WIĄZANA
//////////////////////////////////////////////////

struct Node {

    int data;
    Node* next;

};

class LinkedList {

private:

    Node* head;

public:

    LinkedList() {

        head = nullptr;
    }

    // COPY CONSTRUCTOR

    LinkedList(const LinkedList& other) {

        head = nullptr;

        Node* temp = other.head;
        Node* tail = nullptr;

        while(temp) {

            Node* newNode = new Node{temp->data,nullptr};

            if(!head) {

                head = newNode;
                tail = newNode;

            } else {

                tail->next = newNode;
                tail = newNode;

            }

            temp = temp->next;
        }
    }

    void addFront(int val) {

        Node* n = new Node{val,head};

        head = n;
    }

    void addBack(int val) {

        Node* n = new Node{val,nullptr};

        if(!head) {

            head = n;
            return;
        }

        Node* t = head;

        while(t->next)
            t = t->next;

        t->next = n;
    }

    void addAt(int index,int val) {

        if(index == 0) {

            addFront(val);
            return;
        }

        Node* t = head;

        for(int i=0;i<index-1;i++)
            t = t->next;

        Node* n = new Node{val,t->next};

        t->next = n;
    }

    void removeFront() {

        Node* t = head;

        head = head->next;

        delete t;
    }

    void removeBack() {

        if(!head->next) {

            delete head;
            head = nullptr;

            return;
        }

        Node* t = head;

        while(t->next->next)
            t = t->next;

        delete t->next;

        t->next = nullptr;
    }

    void removeAt(int index) {

        if(index == 0) {

            removeFront();
            return;
        }

        Node* t = head;

        for(int i=0;i<index-1;i++)
            t = t->next;

        Node* del = t->next;

        t->next = del->next;

        delete del;
    }

    int search(int val) {

        Node* t = head;

        int i = 0;

        while(t) {

            if(t->data == val)
                return i;

            t = t->next;

            i++;
        }

        return -1;
    }

};

//////////////////////////////////////////////////
// USUWANIE SKRAJNYCH WARTOŚCI
//////////////////////////////////////////////////

double averageTrim(vector<long long> times) {

    sort(times.begin(),times.end());

    int trim = times.size() * 0.05;

    vector<long long> trimmed(
        times.begin()+trim,
        times.end()-trim
    );

    double sum = accumulate(trimmed.begin(),trimmed.end(),0.0);

    return sum / trimmed.size();
}

//////////////////////////////////////////////////
// POMIAR CZASU
//////////////////////////////////////////////////

template<typename Func>
double measure(Func f) {

    vector<long long> times;

    for(int i=0;i<100;i++) {

        auto start = high_resolution_clock::now();

        f();

        auto end = high_resolution_clock::now();

        times.push_back(
            duration_cast<nanoseconds>(end-start).count()
        );
    }

    return averageTrim(times);
}

//////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////

int main() {

    vector<int> sizes = {1000,5000,10000,50000,100000};

    ofstream file("results.csv");

    file<<"Structure,Operation,Size,Time(ns)\n";

    for(int n : sizes) {

        DynamicArray arr;
        LinkedList list;

        for(int i=0;i<n;i++) {

            arr.addBack(i);
            list.addBack(i);
        }

        double t;

        // TABLICA

        t = measure([&](){

            DynamicArray temp = arr;
            temp.addFront(1);
        });

        file<<"DynamicArray,AddFront,"<<n<<","<<t<<"\n";

        t = measure([&](){

            DynamicArray temp = arr;
            temp.addBack(1);
        });

        file<<"DynamicArray,AddBack,"<<n<<","<<t<<"\n";

        t = measure([&](){

            DynamicArray temp = arr;
            temp.addAt(n/2,1);
        });

        file<<"DynamicArray,AddMiddle,"<<n<<","<<t<<"\n";

        t = measure([&](){

            DynamicArray temp = arr;
            temp.removeFront();
        });

        file<<"DynamicArray,RemoveFront,"<<n<<","<<t<<"\n";

        t = measure([&](){

            DynamicArray temp = arr;
            temp.removeBack();
        });

        file<<"DynamicArray,RemoveBack,"<<n<<","<<t<<"\n";

        t = measure([&](){

            DynamicArray temp = arr;
            temp.removeAt(n/2);
        });

        file<<"DynamicArray,RemoveMiddle,"<<n<<","<<t<<"\n";

        t = measure([&](){

            DynamicArray temp = arr;
            temp.search(n/2);
        });

        file<<"DynamicArray,Search,"<<n<<","<<t<<"\n";

        // LISTA

        t = measure([&](){

            LinkedList temp = list;
            temp.addFront(1);
        });

        file<<"LinkedList,AddFront,"<<n<<","<<t<<"\n";

        t = measure([&](){

            LinkedList temp = list;
            temp.addBack(1);
        });

        file<<"LinkedList,AddBack,"<<n<<","<<t<<"\n";

        t = measure([&](){

            LinkedList temp = list;
            temp.addAt(n/2,1);
        });

        file<<"LinkedList,AddMiddle,"<<n<<","<<t<<"\n";

        t = measure([&](){

            LinkedList temp = list;
            temp.removeFront();
        });

        file<<"LinkedList,RemoveFront,"<<n<<","<<t<<"\n";

        t = measure([&](){

            LinkedList temp = list;
            temp.removeBack();
        });

        file<<"LinkedList,RemoveBack,"<<n<<","<<t<<"\n";

        t = measure([&](){

            LinkedList temp = list;
            temp.removeAt(n/2);
        });

        file<<"LinkedList,RemoveMiddle,"<<n<<","<<t<<"\n";

        t = measure([&](){

            LinkedList temp = list;
            temp.search(n/2);
        });

        file<<"LinkedList,Search,"<<n<<","<<t<<"\n";
    }

    file.close();

    cout<<"Pomiar zakonczony.\n";
    cout<<"Wyniki zapisane w results.csv\n";
}