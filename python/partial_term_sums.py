# Create a series of scatter plots and histograms for partial term sums
# spanning 1 / 2^k brackets (range of terms from 11 to 14)

import os
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
# 2. Directories
# ----------------------------
# Script lives in python/, data and figures are at repo root
base_dir = os.path.dirname(__file__)
data_dir = os.path.join(base_dir, "../data")
figures_dir = os.path.join(base_dir, "../figures")

os.makedirs(figures_dir, exist_ok=True)

# ----------------------------
# 3. Terms to generate
# ----------------------------
terms = [11, 12, 13, 14]

# ----------------------------
# 4. Main loop
# ----------------------------
for term in terms:

    # Input file path
    data_file = os.path.join(data_dir, f"Partial-{term}-csv.txt")

    # Load CSV data
    x, y = np.loadtxt(data_file, delimiter=',', unpack=True)

    count_y = np.count_nonzero(y)
    mean_y  = np.mean(y)
    median_y = np.median(y)

    # Marker choice per original logic
    if term in (11, 14):
        mk = "o"
        sz = 12
    else:
        mk = "."
        sz = 2

    # ----------------------------
    # Create 2-panel figure
    # ----------------------------
    fig, axis = plt.subplots(1, 2, figsize=(14, 6))

    #
    # Panel A: Scatter
    #
    axis[0].scatter(x, y, s=sz, marker=mk)
    axis[0].set_title(f'a) Scatter plot of ratio of {term}-term sums / $2^k$', fontsize=16)
    axis[0].set_xlabel('n of C(n) spanning 1/$2^k$ bracket', fontsize=14)
    axis[0].set_ylabel(f'Ratio of $\\sum$ {term}-terms / $2^k$ bracket', fontsize=14)
    axis[0].grid(True)

    #
    # Panel B: Histogram
    #
    bins = np.linspace(0.90, 1.095, 40)
    counts, bins = np.histogram(y, bins)
    axis[1].stairs(counts, bins)
    axis[1].set_title(f'b) Histogram of {term}-term sums spanning 1/$2^k$ bracket', fontsize=16)
    axis[1].set_xlabel(f'Ratio of $\\sum$ {term}-terms / $2^k$ bracket', fontsize=14)

    # Annotate stats inside plot
    axis[1].annotate(
        f'count\n{count_y}\n\nmean\n{mean_y:7.5f}\n\nmedian\n{median_y:7.5f}',
        xy=(0.05, 0.6),
        xycoords='axes fraction',
        fontsize=14
    )

    # ----------------------------
    # Output filename
    # ----------------------------
    pdf_file = os.path.join(figures_dir, f"{term}_Term_Coverage.pdf")

    # Save figure
    plt.savefig(pdf_file, bbox_inches="tight")
    plt.close(fig)

    print(f"Saved: {pdf_file}")

print("All done.")
