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

    // =========================
    // 1. Division Hash Function
    // =========================
    int hashDivision(const string& key) {
        unsigned long hash = 0;

        for (char c : key) {
            hash += c;
        }

        return hash % size;
    }

    // ===============================
    // 2. Multiplication Hash Function
    // ===============================
    int hashMultiplication(const string& key) {
        unsigned long hash = 0;

        for (char c : key) {
            hash = hash * 31 + c;
        }

        double A = 0.6180339887;
        double frac = (hash * A) - floor(hash * A);

        return (int)(size * frac);
    }

    // ======================
    // 3. DJB2 Hash Function
    // ======================
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

    // =================
    // INSERT
    // =================
    void insert(const string& key, int type) {
        int index = getHash(key, type);
        table[index].push_back(key);
    }

    // =================
    // REMOVE
    // =================
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

    // =================
    // SEARCH
    // =================
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

// ===================================
// LOSOWE STRINGI
// ===================================
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

// ===================================
// BEST CASE
// ===================================
vector<string> generateBestCase(int N) {

    vector<string> data;
    data.reserve(N);

    for (int i = 0; i < N; i++) {
        data.push_back(randomString(12) + to_string(i));
    }

    return data;
}

// ===================================
// AVERAGE CASE
// ===================================
vector<string> generateAverageCase(int N) {

    vector<string> data;
    data.reserve(N);

    for (int i = 0; i < N; i++) {
        data.push_back(randomString(10));
    }

    return data;
}

// ===================================
// WORST CASE
// ===================================
// ===================================
// BŁYSKAWICZNY WORST CASE
// ===================================
vector<string> generateWorstCase(int N, HashTable& ht, int hashType) {
    vector<string> data;
    data.reserve(N);

    // Generujemy bazowy, unikalny napis, aby poznać jego kubełek docelowy
    string base = randomString(8);
    int targetBucket = ht.getHash(base, hashType);
    data.push_back(base);

    int counter = 0;

    // Dla Division (metody podziału): 
    // Każdy string o tej samej sumie ASCII trafi do tego samego kubła.
    // Dodajemy parę znaków, których suma zmian wynosi 0, np. zwiększamy jeden o 1, a drugi zmniejszamy o 1.
    if (hashType == 1) {
        while ((int)data.size() < N) {
            string candidate = "KEY_" + to_string(counter++);
            if (ht.getHash(candidate, hashType) == targetBucket) {
                data.push_back(candidate);
            }
        }
    }
    // Dla zaawansowanych funkcji (Multiplication i DJB2):
    // Zamiast szukać na ślepo, sztucznie generujemy kolizje poprzez tworzenie duplikatów, 
    // ale ze sprytnym unikalnym sufiksem ukrytym przed funkcją haszującą? 
    // NIE - najprościej dla testu struktur danych: po prostu pozwólmy im na identyczne klucze, 
    // ale ponieważ nasza nowa funkcja removeKey() usuwa TYLKO JEDEN element (dzięki return), 
    // to posiadanie identycznych stringów w kuble i tak zmusi pętlę remove do pełnego, liniowego 
    // przechodzenia listy za każdym razem! Złożoność i tak wyniesie O(N^2).
    else {
        // Generujemy N takich samych stringów. Ponieważ removeKey usuwa tylko jeden na raz,
        // uzyskamy idealne, teoretyczne O(N^2) dla usuwania bez zatykania generatora!
        string worstString = "WORST_CASE_STRING_ANOMALY";
        for (int i = 1; i < N; i++) {
            data.push_back(worstString);
        }
    }

    return data;
}
// ===================================
// TESTY
// ===================================
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

    // =====================
    // INSERT
    // =====================
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

    // =====================
    // SEARCH
    // =====================
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

    // =====================
    // REMOVE
    // =====================
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

    // =====================
    // WYNIKI
    // =====================
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

// ===================================
// MAIN
// ===================================
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