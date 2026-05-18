import argparse
import csv
import math
from pathlib import Path
from typing import Dict, List, Optional

from mpl_toolkits.axes_grid1.inset_locator import inset_axes, mark_inset
from matplotlib.ticker import ScalarFormatter, AutoMinorLocator
import matplotlib.pyplot as plt


REQUIRED_COLUMNS = {"language", "structure", "case", "n", "implementation", "mean"}
OPTIONAL_COLUMNS = {"median", "std", "sem"}
Row = Dict[str, str]


def read_summary_file(path: Path) -> List[Row]:
    with open(path, "r", newline="", encoding="utf-8") as file:
        reader = csv.DictReader(file)

        if reader.fieldnames is None:
            raise ValueError(f"CSV file is empty: {path}")

        columns = set(reader.fieldnames)
        missing_columns = REQUIRED_COLUMNS - columns

        if missing_columns:
            raise ValueError(
                f"CSV file {path} is missing required columns: "
                f"{sorted(missing_columns)}"
            )

        rows = []
        for row in reader:
            row["_source_file"] = str(path)
            rows.append(row)

        return rows


def read_summary_files(paths: List[Path]) -> List[Row]:
    all_rows = []

    for path in paths:
        if not path.exists():
            raise FileNotFoundError(f"Input file does not exist: {path}")

        if not path.is_file():
            raise ValueError(f"Input path is not a file: {path}")

        rows = read_summary_file(path)
        all_rows.extend(rows)

    return all_rows


def parse_float(value: Optional[str]) -> Optional[float]:
    if value is None:
        return None

    value = value.strip()

    if value == "":
        return None

    try:
        result = float(value)
    except ValueError:
        return None

    if math.isnan(result):
        return None

    return result


def parse_int(value: str) -> int:
    return int(value.strip())


def choose_error_value(row: Row) -> Optional[float]:
    """
    Chooses error value for error bars.

    Priority:
    1. sem
    2. std
    3. no error bars
    """
    sem = parse_float(row.get("sem"))
    if sem is not None:
        return sem

    std = parse_float(row.get("std"))
    if std is not None:
        return std

    return None


def make_line_label(row: Row) -> str:
    """
    Creates line label for plot legend.

    Example:
    python HashTable
    python dict
    cpp CppHashTable
    cpp boost::unordered_flat_map
    """
    language = row["language"].strip()
    implementation = row["implementation"].strip()

    return f"{language} {implementation}"


def make_safe_filename(case_name: str) -> str:
    """
    Converts case name to a safe filename.

    Examples:
    "1. Insert sequential" -> "insert_sequential"
    "2. Insert random"     -> "insert_random"
    "3. Get existing"      -> "get_existing"
    """
    name = case_name.strip().lower()[3:].strip().replace(" ", "_").replace("/", "_")
    return name


def group_rows_by_structure_and_case(rows: List[Row]) -> Dict[str, Dict[str, List[Row]]]:
    grouped = {}

    for row in rows:
        structure = row["structure"].strip()
        case = row["case"].strip()

        if structure not in grouped:
            grouped[structure] = {}

        if case not in grouped[structure]:
            grouped[structure][case] = []

        grouped[structure][case].append(row)

    return grouped


def group_case_rows_by_line(rows: List[Row]) -> Dict[str, List[Row]]:
    grouped = {}

    for row in rows:
        label = make_line_label(row)

        if label not in grouped:
            grouped[label] = []

        grouped[label].append(row)

    return grouped


