"""
A047749_vs_A186009_odd.py

Compare the discrete odd-index terms of OEIS A186009 with the
continuous odd-index representation of OEIS A047749.

We plot ln(A186009[2m+1]) (discrete) vs ln(A047749(2m+1)) (continuous).
Both sequences are convex and have no positive real roots.
"""

# ======================================================================
# 1. Imports
# ======================================================================

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from scipy.special import gamma

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
# 3. Main figure-generation function
# ======================================================================

def generate_A047749_vs_A186009_odd(figures_dir: str, data_dir: str) -> str:
    """
    Plot ln(A186009_odd) vs ln(A047749_odd_continuous).

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
    # Discrete odd terms from A186009
    # ----------------------------
    a186009_odd = np.array([
        1, 2, 7, 30, 173, 961, 8045, 51033,
        312455, 1900470, 13472296, 87986917,
        820236724, 5723030586, 38036848410, 248369601964
    ])

    m_discrete = np.arange(len(a186009_odd))
    ln_discrete = np.log(a186009_odd)

    # ----------------------------
    # Continuous representation A047749 for odd n = 2m+1
    # ----------------------------
    m = np.linspace(0, len(a186009_odd), 4001)

    numer  = 3*m + 2
    denom1 = 2*m + 2
    denom2 = m + 2

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
        label=r"Discrete A186009 (odd $n=2m+1$)"
    )

    # Continuous smooth curve
    ax.plot(
        m,
        ln_continuous,
        c='red',
        label=r"Continuous A047749 (odd $n=2m+1$)",
        linewidth=2
    )

    # Formatting
    ax.grid()
    ax.set_xlim(0, len(a186009_odd))
    ax.set_ylim(-5, 30)
    ax.set_xlabel("m", fontsize=20)
    ax.set_ylabel("ln value", fontsize=20)
    ax.legend(fontsize=18, loc="upper left")

    # ----------------------------
    # Save PDF
    # ----------------------------
    return save_pdf(fig, "A047749_vs_A186009_odd")

# ======================================================================
# 4. Allow manual standalone execution
# ======================================================================

if __name__ == "__main__":
    figures_dir = ensure_figdir()
    generate_A047749_vs_A186009_odd(figures_dir)
