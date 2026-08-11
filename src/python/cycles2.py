import csv
from pathlib import Path
import matplotlib.pyplot as plt
from collections import defaultdict

data_dir = Path(__file__).resolve().parent.parent.parent / "data"
file = data_dir / "cycle_output.csv"

stacks = defaultdict(list)

with open(file) as f:
    reader = csv.reader(f)
    next(reader)   # skip header
    
    for idx, depth, cyc in reader:
        idx = int(idx)
        depth = int(depth)
        cyc = int(cyc)
        stacks[idx].append((depth,cyc))

for idx, items in stacks.items():

    items.sort()  # order by depth
    depths = [d for d,_ in items]

    # vertical line showing stack
    plt.plot([idx]*len(depths), depths, linewidth=0.6)

cycle_colors = {
    53: "blue",
    41: "orange",
    359: "red",
    306: "purple",
    665: "green",
    15601: "black"
}

for idx, items in stacks.items():
    for depth, cyc in items:
        color = cycle_colors.get(cyc,"gray")
        plt.scatter(idx, depth, color=color, s=12)

plt.xlabel("Index")
plt.ylabel("Depth")
plt.title("Collatz Cycle Stack Structure")
plt.show()

