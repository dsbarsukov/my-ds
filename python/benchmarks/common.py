import csv
import sys
import random
from typing import Any, Callable, List, Sequence, Tuple

Implementation = Tuple[str, Callable[[Any], Any]]

# -----------------------------
# Data generation
# -----------------------------

def make_sequential_values(n: int) -> List[int]:
    return list(range(n))


def make_reverse_sequential_values(n: int) -> List[int]:
    return list(range(n - 1, -1, -1))


def make_random_unique_values(n: int, seed: int) -> List[int]:
    rng = random.Random(seed)
    return rng.sample(range(n * 20), n)


def make_shuffled(values: List[int], seed: int) -> List[int]:
    rng = random.Random(seed)
    data = values.copy()
    rng.shuffle(data)
    return data

# -----------------------------
# CSV
# -----------------------------

def make_stdout_csv_writer() -> Any:
    return csv.writer(sys.stdout)


def write_csv_header(writer: csv.writer) -> None:
    writer.writerow(["language", "structure", "case", "n", "implementation", "run", "time_sec"])


def write_csv_row(writer: csv.writer, language: str, structure: str, case_name: str, n: int, implementation: str, run_number: int, time_sec: float) -> None:
    writer.writerow([language, structure, case_name, n, implementation, run_number, f"{time_sec:.9f}"])


# -----------------------------
# Progress
# -----------------------------

def print_progress(message: str = "") -> None:
    print(message, file=sys.stderr)


# -----------------------------
# Benchmark runner
# -----------------------------

def benchmark_case(writer: Any, structure: str, case_name: str, sizes: List[int], prepare_case: Callable[[int], Any], implementations: Sequence[Implementation], repeats: int, language: str = "python") -> None:
    print_progress(f"Started: {case_name}")

    for n in sizes:
        prepared = prepare_case(n)

        for implementation_name, measure in implementations:
            for run_number in range(1, repeats + 1):
                time_sec = measure(prepared)

                write_csv_row(
                    writer=writer,
                    language=language,
                    structure=structure,
                    case_name=case_name,
                    n=n,
                    implementation=implementation_name,
                    run_number=run_number,
                    time_sec=time_sec,
                )

        print_progress(f"  finished n = {n}")

    print_progress(f"Finished: {case_name}")
    print_progress()