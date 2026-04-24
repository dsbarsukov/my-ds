import random
import statistics
import time
from typing import Callable, List, Tuple

from hash_table import HashTable

# -----------------------------
# Общие настройки
# -----------------------------

SIZES = [10_000, 50_000, 100_000, 200_000, 500_000, 1_000_000, 2_000_000, 5_000_000, 10_000_000, 15_000_000]
REPEATS = 5
RANDOM_SEED = 42

# -----------------------------
# Вспомогательные функции
# -----------------------------

def median(values: List[float]) -> float:
    return statistics.median(values)

def print_section(title: str) -> None:
    print("\n" + "=" * 70)
    print(title)
    print("=" * 70)

def print_result_table(scenario_name: str, results: List[Tuple[int, float, float]]) -> None:
    print(f"\n{scenario_name}")
    print("-" * 70)
    print(f"{'N':>10} | {'HashTable':>18} | {'dict':>18} | {'ratio':>10}")
    print("-" * 70)

    for n, ht_time, dict_time in results:
        ratio = ht_time / dict_time if dict_time > 0 else float("inf")
        print(f"{n:>10} | {ht_time:>14.6f} sec | {dict_time:>14.6f} sec | {ratio:>9.2f}x")

# -----------------------------
# Генерация данных
# -----------------------------

def make_sequential_keys(n: int) -> List[int]:
    return list(range(n))

def make_random_unique_keys(n: int, seed: int) -> List[int]:
    rng = random.Random(seed)
    return rng.sample(range(n * 20), n)

def make_shuffled(keys: List[int], seed: int) -> List[int]:
    rng = random.Random(seed)
    data = keys.copy()
    rng.shuffle(data)
    return data

# -----------------------------
# 1. Insert sequential
# -----------------------------
def prepare_case_insert_sequential(n: int) -> Tuple[List[int], HashTable, dict]:
    return make_sequential_keys(n), HashTable(), {}

def measure_insert_sequential_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys, base_ht, _ = prepared
    ht = base_ht.copy()

    start = time.perf_counter()
    for key in keys:
        ht.put(key, key)
    end = time.perf_counter()

    return end - start

