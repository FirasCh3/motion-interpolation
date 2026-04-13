#!/usr/bin/env python3

import argparse
import csv
import math
from bisect import bisect_left
from pathlib import Path


def read_csv_series(csv_path: Path, x_key: str, y_key: str):
    rows = []
    with csv_path.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(
                {
                    "time": float(row["time"]),
                    "x": float(row[x_key]),
                    "y": float(row[y_key]),
                }
            )
    if not rows:
        raise ValueError(f"No data rows found in {csv_path}")
    rows.sort(key=lambda row: row["time"])
    return rows


def interpolate_point_at_time(rows, target_time):
    times = [row["time"] for row in rows]

    if target_time <= times[0]:
        return rows[0]["x"], rows[0]["y"]
    if target_time >= times[-1]:
        return rows[-1]["x"], rows[-1]["y"]

    upper_index = bisect_left(times, target_time)
    lower_index = upper_index - 1
    start = rows[lower_index]
    end = rows[upper_index]
    span = end["time"] - start["time"]
    ratio = 0.0 if span == 0.0 else (target_time - start["time"]) / span

    x = start["x"] + ratio * (end["x"] - start["x"])
    y = start["y"] + ratio * (end["y"] - start["y"])
    return x, y


def clip_series_to_interval(rows, start_time, end_time):
    clipped = []

    start_x, start_y = interpolate_point_at_time(rows, start_time)
    clipped.append({"time": start_time, "x": start_x, "y": start_y})

    for row in rows:
        if start_time < row["time"] < end_time:
            clipped.append(row)

    end_x, end_y = interpolate_point_at_time(rows, end_time)
    clipped.append({"time": end_time, "x": end_x, "y": end_y})
    return clipped


def cumulative_arc_lengths(points):
    lengths = [0.0]
    for index in range(1, len(points)):
        prev_x, prev_y = points[index - 1]
        curr_x, curr_y = points[index]
        segment_length = math.hypot(curr_x - prev_x, curr_y - prev_y)
        lengths.append(lengths[-1] + segment_length)
    return lengths


def interpolate_point_by_length(points, lengths, target_length):
    if target_length <= 0.0:
        return points[0]
    if target_length >= lengths[-1]:
        return points[-1]

    upper_index = bisect_left(lengths, target_length)
    lower_index = upper_index - 1
    start_length = lengths[lower_index]
    end_length = lengths[upper_index]
    span = end_length - start_length
    ratio = 0.0 if span == 0.0 else (target_length - start_length) / span

    start_x, start_y = points[lower_index]
    end_x, end_y = points[upper_index]
    x = start_x + ratio * (end_x - start_x)
    y = start_y + ratio * (end_y - start_y)
    return x, y


def resample_by_arc_length(points, sample_count):
    if sample_count < 2:
        raise ValueError("sample_count must be at least 2")

    lengths = cumulative_arc_lengths(points)
    total_length = lengths[-1]

    # If the path has no movement, repeat the same point.
    if total_length == 0.0:
        return [points[0]] * sample_count

    step = total_length / (sample_count - 1)
    return [
        interpolate_point_by_length(points, lengths, index * step)
        for index in range(sample_count)
    ]


def percentile(values, fraction):
    sorted_values = sorted(values)
    index = (len(sorted_values) - 1) * fraction
    lower = math.floor(index)
    upper = math.ceil(index)
    if lower == upper:
        return sorted_values[lower]
    ratio = index - lower
    return sorted_values[lower] + ratio * (sorted_values[upper] - sorted_values[lower])


def nearest_distances(source_points, target_points):
    distances = []
    for source_x, source_y in source_points:
        best_distance = min(
            math.hypot(target_x - source_x, target_y - source_y)
            for target_x, target_y in target_points
        )
        distances.append(best_distance)
    return distances


def compute_distance_metrics(distances):
    if not distances:
        raise ValueError("No distances were computed.")

    count = len(distances)
    return {
        "mean_distance": sum(distances) / count,
        "rmse": math.sqrt(sum(distance * distance for distance in distances) / count),
        "p95": percentile(distances, 0.95),
        "max_distance": max(distances),
    }


def main():
    parser = argparse.ArgumentParser(
        description="Compare the shape of two 2D paths over their shared time interval."
    )
    parser.add_argument("--client1", default="client1_output.csv", help="Path to client 1 CSV.")
    parser.add_argument("--client2", default="client2_output.csv", help="Path to client 2 CSV.")
    parser.add_argument(
        "--samples",
        type=int,
        default=400,
        help="Number of equally spaced arc-length samples per curve.",
    )
    args = parser.parse_args()

    client1_rows = read_csv_series(Path(args.client1), "local_x", "local_y")
    client2_rows = read_csv_series(Path(args.client2), "remote_x", "remote_y")

    shared_start = max(client1_rows[0]["time"], client2_rows[0]["time"], 1.0)
    shared_end = min(client1_rows[-1]["time"], client2_rows[-1]["time"]) - 1.0
    if shared_start >= shared_end:
        raise ValueError("The two series do not have enough overlap after trimming to 1s through end-1s.")

    # First trim both paths to the same time window.
    client1_clipped = clip_series_to_interval(client1_rows, shared_start, shared_end)
    client2_clipped = clip_series_to_interval(client2_rows, shared_start, shared_end)

    # After clipping, ignore time and compare the path geometry only.
    # This measures spatial similarity between the two curve shapes,
    # not time alignment and not equal-progress matching.
    client1_points = [(row["x"], row["y"]) for row in client1_clipped]
    client2_points = [(row["x"], row["y"]) for row in client2_clipped]
    client1_resampled = resample_by_arc_length(client1_points, args.samples)
    client2_resampled = resample_by_arc_length(client2_points, args.samples)

    distances_a_to_b = nearest_distances(client1_resampled, client2_resampled)
    distances_b_to_a = nearest_distances(client2_resampled, client1_resampled)
    all_distances = distances_a_to_b + distances_b_to_a
    metrics = compute_distance_metrics(all_distances)

    print(f"Shared interval: {shared_start:.4f}s to {shared_end:.4f}s")
    print(f"Samples on curve A: {len(client1_resampled)}")
    print(f"Samples on curve B: {len(client2_resampled)}")
    print(f"Mean distance: {metrics['mean_distance']:.4f}")
    print(f"RMSE: {metrics['rmse']:.4f}")
    print(f"P95 nearest distance: {metrics['p95']:.4f}")
    print(f"Max nearest distance: {metrics['max_distance']:.4f}")


if __name__ == "__main__":
    main()
