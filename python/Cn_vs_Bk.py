"""
Cn_vs_Bk.py

Plot a comparison of the natural logarithm of the covering numbers C(n)
versus the natural logarithm of the binomial coefficients
B(k) = C(k, k-2).

We load filtered covering-limit data, compute ln(C(n)) and ln(B(k)),
and compare their growth behavior.
"""

# ======================================================================
# 1. Imports
# ======================================================================

import numpy as np
from numpy import log as ln
from scipy.special import binom
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
    "text.latex.preamble": r"\usepackage{amsmath}",
})

# ======================================================================
# 3. Main figure-generation function
# ======================================================================

def generate_Cn_vs_Bk(figures_dir: str, data_dir: str) -> str:
    """
    Compare ln(C(n)) vs ln(B(k)), where B(k) = C(k, k-2).

    Parameters
    ----------
    figures_dir : str
        Absolute path to the /figures output directory.

    Returns
    -------
    str
        Absolute path to the generated PDF file.
    """

    # ----------------------------
    # Load covering-number data
    # ----------------------------
    data_file = figures_dir.replace("figures", "data") + "/Covering-Limits-80000.txt"
    y = np.loadtxt(data_file)

    # Filter out zero values (log undefined)
    y = y[y > 0]
    count_y = len(y)

    # ----------------------------
    # Binomial coefficients B(k) = C(k, k-2)
    # ----------------------------
    k_vals = np.arange(2, count_y)
    y_bin = binom(k_vals, k_vals - 2)

    # ----------------------------
    # Take logs
    # ----------------------------
    ln_y = ln(y)
    ln_y_bin = ln(y_bin)

    # ----------------------------
    # Plot
    # ----------------------------
    fig, ax = plt.subplots(figsize=(7, 5))

    ax.plot(ln_y_bin, label=r'$\ln B(k),\; B(k)=\binom{k}{k-2}$', linewidth=2)
    ax.plot(ln_y, label=r'$\ln C(n),\; \max(C(n)) \le \frac{2^{k}-1}{2^{k}}$', linewidth=2)

    ax.set_title(r'Natural logarithm comparison: $C(n)$ vs.\ $B(k)$')
    ax.set_xlabel(r'$k$')
    ax.set_ylabel(r'$\ln$ value')

    ax.legend(fontsize=12)
    ax.grid(True)

    # ----------------------------
    # Save PDF
    # ----------------------------
    return save_pdf(fig, "Cn_vs_Bk")

# ======================================================================
# 4. Standalone execution
# ======================================================================

if __name__ == "__main__":
    figures_dir = ensure_figdir()
    generate_Cn_vs_Bk(figures_dir)
