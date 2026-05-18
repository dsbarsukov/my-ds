#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <unordered_set>

using ImplementationName = std::string;

// -----------------------------
// Data generation
// -----------------------------

std::vector<int> make_sequential_values(int n) {
    std::vector<int> values;
    values.reserve(n);

    for (int i = 0; i < n; ++i) {
        values.push_back(i);
    }

    return values;
}

std::vector<int> make_reverse_sequential_values(int n) {
    std::vector<int> values;
    values.reserve(n);

    for (int i = n - 1; i >= 0; --i) {
        values.push_back(i);
    }

    return values;
}

std::vector<int> make_random_unique_values(int n, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, n * 20 - 1);

    std::unordered_set<int> used;
    used.reserve(n * 2);

    std::vector<int> values;
    values.reserve(n);

    while (static_cast<int>(values.size()) < n) {
        int x = dist(rng);

        if (used.insert(x).second) {
            values.push_back(x);
        }
    }

    return values;
}


std::vector<int> make_shuffled(const std::vector<int>& values, int seed) {
    std::mt19937 rng(seed);

    std::vector<int> data = values;
    std::shuffle(data.begin(), data.end(), rng);

    return data;
}

// -----------------------------
// CSV
// -----------------------------

void write_csv_header(std::ostream& out) {
    out << "language,structure,case,n,implementation,run,time_sec\n";
}

void write_csv_row(std::ostream& out, const std::string& language, const std::string& structure, const std::string& case_name, int n, const std::string& implementation, int run_number, double time_sec) {
    out << language << ',' << structure << ',' << case_name << ',' << n << ',' << implementation << ',' << run_number << ',';

    out.setf(std::ios::fixed);
    out.precision(9);
    out << time_sec << '\n';
}

// -----------------------------
// Progress
// -----------------------------

void print_progress(const std::string& message = "") {
    std::cerr << message << '\n';
}

// -----------------------------
// Benchmark runner
// -----------------------------

template <typename Prepared>
using MeasureFunction = std::function<double(const Prepared&)>;

template <typename Prepared>
using Implementation = std::pair<ImplementationName, MeasureFunction<Prepared>>;

template <typename Prepared, typename PrepareFunction>
void benchmark_case(std::ostream& out, const std::string& structure, const std::string& case_name, const std::vector<int>& sizes, PrepareFunction prepare_case, const std::vector<Implementation<Prepared>>& implementations, int repeats, const std::string& language = "cpp") {
    print_progress("Started: " + case_name);

    for (int n : sizes) {
        Prepared prepared = prepare_case(n);

        for (const auto& [implementation_name, measure] : implementations) {
            for (int run_number = 1; run_number <= repeats; ++run_number) {
                double time_sec = measure(prepared);

                write_csv_row(out, language, structure, case_name, n, implementation_name, run_number, time_sec);
            }
        }

        print_progress("  finished n = " + std::to_string(n));
    }

    print_progress("Finished: " + case_name);
    print_progress();
}
