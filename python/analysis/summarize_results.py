import csv
import statistics
import sys
from typing import Dict, List, Tuple


GroupKey = Tuple[str, str, str, int, str]


def read_raw_results(input_path: str) -> Dict[GroupKey, List[float]]:
    groups = {}

    with open(input_path, "r", newline="") as file:
        reader = csv.DictReader(file)

        for row in reader:
            key = (row["language"], row["structure"], row["case"], int(row["n"]), row["implementation"])

            time_sec = float(row["time_sec"])
            if key not in groups:
                groups[key] = []

            groups[key].append(time_sec)

    return groups


def write_summary(groups: Dict[GroupKey, List[float]], output_path: str) -> None:
    with open(output_path, "w", newline="") as file:
        writer = csv.writer(file)

        writer.writerow(["language", "structure", "case", "n", "implementation", "mean", "median", "std", "sem"])

        for key in sorted(groups.keys()):
            language, structure, case_name, n, implementation = key
            values = groups[key]

            mean = statistics.mean(values)
            median = statistics.median(values)

            if len(values) >= 2:
                std = statistics.stdev(values)
                sem = std / len(values)**0.5
            else:
                std = 0.0
                sem = 0.0

            writer.writerow([language, structure, case_name, n, implementation, f"{mean:.9f}", f"{median:.9f}", f"{std:.9f}", f"{sem:.9f}"])


def main() -> None:
    if len(sys.argv) != 3:
        print("Usage: python summarize_results.py <input_raw_csv> <output_summary_csv>", file=sys.stderr)
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]

    groups = read_raw_results(input_path)
    write_summary(groups, output_path)


if __name__ == "__main__":
    main()