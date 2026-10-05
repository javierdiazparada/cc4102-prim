#!/usr/bin/env python3
"""Construye promedios, tabla y doce graficos desde raw_results.csv."""

from __future__ import annotations

import argparse
import csv
import math
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", default="results/raw_results.csv")
    parser.add_argument("--plots-dir", default="plots")
    parser.add_argument("--summary", default="results/summary.csv")
    parser.add_argument("--constants", default="results/fit_constants.csv")
    return parser.parse_args()


def mean(values: list[float]) -> float:
    return sum(values) / len(values)


def scale_through_origin(theory: list[float], measured: list[float]) -> float:
    denominator = sum(value * value for value in theory)
    return sum(x * y for x, y in zip(theory, measured)) / denominator


def read_averages(path: Path) -> list[dict[str, float | str]]:
    grouped: dict[tuple[str, int, int, str], list[dict[str, str]]] = defaultdict(list)
    with path.open(newline="", encoding="utf-8") as handle:
        for row in csv.DictReader(handle):
            key = (row["series"], int(row["i"]), int(row["j"]), row["heap"])
            grouped[key].append(row)

    averages: list[dict[str, float | str]] = []
    for (series, i, j, heap), rows in sorted(grouped.items()):
        averages.append(
            {
                "series": series,
                "i": i,
                "j": j,
                "vertices": float(2**i),
                "edges": float(2**j),
                "heap": heap,
                "total_ms": mean([float(row["total_ms"]) for row in rows]),
                "mst_weight": mean([float(row["mst_weight"]) for row in rows]),
                "decrease_calls": mean(
                    [float(row["decrease_calls"]) for row in rows]
                ),
                "decrease_ms": mean(
                    [float(row["decrease_ns"]) / 1_000_000.0 for row in rows]
                ),
                "structural_operations": mean(
                    [float(row["structural_operations"]) for row in rows]
                ),
                "repetitions": float(len(rows)),
            }
        )
    return averages


def write_summary(path: Path, rows: list[dict[str, float | str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fields = list(rows[0].keys())
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)


def theory_total(row: dict[str, float | str]) -> float:
    vertices = float(row["vertices"])
    edges = float(row["edges"])
    if row["heap"] == "binomial":
        return edges * math.log2(vertices)
    return edges + vertices * math.log2(vertices)


def theory_amortized(row: dict[str, float | str]) -> float:
    calls = float(row["decrease_calls"])
    if row["heap"] == "binomial":
        return calls * math.log2(float(row["vertices"]))
    return calls


def save_plot(
    rows: list[dict[str, float | str]],
    x_name: str,
    y_name: str,
    theory_function,
    output: Path,
    title: str,
    x_label: str,
    y_label: str,
    y_limit: float,
) -> float:
    rows = sorted(rows, key=lambda row: float(row[x_name]))
    x = [float(row[x_name]) for row in rows]
    y = [float(row[y_name]) for row in rows]
    theory = [theory_function(row) for row in rows]
    constant = scale_through_origin(theory, y)
    fitted = [constant * value for value in theory]

    figure, axis = plt.subplots(figsize=(7, 4.5))
    axis.plot(x, y, marker="o", label="Medición promedio")
    axis.plot(x, fitted, linestyle="--", label=f"Cota escalada (c={constant:.3g})")
    axis.set_title(title)
    axis.set_xlabel(x_label)
    axis.set_ylabel(y_label)
    axis.set_ylim(0, y_limit)
    axis.grid(True, alpha=0.3)
    axis.legend()
    figure.tight_layout()
    figure.savefig(output, dpi=180)
    plt.close(figure)
    return constant


def main() -> int:
    args = arguments()
    rows = read_averages(Path(args.csv))
    if not rows:
        raise RuntimeError("the results CSV is empty")
    write_summary(Path(args.summary), rows)
    output = Path(args.plots_dir)
    output.mkdir(parents=True, exist_ok=True)
    constants: list[dict[str, str | float]] = []

    for series in ("A", "B"):
        series_rows = [row for row in rows if row["series"] == series]
        shared_limit = 1.08 * max(float(row["total_ms"]) for row in series_rows)
        x_name = "edges" if series == "A" else "vertices"
        x_label = "Cantidad de aristas" if series == "A" else "Cantidad de vértices"
        for heap in ("binomial", "fibonacci"):
            selected = [row for row in series_rows if row["heap"] == heap]
            constant = save_plot(
                selected,
                x_name,
                "total_ms",
                theory_total,
                output / f"total_{series}_{heap}.png",
                f"Costo total - serie {series} - cola {heap}",
                x_label,
                "Tiempo total promedio (ms)",
                shared_limit,
            )
            constants.append(
                {"series": series, "heap": heap, "metric": "total_ms", "constant": constant}
            )

    for series in ("C", "D"):
        series_rows = [row for row in rows if row["series"] == series]
        for metric, label, suffix in (
            ("decrease_ms", "Tiempo acumulado de decreaseKey (ms)", "time"),
            ("structural_operations", "Operaciones estructurales", "operations"),
        ):
            shared_limit = 1.08 * max(float(row[metric]) for row in series_rows)
            for heap in ("binomial", "fibonacci"):
                selected = [row for row in series_rows if row["heap"] == heap]
                constant = save_plot(
                    selected,
                    "decrease_calls",
                    metric,
                    theory_amortized,
                    output / f"amortized_{series}_{heap}_{suffix}.png",
                    f"decreaseKey - serie {series} - cola {heap}",
                    "Cantidad promedio de llamadas",
                    label,
                    shared_limit,
                )
                constants.append(
                    {"series": series, "heap": heap, "metric": metric, "constant": constant}
                )
    constants_path = Path(args.constants)
    constants_path.parent.mkdir(parents=True, exist_ok=True)
    with constants_path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=["series", "heap", "metric", "constant"])
        writer.writeheader()
        writer.writerows(constants)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
