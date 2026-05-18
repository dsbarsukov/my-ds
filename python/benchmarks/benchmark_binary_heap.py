import heapq
import random
import time
from typing import List, Tuple

from binary_heap import BinaryHeap
from common import make_stdout_csv_writer, write_csv_header, benchmark_case, make_sequential_values, make_reverse_sequential_values, make_random_unique_values, make_shuffled


# -----------------------------
# Общие настройки
# -----------------------------

SIZES = [10_000, 50_000, 100_000, 200_000, 500_000, 1_000_000, 2_000_000, 5_000_000, 10_000_000, 15_000_000]
REPEATS = 5
RANDOM_SEED = 42
TOP_K = 100


# -----------------------------
# 1. Push sequential
# -----------------------------

def prepare_case_push_sequential(n: int) -> Tuple[List[int], BinaryHeap, List[int]]:
    values = make_sequential_values(n)
    return values, BinaryHeap(), []


def measure_push_sequential_binary_heap(prepared: Tuple[List[int], BinaryHeap, List[int]]) -> float:
    values, base_heap, _ = prepared
    heap = base_heap.copy()

    start = time.perf_counter()
    for value in values:
        heap.push(value)
    end = time.perf_counter()

    return end - start


def measure_push_sequential_heapq(prepared: Tuple[List[int], BinaryHeap, List[int]]) -> float:
    values, _, base_heapq = prepared
    heap = base_heapq.copy()

    start = time.perf_counter()
    for value in values:
        heapq.heappush(heap, value)
    end = time.perf_counter()

    return end - start


# -----------------------------
# 2. Push reverse sequential
# -----------------------------

def prepare_case_push_reverse_sequential(n: int) -> Tuple[List[int], BinaryHeap, List[int]]:
    values = make_reverse_sequential_values(n)
    return values, BinaryHeap(), []


def measure_push_reverse_sequential_binary_heap(prepared: Tuple[List[int], BinaryHeap, List[int]]) -> float:
    values, base_heap, _ = prepared
    heap = base_heap.copy()

    start = time.perf_counter()
    for value in values:
        heap.push(value)
    end = time.perf_counter()

    return end - start


def measure_push_reverse_sequential_heapq(prepared: Tuple[List[int], BinaryHeap, List[int]]) -> float:
    values, _, base_heapq = prepared
    heap = base_heapq.copy()

    start = time.perf_counter()
    for value in values:
        heapq.heappush(heap, value)
    end = time.perf_counter()

    return end - start


# -----------------------------
# 3. Push random
# -----------------------------

def prepare_case_push_random(n: int) -> Tuple[List[int], BinaryHeap, List[int]]:
    values = make_random_unique_values(n, RANDOM_SEED)
    return values, BinaryHeap(), []


def measure_push_random_binary_heap(prepared: Tuple[List[int], BinaryHeap, List[int]]) -> float:
    values, base_heap, _ = prepared
    heap = base_heap.copy()

    start = time.perf_counter()
    for value in values:
        heap.push(value)
    end = time.perf_counter()

    return end - start


def measure_push_random_heapq(prepared: Tuple[List[int], BinaryHeap, List[int]]) -> float:
    values, _, base_heapq = prepared
    heap = base_heapq.copy()

    start = time.perf_counter()
    for value in values:
        heapq.heappush(heap, value)
    end = time.perf_counter()

    return end - start


# -----------------------------
# 4. Heapify random
# -----------------------------

def prepare_case_heapify_random(n: int) -> List[int]:
    return make_random_unique_values(n, RANDOM_SEED)


def measure_heapify_random_binary_heap(values: List[int]) -> float:
    data = values.copy()

    start = time.perf_counter()
    _ = BinaryHeap(data)
    end = time.perf_counter()

    return end - start


def measure_heapify_random_heapq(values: List[int]) -> float:
    data = values.copy()

    start = time.perf_counter()
    heapq.heapify(data)
    end = time.perf_counter()

    return end - start


# -----------------------------
# 5. Pop all after heapify
# -----------------------------

def prepare_case_pop_all_after_heapify(n: int) -> Tuple[BinaryHeap, List[int]]:
    values = make_random_unique_values(n, RANDOM_SEED)

    base_heap = BinaryHeap(values)

    base_heapq = values.copy()
    heapq.heapify(base_heapq)

    return base_heap, base_heapq


def measure_pop_all_after_heapify_binary_heap(prepared: Tuple[BinaryHeap, List[int]]) -> float:
    base_heap, _ = prepared
    heap = base_heap.copy()

    start = time.perf_counter()
    while len(heap) > 0:
        heap.pop()
    end = time.perf_counter()

    return end - start


def measure_pop_all_after_heapify_heapq(prepared: Tuple[BinaryHeap, List[int]]) -> float:
    _, base_heapq = prepared
    heap = base_heapq.copy()

    start = time.perf_counter()
    while len(heap) > 0:
        heapq.heappop(heap)
    end = time.perf_counter()

    return end - start


