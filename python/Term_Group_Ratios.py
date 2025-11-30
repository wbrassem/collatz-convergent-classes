# Plot of the ratio of current to previous group sums for a given term

import os
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

# -----------------------------------------------
# 1. Consistent LaTeX font / style
# -----------------------------------------------
mpl.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
})

# -----------------------------------------------
# 2. Directory setup
# -----------------------------------------------
base_dir = os.path.dirname(__file__)
data_dir = os.path.join(base_dir, "../data")
figures_dir = os.path.join(base_dir, "../figures")
os.makedirs(figures_dir, exist_ok=True)

# -----------------------------------------------
# 3. Threshold arrays (as in your original script)
# -----------------------------------------------
upper_thresholds = np.array([0.61, 0.59, 0.55, 0.52, 0.49, 0.462])
lower_thresholds = np.array([0.53, 0.51, 0.50, 0.46, 0.44, 0.41])

# -----------------------------------------------
# 4. Loop over terms 10 to 15
# -----------------------------------------------
for term in [10, 11, 12, 13, 14, 15]:

    # Load the group ratio data
    infile = os.path.join(data_dir, f"group_ratios_{term}_500.txt")
    x, y = np.loadtxt(infile, delimiter=",", unpack=True)

    # Apply the corresponding threshold (this uses the "upper" threshold from your original code)
    threshold = upper_thresholds[term - 10]
    indices = np.where(y > threshold)

    filtered_x = x[indices]
    filtered_y = y[indices]

    # Optional: print threshold exceeders (keeping for debugging; comment if unwanted)
    print(f"\n=== Term {term}: Values exceeding threshold {threshold} ===")
    for xx, yy in zip(filtered_x, filtered_y):
        print(f"({xx}, {yy})")

    # -------------------------------------------
    # 5. Generate plot
    # -------------------------------------------
    fig, ax = plt.subplots(figsize=(16, 6))

    ax.scatter(x, y, s=4, marker=".", alpha=0.8)

    ax.set_title(
        rf"Scatter plot of ratio of current {term}-term sums to previous {term}-term sums",
        fontsize=20
    )
    ax.set_xlabel(rf"$n$ of $C(n)$ for consecutive {term}-term summations", fontsize=18)
    ax.set_ylabel(
        rf"Ratio of $\sum$ curr-{term}-terms / $\sum$ prev-{term}-terms",
        fontsize=18
    )
    ax.grid(True, alpha=0.3)

    # -------------------------------------------
    # 6. Save figure
    # -------------------------------------------
    outfile = os.path.join(figures_dir, f"{term}_Term_Group_Ratios.pdf")
    plt.savefig(outfile, bbox_inches="tight")
    plt.close(fig)

    print(f"Saved: {outfile}")

print("\nAll done.")
