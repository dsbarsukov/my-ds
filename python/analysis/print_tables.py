import csv
import sys
from typing import Dict, List, Tuple


ResultKey = Tuple[str, int]
CaseResults = Dict[ResultKey, float]


def read_summary_results(input_path: str) -> Dict[str, CaseResults]:
    tables = {}

    with open(input_path, "r", newline="") as file:
        reader = csv.DictReader(file)

        for row in reader:
            case_name = row["case"]
            n = int(row["n"])
            implementation = row["implementation"]
            median = float(row["median"])

            if case_name not in tables:
                tables[case_name] = {}

            tables[case_name][(implementation, n)] = median

    return tables


def get_sorted_sizes(case_results: CaseResults) -> List[int]:
    sizes = []

    for implementation, n in case_results.keys():
        if n not in sizes:
            sizes.append(n)

    return sorted(sizes)

def format_seconds(value: float) -> str:
    return f"~{value:.3f} s"

def write_pair_tables(tables: Dict[str, CaseResults], first_implementation: str, second_implementation: str, output_path: str) -> None:
    with open(output_path, "w", newline="") as file:
        for case_name in sorted(tables.keys()):
            file.write(f"\n{case_name}\n")
            file.write("-" * 70 + "\n")
            file.write(f"{'N':>10} | {first_implementation:>18} | {second_implementation:>18} | {'ratio':>10}\n")
            file.write("-" * 70 + "\n")

            case_results = tables[case_name]
            sizes = get_sorted_sizes(case_results)

            for n in sizes:
                first_time = case_results[(first_implementation, n)]
                second_time = case_results[(second_implementation, n)]

                ratio = first_time / second_time if second_time > 0 else float("inf")

                file.write(f"{n:>10} | {first_time:>14.6f} sec | {second_time:>14.6f} sec | {ratio:>9.2f}x\n")

def write_four_tables(
    tables: Dict[str, CaseResults],
    first_implementation: str,
    second_implementation: str,
    third_implementation: str,
    fourth_implementation: str,
    output_path: str,
) -> None:
    with open(output_path, "w", newline="") as file:
        for case_name in sorted(tables.keys()):
            file.write(f"\n{case_name}\n")
            file.write("-" * 90 + "\n")
            file.write(
                f"{'N':>10} | "
                f"{first_implementation:>16} | "
                f"{second_implementation:>18} | "
                f"{third_implementation:>18} | "
                f"{fourth_implementation:>16}\n"
            )
            file.write("-" * 90 + "\n")

            case_results = tables[case_name]
            sizes = get_sorted_sizes(case_results)[-5:]

            for n in sizes:
                first_time = case_results[(first_implementation, n)]
                second_time = case_results[(second_implementation, n)]
                third_time = case_results[(third_implementation, n)]
                fourth_time = case_results[(fourth_implementation, n)]

                file.write(
                    f"{n:>10} | "
                    f"{format_seconds(first_time):>16} | "
                    f"{format_seconds(second_time):>18} | "
                    f"{format_seconds(third_time):>18} | "
                    f"{format_seconds(fourth_time):>16}\n"
                )

def main() -> None:
    if len(sys.argv) not in (5, 7):
        print(
            "Usage: python print_tables.py <summary_csv> <output_txt> <first_implementation> <second_implementation>",
            file=sys.stderr,
        )
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]
    implementations = sys.argv[3:]

    tables = read_summary_results(input_path)

    if len(implementations) == 2:
        write_pair_tables(
            tables=tables,
            first_implementation=implementations[0],
            second_implementation=implementations[1],
            output_path=output_path,
        )
    else:
        write_four_tables(
            tables=tables,
            first_implementation=implementations[0],
            second_implementation=implementations[1],
            third_implementation=implementations[2],
            fourth_implementation=implementations[3],
            output_path=output_path,
        )


if __name__ == "__main__":
    main()