import random
import time
from typing import List, Tuple

from scipy.spatial import cKDTree

from kd_tree import KDTree
from common import make_stdout_csv_writer, write_csv_header, benchmark_case


Point = tuple[float, ...]


# ---------------------------------------
# Общие настройки
# ---------------------------------------

SIZES = [1_000, 5_000, 10_000, 50_000, 100_000, 200_000, 500_000, 1_000_000, 2_000_000, 5_000_000]
BAD_ORDER_SIZES = [100, 500, 1_000, 2_000, 5_000, 10_000, 20_000, 40_000]
REPEATS = 5
RANDOM_SEED = 42

DIMENSIONS_2D = 2
DIMENSIONS_HIGH = 10

QUERY_COUNT = 100
RANGE_QUERY_COUNT = 100


# ---------------------------------------
# Data generation
# ---------------------------------------

def make_random_points(n: int, dimensions: int, seed: int) -> List[Point]:
    rng = random.Random(seed)
    points = []

    for _ in range(n):
        point = tuple(rng.random() for _ in range(dimensions))
        points.append(point)

    return points


def make_sequential_points(n: int, dimensions: int) -> List[Point]:
    points = []

    for value in range(n):
        point = tuple(float(value) for _ in range(dimensions))
        points.append(point)

    return points


def make_random_range_queries(query_count: int, dimensions: int, seed: int) -> List[Tuple[Point, Point]]:
    rng = random.Random(seed)
    queries = []

    for _ in range(query_count):
        first_point = tuple(rng.random() for _ in range(dimensions))
        second_point = tuple(rng.random() for _ in range(dimensions))

        lower_bound = tuple(min(first_point[index], second_point[index]) for index in range(dimensions))
        upper_bound = tuple(max(first_point[index], second_point[index]) for index in range(dimensions))

        queries.append((lower_bound, upper_bound))

    return queries


# ---------------------------------------
# 1. Build random 2D
# ---------------------------------------

def prepare_case_build_random_2d(n: int) -> List[Point]:
    return make_random_points(n, DIMENSIONS_2D, RANDOM_SEED)


def measure_build_random_2d_kd_tree(points: List[Point]) -> float:
    data = points.copy()

    start = time.perf_counter()
    _ = KDTree(data)
    end = time.perf_counter()

    return end - start


def measure_build_random_2d_ckd_tree(points: List[Point]) -> float:
    data = points.copy()

    start = time.perf_counter()
    _ = cKDTree(data)
    end = time.perf_counter()

    return end - start


# ---------------------------------------
# 2. Insert random 2D
# ---------------------------------------

def prepare_case_insert_random_2d(n: int) -> List[Point]:
    return make_random_points(n, DIMENSIONS_2D, RANDOM_SEED)


def measure_insert_random_2d_kd_tree(points: List[Point]) -> float:
    tree = KDTree()

    start = time.perf_counter()
    for point in points:
        tree.insert(point)
    end = time.perf_counter()

    return end - start


def measure_insert_random_2d_kd_tree_auto_rebuild(points: List[Point]) -> float:
    tree = KDTree(auto_rebuild=True, rebuild_factor=2.0)

    start = time.perf_counter()
    for point in points:
        tree.insert(point)
    end = time.perf_counter()

    return end - start


# ---------------------------------------
# 3. Insert sequential bad order 2D small
# ---------------------------------------

def prepare_case_insert_sequential_bad_order_2d_small(n: int) -> List[Point]:
    return make_sequential_points(n, DIMENSIONS_2D)


def measure_insert_sequential_bad_order_2d_small_kd_tree(points: List[Point]) -> float:
    tree = KDTree()

    start = time.perf_counter()
    for point in points:
        tree.insert(point)
    end = time.perf_counter()

    return end - start


def measure_insert_sequential_bad_order_2d_small_kd_tree_auto_rebuild(points: List[Point]) -> float:
    tree = KDTree(auto_rebuild=True, rebuild_factor=2.0)

    start = time.perf_counter()
    for point in points:
        tree.insert(point)
    end = time.perf_counter()

    return end - start


