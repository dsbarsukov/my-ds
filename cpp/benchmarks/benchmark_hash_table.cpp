#include <algorithm>
#include <chrono>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>

#include "common.hpp"
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

// ----------------------------
// 1. Insert sequential
// ----------------------------

BasicPrepared prepare_case_insert_sequential(int n) {
    return {make_sequential_values(n), HashTable<int, int>(), {}};
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
    return {make_random_unique_values(n, RANDOM_SEED), HashTable<int, int>(), {}};
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
    vector<int> keys = make_sequential_values(n);
    HashTable<int, int> base_ht;
    boost::unordered_flat_map<int, int> base_map;

    for (int key : keys) {
        base_ht.put(key, key);
        base_map[key] = key;
    }

    return {keys, base_ht, base_map};
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
    vector<int> insert_keys = make_random_unique_values(n, RANDOM_SEED);
    vector<int> query_keys = make_shuffled(insert_keys, RANDOM_SEED + 1);

    HashTable<int, int> base_ht;
    boost::unordered_flat_map<int, int> base_map;

    for (int key : insert_keys) {
        base_ht.put(key, key);
        base_map[key] = key;
    }

    return {query_keys, base_ht, base_map};
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
    vector<int> existing_keys = make_random_unique_values(n, RANDOM_SEED);

    mt19937 rng(RANDOM_SEED + 1);
    vector<int> missing_keys;
    missing_keys.reserve(n * 20);

    for (int i = n * 20; i < n * 40; ++i) {
        missing_keys.push_back(i);
    }

    shuffle(missing_keys.begin(), missing_keys.end(), rng);
    missing_keys.resize(n);
    missing_keys = make_shuffled(missing_keys, RANDOM_SEED + 2);

    HashTable<int, int> base_ht;
    boost::unordered_flat_map<int, int> base_map;

    for (int key : existing_keys) {
        base_ht.put(key, key);
        base_map[key] = key;
    }

    return {missing_keys, base_ht, base_map};
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
    vector<int> keys = make_sequential_values(n);
    HashTable<int, int> base_ht;
    boost::unordered_flat_map<int, int> base_map;

    for (int key : keys) {
        base_ht.put(key, key);
        base_map[key] = key;
    }

    return {keys, base_ht, base_map};
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
    vector<int> keys = make_random_unique_values(n, RANDOM_SEED);
    vector<int> keys_to_delete;
    vector<int> keys_to_keep;

    keys_to_delete.reserve((keys.size() + 1) / 2);
    keys_to_keep.reserve(keys.size() / 2);

    for (size_t i = 0; i < keys.size(); ++i) {
        if (i % 2 == 0) {
            keys_to_delete.push_back(keys[i]);
        } else {
            keys_to_keep.push_back(keys[i]);
        }
    }

    HashTable<int, int> base_ht;
    boost::unordered_flat_map<int, int> base_map;

    for (int key : keys) {
        base_ht.put(key, key);
        base_map[key] = key;
    }

    for (int key : keys_to_delete) {
        base_ht.erase(key);
        base_map.erase(key);
    }

    return {keys_to_keep, base_ht, base_map};
}

