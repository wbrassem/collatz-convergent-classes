"""
term_group_ratios.py

Generate scatter plots of the ratio of current-to-previous group sums
for several term lengths (10–15). Each term produces a separate figure.

The data intentionally omits the first 500 points to focus on the flatter
portions of the data sets.  Each set goes between 500 and 80,000 points.
"""

# ======================================================================
# 1. Imports
# ======================================================================

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from pathlib import Path

from figure_utils import ensure_figdir, save_pdf

# ======================================================================
# 2. LaTeX-style formatting
# ======================================================================

mpl.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
})

# ======================================================================
# 3. Main generator
# ======================================================================

def generate_term_group_ratios(figures_dir: Path, data_dir: Path) -> list[Path]:
    """
    Generate scatter plots for group-ratio data for terms 10–15.

    Parameters
    ----------
    figures_dir : Path
        Path to the /figures directory.
    data_dir : Path
        Path to the /data directory.

    Returns
    -------
    list[Path]
        List of absolute paths to the generated PDF files.
    """

    # Threshold arrays from original script
    upper_thresholds = np.array([0.61, 0.59, 0.55, 0.52, 0.49, 0.462])
    lower_thresholds = np.array([0.53, 0.51, 0.50, 0.46, 0.44, 0.41])

    output_files = []

    # Loop over term values 10–15
    for idx, term in enumerate([10, 11, 12, 13, 14, 15]):
        infile = data_dir / f"group_ratios_{term}_500.txt"
        x, y = np.loadtxt(infile, delimiter=",", unpack=True)

        # Apply the corresponding upper threshold
        threshold = upper_thresholds[idx]
        exceed_mask = y > threshold  # can be used for highlighting if needed

        # ========== Plotting ==========
        fig, ax = plt.subplots(figsize=(16, 6))

        ax.scatter(x, y, s=4, marker=".", alpha=0.8)

        ax.set_title(
            rf"Scatter plot of ratio of current {term}-term sums "
            rf"to previous {term}-term sums",
            fontsize=20
        )
        ax.set_xlabel(
            rf"$n$ of $C(n)$ for consecutive {term}-term summations",
            fontsize=18
        )
        ax.set_ylabel(
            rf"Ratio of $\sum$ curr-{term}-terms / $\sum$ prev-{term}-terms",
            fontsize=18
        )
        ax.grid(True, alpha=0.3)

        # ========== Save output ==========
        outfile = save_pdf(fig, f"{term}_term_group_ratios", fig_dir=figures_dir)
        output_files.append(outfile)

    return output_files

# ======================================================================
# 4. Standalone execution
# ======================================================================

if __name__ == "__main__":
    figures_dir = ensure_figdir()
    data_dir = Path(__file__).parent.parent / "data"
    generate_term_group_ratios(figures_dir, data_dir)
