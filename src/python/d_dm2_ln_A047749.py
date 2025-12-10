"""
d_dm2_ln_A047749.py

Plot the second derivative (1st-order polygamma) of ln(A047749)
for even and odd indices n = 2m and n = 2m+1.

Callable function suitable for integration with generate_all_figures.py
and utils paths.
"""

# ======================================================================
# 1. Imports
# ======================================================================

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from scipy.special import polygamma
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

def generate_d_dm2_ln_A047749(figures_dir: Path, data_dir: Path = None) -> Path:
    """
    Generate the PDF plot of the second derivative of ln(A047749)
    using first-order polygamma ψ¹ for both even and odd subsequences.

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

    # ----------------------------
    # Setup figure
    # ----------------------------
    fig, ax = plt.subplots(figsize=(16, 9))

    # Domain
    m = np.linspace(0, 6.5, 4001)

    # ----------------------------
    # Even terms ψ¹(3m+1), ψ¹(2m+2), ψ¹(m+1)
    # ----------------------------
    eterm1 = 3*m + 1
    eterm2 = 2*m + 2
    eterm3 = m + 1

    epoly1 =  9 * polygamma(1, eterm1)   # 3²
    epoly2 = -4 * polygamma(1, eterm2)   # 2²
    epoly3 = -1 * polygamma(1, eterm3)

    # ----------------------------
    # Odd terms ψ¹(3m+2), ψ¹(2m+2), ψ¹(m+2)
    # ----------------------------
    oterm1 = 3*m + 2
    oterm2 = 2*m + 2
    oterm3 = m + 2

    opoly1 =  9 * polygamma(1, oterm1)
    opoly2 = -4 * polygamma(1, oterm2)
    opoly3 = -1 * polygamma(1, oterm3)

    # ----------------------------
    # Plot
    # ----------------------------
    ax.plot(
        m,
        epoly1 + epoly2 + epoly3,
        label=r'Even: $3^{2}\psi^{(1)}(3m+1) - 2^{2}\psi^{(1)}(2m+2) - \psi^{(1)}(m+1)$'
    )

    ax.plot(
        m,
        opoly1 + opoly2 + opoly3,
        label=r'Odd: $3^{2}\psi^{(1)}(3m+2) - 2^{2}\psi^{(1)}(2m+2) - \psi^{(1)}(m+2)$'
    )

    # ----------------------------
    # Formatting
    # ----------------------------
    ax.grid()
    ax.set_xlim(0, 6)
    ax.set_ylim(0, 12)
    ax.set_xlabel('m', fontsize=20)
    ax.set_ylabel('f(m)', fontsize=20)
    ax.legend(fontsize=18, loc='upper right')

    # ----------------------------
    # Save PDF
    # ----------------------------
    return save_pdf(fig, "d_dm2_ln_A047749", fig_dir=figures_dir)

# ======================================================================
# 4. Allow manual standalone execution
# ======================================================================

if __name__ == "__main__":
    from figure_utils import ensure_figdir
    figures_dir = ensure_figdir()
    generate_d_dm2_ln_A047749(figures_dir)