double measure_get_after_deletions_hash_table(const BasicPrepared& prepared) {
    const vector<int>& keys_to_keep = prepared.keys;
    const HashTable<int, int>& ht = prepared.ht;

    Time::time_point start = Time::now();
    for (int key : keys_to_keep) {
        auto value = ht.get(key);
        (void)value;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_get_after_deletions_unordered_flat_map(const BasicPrepared& prepared) {
    const vector<int>& keys_to_keep = prepared.keys;
    const boost::unordered_flat_map<int, int>& map = prepared.map;

    Time::time_point start = Time::now();
    for (int key : keys_to_keep) {
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
    vector<int> initial_keys = make_sequential_values(n);
    vector<int> keys_to_delete;
    vector<int> new_keys;

    keys_to_delete.reserve((initial_keys.size() + 1) / 2);

    for (size_t i = 0; i < initial_keys.size(); ++i) {
        if (i % 2 == 0) {
            keys_to_delete.push_back(initial_keys[i]);
        }
    }

    new_keys.reserve(keys_to_delete.size());
    for (size_t i = 0; i < keys_to_delete.size(); ++i) {
        new_keys.push_back(n + static_cast<int>(i));
    }

    HashTable<int, int> base_ht;
    boost::unordered_flat_map<int, int> base_map;

    for (int key : initial_keys) {
        base_ht.put(key, key);
        base_map[key] = key;
    }

    for (int key : keys_to_delete) {
        base_ht.erase(key);
        base_map.erase(key);
    }

    return {new_keys, base_ht, base_map};
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

    vector<int> initial_fill = make_random_unique_values(n / 2, seed);
    vector<int> live_keys_list = initial_fill;

    vector<pair<string, int>> operations;
    operations.reserve(n);

    int next_new_key = *max_element(initial_fill.begin(), initial_fill.end()) + 1;
    uniform_real_distribution<double> probability(0.0, 1.0);

    for (int _ = 0; _ < n; ++_) {
        double op_choice = probability(rng);

        if (op_choice < 0.5) {
            // GET
            int key;
            if (!live_keys_list.empty() && probability(rng) < 0.8) {
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
            if (!live_keys_list.empty() && probability(rng) < 0.3) {
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

    HashTable<int, int> base_ht;
    boost::unordered_flat_map<int, int> base_map;

    for (int key : initial_fill) {
        base_ht.put(key, key);
        base_map[key] = key;
    }

    return {operations, base_ht, base_map};
}

double measure_mixed_hash_table(const MixedPrepared& prepared) {
    const vector<pair<string, int>>& operations = prepared.operations;
    HashTable<int, int> ht = prepared.ht.copy();

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
    return Seconds(end - start).count();
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

// -----------------------------
// Запуск всех benchmark-ов
// -----------------------------

void run_all() {
    write_csv_header(cout);

    benchmark_case<BasicPrepared>(
        cout,
        "hash_table",
        "1. Insert sequential",
        SIZES,
        prepare_case_insert_sequential,
        {
            {"HashTable", measure_insert_sequential_hash_table},
            {"unordered_flat_map", measure_insert_sequential_unordered_flat_map}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "hash_table",
        "2. Insert random",
        SIZES,
        prepare_case_insert_random,
        {
            {"HashTable", measure_insert_random_hash_table},
            {"unordered_flat_map", measure_insert_random_unordered_flat_map}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "hash_table",
        "3. Get existing sequential",
        SIZES,
        prepare_case_get_existing_sequential,
        {
            {"HashTable", measure_get_existing_sequential_hash_table},
            {"unordered_flat_map", measure_get_existing_sequential_unordered_flat_map}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "hash_table",
        "4. Get existing random order",
        SIZES,
        prepare_case_get_existing_random,
        {
            {"HashTable", measure_get_existing_random_hash_table},
            {"unordered_flat_map", measure_get_existing_random_unordered_flat_map}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "hash_table",
        "5. Get missing keys",
        SIZES,
        prepare_case_get_missing,
        {
            {"HashTable", measure_get_missing_hash_table},
            {"unordered_flat_map", measure_get_missing_unordered_flat_map}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "hash_table",
        "6. Delete",
        SIZES,
        prepare_case_delete,
        {
            {"HashTable", measure_delete_hash_table},
            {"unordered_flat_map", measure_delete_unordered_flat_map}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "hash_table",
        "7. Get after deletions",
        SIZES,
        prepare_case_get_after_deletions,
        {
            {"HashTable", measure_get_after_deletions_hash_table},
            {"unordered_flat_map", measure_get_after_deletions_unordered_flat_map}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "hash_table",
        "8. Insert after deletions",
        SIZES,
        prepare_case_insert_after_deletions,
        {
            {"HashTable", measure_insert_after_deletions_hash_table},
            {"unordered_flat_map", measure_insert_after_deletions_unordered_flat_map}
        },
        REPEATS
    );

    benchmark_case<MixedPrepared>(
        cout,
        "hash_table",
        "9. Mixed workload",
        SIZES,
        [](int n) { return prepare_case_mixed(n, RANDOM_SEED); },
        {
            {"HashTable", measure_mixed_hash_table},
            {"unordered_flat_map", measure_mixed_unordered_flat_map}
        },
        REPEATS
    );
}

int main() {
    run_all();
    return 0;
}