"""
A047749.py

Plot the continuous representation of A047749 for
even and odd terms using the gamma function.

Callable function suitable for integration with generate_all_figures.py
and utils paths.
"""

# ======================================================================
# 1. Imports
# ======================================================================

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
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

def generate_A047749(figures_dir: Path, data_dir: Path = None) -> Path:
    """
    Generate the continuous-function plot for OEIS A047749 (even/odd forms)
    and save it as A047749.pdf in the provided figures directory.

    Parameters
    ----------
    figures_dir : Path
        Path to the /figures directory.
    data_dir : Path, optional
        Path to /data directory (not used here, included for consistency).

    Returns
    -------
    Path
        Full path to the resulting PDF file.
    """

    fig, ax = plt.subplots(figsize=(16, 9))

    # Generate 4001 datapoints between limits of m
    m = np.linspace(0, 10, 4001)

    # Define arguments for even gamma function
    enumer  = 3*m + 1
    edenom1 = 2*m + 2
    edenom2 = m + 1

    # Define arguments for odd gamma function
    onumer  = 3*m + 2
    odenom1 = 2*m + 2
    odenom2 = m + 2

    # Even
    ax.plot(
        m,
        gamma(enumer)/(gamma(edenom1)*gamma(edenom2)),
        label=r'Even: $\Gamma(3m+1)\,/\,(\Gamma(2m+2)\,\Gamma(m+1))$'
    )

    # Odd
    ax.plot(
        m,
        gamma(onumer)/(gamma(odenom1)*gamma(odenom2)),
        label=r'Odd: $\Gamma(3m+2)\,/\,(\Gamma(2m+2)\,\Gamma(m+2))$'
    )

    ax.grid()
    ax.set_xlim(0, 6)
    ax.set_ylim(0, 1000)
    ax.set_xlabel('m', fontsize=20)
    ax.set_ylabel('f(m)', fontsize=20)
    ax.legend(fontsize=18, loc='upper left')

    # ----------------------------
    # Save PDF
    # ----------------------------
    return save_pdf(fig, "A047749", fig_dir=figures_dir)

# ======================================================================
# 4. Allow manual standalone execution
# ======================================================================

if __name__ == "__main__":
    from figure_utils import ensure_figdir
    figures_dir = ensure_figdir()
    generate_A047749(figures_dir)
