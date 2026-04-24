#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>

#include "hash_table.h"

using namespace std;

using Time = chrono::high_resolution_clock;
using Seconds = chrono::duration<double>;

// -----------------------------
// Общие настройки
// -----------------------------

const vector<int> SIZES = {10'000, 50'000, 100'000, 200'000, 500'000, 1'000'000, 2'000'000, 5'000'000, 10'000'000, 15'000'000};
const int REPEATS = 5;
const int RANDOM_SEED = 42;

// -----------------------------
// Вспомогательные структуры
// -----------------------------

struct BasicPrepared {
    vector<int> keys;
    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;
};

struct MixedPrepared {
    vector<pair<string, int>> operations;
    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;
};

struct ProbeStats {
    double time;
    size_t calls;
    size_t steps;
};

// -----------------------------
// Вспомогательные функции
// -----------------------------

double median(vector<double> values) {
    sort(values.begin(), values.end());
    size_t n = values.size();

    if (n % 2 == 1) {
        return values[n / 2];
    }
    return (values[n / 2 - 1] + values[n / 2]) / 2.0;
}

void print_section(const string& title) {
    cout << "\n" << string(70, '=') << "\n";
    cout << title << "\n";
    cout << string(70, '=') << "\n";
}

void print_result_table(const string& scenario_name, const vector<tuple<int, double, double>>& results) {
    cout << "\n" << scenario_name << "\n";
    cout << string(70, '-') << "\n";
    cout << setw(13) << "N | " << setw(21) << "HashTable | " << setw(21) << "unordered_flat_map | " << setw(10) << "ratio" << "\n";
    cout << string(70, '-') << "\n";

    for (const auto& [n, ht_time, map_time] : results) {
        double ratio = (map_time > 0.0) ? (ht_time / map_time) : 0.0;

        cout << setw(10) << n << " | " << setw(14) << fixed << setprecision(6) << ht_time << " sec | " << setw(14) << fixed << setprecision(6) << map_time << " sec | " << setw(9) << fixed << setprecision(2) << ratio << "x\n";
    }
}

// -----------------------------
// Генерация данных
// -----------------------------

vector<int> make_sequential_keys(int n) {
    vector<int> keys;
    keys.reserve(n);

    for (int i = 0; i < n; ++i) {
        keys.push_back(i);
    }
    return keys;
}

vector<int> make_random_unique_keys(int n, int seed) {
    mt19937 rng(seed);
    vector<int> pool;
    pool.reserve(n * 20);

    for (int i = 0; i < n * 20; ++i) {
        pool.push_back(i);
    }

    shuffle(pool.begin(), pool.end(), rng);
    pool.resize(n);
    return pool;
}

vector<int> make_shuffled(const vector<int>& keys, int seed) {
    mt19937 rng(seed);
    vector<int> data = keys;
    shuffle(data.begin(), data.end(), rng);
    return data;
}

// ----------------------------
// 1. Insert sequential
// ----------------------------

BasicPrepared prepare_case_insert_sequential(int n) {
    return {make_sequential_keys(n), HashTable<int, int>(), {}};
}

double measure_insert_sequential_hash_table(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    HashTable<int, int> ht = prepared.ht.copy();

    Time::time_point start = Time::now();
    for (int key : keys) {
        ht.put(key, key);
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_insert_sequential_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    boost::unordered_flat_map<int, int> map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : keys) {
        map[key] = key;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// ----------------------------
// 2. Insert random
// ----------------------------

BasicPrepared prepare_case_insert_random(int n) {
    return {make_random_unique_keys(n, RANDOM_SEED), HashTable<int, int>(), {}};
}

double measure_insert_random_hash_table(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    HashTable<int, int> ht = prepared.ht.copy();

    Time::time_point start = Time::now();
    for (int key : keys) {
        ht.put(key, key);
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_insert_random_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    boost::unordered_flat_map<int, int> map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : keys) {
        map[key] = key;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// ----------------------------
// 3. Get existing sequential
// ----------------------------

BasicPrepared prepare_case_get_existing_sequential(int n) {
    vector<int> keys = make_sequential_keys(n);
    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;

    for (int key : keys) {
        ht.put(key, key);
        map[key] = key;
    }

    return {keys, ht, map};
}

double measure_get_existing_sequential_hash_table(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    const HashTable<int, int>& ht = prepared.ht;

    Time::time_point start = Time::now();
    for (int key : keys) {
        auto value = ht.get(key);
        (void)value;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_get_existing_sequential_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    const boost::unordered_flat_map<int, int>& map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : keys) {
        auto it = map.find(key);
        (void)it;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// ----------------------------
// 4. Get existing random order
// ----------------------------

BasicPrepared prepare_case_get_existing_random(int n) {
    vector<int> insert_keys = make_random_unique_keys(n, RANDOM_SEED);
    vector<int> query_keys = make_shuffled(insert_keys, RANDOM_SEED + 1);

    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;

    for (int key : insert_keys) {
        ht.put(key, key);
        map[key] = key;
    }

    return {query_keys, ht, map};
}

double measure_get_existing_random_hash_table(const BasicPrepared& prepared) {
    const vector<int>& query_keys = prepared.keys;
    const HashTable<int, int>& ht = prepared.ht;

    Time::time_point start = Time::now();
    for (int key : query_keys) {
        auto value = ht.get(key);
        (void)value;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_get_existing_random_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& query_keys = prepared.keys;
    const boost::unordered_flat_map<int, int>& map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : query_keys) {
        auto it = map.find(key);
        (void)it;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// ----------------------------
// 5. Get missing keys
// ----------------------------

BasicPrepared prepare_case_get_missing(int n) {
    vector<int> existing_keys = make_random_unique_keys(n, RANDOM_SEED);

    mt19937 rng(RANDOM_SEED + 1);
    vector<int> pool;
    pool.reserve(n * 20);

    for (int i = n * 20; i < n * 40; ++i) {
        pool.push_back(i);
    }

    shuffle(pool.begin(), pool.end(), rng);
    pool.resize(n);
    vector<int> missing_keys = make_shuffled(pool, RANDOM_SEED + 2);

    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;

    for (int key : existing_keys) {
        ht.put(key, key);
        map[key] = key;
    }

    return {missing_keys, ht, map};
}

double measure_get_missing_hash_table(const BasicPrepared& prepared) {
    const vector<int>& missing_keys = prepared.keys;
    const HashTable<int, int>& ht = prepared.ht;

    Time::time_point start = Time::now();
    for (int key : missing_keys) {
        auto value = ht.get(key);
        (void)value;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_get_missing_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& missing_keys = prepared.keys;
    const boost::unordered_flat_map<int, int>& map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : missing_keys) {
        auto it = map.find(key);
        (void)it;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// ----------------------------
// 6. Delete
// ----------------------------

BasicPrepared prepare_case_delete(int n) {
    vector<int> keys = make_sequential_keys(n);
    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;

    for (int key : keys) {
        ht.put(key, key);
        map[key] = key;
    }

    return {keys, ht, map};
}

double measure_delete_hash_table(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    HashTable<int, int> ht = prepared.ht.copy();

    Time::time_point start = Time::now();
    for (int key : keys) {
        ht.erase(key);
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_delete_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    boost::unordered_flat_map<int, int> map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : keys) {
        map.erase(key);
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// ----------------------------
// 7. Get after deletions
// ----------------------------

BasicPrepared prepare_case_get_after_deletions(int n) {
    vector<int> keys = make_random_unique_keys(n, RANDOM_SEED);
    vector<int> keys_to_delete;
    vector<int> keys_to_keep;

    for (size_t i = 0; i < keys.size(); ++i) {
        if (i % 2 == 0) {
            keys_to_delete.push_back(keys[i]);
        } else {
            keys_to_keep.push_back(keys[i]);
        }
    }

    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;

    for (int key : keys) {
        ht.put(key, key);
        map[key] = key;
    }

    for (int key : keys_to_delete) {
        ht.erase(key);
        map.erase(key);
    }

    return {keys_to_keep, ht, map};
}

double measure_get_after_deletions_hash_table(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    const HashTable<int, int>& ht = prepared.ht;

    Time::time_point start = Time::now();
    for (int key : keys) {
        auto value = ht.get(key);
        (void)value;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_get_after_deletions_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& keys = prepared.keys;
    const boost::unordered_flat_map<int, int>& map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : keys) {
        auto it = map.find(key);
        (void)it;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// ----------------------------
// 8. Insert after deletions
// ----------------------------

BasicPrepared prepare_case_insert_after_deletions(int n) {
    vector<int> initial_keys = make_sequential_keys(n);
    vector<int> keys_to_delete;
    vector<int> new_keys;

    for (size_t i = 0; i < initial_keys.size(); ++i) {
        if (i % 2 == 0) {
            keys_to_delete.push_back(initial_keys[i]);
        }
    }

    for (size_t i = 0; i < keys_to_delete.size(); ++i) {
        new_keys.push_back(n + static_cast<int>(i));
    }

    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;

    for (int key : initial_keys) {
        ht.put(key, key);
        map[key] = key;
    }

    for (int key : keys_to_delete) {
        ht.erase(key);
        map.erase(key);
    }

    return {new_keys, ht, map};
}

double measure_insert_after_deletions_hash_table(const BasicPrepared& prepared) {
    const vector<int>& new_keys = prepared.keys;
    HashTable<int, int> ht = prepared.ht.copy();

    Time::time_point start = Time::now();
    for (int key : new_keys) {
        ht.put(key, key);
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_insert_after_deletions_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& new_keys = prepared.keys;
    boost::unordered_flat_map<int, int> map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : new_keys) {
        map[key] = key;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// ----------------------------
// 9. Mixed workload
// ----------------------------

MixedPrepared prepare_case_mixed(int n, int seed) {
    mt19937 rng(seed);

    vector<int> initial_fill = make_random_unique_keys(n / 2, seed);
    vector<int> live_keys_list = initial_fill;

    vector<pair<string, int>> operations;
    int next_new_key = *max_element(initial_fill.begin(), initial_fill.end()) + 1;

    uniform_real_distribution<double> prob(0.0, 1.0);

    for (int _ = 0; _ < n; ++_) {
        double op_choice = prob(rng);

        if (op_choice < 0.5) {
            // GET
            int key;
            if (!live_keys_list.empty() && prob(rng) < 0.8) {
                uniform_int_distribution<int> idx_dist(0, static_cast<int>(live_keys_list.size()) - 1);
                key = live_keys_list[idx_dist(rng)];
            } else {
                uniform_int_distribution<int> miss_dist(1, n);
                key = next_new_key + miss_dist(rng);
            }
            operations.push_back({"get", key});
        }

        else if (op_choice < 0.8) {
            // PUT
            int key;
            if (!live_keys_list.empty() && prob(rng) < 0.3) {
                uniform_int_distribution<int> idx_dist(0, static_cast<int>(live_keys_list.size()) - 1);
                key = live_keys_list[idx_dist(rng)];
            } else {
                key = next_new_key;
                ++next_new_key;
                live_keys_list.push_back(key);
            }
            operations.push_back({"put", key});
        }

        else {
            // DELETE
            int key;
            if (!live_keys_list.empty()) {
                uniform_int_distribution<int> idx_dist(0, static_cast<int>(live_keys_list.size()) - 1);
                int idx = idx_dist(rng);

                key = live_keys_list[idx];
                live_keys_list[idx] = live_keys_list.back();
                live_keys_list.pop_back();
            } else {
                uniform_int_distribution<int> miss_dist(1, n);
                key = next_new_key + miss_dist(rng);
            }
            operations.push_back({"delete", key});
        }
    }

    HashTable<int, int> ht;
    boost::unordered_flat_map<int, int> map;

    for (int key : initial_fill) {
        ht.put(key, key);
        map[key] = key;
    }

    return {operations, ht, map};
}

ProbeStats measure_mixed_hash_table(const MixedPrepared& prepared) {
    const vector<pair<string, int>>& operations = prepared.operations;
    HashTable<int, int> ht = prepared.ht.copy();

    ht.reset_probe_stats();

    Time::time_point start = Time::now();

    for (const auto& [op, key] : operations) {
        if (op == "put") {
            ht.put(key, key);
        }
        else if (op == "get") {
            auto value = ht.get(key);
            (void)value;
        }
        else if (op == "delete") {
            try {
                ht.erase(key);
            } catch (const out_of_range&) {
            }
        }
    }

    Time::time_point end = Time::now();

    return {
        Seconds(end - start).count(),
        ht.find_slot_calls(),
        ht.find_slot_total_steps()
    };
}

double measure_mixed_unordered_flat_map(const MixedPrepared& prepared) {
    const vector<pair<string, int>>& operations = prepared.operations;
    boost::unordered_flat_map<int, int> map = prepared.map;

    Time::time_point start = Time::now();

    for (const auto& [op, key] : operations) {
        if (op == "put") {
            map[key] = key;
        }
        else if (op == "get") {
            auto it = map.find(key);
            (void)it;
        }
        else if (op == "delete") {
            map.erase(key);
        }
    }

    Time::time_point end = Time::now();
    return Seconds(end - start).count();
}

// ----------------------------
// Универсальные запускатели
// ----------------------------

void benchmark_case(const string& scenario_name, const vector<int>& sizes, BasicPrepared (*prepare_case)(int), double (*measure_ht)(const BasicPrepared&), double (*measure_map)(const BasicPrepared&), int repeats = REPEATS) {
    vector<tuple<int, double, double>> results;

    for (int n : sizes) {
        BasicPrepared prepared = prepare_case(n);

        vector<double> ht_runs;
        vector<double> map_runs;

        for (int i = 0; i < repeats; ++i) {
            ht_runs.push_back(measure_ht(prepared));
            map_runs.push_back(measure_map(prepared));
        }

        double ht_med = median(ht_runs);
        double map_med = median(map_runs);

        results.push_back({n, ht_med, map_med});
    }

    print_result_table(scenario_name, results);
}

void benchmark_case_mixed(const vector<int>& sizes, int repeats = REPEATS) {
    vector<tuple<int, double, double>> results;

    for (int n : sizes) {
        MixedPrepared prepared = prepare_case_mixed(n, RANDOM_SEED);

        vector<double> ht_runs;
        vector<double> map_runs;
        vector<size_t> calls_runs;
        vector<size_t> steps_runs;

        for (int i = 0; i < repeats; ++i) {
            ProbeStats stats = measure_mixed_hash_table(prepared);
            ht_runs.push_back(stats.time);
            calls_runs.push_back(stats.calls);
            steps_runs.push_back(stats.steps);

            map_runs.push_back(measure_mixed_unordered_flat_map(prepared));
        }

        double ht_med = median(ht_runs);
        double map_med = median(map_runs);
        results.push_back({n, ht_med, map_med});

        size_t total_calls = 0;
        size_t total_steps = 0;

        for (size_t x : calls_runs) total_calls += x;
        for (size_t x : steps_runs) total_steps += x;

        double avg_probe_length = total_calls > 0 ? static_cast<double>(total_steps) / total_calls : 0.0;

        cout << "\nProbe stats for N = " << n << "\n";
        cout << "avg_probe_length: " << avg_probe_length << "\n";
        cout << "find_slot_calls: " << static_cast<double>(total_calls) / repeats << "\n";
        cout << "total_steps: " << static_cast<double>(total_steps) / repeats << "\n";
    }

    print_result_table("9. Mixed workload", results);
}

// -----------------------------
// Запуск всех benchmark-ов
// -----------------------------

void run_all() {
    print_section("HashTable vs unordered_flat_map benchmarks");

    benchmark_case("1. Insert sequential", SIZES, prepare_case_insert_sequential, measure_insert_sequential_hash_table, measure_insert_sequential_unordered_flat_map);

    benchmark_case("2. Insert random", SIZES, prepare_case_insert_random, measure_insert_random_hash_table, measure_insert_random_unordered_flat_map);

    benchmark_case("3. Get existing sequential", SIZES, prepare_case_get_existing_sequential, measure_get_existing_sequential_hash_table, measure_get_existing_sequential_unordered_flat_map);

    benchmark_case("4. Get existing random order", SIZES, prepare_case_get_existing_random, measure_get_existing_random_hash_table, measure_get_existing_random_unordered_flat_map);

    benchmark_case("5. Get missing keys", SIZES, prepare_case_get_missing, measure_get_missing_hash_table, measure_get_missing_unordered_flat_map);

    benchmark_case("6. Delete", SIZES, prepare_case_delete, measure_delete_hash_table, measure_delete_unordered_flat_map);

    benchmark_case("7. Get after deletions", SIZES, prepare_case_get_after_deletions, measure_get_after_deletions_hash_table, measure_get_after_deletions_unordered_flat_map);

    benchmark_case("8. Insert after deletions", SIZES, prepare_case_insert_after_deletions, measure_insert_after_deletions_hash_table, measure_insert_after_deletions_unordered_flat_map);

    benchmark_case_mixed(SIZES);
}

int main() {
    run_all();
    return 0;
}