# -----------------------------
# 6. Alternating push/pop
# -----------------------------

def prepare_case_alternating_push_pop(n: int) -> Tuple[List[Tuple[str, int]], BinaryHeap, List[int]]:
    rng = random.Random(RANDOM_SEED)

    initial_values = make_random_unique_values(n // 2, RANDOM_SEED)
    operation_values = make_random_unique_values(n, RANDOM_SEED + 1)

    operations = []
    current_size = len(initial_values)

    for value in operation_values:
        if current_size == 0:
            operations.append(("push", value))
            current_size += 1
            continue

        if rng.random() < 0.5:
            operations.append(("push", value))
            current_size += 1
        else:
            operations.append(("pop", value))
            current_size -= 1

    base_heap = BinaryHeap(initial_values)

    base_heapq = initial_values.copy()
    heapq.heapify(base_heapq)

    return operations, base_heap, base_heapq


def measure_alternating_push_pop_binary_heap(prepared: Tuple[List[Tuple[str, int]], BinaryHeap, List[int]]) -> float:
    operations, base_heap, _ = prepared
    heap = base_heap.copy()

    start = time.perf_counter()

    for operation, value in operations:
        if operation == "push":
            heap.push(value)
        else:
            heap.pop()

    end = time.perf_counter()

    return end - start


def measure_alternating_push_pop_heapq(prepared: Tuple[List[Tuple[str, int]], BinaryHeap, List[int]]) -> float:
    operations, _, base_heapq = prepared
    heap = base_heapq.copy()

    start = time.perf_counter()

    for operation, value in operations:
        if operation == "push":
            heapq.heappush(heap, value)
        else:
            heapq.heappop(heap)

    end = time.perf_counter()

    return end - start


# -----------------------------
# 7. Top-k largest
# -----------------------------

def prepare_case_top_k_largest(n: int) -> Tuple[List[int], int]:
    values = make_random_unique_values(n, RANDOM_SEED)
    k = min(TOP_K, n)
    return values, k


def measure_top_k_largest_binary_heap(prepared: Tuple[List[int], int]) -> float:
    values, k = prepared
    heap = BinaryHeap()

    start = time.perf_counter()

    for value in values:
        if len(heap) < k:
            heap.push(value)
        elif value > heap.peek():
            heap.pop()
            heap.push(value)

    end = time.perf_counter()

    return end - start


def measure_top_k_largest_heapq(prepared: Tuple[List[int], int]) -> float:
    values, k = prepared
    heap = []

    start = time.perf_counter()

    for value in values:
        if len(heap) < k:
            heapq.heappush(heap, value)
        elif value > heap[0]:
            heapq.heappop(heap)
            heapq.heappush(heap, value)

    end = time.perf_counter()

    return end - start


# -----------------------------
# Запуск всех benchmark-ов
# -----------------------------

def run_all() -> None:
    writer = make_stdout_csv_writer()
    write_csv_header(writer)

    benchmark_case(
        writer=writer,
        structure="binary_heap",
        case_name="1. Push sequential",
        sizes=SIZES,
        prepare_case=prepare_case_push_sequential,
        implementations=[("BinaryHeap", measure_push_sequential_binary_heap), ("heapq", measure_push_sequential_heapq)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="binary_heap",
        case_name="2. Push reverse sequential",
        sizes=SIZES,
        prepare_case=prepare_case_push_reverse_sequential,
        implementations=[("BinaryHeap", measure_push_reverse_sequential_binary_heap), ("heapq", measure_push_reverse_sequential_heapq)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="binary_heap",
        case_name="3. Push random",
        sizes=SIZES,
        prepare_case=prepare_case_push_random,
        implementations=[("BinaryHeap", measure_push_random_binary_heap), ("heapq", measure_push_random_heapq)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="binary_heap",
        case_name="4. Heapify random",
        sizes=SIZES,
        prepare_case=prepare_case_heapify_random,
        implementations=[("BinaryHeap", measure_heapify_random_binary_heap), ("heapq", measure_heapify_random_heapq)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="binary_heap",
        case_name="5. Pop all after heapify",
        sizes=SIZES,
        prepare_case=prepare_case_pop_all_after_heapify,
        implementations=[("BinaryHeap", measure_pop_all_after_heapify_binary_heap), ("heapq", measure_pop_all_after_heapify_heapq)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="binary_heap",
        case_name="6. Alternating push/pop",
        sizes=SIZES,
        prepare_case=prepare_case_alternating_push_pop,
        implementations=[("BinaryHeap", measure_alternating_push_pop_binary_heap), ("heapq", measure_alternating_push_pop_heapq)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="binary_heap",
        case_name="7. Top-k largest",
        sizes=SIZES,
        prepare_case=prepare_case_top_k_largest,
        implementations=[("BinaryHeap", measure_top_k_largest_binary_heap), ("heapq", measure_top_k_largest_heapq)],
        repeats=REPEATS,
    )


if __name__ == "__main__":
    run_all()