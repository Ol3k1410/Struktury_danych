#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <algorithm>
#include <cstdlib>

using namespace std;

struct Element {
    int value;
    int priority;

    Element(int v, int p) : value(v), priority(p) {}
};

class IPriorityQueue {
public:
    virtual void push(int value, int priority) = 0;
    virtual void pop() = 0;
    virtual Element peek() = 0;
    virtual int size() = 0;
    virtual void changePriority(int value, int newPriority) = 0;
    virtual ~IPriorityQueue() {}
};

class BinaryHeap : public IPriorityQueue {
    vector<Element> heap;

    int parent(int i) { return (i - 1) / 2; }
    int left(int i) { return 2 * i + 1; }
    int right(int i) { return 2 * i + 2; }

    void heapifyUp(int i) {
        while (i > 0 && heap[parent(i)].priority < heap[i].priority) {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    void heapifyDown(int i) {
        int largest = i;
        int l = left(i), r = right(i);

        if (l < heap.size() && heap[l].priority > heap[largest].priority)
            largest = l;
        if (r < heap.size() && heap[r].priority > heap[largest].priority)
            largest = r;

        if (largest != i) {
            swap(heap[i], heap[largest]);
            heapifyDown(largest);
        }
    }

public:
    void push(int value, int priority) override {
        heap.emplace_back(value, priority);
        heapifyUp(heap.size() - 1);
    }

    void pop() override {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
    }

    Element peek() override { return heap.front(); }
    int size() override { return heap.size(); }

    void changePriority(int value, int newPriority) override {
        for (int i = 0; i < heap.size(); i++) {
            if (heap[i].value == value) {
                int old = heap[i].priority;
                heap[i].priority = newPriority;
                if (newPriority > old) heapifyUp(i);
                else heapifyDown(i);
                return;
            }
        }
    }
};
class UnsortedListPQ : public IPriorityQueue {
    list<Element> data;

public:
    void push(int v, int p) override { data.emplace_back(v, p); }

    void pop() override {
        if (data.empty()) return;
        auto maxIt = data.begin();
        for (auto it = data.begin(); it != data.end(); ++it)
            if (it->priority > maxIt->priority) maxIt = it;
        data.erase(maxIt);
    }

    Element peek() override {
        auto maxIt = data.begin();
        for (auto it = data.begin(); it != data.end(); ++it)
            if (it->priority > maxIt->priority) maxIt = it;
        return *maxIt;
    }

    int size() override { return data.size(); }

    void changePriority(int v, int p) override {
        for (auto& e : data)
            if (e.value == v) e.priority = p;
    }
};

class SortedListPQ : public IPriorityQueue {
    list<Element> data;

public:
    void push(int v, int p) override {
        auto it = data.begin();
        while (it != data.end() && it->priority > p) ++it;
        data.insert(it, Element(v, p));
    }

    void pop() override { if (!data.empty()) data.pop_front(); }
    Element peek() override { return data.front(); }
    int size() override { return data.size(); }

    void changePriority(int v, int p) override {
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->value == v) {
                data.erase(it);
                push(v, p);
                return;
            }
        }
    }
};

double averageTrim(vector<long long>& times) {
    sort(times.begin(), times.end());
    int cut = times.size() * 0.05;

    double sum = 0;
    for (int i = cut; i < times.size() - cut; i++)
        sum += times[i];

    return sum / (times.size() - 2 * cut);
}

template<typename Func>
double measure(Func f) {
    const int TRIALS = 100;
    vector<long long> times;

    for (int i = 0; i < TRIALS; i++) {
        auto start = chrono::high_resolution_clock::now();
        f();
        auto end = chrono::high_resolution_clock::now();

        times.push_back(
            chrono::duration_cast<chrono::nanoseconds>(end - start).count()
        );
    }

    return averageTrim(times);
}

const int N = 20000;

void fill(IPriorityQueue& pq) {
    for (int i = 0; i < N; i++)
        pq.push(i, rand() % 1000);
}

template<typename PQ>
void runTests(string name) {

    cout << "\n=== " << name << " ===\n";

    cout << "Insert: " << measure([&]() {
        PQ pq;
        for (int i = 0; i < N; i++)
            pq.push(i, rand() % 1000);
        }) << " ns\n";

    cout << "Delete: " << measure([&]() {
        PQ pq;
        fill(pq);
        for (int i = 0; i < N; i++)
            pq.pop();
        }) << " ns\n";

    cout << "Peek: " << measure([&]() {
        PQ pq;
        fill(pq);
        for (int i = 0; i < N; i++)
            pq.peek();
        }) << " ns\n";

    cout << "Size: " << measure([&]() {
        PQ pq;
        fill(pq);
        for (int i = 0; i < N; i++)
            pq.size();
        }) << " ns\n";

    cout << "ChangePriority: " << measure([&]() {
        PQ pq;
        fill(pq);
        for (int i = 0; i < 1000; i++)
            pq.changePriority(rand() % N, rand() % 1000);
        }) << " ns\n";
}

int main() {
    srand(time(nullptr));

    runTests<BinaryHeap>("Binary Heap");
    runTests<UnsortedListPQ>("Unsorted List");
    runTests<SortedListPQ>("Sorted List");

    return 0;
}
