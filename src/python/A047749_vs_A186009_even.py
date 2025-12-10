"""
A047749_vs_A186009_even.py

Compare the discrete even-index terms of OEIS A186009 with the
continuous even-index representation of OEIS A047749.

We plot ln(A186009[2m]) (discrete) vs ln(A047749(2m)) (continuous).
"""

# ======================================================================
# 1. Imports
# ======================================================================

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from scipy.special import gamma
from pathlib import Path

from figure_utils import save_pdf

# ======================================================================
# 2. Configure LaTeX-style plots
# ======================================================================

mpl.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
})

# ======================================================================
# 3. Main figure-generation function
# ======================================================================

def generate_A047749_vs_A186009_even(figures_dir: Path, data_dir: Path = None) -> Path:
    """
    Plot ln(A186009_even) vs ln(A047749_even_continuous).

    Parameters
    ----------
    figures_dir : Path
        Path to the /figures output directory.
    data_dir : Path, optional
        Path to /data directory (not used here, included for consistency).

    Returns
    -------
    Path
        Full path to the generated PDF file.
    """

    # ----------------------------
    # Discrete even terms from A186009
    # ----------------------------
    a186009_even = np.array([
        1, 1, 3, 12, 85, 476, 2652, 17637,
        108950, 663535, 5936673, 39993895,
        257978502, 1899474678, 12809477536, 84141805077
    ])

    m_discrete = np.arange(len(a186009_even))
    ln_discrete = np.log(a186009_even)

    # ----------------------------
    # Continuous representation A047749 for even n = 2m
    # ----------------------------
    m = np.linspace(0, len(a186009_even), 4001)

    numer  = 3*m + 1
    denom1 = 2*m + 2
    denom2 = m + 1

    a047749 = gamma(numer) / (gamma(denom1) * gamma(denom2))
    ln_continuous = np.log(a047749)

    # ----------------------------
    # Plotting
    # ----------------------------
    fig, ax = plt.subplots(figsize=(16, 9))

    # Discrete scatter
    ax.scatter(
        m_discrete,
        ln_discrete,
        s=25,
        label=r"Discrete A186009 (even $n=2m$)"
    )

    # Continuous smooth curve
    ax.plot(
        m,
        ln_continuous,
        c='red',
        label=r"Continuous A047749 (even $n=2m$)",
        linewidth=2
    )

    # Formatting
    ax.grid()
    ax.set_xlim(0, len(a186009_even))
    ax.set_ylim(-5, 30)
    ax.set_xlabel("m", fontsize=20)
    ax.set_ylabel("ln value", fontsize=20)
    ax.legend(fontsize=18, loc="upper left")

    # ----------------------------
    # Save PDF
    # ----------------------------
    return save_pdf(fig, "A047749_vs_A186009_even", fig_dir=figures_dir)

# ======================================================================
# 4. Allow manual standalone execution
# ======================================================================

if __name__ == "__main__":
    from figure_utils import ensure_figdir
    figures_dir = ensure_figdir()
    generate_A047749_vs_A186009_even(figures_dir)
