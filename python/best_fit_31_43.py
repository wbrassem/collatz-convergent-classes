"""
best_fit_31_43.py

Compute and plot best-fit lines for the 31-term and 43-term
outlier sequences found in partial-13 brackets.

Produces:
    best_fit_31_43.pdf

Supports both:
- Direct standalone execution
- Invocation from generate_all_figures.py
"""

# ======================================================================
# 1. Imports
# ======================================================================

import os
import csv
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

def generate_best_fit_31_43(figures_dir: str) -> list[str]:
    """
    Generate the best-fit comparison plot for the 31-term and
    43-term partial-13 outlier sequences.

    Parameters
    ----------
    figures_dir : str
        Absolute path to /figures.

    Returns
    -------
    list[str]
        List containing the generated PDF path.
    """

    # ----------------------------
    # Paths
    # ----------------------------
    base_dir = os.path.dirname(__file__)
    data_dir = os.path.join(base_dir, "../data")
    csv_file = os.path.join(data_dir, "partial-13-csv-less-0.993.csv")

    # ----------------------------
    # Load CSV data
    # ----------------------------
    n_values, series_31, series_43 = [], [], []

    with open(csv_file, newline="") as f:
        reader = csv.reader(f)
        next(reader)  # skip header
        for row in reader:
            n_values.append(int(row[0]))
            series_31.append(float(row[1]) if row[1] else np.nan)
            series_43.append(float(row[2]) if row[2] else np.nan)

    # Convert to arrays, filter NaN entries
    n_31 = np.array([n for n, v in zip(n_values, series_31) if not np.isnan(v)])
    v_31 = np.array([v for v in series_31 if not np.isnan(v)])

    n_43 = np.array([n for n, v in zip(n_values, series_43) if not np.isnan(v)])
    v_43 = np.array([v for v in series_43 if not np.isnan(v)])

    # ----------------------------
    # Compute best-fit lines
    # ----------------------------
    m_31, b_31 = np.polyfit(n_31, v_31, 1)
    m_43, b_43 = np.polyfit(n_43, v_43, 1)

    fit_31 = m_31 * n_31 + b_31
    fit_43 = m_43 * n_43 + b_43

    # ----------------------------
    # Create figure
    # ----------------------------
    fig, ax = plt.subplots(figsize=(14, 8))

    # Scatter plots
    ax.plot(n_31, v_31, "o", alpha=0.7, label="Series 31")
    ax.plot(n_43, v_43, "s", alpha=0.7, label="Series 43")

    # Best-fit lines
    ax.plot(
        n_31, fit_31,
        linestyle="-",
        color="blue",
        alpha=0.8,
        label=rf"Best fit 31 ($m={m_31:.2e}$)"
    )

    ax.plot(
        n_43, fit_43,
        linestyle="-",
        color="red",
        alpha=0.8,
        label=rf"Best fit 43 ($m={m_43:.2e}$)"
    )

    # Labels and titles
    ax.set_xlabel("n", fontsize=18)
    ax.set_ylabel(r"Ratio of $\sum$ 13-terms / $2^k$ bracket", fontsize=18)
    ax.set_title("Series 31 vs 43 with Best-Fit Lines", fontsize=20)
    ax.grid(True)
    ax.legend(fontsize=14)

    # ----------------------------
    # Save PDF
    # ----------------------------
    pdf_path = save_pdf(fig, "best_fit_31_43", figures_dir)

    return [pdf_path]


# ======================================================================
# 4. Manual standalone entry point
# ======================================================================

if __name__ == "__main__":
    figures_dir = ensure_figdir()
    out = generate_best_fit_31_43(figures_dir)

    print("\nGenerated files:")
    for f in out:
        print("  ", f)
