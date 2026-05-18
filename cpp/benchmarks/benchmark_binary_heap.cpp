#include <chrono>
#include <functional>
#include <queue>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "common.hpp"
#include "binary_heap.h"

using namespace std;

using Time = chrono::high_resolution_clock;
using Seconds = chrono::duration<double>;
using MinPriorityQueue = priority_queue<int, vector<int>, greater<int>>;

// This variable prevents the optimizer from removing benchmarked work
// when the result of operations is not otherwise used.
static volatile size_t benchmark_sink = 0;

// -----------------------------
// Общие настройки
// -----------------------------

const vector<int> SIZES = {10'000, 50'000, 100'000, 200'000, 500'000, 1'000'000, 2'000'000, 5'000'000, 10'000'000, 15'000'000};
const int REPEATS = 5;
const int RANDOM_SEED = 42;
const int TOP_K = 100;

// -----------------------------
// Вспомогательные структуры
// -----------------------------

struct BasicPrepared {
    vector<int> values;
    BinaryHeap<int> heap;
    MinPriorityQueue std_heap;
};

struct HeapifyPrepared {
    vector<int> values;
};

struct PopAllPrepared {
    BinaryHeap<int> heap;
    MinPriorityQueue std_heap;
};

struct AlternatingPrepared {
    vector<pair<string, int>> operations;
    BinaryHeap<int> heap;
    MinPriorityQueue std_heap;
};

struct TopKPrepared {
    vector<int> values;
    int k;
};

// -----------------------------
// 1. Push sequential
// -----------------------------

BasicPrepared prepare_case_push_sequential(int n) {
    return {make_sequential_values(n), BinaryHeap<int>(), MinPriorityQueue()};
}

double measure_push_sequential_binary_heap(const BasicPrepared& prepared) {
    const vector<int>& values = prepared.values;
    BinaryHeap<int> heap = prepared.heap.copy();

    Time::time_point start = Time::now();
    for (int value : values) {
        heap.push(value);
    }
    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

double measure_push_sequential_std_priority_queue(const BasicPrepared& prepared) {
    const vector<int>& values = prepared.values;
    MinPriorityQueue heap = prepared.std_heap;

    Time::time_point start = Time::now();
    for (int value : values) {
        heap.push(value);
    }
    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

// -----------------------------
// 2. Push reverse sequential
// -----------------------------

BasicPrepared prepare_case_push_reverse_sequential(int n) {
    return {make_reverse_sequential_values(n), BinaryHeap<int>(), MinPriorityQueue()};
}

double measure_push_reverse_sequential_binary_heap(const BasicPrepared& prepared) {
    const vector<int>& values = prepared.values;
    BinaryHeap<int> heap = prepared.heap.copy();

    Time::time_point start = Time::now();
    for (int value : values) {
        heap.push(value);
    }
    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

double measure_push_reverse_sequential_std_priority_queue(const BasicPrepared& prepared) {
    const vector<int>& values = prepared.values;
    MinPriorityQueue heap = prepared.std_heap;

    Time::time_point start = Time::now();
    for (int value : values) {
        heap.push(value);
    }
    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

// -----------------------------
// 3. Push random
// -----------------------------

BasicPrepared prepare_case_push_random(int n) {
    return {make_random_unique_values(n, RANDOM_SEED), BinaryHeap<int>(), MinPriorityQueue()};
}

double measure_push_random_binary_heap(const BasicPrepared& prepared) {
    const vector<int>& values = prepared.values;
    BinaryHeap<int> heap = prepared.heap.copy();

    Time::time_point start = Time::now();
    for (int value : values) {
        heap.push(value);
    }
    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

double measure_push_random_std_priority_queue(const BasicPrepared& prepared) {
    const vector<int>& values = prepared.values;
    MinPriorityQueue heap = prepared.std_heap;

    Time::time_point start = Time::now();
    for (int value : values) {
        heap.push(value);
    }
    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

// -----------------------------
// 4. Heapify random
// -----------------------------

HeapifyPrepared prepare_case_heapify_random(int n) {
    return {make_random_unique_values(n, RANDOM_SEED)};
}

double measure_heapify_random_binary_heap(const HeapifyPrepared& prepared) {
    const vector<int>& values = prepared.values;

    Time::time_point start = Time::now();
    BinaryHeap<int> heap(values);
    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

double measure_heapify_random_std_priority_queue(const HeapifyPrepared& prepared) {
    vector<int> data = prepared.values;

    Time::time_point start = Time::now();
    MinPriorityQueue heap(greater<int>(), std::move(data));
    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

// -----------------------------
// 5. Pop all after heapify
// -----------------------------

PopAllPrepared prepare_case_pop_all_after_heapify(int n) {
    vector<int> values = make_random_unique_values(n, RANDOM_SEED);

    BinaryHeap<int> base_heap(values);

    vector<int> data = values;
    MinPriorityQueue base_std_heap(greater<int>(), std::move(data));

    return {base_heap, base_std_heap};
}

double measure_pop_all_after_heapify_binary_heap(const PopAllPrepared& prepared) {
    BinaryHeap<int> heap = prepared.heap.copy();
    size_t removed_count = 0;

    Time::time_point start = Time::now();
    while (!heap.empty()) {
        heap.pop();
        ++removed_count;
    }
    Time::time_point end = Time::now();

    benchmark_sink += removed_count;
    return Seconds(end - start).count();
}

double measure_pop_all_after_heapify_std_priority_queue(const PopAllPrepared& prepared) {
    MinPriorityQueue heap = prepared.std_heap;
    size_t removed_count = 0;

    Time::time_point start = Time::now();
    while (!heap.empty()) {
        heap.pop();
        ++removed_count;
    }
    Time::time_point end = Time::now();

    benchmark_sink += removed_count;
    return Seconds(end - start).count();
}

// -----------------------------
// 6. Alternating push/pop
// -----------------------------

AlternatingPrepared prepare_case_alternating_push_pop(int n) {
    mt19937 rng(RANDOM_SEED);
    uniform_real_distribution<double> probability(0.0, 1.0);

    vector<int> initial_values = make_random_unique_values(n / 2, RANDOM_SEED);
    vector<int> operation_values = make_random_unique_values(n, RANDOM_SEED + 1);

    vector<pair<string, int>> operations;
    operations.reserve(n);

    int current_size = static_cast<int>(initial_values.size());

    for (int value : operation_values) {
        if (current_size == 0) {
            operations.emplace_back("push", value);
            ++current_size;
            continue;
        }

        if (probability(rng) < 0.5) {
            operations.emplace_back("push", value);
            ++current_size;
        } else {
            operations.emplace_back("pop", value);
            --current_size;
        }
    }

    BinaryHeap<int> base_heap(initial_values);

    vector<int> data = initial_values;
    MinPriorityQueue base_std_heap(greater<int>(), std::move(data));

    return {operations, base_heap, base_std_heap};
}

double measure_alternating_push_pop_binary_heap(const AlternatingPrepared& prepared) {
    const vector<pair<string, int>>& operations = prepared.operations;
    BinaryHeap<int> heap = prepared.heap.copy();
    size_t operation_count = 0;

    Time::time_point start = Time::now();

    for (const auto& [operation, value] : operations) {
        if (operation == "push") {
            heap.push(value);
        } else {
            heap.pop();
        }
        ++operation_count;
    }

    Time::time_point end = Time::now();

    benchmark_sink += operation_count + heap.size();
    return Seconds(end - start).count();
}

double measure_alternating_push_pop_std_priority_queue(const AlternatingPrepared& prepared) {
    const vector<pair<string, int>>& operations = prepared.operations;
    MinPriorityQueue heap = prepared.std_heap;
    size_t operation_count = 0;

    Time::time_point start = Time::now();

    for (const auto& [operation, value] : operations) {
        if (operation == "push") {
            heap.push(value);
        } else {
            heap.pop();
        }
        ++operation_count;
    }

    Time::time_point end = Time::now();

    benchmark_sink += operation_count + heap.size();
    return Seconds(end - start).count();
}

// -----------------------------
// 7. Top-k largest
// -----------------------------

TopKPrepared prepare_case_top_k_largest(int n) {
    int k = min(TOP_K, n);
    return {make_random_unique_values(n, RANDOM_SEED), k};
}

double measure_top_k_largest_binary_heap(const TopKPrepared& prepared) {
    const vector<int>& values = prepared.values;
    int k = prepared.k;
    BinaryHeap<int> heap;

    Time::time_point start = Time::now();

    for (int value : values) {
        if (static_cast<int>(heap.size()) < k) {
            heap.push(value);
        } else if (value > heap.peek()) {
            heap.pop();
            heap.push(value);
        }
    }

    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

double measure_top_k_largest_std_priority_queue(const TopKPrepared& prepared) {
    const vector<int>& values = prepared.values;
    int k = prepared.k;
    MinPriorityQueue heap;

    Time::time_point start = Time::now();

    for (int value : values) {
        if (static_cast<int>(heap.size()) < k) {
            heap.push(value);
        } else if (value > heap.top()) {
            heap.pop();
            heap.push(value);
        }
    }

    Time::time_point end = Time::now();

    benchmark_sink += heap.size();
    return Seconds(end - start).count();
}

// -----------------------------
// Запуск всех benchmark-ов
// -----------------------------

void run_all() {
    write_csv_header(cout);

    benchmark_case<BasicPrepared>(
        cout,
        "binary_heap",
        "1. Push sequential",
        SIZES,
        prepare_case_push_sequential,
        {
            {"BinaryHeap", measure_push_sequential_binary_heap},
            {"priority_queue", measure_push_sequential_std_priority_queue}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "binary_heap",
        "2. Push reverse sequential",
        SIZES,
        prepare_case_push_reverse_sequential,
        {
            {"BinaryHeap", measure_push_reverse_sequential_binary_heap},
            {"priority_queue", measure_push_reverse_sequential_std_priority_queue}
        },
        REPEATS
    );

    benchmark_case<BasicPrepared>(
        cout,
        "binary_heap",
        "3. Push random",
        SIZES,
        prepare_case_push_random,
        {
            {"BinaryHeap", measure_push_random_binary_heap},
            {"priority_queue", measure_push_random_std_priority_queue}
        },
        REPEATS
    );

    benchmark_case<HeapifyPrepared>(
        cout,
        "binary_heap",
        "4. Heapify random",
        SIZES,
        prepare_case_heapify_random,
        {
            {"BinaryHeap", measure_heapify_random_binary_heap},
            {"priority_queue", measure_heapify_random_std_priority_queue}
        },
        REPEATS
    );

    benchmark_case<PopAllPrepared>(
        cout,
        "binary_heap",
        "5. Pop all after heapify",
        SIZES,
        prepare_case_pop_all_after_heapify,
        {
            {"BinaryHeap", measure_pop_all_after_heapify_binary_heap},
            {"priority_queue", measure_pop_all_after_heapify_std_priority_queue}
        },
        REPEATS
    );

    benchmark_case<AlternatingPrepared>(
        cout,
        "binary_heap",
        "6. Alternating push/pop",
        SIZES,
        prepare_case_alternating_push_pop,
        {
            {"BinaryHeap", measure_alternating_push_pop_binary_heap},
            {"priority_queue", measure_alternating_push_pop_std_priority_queue}
        },
        REPEATS
    );

    benchmark_case<TopKPrepared>(
        cout,
        "binary_heap",
        "7. Top-k largest",
        SIZES,
        prepare_case_top_k_largest,
        {
            {"BinaryHeap", measure_top_k_largest_binary_heap},
            {"priority_queue", measure_top_k_largest_std_priority_queue}
        },
        REPEATS
    );
}

int main() {
    run_all();
    return 0;
}