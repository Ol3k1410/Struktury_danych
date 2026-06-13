#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <string>
#include <random>

using namespace std;
using namespace std::chrono;

class HashTable {
private:
    int size;
    vector<list<string>> table;

public:
    HashTable(int s) {
        size = s;
        table.resize(size);
    }

    int hashDivision(const string& key) {
        unsigned long hash = 0;

        for (char c : key) {
            hash += c;
        }

        return hash % size;
    }

    int hashMultiplication(const string& key) {
        unsigned long hash = 0;

        for (char c : key) {
            hash = hash * 31 + c;
        }

        double A = 0.6180339887;
        double frac = (hash * A) - floor(hash * A);

        return (int)(size * frac);
    }

    
    int hashDJB2(const string& key) {
        unsigned long hash = 5381;

        for (char c : key) {
            hash = ((hash << 5) + hash) + c;
        }

        return hash % size;
    }

    int getHash(const string& key, int type) {
        switch (type) {
        case 1:
            return hashDivision(key);

        case 2:
            return hashMultiplication(key);

        case 3:
            return hashDJB2(key);

        default:
            return hashDivision(key);
        }
    }

    void insert(const string& key, int type) {
        int index = getHash(key, type);
        table[index].push_back(key);
    }

    void removeKey(const string& key, int type) {

        int index = getHash(key, type);

        auto& bucket = table[index];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {

            if (*it == key) {
                bucket.erase(it);
                return;
            }
        }
    }

    bool search(const string& key, int type) {

        int index = getHash(key, type);

        for (const auto& element : table[index]) {

            if (element == key) {
                return true;
            }
        }

        return false;
    }

    void clear() {

        for (auto& bucket : table) {
            bucket.clear();
        }
    }
};

string randomString(int length) {

    static const string chars =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    static random_device rd;
    static mt19937 gen(rd());

    uniform_int_distribution<> dist(0, chars.size() - 1);

    string result;

    for (int i = 0; i < length; i++) {
        result += chars[dist(gen)];
    }

    return result;
}

vector<string> generateBestCase(int N) {

    vector<string> data;
    data.reserve(N);

    for (int i = 0; i < N; i++) {
        data.push_back(randomString(12) + to_string(i));
    }

    return data;
}

vector<string> generateAverageCase(int N) {

    vector<string> data;
    data.reserve(N);

    for (int i = 0; i < N; i++) {
        data.push_back(randomString(10));
    }

    return data;
}

vector<string> generateWorstCase(int N, HashTable& ht, int hashType) {
    vector<string> data;
    data.reserve(N);

    string base = randomString(8);
    int targetBucket = ht.getHash(base, hashType);
    data.push_back(base);

    int counter = 0;

    if (hashType == 1) {
        while ((int)data.size() < N) {
            string candidate = "KEY_" + to_string(counter++);
            if (ht.getHash(candidate, hashType) == targetBucket) {
                data.push_back(candidate);
            }
        }
    }
    else {
        string worstString = "WORST_CASE_STRING_ANOMALY";
        for (int i = 1; i < N; i++) {
            data.push_back(worstString);
        }
    }

    return data;
}

void testCase(
    const string& caseName,
    int hashType)
{
    const int N = 10000;

    HashTable ht(10007);

    vector<string> data;

    if (caseName == "best") {

        data = generateBestCase(N);

    }
    else if (caseName == "average") {

        data = generateAverageCase(N);

    }
    else {

        data = generateWorstCase(
            N,
            ht,
            hashType);
    }

    auto startInsert =
        high_resolution_clock::now();

    for (const auto& s : data) {
        ht.insert(s, hashType);
    }

    auto endInsert =
        high_resolution_clock::now();

    auto insertTime =
        duration_cast<milliseconds>(
            endInsert - startInsert);

    auto startSearch =
        high_resolution_clock::now();

    for (const auto& s : data) {
        ht.search(s, hashType);
    }

    auto endSearch =
        high_resolution_clock::now();

    auto searchTime =
        duration_cast<milliseconds>(
            endSearch - startSearch);

    auto startRemove =
        high_resolution_clock::now();

    for (const auto& s : data) {
        ht.removeKey(s, hashType);
    }

    auto endRemove =
        high_resolution_clock::now();

    auto removeTime =
        duration_cast<milliseconds>(
            endRemove - startRemove);

    cout << "=====================================\n";

    cout << "Hash Function: ";

    if (hashType == 1)
        cout << "Division\n";

    else if (hashType == 2)
        cout << "Multiplication\n";

    else
        cout << "DJB2\n";

    cout << "Case: "
        << caseName
        << "\n";

    cout << "Insert time: "
        << insertTime.count()
        << " ms\n";

  

    cout << "Remove time: "
        << removeTime.count()
        << " ms\n";

    cout << "=====================================\n\n";
}

int main() {

    cout << "========== HASH TABLE PROJECT ==========\n\n";

    for (int hashType = 1;
        hashType <= 3;
        hashType++)
    {
        testCase("best", hashType);

        testCase("average", hashType);

        testCase("worst", hashType);
    }

    return 0;
}