# ---------------------------------------
# 4. Nearest neighbor random queries 2D
# ---------------------------------------

def prepare_case_nearest_neighbor_random_queries_2d(n: int) -> Tuple[List[Point], List[Point], KDTree, cKDTree]:
    points = make_random_points(n, DIMENSIONS_2D, RANDOM_SEED)
    queries = make_random_points(QUERY_COUNT, DIMENSIONS_2D, RANDOM_SEED + 1)

    kd_tree = KDTree(points)
    scipy_tree = cKDTree(points)

    return points, queries, kd_tree, scipy_tree


def measure_nearest_neighbor_random_queries_2d_kd_tree(prepared: Tuple[List[Point], List[Point], KDTree, cKDTree]) -> float:
    _, queries, kd_tree, _ = prepared

    start = time.perf_counter()
    for query in queries:
        kd_tree.nearest_neighbor(query)
    end = time.perf_counter()

    return end - start


def measure_nearest_neighbor_random_queries_2d_brute_force(prepared: Tuple[List[Point], List[Point], KDTree, cKDTree]) -> float:
    points, queries, kd_tree, _ = prepared

    start = time.perf_counter()
    for query in queries:
        kd_tree.brute_force_nearest(points, query)
    end = time.perf_counter()

    return end - start


def measure_nearest_neighbor_random_queries_2d_ckd_tree(prepared: Tuple[List[Point], List[Point], KDTree, cKDTree]) -> float:
    _, queries, _, scipy_tree = prepared

    start = time.perf_counter()
    for query in queries:
        scipy_tree.query(query, k=1)
    end = time.perf_counter()

    return end - start


# ---------------------------------------
# 5. Range search random boxes 2D
# ---------------------------------------

def prepare_case_range_search_random_boxes_2d(n: int) -> Tuple[List[Point], List[Tuple[Point, Point]], KDTree]:
    points = make_random_points(n, DIMENSIONS_2D, RANDOM_SEED)
    queries = make_random_range_queries(RANGE_QUERY_COUNT, DIMENSIONS_2D, RANDOM_SEED + 2)

    kd_tree = KDTree(points)

    return points, queries, kd_tree


def measure_range_search_random_boxes_2d_kd_tree(prepared: Tuple[List[Point], List[Tuple[Point, Point]], KDTree]) -> float:
    _, queries, kd_tree = prepared

    start = time.perf_counter()
    for lower_bound, upper_bound in queries:
        kd_tree.range_search(lower_bound, upper_bound)
    end = time.perf_counter()

    return end - start


def measure_range_search_random_boxes_2d_brute_force(prepared: Tuple[List[Point], List[Tuple[Point, Point]], KDTree]) -> float:
    points, queries, kd_tree = prepared

    start = time.perf_counter()
    for lower_bound, upper_bound in queries:
        kd_tree.brute_force_range_search(points, lower_bound, upper_bound)
    end = time.perf_counter()

    return end - start


# ---------------------------------------
# 6. Nearest neighbor random queries 10D
# ---------------------------------------

def prepare_case_nearest_neighbor_random_queries_10d(n: int) -> Tuple[List[Point], List[Point], KDTree, cKDTree]:
    points = make_random_points(n, DIMENSIONS_HIGH, RANDOM_SEED)
    queries = make_random_points(QUERY_COUNT, DIMENSIONS_HIGH, RANDOM_SEED + 3)

    kd_tree = KDTree(points)
    scipy_tree = cKDTree(points)

    return points, queries, kd_tree, scipy_tree


def measure_nearest_neighbor_random_queries_10d_kd_tree(prepared: Tuple[List[Point], List[Point], KDTree, cKDTree]) -> float:
    _, queries, kd_tree, _ = prepared

    start = time.perf_counter()
    for query in queries:
        kd_tree.nearest_neighbor(query)
    end = time.perf_counter()

    return end - start


def measure_nearest_neighbor_random_queries_10d_brute_force(prepared: Tuple[List[Point], List[Point], KDTree, cKDTree]) -> float:
    points, queries, kd_tree, _ = prepared

    start = time.perf_counter()
    for query in queries:
        kd_tree.brute_force_nearest(points, query)
    end = time.perf_counter()

    return end - start