def measure_insert_sequential_dict(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys, _, base_d = prepared
    d = base_d.copy()

    start = time.perf_counter()
    for key in keys:
        d[key] = key
    end = time.perf_counter()

    return end - start

# -----------------------------
# 2. Insert random
# -----------------------------

def prepare_case_insert_random(n: int) -> Tuple[List[int], HashTable, dict]:
    return make_random_unique_keys(n, RANDOM_SEED), HashTable(), {}

def measure_insert_random_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys, base_ht, _ = prepared
    ht = base_ht.copy()

    start = time.perf_counter()
    for key in keys:
        ht.put(key, key)
    end = time.perf_counter()

    return end - start

def measure_insert_random_dict(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys, _, base_d = prepared
    d = base_d.copy()

    start = time.perf_counter()
    for key in keys:
        d[key] = key
    end = time.perf_counter()

    return end - start

# -----------------------------
# 3. Get existing sequential
# -----------------------------

def prepare_case_get_existing_sequential(n: int) -> Tuple[List[int], HashTable, dict]:
    keys = make_sequential_keys(n)
    base_ht = HashTable()
    base_d = {}
    for key in keys:
        base_ht.put(key, key)
        base_d[key] = key
    return keys, base_ht, base_d

def measure_get_existing_sequential_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys, ht, _ = prepared

    start = time.perf_counter()
    for key in keys:
        _ = ht.get(key)
    end = time.perf_counter()

    return end - start

def measure_get_existing_sequential_dict(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys, _, d = prepared

    start = time.perf_counter()
    for key in keys:
        _ = d.get(key)
    end = time.perf_counter()

    return end - start

# -----------------------------
# 4. Get existing random order
# -----------------------------

def prepare_case_get_existing_random(n: int) -> Tuple[List[int], HashTable, dict]:
    insert_keys = make_random_unique_keys(n, RANDOM_SEED)
    query_keys = make_shuffled(insert_keys, RANDOM_SEED + 1)
    base_ht = HashTable()
    base_d = {}
    for key in insert_keys:
        base_ht.put(key, key)
        base_d[key] = key
    return query_keys, base_ht, base_d

def measure_get_existing_random_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
    query_keys, ht, _ = prepared

    start = time.perf_counter()
    for key in query_keys:
        _ = ht.get(key)
    end = time.perf_counter()

    return end - start

def measure_get_existing_random_dict(prepared: Tuple[List[int], HashTable, dict]) -> float:
    query_keys, _, d = prepared

    start = time.perf_counter()
    for key in query_keys:
        _ = d.get(key)
    end = time.perf_counter()

    return end - start

# -----------------------------
# 5. Get missing keys
# -----------------------------

def prepare_case_get_missing(n: int) -> Tuple[List[int], HashTable, dict]:
    existing_keys = make_random_unique_keys(n, RANDOM_SEED)

    rng = random.Random(RANDOM_SEED + 1)
    missing_keys = rng.sample(range(n * 20, n * 40), n)
    missing_keys = make_shuffled(missing_keys, RANDOM_SEED + 2)

    base_ht = HashTable()
    base_d = {}
    for key in existing_keys:
        base_ht.put(key, key)
        base_d[key] = key
    return missing_keys, base_ht, base_d

def measure_get_missing_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
    missing_keys, ht, _ = prepared

    start = time.perf_counter()
    for key in missing_keys:
        _ = ht.get(key)
    end = time.perf_counter()

    return end - start

def measure_get_missing_dict(prepared: Tuple[List[int], HashTable, dict]) -> float:
    missing_keys, _, d = prepared

    start = time.perf_counter()
    for key in missing_keys:
        _ = d.get(key)
    end = time.perf_counter()

    return end - start

# -----------------------------
# 6. Delete
# -----------------------------

def prepare_case_delete(n: int) -> Tuple[List[int], HashTable, dict]:
    keys = make_sequential_keys(n)
    base_ht = HashTable()
    base_d = {}
    for key in keys:
        base_ht.put(key, key)
        base_d[key] = key
    return keys, base_ht, base_d

def measure_delete_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys, base_ht, _ = prepared
    ht = base_ht.copy()

    start = time.perf_counter()
    for key in keys:
        ht.delete(key)
    end = time.perf_counter()

    return end - start

def measure_delete_dict(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys, _, base_d = prepared
    d = base_d.copy()

    start = time.perf_counter()
    for key in keys:
        del d[key]
    end = time.perf_counter()

    return end - start

# -----------------------------
# 7. Get after deletions
# -----------------------------

def prepare_case_get_after_deletions(n: int) -> Tuple[List[int], HashTable, dict]:
    keys = make_random_unique_keys(n, RANDOM_SEED)
    keys_to_delete = keys[::2]
    keys_to_keep = keys[1::2]
    base_ht = HashTable()
    base_d = {}
    for key in keys:
        base_ht.put(key, key)
        base_d[key] = key
    for key in keys_to_delete:
        base_ht.delete(key)
        del base_d[key]
    return keys_to_keep, base_ht, base_d

def measure_get_after_deletions_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys_to_keep, ht, _ = prepared

    start = time.perf_counter()
    for key in keys_to_keep:
        _ = ht.get(key)
    end = time.perf_counter()

    return end - start

def measure_get_after_deletions_dict(prepared: Tuple[List[int], HashTable, dict]) -> float:
    keys_to_keep, _, d = prepared

    start = time.perf_counter()
    for key in keys_to_keep:
        _ = d.get(key)
    end = time.perf_counter()

    return end - start

# -----------------------------
# 8. Insert after deletions
# -----------------------------

def prepare_case_insert_after_deletions(n: int) -> Tuple[List[int], HashTable, dict]:
    initial_keys = make_sequential_keys(n)
    keys_to_delete = initial_keys[::2]
    new_keys = list(range(n, n + len(keys_to_delete)))
    base_ht = HashTable()
    base_d = {}
    for key in initial_keys:
        base_ht.put(key, key)
        base_d[key] = key
    for key in keys_to_delete:
        base_ht.delete(key)
        del base_d[key]
    return new_keys, base_ht, base_d

def measure_insert_after_deletions_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
    new_keys, base_ht, _ = prepared
    ht = base_ht.copy()

    start = time.perf_counter()
    for key in new_keys:
        ht.put(key, key)
    end = time.perf_counter()

    return end - start

def measure_insert_after_deletions_dict(prepared: Tuple[List[int], HashTable, dict]) -> float:
    new_keys, _, base_d = prepared
    d = base_d.copy()

    start = time.perf_counter()
    for key in new_keys:
        d[key] = key
    end = time.perf_counter()

    return end - start

# -----------------------------
# 9. Mixed workload
# -----------------------------

def prepare_case_mixed(n: int, seed: int) -> Tuple[List[Tuple[str, int]], HashTable, dict]:
    rng = random.Random(seed)

    initial_fill = make_random_unique_keys(n // 2, seed)

    live_keys_list = list(initial_fill)

    operations = []
    next_new_key = max(initial_fill) + 1

    for _ in range(n):
        op_choice = rng.random()

        if op_choice < 0.5:
            # GET
            if live_keys_list and rng.random() < 0.8:
                key = rng.choice(live_keys_list)
            else:
                key = next_new_key + rng.randint(1, n)
            operations.append(("get", key))

        elif op_choice < 0.8:
            # PUT
            if live_keys_list and rng.random() < 0.3:
                key = rng.choice(live_keys_list)
            else:
                key = next_new_key
                next_new_key += 1
                live_keys_list.append(key)
            operations.append(("put", key))

        else:
            # DELETE
            if live_keys_list:
                idx = rng.randrange(len(live_keys_list))
                key = live_keys_list[idx]

                last_key = live_keys_list[-1]
                live_keys_list[idx] = last_key
                live_keys_list.pop()

                operations.append(("delete", key))
            else:
                key = next_new_key + rng.randint(1, n)
                operations.append(("delete", key))

    base_ht = HashTable()
    base_d = {}
    for key in initial_fill:
        base_ht.put(key, key)
        base_d[key] = key

    return operations, base_ht, base_d

def measure_mixed_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> Tuple[float, int, int]:
    operations, base_ht, _ = prepared
    ht = base_ht.copy()

    start = time.perf_counter()

    for op, key in operations:
        if op == "put":
            ht.put(key, key)
        elif op == "get":
            _ = ht.get(key)
        elif op == "delete":
            try:
                ht.delete(key)
            except KeyError:
                pass

    end = time.perf_counter()

    return end - start, ht._find_slot_calls, ht._find_slot_total_steps

def measure_mixed_dict(prepared: Tuple[List[Tuple[str, int]], HashTable, dict]) -> float:
    operations, _, base_d = prepared
    d = base_d.copy()

    start = time.perf_counter()

    for op, key in operations:
        if op == "put":
            d[key] = key
        elif op == "get":
            _ = d.get(key)
        elif op == "delete":
            d.pop(key, None)

    end = time.perf_counter()
    return end - start

# ----------------------------
# Универсальные запускатели
# ----------------------------

def benchmark_case(scenario_name: str, sizes: List[int], prepare_case: Callable[[int], Tuple[List[int], HashTable, dict]], measure_ht: Callable[[Tuple[List[int], HashTable, dict]], float], measure_dict: Callable[[Tuple[List[int], HashTable, dict]], float], repeats: int = REPEATS) -> None:
    results: List[Tuple[int, float, float]] = []

    for n in sizes:
        prepared = prepare_case(n)
        ht_runs = [measure_ht(prepared) for _ in range(repeats)]
        dict_runs = [measure_dict(prepared) for _ in range(repeats)]

        ht_med = median(ht_runs)
        dict_med = median(dict_runs)
        results.append((n, ht_med, dict_med))

    print_result_table(scenario_name, results)

def benchmark_case_mixed(sizes: List[int], prepare_case_mixed: Callable[[Tuple[int, int]], Tuple[List[int], HashTable, dict]], measure_ht: Callable[[Tuple[List[int], HashTable, dict]], float], measure_dict: Callable[[Tuple[List[int], HashTable, dict]], float], repeats: int = REPEATS) -> None:
    mixed_results = []

    for n in sizes:
        prepared = prepare_case_mixed(n, RANDOM_SEED)

        ht_runs = []
        calls_runs = []
        steps_runs = []

        for _ in range(repeats):
            ht_time, calls, steps = measure_ht(prepared)
            ht_runs.append(ht_time)
            calls_runs.append(calls)
            steps_runs.append(steps)

        dict_runs = [measure_dict(prepared) for _ in range(repeats)]

        ht_med = median(ht_runs)
        dict_med = median(dict_runs)
        mixed_results.append((n, ht_med, dict_med))

        total_calls = sum(calls_runs)
        total_steps = sum(steps_runs)
        avg_probe_length = total_steps / total_calls if total_calls > 0 else 0.0

        print(f"\nProbe stats for N = {n}")
        print("avg_probe_length:", avg_probe_length)
        print("find_slot_calls:", total_calls / REPEATS)
        print("total_steps:", total_steps / REPEATS)

    print_result_table("9. Mixed workload", mixed_results)

# -----------------------------
# Запуск всех benchmark-ов
# -----------------------------

def run_all() -> None:
    print_section("HashTable vs dict benchmarks")

    benchmark_case("1. Insert sequential", SIZES, prepare_case_insert_sequential, measure_insert_sequential_hash_table, measure_insert_sequential_dict)

    benchmark_case("2. Insert random", SIZES, prepare_case_insert_random, measure_insert_random_hash_table, measure_insert_random_dict)

    benchmark_case("3. Get existing sequential", SIZES, prepare_case_get_existing_sequential, measure_get_existing_sequential_hash_table, measure_get_existing_sequential_dict)

    benchmark_case("4. Get existing random order", SIZES, prepare_case_get_existing_random, measure_get_existing_random_hash_table, measure_get_existing_random_dict)

    benchmark_case("5. Get missing keys", SIZES, prepare_case_get_missing, measure_get_missing_hash_table, measure_get_missing_dict)

    benchmark_case("6. Delete", SIZES, prepare_case_delete, measure_delete_hash_table, measure_delete_dict)

    benchmark_case("7. Get after deletions", SIZES, prepare_case_get_after_deletions, measure_get_after_deletions_hash_table, measure_get_after_deletions_dict)

    benchmark_case("8. Insert after deletions" , SIZES, prepare_case_insert_after_deletions, measure_insert_after_deletions_hash_table, measure_insert_after_deletions_dict)

    benchmark_case_mixed(SIZES, prepare_case_mixed, measure_mixed_hash_table, measure_mixed_dict)

if __name__ == "__main__":
    run_all()