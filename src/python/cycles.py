from pathlib import Path
import matplotlib.pyplot as plt
import csv

data_dir = Path(__file__).resolve().parent.parent.parent / "data"
file = data_dir / "cycle_output.csv"

index = []
depth = []
cycle = []

with open(file) as f:
    reader = csv.reader(f)
    next(reader)  # skip header
    
    for row in reader:
        index.append(int(row[0]))
        depth.append(int(row[1]))
        cycle.append(int(row[2]))

plt.scatter(index, depth, c=cycle, s=10)
plt.xlabel("Index")
plt.ylabel("Depth")
plt.title("Collatz Cycle Structure")
plt.colorbar(label="Cycle length")
plt.show()