def measure_nearest_neighbor_random_queries_10d_ckd_tree(prepared: Tuple[List[Point], List[Point], KDTree, cKDTree]) -> float:
    _, queries, _, scipy_tree = prepared

    start = time.perf_counter()
    for query in queries:
        scipy_tree.query(query, k=1)
    end = time.perf_counter()

    return end - start


# ---------------------------------------
# 7. Build random 10D
# ---------------------------------------

def prepare_case_build_random_10d(n: int) -> List[Point]:
    return make_random_points(n, DIMENSIONS_HIGH, RANDOM_SEED)


def measure_build_random_10d_kd_tree(points: List[Point]) -> float:
    data = points.copy()

    start = time.perf_counter()
    _ = KDTree(data)
    end = time.perf_counter()

    return end - start


def measure_build_random_10d_ckd_tree(points: List[Point]) -> float:
    data = points.copy()

    start = time.perf_counter()
    _ = cKDTree(data)
    end = time.perf_counter()

    return end - start


# ---------------------------------------
# Запуск всех benchmark-ов
# ---------------------------------------

def run_all() -> None:
    writer = make_stdout_csv_writer()
    write_csv_header(writer)

    benchmark_case(
        writer=writer,
        structure="kd_tree",
        case_name="1. Build random 2D",
        sizes=SIZES,
        prepare_case=prepare_case_build_random_2d,
        implementations=[
            ("KDTree", measure_build_random_2d_kd_tree),
            ("scipy cKDTree", measure_build_random_2d_ckd_tree),
        ],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="kd_tree",
        case_name="2. Insert random 2D",
        sizes=SIZES,
        prepare_case=prepare_case_insert_random_2d,
        implementations=[
            ("KDTree", measure_insert_random_2d_kd_tree),
            ("KDTree auto rebuild", measure_insert_random_2d_kd_tree_auto_rebuild),
        ],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="kd_tree",
        case_name="3. Insert sequential bad order 2D small",
        sizes=BAD_ORDER_SIZES,
        prepare_case=prepare_case_insert_sequential_bad_order_2d_small,
        implementations=[
            ("KDTree", measure_insert_sequential_bad_order_2d_small_kd_tree),
            ("KDTree auto rebuild", measure_insert_sequential_bad_order_2d_small_kd_tree_auto_rebuild),
        ],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="kd_tree",
        case_name="4. Nearest neighbor random queries 2D",
        sizes=SIZES,
        prepare_case=prepare_case_nearest_neighbor_random_queries_2d,
        implementations=[
            ("KDTree", measure_nearest_neighbor_random_queries_2d_kd_tree),
            ("brute force", measure_nearest_neighbor_random_queries_2d_brute_force),
            ("scipy cKDTree", measure_nearest_neighbor_random_queries_2d_ckd_tree),
        ],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="kd_tree",
        case_name="5. Range search random boxes 2D",
        sizes=SIZES,
        prepare_case=prepare_case_range_search_random_boxes_2d,
        implementations=[
            ("KDTree", measure_range_search_random_boxes_2d_kd_tree),
            ("brute force", measure_range_search_random_boxes_2d_brute_force),
        ],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="kd_tree",
        case_name="6. Nearest neighbor random queries 10D",
        sizes=SIZES,
        prepare_case=prepare_case_nearest_neighbor_random_queries_10d,
        implementations=[
            ("KDTree", measure_nearest_neighbor_random_queries_10d_kd_tree),
            ("brute force", measure_nearest_neighbor_random_queries_10d_brute_force),
            ("scipy cKDTree", measure_nearest_neighbor_random_queries_10d_ckd_tree),
        ],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="kd_tree",
        case_name="7. Build random 10D",
        sizes=SIZES,
        prepare_case=prepare_case_build_random_10d,
        implementations=[
            ("KDTree", measure_build_random_10d_kd_tree),
            ("scipy cKDTree", measure_build_random_10d_ckd_tree),
        ],
        repeats=REPEATS,
    )


if __name__ == "__main__":
    run_all()