import random
import time
from typing import List, Tuple

from hash_table import HashTable
from common import make_stdout_csv_writer, write_csv_header, benchmark_case, make_sequential_values, make_random_unique_values, make_shuffled

# -----------------------------
# Общие настройки
# -----------------------------

SIZES = [10_000, 50_000, 100_000, 200_000, 500_000, 1_000_000, 2_000_000, 5_000_000, 10_000_000, 15_000_000]
REPEATS = 5
RANDOM_SEED = 42


# -----------------------------
# 1. Insert sequential
# -----------------------------

def prepare_case_insert_sequential(n: int) -> Tuple[List[int], HashTable, dict]:
    return make_sequential_values(n), HashTable(), {}


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
    return make_random_unique_values(n, RANDOM_SEED), HashTable(), {}


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
    keys = make_sequential_values(n)
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
    insert_keys = make_random_unique_values(n, RANDOM_SEED)
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
    existing_keys = make_random_unique_values(n, RANDOM_SEED)

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
    keys = make_sequential_values(n)
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
    keys = make_random_unique_values(n, RANDOM_SEED)
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
    initial_keys = make_sequential_values(n)
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

    initial_fill = make_random_unique_values(n // 2, seed)

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


def measure_mixed_hash_table(prepared: Tuple[List[int], HashTable, dict]) -> float:
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

    return end - start


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


# -----------------------------
# Запуск всех benchmark-ов
# -----------------------------

def run_all() -> None:
    writer = make_stdout_csv_writer()
    write_csv_header(writer)

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="1. Insert sequential",
        sizes=SIZES,
        prepare_case=prepare_case_insert_sequential,
        implementations=[("HashTable", measure_insert_sequential_hash_table), ("dict", measure_insert_sequential_dict)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="2. Insert random",
        sizes=SIZES,
        prepare_case=prepare_case_insert_random,
        implementations=[("HashTable", measure_insert_random_hash_table), ("dict", measure_insert_random_dict)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="3. Get existing sequential",
        sizes=SIZES,
        prepare_case=prepare_case_get_existing_sequential,
        implementations=[("HashTable", measure_get_existing_sequential_hash_table),("dict", measure_get_existing_sequential_dict)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="4. Get existing random order",
        sizes=SIZES,
        prepare_case=prepare_case_get_existing_random,
        implementations=[("HashTable", measure_get_existing_random_hash_table),("dict", measure_get_existing_random_dict)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="5. Get missing keys",
        sizes=SIZES,
        prepare_case=prepare_case_get_missing,
        implementations=[("HashTable", measure_get_missing_hash_table),("dict", measure_get_missing_dict)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="6. Delete",
        sizes=SIZES,
        prepare_case=prepare_case_delete,
        implementations=[("HashTable", measure_delete_hash_table),("dict", measure_delete_dict)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="7. Get after deletions",
        sizes=SIZES,
        prepare_case=prepare_case_get_after_deletions,
        implementations=[("HashTable", measure_get_after_deletions_hash_table),("dict", measure_get_after_deletions_dict)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="8. Insert after deletions",
        sizes=SIZES,
        prepare_case=prepare_case_insert_after_deletions,
        implementations=[("HashTable", measure_insert_after_deletions_hash_table),("dict", measure_insert_after_deletions_dict)],
        repeats=REPEATS,
    )

    benchmark_case(
        writer=writer,
        structure="hash_table",
        case_name="9. Mixed workload",
        sizes=SIZES,
        prepare_case=lambda n: prepare_case_mixed(n, RANDOM_SEED),
        implementations=[("HashTable", measure_mixed_hash_table),("dict", measure_mixed_dict)],
        repeats=REPEATS,
    )


if __name__ == "__main__":
    run_all()