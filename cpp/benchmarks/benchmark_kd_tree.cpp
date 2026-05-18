#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <nanoflann.hpp>

#include "common.hpp"
#include "kd_tree.h"

using namespace std;

using Time = chrono::high_resolution_clock;
using Seconds = chrono::duration<double>;

using Point = vector<double>;

// -----------------------------
// Общие настройки
// -----------------------------

const vector<int> SIZES = {1'000, 5'000, 10'000, 50'000, 100'000, 200'000, 500'000, 1'000'000, 2'000'000, 5'000'000};
const vector<int> BAD_ORDER_SIZES = {100, 500, 1'000, 2'000, 5'000, 10'000, 20'000, 40'000};
const int REPEATS = 5;
const int RANDOM_SEED = 42;

const int DIMENSIONS_2D = 2;
const int DIMENSIONS_HIGH = 10;

const int QUERY_COUNT = 100;
const int RANGE_QUERY_COUNT = 100;

static volatile size_t benchmark_sink = 0;

// -----------------------------
// Data generation
// -----------------------------

vector<Point> make_random_points(int n, int dimensions, int seed) {
    mt19937 rng(seed);
    uniform_real_distribution<double> dist(0.0, 1.0);

    vector<Point> points;
    points.reserve(n);

    for (int i = 0; i < n; ++i) {
        Point point;
        point.reserve(dimensions);

        for (int j = 0; j < dimensions; ++j) {
            point.push_back(dist(rng));
        }

        points.push_back(std::move(point));
    }

    return points;
}

vector<Point> make_sequential_points(int n, int dimensions) {
    vector<Point> points;
    points.reserve(n);

    for (int value = 0; value < n; ++value) {
        points.push_back(Point(static_cast<size_t>(dimensions), static_cast<double>(value)));
    }

    return points;
}

vector<pair<Point, Point>> make_random_range_queries(int query_count, int dimensions, int seed) {
    mt19937 rng(seed);
    uniform_real_distribution<double> dist(0.0, 1.0);

    vector<pair<Point, Point>> queries;
    queries.reserve(query_count);

    for (int i = 0; i < query_count; ++i) {
        Point first_point;
        Point second_point;
        Point lower_bound;
        Point upper_bound;

        first_point.reserve(dimensions);
        second_point.reserve(dimensions);
        lower_bound.reserve(dimensions);
        upper_bound.reserve(dimensions);

        for (int j = 0; j < dimensions; ++j) {
            first_point.push_back(dist(rng));
            second_point.push_back(dist(rng));
        }

        for (int j = 0; j < dimensions; ++j) {
            lower_bound.push_back(min(first_point[j], second_point[j]));
            upper_bound.push_back(max(first_point[j], second_point[j]));
        }

        queries.push_back({std::move(lower_bound), std::move(upper_bound)});
    }

    return queries;
}

// -----------------------------
// nanoflann wrapper
// -----------------------------

struct NanoflannPointCloud {
    const vector<Point>& points;

    explicit NanoflannPointCloud(const vector<Point>& points_value) : points(points_value) {}

    inline size_t kdtree_get_point_count() const {
        return points.size();
    }

    inline double kdtree_get_pt(size_t index, size_t dimension) const {
        return points[index][dimension];
    }

    template <class BBox>
    bool kdtree_get_bbox(BBox&) const {
        return false;
    }
};

class NanoflannKDTree {
private:
    using Adaptor = NanoflannPointCloud;
    using Distance = nanoflann::L2_Simple_Adaptor<double, Adaptor>;
    using Index = nanoflann::KDTreeSingleIndexAdaptor<Distance, Adaptor, -1, size_t>;

    vector<Point> points_;
    Adaptor adaptor_;
    unique_ptr<Index> index_;
    int dimensions_;

public:
    NanoflannKDTree(const vector<Point>& points, int dimensions) : points_(points), adaptor_(points_), dimensions_(dimensions) {
        index_ = make_unique<Index>(dimensions_, adaptor_, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        index_->buildIndex();
    }

    pair<size_t, double> nearest_neighbor(const Point& query) const {
        size_t result_index = 0;
        double squared_distance_result = 0.0;

        index_->knnSearch(query.data(), 1, &result_index, &squared_distance_result);

        return {result_index, sqrt(squared_distance_result)};
    }
};

// -----------------------------
// Prepared data structs
// -----------------------------

struct NearestPrepared {
    vector<Point> points;
    vector<Point> queries;
    shared_ptr<KDTree> kd_tree;
    shared_ptr<NanoflannKDTree> nanoflann_tree;
};

struct RangePrepared {
    vector<Point> points;
    vector<pair<Point, Point>> queries;
    shared_ptr<KDTree> kd_tree;
};

// -----------------------------
// 1. Build random 2D
// -----------------------------

vector<Point> prepare_case_build_random_2d(int n) {
    return make_random_points(n, DIMENSIONS_2D, RANDOM_SEED);
}

double measure_build_random_2d_kd_tree(const vector<Point>& points) {
    vector<Point> data = points;

    Time::time_point start = Time::now();
    KDTree tree(data);
    Time::time_point end = Time::now();

    benchmark_sink += tree.size();
    return Seconds(end - start).count();
}

double measure_build_random_2d_nanoflann(const vector<Point>& points) {
    vector<Point> data = points;

    Time::time_point start = Time::now();
    NanoflannKDTree tree(data, DIMENSIONS_2D);
    Time::time_point end = Time::now();

    benchmark_sink += data.size();
    return Seconds(end - start).count();
}

// -----------------------------
// 2. Insert random 2D
// -----------------------------

vector<Point> prepare_case_insert_random_2d(int n) {
    return make_random_points(n, DIMENSIONS_2D, RANDOM_SEED);
}

double measure_insert_random_2d_kd_tree(const vector<Point>& points) {
    KDTree tree;

    Time::time_point start = Time::now();
    for (const Point& point : points) {
        tree.insert(point);
    }
    Time::time_point end = Time::now();

    benchmark_sink += tree.size();
    return Seconds(end - start).count();
}

double measure_insert_random_2d_kd_tree_auto_rebuild(const vector<Point>& points) {
    KDTree tree(true, 2.0);

    Time::time_point start = Time::now();
    for (const Point& point : points) {
        tree.insert(point);
    }
    Time::time_point end = Time::now();

    benchmark_sink += tree.size();
    return Seconds(end - start).count();
}

// -----------------------------
// 3. Insert sequential bad order 2D small
// -----------------------------

vector<Point> prepare_case_insert_sequential_bad_order_2d_small(int n) {
    return make_sequential_points(n, DIMENSIONS_2D);
}

double measure_insert_sequential_bad_order_2d_small_kd_tree(const vector<Point>& points) {
    KDTree tree;

    Time::time_point start = Time::now();
    for (const Point& point : points) {
        tree.insert(point);
    }
    Time::time_point end = Time::now();

    benchmark_sink += tree.size();
    return Seconds(end - start).count();
}

double measure_insert_sequential_bad_order_2d_small_kd_tree_auto_rebuild(const vector<Point>& points) {
    KDTree tree(true, 2.0);

    Time::time_point start = Time::now();
    for (const Point& point : points) {
        tree.insert(point);
    }
    Time::time_point end = Time::now();

    benchmark_sink += tree.size();
    return Seconds(end - start).count();
}

// -----------------------------
// 4. Nearest neighbor random queries 2D
// -----------------------------

NearestPrepared prepare_case_nearest_neighbor_random_queries_2d(int n) {
    vector<Point> points = make_random_points(n, DIMENSIONS_2D, RANDOM_SEED);
    vector<Point> queries = make_random_points(QUERY_COUNT, DIMENSIONS_2D, RANDOM_SEED + 1);

    NearestPrepared prepared;
    prepared.points = points;
    prepared.queries = queries;
    prepared.kd_tree = make_shared<KDTree>(points);
    prepared.nanoflann_tree = make_shared<NanoflannKDTree>(points, DIMENSIONS_2D);

    return prepared;
}

double measure_nearest_neighbor_random_queries_2d_kd_tree(const NearestPrepared& prepared) {
    Time::time_point start = Time::now();
    for (const Point& query : prepared.queries) {
        auto [point, distance] = prepared.kd_tree->nearest_neighbor(query);
        benchmark_sink += static_cast<int>(distance);

    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_nearest_neighbor_random_queries_2d_brute_force(const NearestPrepared& prepared) {
    Time::time_point start = Time::now();
    for (const Point& query : prepared.queries) {
        auto [point, distance] = prepared.kd_tree->brute_force_nearest(prepared.points, query);
        benchmark_sink += static_cast<int>(distance);
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_nearest_neighbor_random_queries_2d_nanoflann(const NearestPrepared& prepared) {
    Time::time_point start = Time::now();
    for (const Point& query : prepared.queries) {
        auto [index, distance] = prepared.nanoflann_tree->nearest_neighbor(query);
        benchmark_sink += index;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// -----------------------------
// 5. Range search random boxes 2D
// -----------------------------

RangePrepared prepare_case_range_search_random_boxes_2d(int n) {
    vector<Point> points = make_random_points(n, DIMENSIONS_2D, RANDOM_SEED);
    vector<pair<Point, Point>> queries = make_random_range_queries(RANGE_QUERY_COUNT, DIMENSIONS_2D, RANDOM_SEED + 2);

    RangePrepared prepared;
    prepared.points = points;
    prepared.queries = queries;
    prepared.kd_tree = make_shared<KDTree>(points);

    return prepared;
}

double measure_range_search_random_boxes_2d_kd_tree(const RangePrepared& prepared) {
    Time::time_point start = Time::now();
    for (const auto& [lower_bound, upper_bound] : prepared.queries) {
        vector<Point> result = prepared.kd_tree->range_search(lower_bound, upper_bound);
        benchmark_sink += result.size();
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_range_search_random_boxes_2d_brute_force(const RangePrepared& prepared) {
    Time::time_point start = Time::now();
    for (const auto& [lower_bound, upper_bound] : prepared.queries) {
        vector<Point> result = prepared.kd_tree->brute_force_range_search(prepared.points, lower_bound, upper_bound);
        benchmark_sink += result.size();
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// -----------------------------
// 6. Nearest neighbor random queries 10D
// -----------------------------

NearestPrepared prepare_case_nearest_neighbor_random_queries_10d(int n) {
    vector<Point> points = make_random_points(n, DIMENSIONS_HIGH, RANDOM_SEED);
    vector<Point> queries = make_random_points(QUERY_COUNT, DIMENSIONS_HIGH, RANDOM_SEED + 3);

    NearestPrepared prepared;
    prepared.points = points;
    prepared.queries = queries;
    prepared.kd_tree = make_shared<KDTree>(points);
    prepared.nanoflann_tree = make_shared<NanoflannKDTree>(points, DIMENSIONS_HIGH);

    return prepared;
}

double measure_nearest_neighbor_random_queries_10d_kd_tree(const NearestPrepared& prepared) {
    Time::time_point start = Time::now();
    for (const Point& query : prepared.queries) {
        auto [point, distance] = prepared.kd_tree->nearest_neighbor(query);
        benchmark_sink += static_cast<int>(distance);
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_nearest_neighbor_random_queries_10d_brute_force(const NearestPrepared& prepared) {
    Time::time_point start = Time::now();
    for (const Point& query : prepared.queries) {
        auto [point, distance] = prepared.kd_tree->brute_force_nearest(prepared.points, query);
        benchmark_sink += static_cast<int>(distance);
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

double measure_nearest_neighbor_random_queries_10d_nanoflann(const NearestPrepared& prepared) {
    Time::time_point start = Time::now();
    for (const Point& query : prepared.queries) {
        auto [index, distance] = prepared.nanoflann_tree->nearest_neighbor(query);
        benchmark_sink += index;
    }
    Time::time_point end = Time::now();

    return Seconds(end - start).count();
}

// -----------------------------
// 7. Build random 10D
// -----------------------------

vector<Point> prepare_case_build_random_10d(int n) {
    return make_random_points(n, DIMENSIONS_HIGH, RANDOM_SEED);
}

double measure_build_random_10d_kd_tree(const vector<Point>& points) {
    vector<Point> data = points;

    Time::time_point start = Time::now();
    KDTree tree(data);
    Time::time_point end = Time::now();

    benchmark_sink += tree.size();
    return Seconds(end - start).count();
}

double measure_build_random_10d_nanoflann(const vector<Point>& points) {
    vector<Point> data = points;

    Time::time_point start = Time::now();
    NanoflannKDTree tree(data, DIMENSIONS_HIGH);
    Time::time_point end = Time::now();

    benchmark_sink += data.size();
    return Seconds(end - start).count();
}

// -----------------------------
// Запуск всех benchmark-ов
// -----------------------------

void run_all() {
    write_csv_header(cout);

    benchmark_case<vector<Point>>(
        cout,
        "kd_tree",
        "1. Build random 2D",
        SIZES,
        prepare_case_build_random_2d,
        {
            {"KDTree", measure_build_random_2d_kd_tree},
            {"nanoflann", measure_build_random_2d_nanoflann},
        },
        REPEATS
    );

    benchmark_case<vector<Point>>(
        cout,
        "kd_tree",
        "2. Insert random 2D",
        SIZES,
        prepare_case_insert_random_2d,
        {
            {"KDTree", measure_insert_random_2d_kd_tree},
            {"KDTree auto rebuild", measure_insert_random_2d_kd_tree_auto_rebuild},
        },
        REPEATS
    );

    benchmark_case<vector<Point>>(
        cout,
        "kd_tree",
        "3. Insert sequential bad order 2D small",
        BAD_ORDER_SIZES,
        prepare_case_insert_sequential_bad_order_2d_small,
        {
            {"KDTree", measure_insert_sequential_bad_order_2d_small_kd_tree},
            {"KDTree auto rebuild", measure_insert_sequential_bad_order_2d_small_kd_tree_auto_rebuild},
        },
        REPEATS
    );

    benchmark_case<NearestPrepared>(
        cout,
        "kd_tree",
        "4. Nearest neighbor random queries 2D",
        SIZES,
        prepare_case_nearest_neighbor_random_queries_2d,
        {
            {"KDTree", measure_nearest_neighbor_random_queries_2d_kd_tree},
            {"brute force", measure_nearest_neighbor_random_queries_2d_brute_force},
            {"nanoflann", measure_nearest_neighbor_random_queries_2d_nanoflann},
        },
        REPEATS
    );

    benchmark_case<RangePrepared>(
        cout,
        "kd_tree",
        "5. Range search random boxes 2D",
        SIZES,
        prepare_case_range_search_random_boxes_2d,
        {
            {"KDTree", measure_range_search_random_boxes_2d_kd_tree},
            {"brute force", measure_range_search_random_boxes_2d_brute_force},
        },
        REPEATS
    );

    benchmark_case<NearestPrepared>(
        cout,
        "kd_tree",
        "6. Nearest neighbor random queries 10D",
        SIZES,
        prepare_case_nearest_neighbor_random_queries_10d,
        {
            {"KDTree", measure_nearest_neighbor_random_queries_10d_kd_tree},
            {"brute force", measure_nearest_neighbor_random_queries_10d_brute_force},
            {"nanoflann", measure_nearest_neighbor_random_queries_10d_nanoflann},
        },
        REPEATS
    );

    benchmark_case<vector<Point>>(
        cout,
        "kd_tree",
        "7. Build random 10D",
        SIZES,
        prepare_case_build_random_10d,
        {
            {"KDTree", measure_build_random_10d_kd_tree},
            {"nanoflann", measure_build_random_10d_nanoflann},
        },
        REPEATS
    );
}

int main() {
    run_all();
    return 0;
}