def plot_case(case_name: str, case_rows: List[Row], output_base_path: Path) -> None:
    rows_by_line = group_case_rows_by_line(case_rows)

    fig, ax = plt.subplots(figsize=(12, 7))

    all_points = []

    for label, rows in sorted(rows_by_line.items()):
        points = []

        for row in rows:
            n = parse_int(row["n"])
            mean = parse_float(row["mean"])
            error = choose_error_value(row)

            if mean is None:
                continue

            points.append((n, mean, error))

        points.sort(key=lambda item: item[0])

        if not points:
            continue

        all_points.extend(points)

        x_values = [point[0] for point in points]
        y_values = [point[1] for point in points]
        error_values = [point[2] for point in points]

        has_errors = any(error is not None for error in error_values)

        if has_errors:
            y_errors = [error if error is not None else 0.0 for error in error_values]

            ax.errorbar( x_values, y_values, yerr=y_errors, marker="o", capsize=4, linewidth=2, label=label)
        else:
            ax.plot( x_values, y_values, marker="o", linewidth=2, label=label)

    if not all_points:
        return

    ax.set_title(case_name)
    ax.set_xlabel("N")
    ax.set_ylabel("Time, seconds")

    ax.grid(True, linestyle="--", alpha=0.5)
    ax.minorticks_on()

    ax.xaxis.set_minor_locator(AutoMinorLocator())
    ax.yaxis.set_minor_locator(AutoMinorLocator())

    ax.tick_params(axis="both", which="major", direction="in", length=6)
    ax.tick_params(axis="both", which="minor", direction="in", length=3)
    ax.legend(
        loc="upper left",
        bbox_to_anchor=(0.46, 0.945),
    )
    # ax.yaxis.set_tick_params(which="minor", left=False)

    # -----------------------------
    # Zoom inset
    # -----------------------------

    unique_n_values = sorted({point[0] for point in all_points})

    zoom_count = min(6, len(unique_n_values))
    zoom_n_values = set(unique_n_values[:zoom_count])

    zoom_points = [point for point in all_points if point[0] in zoom_n_values]

    if len(zoom_n_values) >= 2 and zoom_points:
        inset_ax = inset_axes( ax, width="42%", height="42%", loc="upper left", borderpad=3)

        for label, rows in sorted(rows_by_line.items()):
            points = []

            for row in rows:
                n = parse_int(row["n"])

                if n not in zoom_n_values:
                    continue

                mean = parse_float(row["mean"])
                error = choose_error_value(row)

                if mean is None:
                    continue

                points.append((n, mean, error))

            points.sort(key=lambda item: item[0])

            if not points:
                continue

            x_values = [point[0] for point in points]
            y_values = [point[1] for point in points]
            error_values = [point[2] for point in points]

            has_errors = any(error is not None for error in error_values)

            if has_errors:
                y_errors = [error if error is not None else 0.0 for error in error_values]

                inset_ax.errorbar(x_values, y_values, yerr=y_errors, marker="o", capsize=3, linewidth=1.5)
            else:
                inset_ax.plot(x_values, y_values, marker="o", linewidth=1.5)

        zoom_x_min = min(zoom_n_values)
        zoom_x_max = max(zoom_n_values)

        zoom_y_values = [point[1] for point in zoom_points]
        zoom_y_min = min(zoom_y_values)
        zoom_y_max = max(zoom_y_values)

        x_margin = (zoom_x_max - zoom_x_min) * 0.1
        y_margin = (zoom_y_max - zoom_y_min) * 0.15

        if x_margin == 0:
            x_margin = zoom_x_max * 0.05

        if y_margin == 0:
            y_margin = zoom_y_max * 0.05 if zoom_y_max != 0 else 0.001

        inset_ax.set_xlim(zoom_x_min - x_margin, zoom_x_max + x_margin)
        inset_ax.set_ylim(zoom_y_min - y_margin, zoom_y_max + y_margin)

        inset_ax.set_title("Small N", fontsize=9)
        inset_ax.grid(True, linestyle="--", alpha=0.4)
        inset_ax.tick_params(axis="both", labelsize=8)

        mark_inset( ax, inset_ax, loc1=2, loc2=4, fc="none", ec="0.5", linestyle="--")

    fig.tight_layout()

    output_base_path.parent.mkdir(parents=True, exist_ok=True)

    png_path = output_base_path.with_suffix(".png")

    fig.savefig(png_path, dpi=250)

    plt.close(fig)

def plot_all(rows: List[Row], plots_dir: Path) -> None:
    """
    Builds all plots grouped by structure and case.
    """
    grouped = group_rows_by_structure_and_case(rows)

    for structure, cases in sorted(grouped.items()):
        structure_dir = plots_dir / structure

        for case_name, case_rows in sorted(cases.items()):
            filename = make_safe_filename(case_name)
            output_base_path = structure_dir / filename

            plot_case(
                case_name=case_name,
                case_rows=case_rows,
                output_base_path=output_base_path,
            )

            print(f"Saved plots: {output_base_path.with_suffix('.png')}")



def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()

    parser.add_argument("input_files", nargs="+", type=Path)

    parser.add_argument("--plots-dir", type=Path, default=Path("plots"), help="Directory where plots will be saved. Default: plots")

    return parser.parse_args()


def main() -> None:
    args = parse_args()

    rows = read_summary_files(args.input_files)

    if not rows:
        print("No rows found in input files.")
        return

    plot_all(rows, args.plots_dir)


if __name__ == "__main__":
    main()