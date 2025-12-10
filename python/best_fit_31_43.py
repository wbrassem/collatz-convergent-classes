# Compare two 13-term series from CSV data, plot them with best-fit lines, and save as PDF.

import os
import csv
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

# ----------------------------
# 1. Configure LaTeX-style fonts
# ----------------------------
mpl.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
})

# ----------------------------
# 2. Set up directories
# ----------------------------
base_dir = os.path.dirname(__file__)
data_dir = os.path.join(base_dir, "../data")
figures_dir = os.path.join(base_dir, "../figures")
os.makedirs(figures_dir, exist_ok=True)

# ----------------------------
# 3. Load CSV data
# ----------------------------
csv_file = os.path.join(data_dir, "partial-13-csv-less-0.993.csv")

n_values, series_31, series_43 = [], [], []

with open(csv_file, newline="") as csvfile:
    reader = csv.reader(csvfile)
    next(reader)  # skip header
    for row in reader:
        n_values.append(int(row[0]))
        series_31.append(float(row[1]) if row[1] else np.nan)
        series_43.append(float(row[2]) if row[2] else np.nan)

# Filter out NaN values
n_31 = np.array([n for n, v in zip(n_values, series_31) if not np.isnan(v)])
v_31 = np.array([v for v in series_31 if not np.isnan(v)])
n_43 = np.array([n for n, v in zip(n_values, series_43) if not np.isnan(v)])
v_43 = np.array([v for v in series_43 if not np.isnan(v)])

# ----------------------------
# 4. Compute linear fits
# ----------------------------
m_31, b_31 = np.polyfit(n_31, v_31, 1)
m_43, b_43 = np.polyfit(n_43, v_43, 1)
fit_31 = m_31 * n_31 + b_31
fit_43 = m_43 * n_43 + b_43

# ----------------------------
# 5. Generate figure
# ----------------------------
fig, ax = plt.subplots(figsize=(16, 9))

# Scatter plots
ax.plot(n_31, v_31, "o", label="Series 31", alpha=0.7)
ax.plot(n_43, v_43, "s", label="Series 43", alpha=0.7)

# Best-fit lines
ax.plot(n_31, fit_31, label=rf"Best fit 31 ($m={m_31:.2e}$)", linestyle="-", color="blue", alpha=0.7)
ax.plot(n_43, fit_43, label=rf"Best fit 43 ($m={m_43:.2e}$)", linestyle="-", color="red", alpha=0.7)

# Labels, legend, grid
ax.set_xlabel("n", fontsize=20)
ax.set_ylabel(f'Ratio of $\\sum$ 13-terms / $2^k$ bracket', fontsize=20)
ax.set_title("Series 31 and 43 with Best-Fit Lines", fontsize=22)
ax.grid()
ax.legend(fontsize=16)

# ----------------------------
# 6. Save PDF
# ----------------------------
pdf_file = os.path.join(figures_dir, "31_43_best_fit.pdf")
plt.savefig(pdf_file, bbox_inches="tight")

# ----------------------------
# 7. Display interactively
# ----------------------------
plt.show()

print(f"Figure saved to: {pdf_file}")
print(f"Slope of best-fit line for Series 31: {m_31:.6e}")
print(f"Slope of best-fit line for Series 43: {m_43:.6e}")
