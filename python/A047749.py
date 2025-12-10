"""
A047749.py

Plot the continuous represent of A047749) for
even and odd terms using the gamma function.

This version is refactored into a callable function suitable for
integration with generate_all_figures.py and utils paths.
"""

# ======================================================================
# 1. Imports
# ======================================================================

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
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

def generate_A047749(figures_dir: str):
    """
    Generate the continuous-function plot for OEIS A047729 (even/odd forms)
    and save it as A047749.pdf in the provided figures directory.

    Parameters
    ----------
    figures_dir : str
        Absolute path to the /figures directory.

    Returns
    -------
    str
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
    return save_pdf(fig, "A047749")

# ======================================================================
# 4. Allow manual standalone execution
# ======================================================================

if __name__ == "__main__":
    figures_dir = ensure_figdir()
    generate_A047749(figures_dir)
