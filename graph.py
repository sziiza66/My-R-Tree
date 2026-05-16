import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

# input files
FILES = [
    "benchWindowQ_p1000_q1000.txt",
    "benchWindowQ_p50_q100.txt",
    "benchNNQ_p1000_q1000_c3.txt",
    "benchNNQ_p50_q100_c10.txt",
    "benchNNQ_p50_q100_c3.txt",
    "benchInsert_p1000.txt",
    "benchInsert_p50.txt",
]

for filename in FILES:
    path = Path(filename)

    if not path.exists():
        print(f"skip missing file: {filename}")
        continue

    data = np.loadtxt(path)

    if data.ndim != 2 or data.shape[1] != 2:
        print(f"invalid format in {filename}")
        continue

    x = data[:, 0]
    y = data[:, 1]

    plt.figure(figsize=(8, 5))

    plt.plot(x, y, marker="o")

    plt.xlabel("dimension")
    plt.ylabel("time (ms)")

    # plt.xscale("log")
    # plt.yscale("log")

    plt.grid(True)

    plt.title(path.stem)

    out_name = path.with_suffix(".svg")

    plt.savefig(out_name, bbox_inches="tight")

    plt.close()

    print(f"saved {out_name}")