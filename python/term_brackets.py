"""
partial_term_sums.py

Generate scatter + histogram panels for partial term sums
spanning 1 / 2^k brackets, for term counts {11, 12, 13, 14}.

Each term produces its own figure:
    {term}_Term_Coverage.pdf

This script supports both:
- Direct standalone execution
- Invocation from generate_all_figures.py
"""

# ======================================================================
# 1. Imports
# ======================================================================

import os
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

from figure_utils import ensure_figdir, save_pdf


# ======================================================================
# 2. Configure LaTeX-style plots
# ======================================================================

mpl.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
})


# ======================================================================
# 3. Main generator function
# ======================================================================

def generate_term_brackets(figures_dir: str) -> list[str]:
    """
    Generate scatter + histogram figures for term counts {11,12,13,14}.

    Parameters
    ----------
    figures_dir : str
        Absolute path to /figures.

    Returns
    -------
    list[str]
        List of absolute paths to all generated PDFs.
    """

    # ----------------------------
    # Term counts to process
    # ----------------------------
    terms = [11, 12, 13, 14]

    # Data directory
    data_dir = os.path.join(os.path.dirname(__file__), "../data")

    outputs = []   # <- list of generated PDF paths

    # ----------------------------
    # Main loop
    # ----------------------------
    for term in terms:

        # Input file
        data_file = os.path.join(data_dir, f"{term}_term_brackets_csv.txt")

        # Load CSV data
        x, y = np.loadtxt(data_file, delimiter=',', unpack=True)

        count_y  = np.count_nonzero(y)
        mean_y   = np.mean(y)
        median_y = np.median(y)

        # Marker logic per original code
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
        # Panel A — Scatter
        #
        axis[0].scatter(x, y, s=sz, marker=mk)
        axis[0].set_title(
            f"a) Scatter plot of ratio of {term}-term sums / $2^k$",
            fontsize=16
        )
        axis[0].set_xlabel("n of C(n) spanning 1/$2^k$ bracket", fontsize=14)
        axis[0].set_ylabel(
            f"Ratio of $\\sum${term}-terms / $2^k$ bracket",
            fontsize=14
        )
        axis[0].grid(True)

        #
        # Panel B — Histogram
        #
        bins = np.linspace(0.90, 1.095, 40)
        counts, bins = np.histogram(y, bins)
        axis[1].stairs(counts, bins)
        axis[1].set_title(
            f"b) Histogram of {term}-term sums spanning 1/$2^k$ bracket",
            fontsize=16
        )
        axis[1].set_xlabel(
            f"Ratio of $\\sum${term}-terms / $2^k$ bracket",
            fontsize=14
        )

        axis[1].annotate(
            f"count\n{count_y}\n\nmean\n{mean_y:7.5f}\n\nmedian\n{median_y:7.5f}",
            xy=(0.05, 0.6),
            xycoords="axes fraction",
            fontsize=14
        )

        # ----------------------------
        # Save PDF (reuse the utility)
        # ----------------------------
        pdf_path = save_pdf(fig, f"{term}_term_brackets", figures_dir)
        outputs.append(pdf_path)

    return outputs


# ======================================================================
# 4. Manual standalone entry point
# ======================================================================

if __name__ == "__main__":
    figures_dir = ensure_figdir()
    out = generate_partial_term_sums(figures_dir)
    print("\nGenerated files:")
    for file in out:
        print("  ", file)
