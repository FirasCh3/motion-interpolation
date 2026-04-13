#!/usr/bin/env python3

import argparse
import csv
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def read_positions(csv_path: Path):
    rows = []
    with csv_path.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(
                {
                    "time": float(row["time"]),
                    "local_x": float(row["local_x"]),
                    "local_y": float(row["local_y"]),
                    "remote_x": float(row["remote_x"]),
                    "remote_y": float(row["remote_y"]),
                }
            )
    if not rows:
        raise ValueError(f"No data rows found in {csv_path}")
    return rows


def main():
    parser = argparse.ArgumentParser(
        description="Plot client 1 local trajectory with client 2 remote trajectory."
    )
    parser.add_argument(
        "--client1",
        default="client1_output.csv",
        help="Path to client 1 CSV output.",
    )
    parser.add_argument(
        "--client2",
        default="client2_output.csv",
        help="Path to client 2 CSV output (validated for convenience).",
    )
    parser.add_argument(
        "--output",
        default="client1_paths.png",
        help="Path to save the generated plot image.",
    )
    args = parser.parse_args()

    client1_path = Path(args.client1)
    client2_path = Path(args.client2)
    output_path = Path(args.output)

    client1_rows = read_positions(client1_path)
    client2_rows = read_positions(client2_path)

    local_x = [row["local_x"] for row in client1_rows]
    local_y = [row["local_y"] for row in client1_rows]
    remote_x = [row["remote_x"] for row in client2_rows]
    remote_y = [row["remote_y"] for row in client2_rows]

    plt.figure(figsize=(10, 8))
    plt.plot(local_x, local_y, label="Client 1 Local", linewidth=2.2, color="#2ecc71")
    plt.plot(remote_x, remote_y, label="Client 2 Remote", linewidth=2.0, color="#e74c3c", alpha=0.9)
    plt.scatter(local_x[0], local_y[0], color="#2ecc71", s=40)
    plt.scatter(remote_x[0], remote_y[0], color="#e74c3c", s=40)
    plt.title("Client 1 Local vs Client 2 Remote Path")
    plt.xlabel("X Position")
    plt.ylabel("Y Position")
    plt.legend()
    plt.grid(True, alpha=0.25)
    plt.axis("equal")
    plt.tight_layout()
    plt.savefig(output_path, dpi=160)
    print(f"Saved plot to {output_path}")


if __name__ == "__main__":
    main()
