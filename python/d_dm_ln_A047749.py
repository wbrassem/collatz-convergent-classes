"""
d_dm_ln_A047749.py

Plot the first derivative (0th-order polygamma) of ln(A047749)
for even and odd indices n = 2m and n = 2m+1.

This function has no positive roots and approaches ln(27/4)
from below as m → ∞.
"""

# ======================================================================
# 1. Imports
# ======================================================================

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
from scipy.special import polygamma

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

def generate_d_dm_ln_A047749(figures_dir):
    """
    Generate the PDF plot of d/dm ln(A047749) for even and odd indices.

    Parameters
    ----------
    figures_dir : str
        Absolute path to the /figures directory.

    Returns
    -------
    str
        Full path to the resulting PDF file.
    """

    # ----------------------------
    # Prepare figure
    # ----------------------------
    fig, ax = plt.subplots(figsize=(16, 9))

    # Domain
    m = np.linspace(0, 40, 4001)

    # ----------------------------
    # Even terms
    # ----------------------------
    eterm1 = 3*m + 1
    eterm2 = 2*m + 2
    eterm3 = m + 1

    epoly1 =  3 * polygamma(0, eterm1)
    epoly2 = -2 * polygamma(0, eterm2)
    epoly3 = -1 * polygamma(0, eterm3)

    # ----------------------------
    # Odd terms
    # ----------------------------
    oterm1 = 3*m + 2
    oterm2 = 2*m + 2
    oterm3 = m + 2

    opoly1 =  3 * polygamma(0, oterm1)
    opoly2 = -2 * polygamma(0, oterm2)
    opoly3 = -1 * polygamma(0, oterm3)

    # ----------------------------
    # Plot curves
    # ----------------------------
    ax.plot(
        m,
        epoly1 + epoly2 + epoly3,
        label=r'Even: $3\psi^{(0)}(3m+1) - 2\psi^{(0)}(2m+2) - \psi^{(0)}(m+1)$'
    )

    ax.plot(
        m,
        opoly1 + opoly2 + opoly3,
        label=r'Odd: $3\psi^{(0)}(3m+2) - 2\psi^{(0)}(2m+2) - \psi^{(0)}(m+2)$'
    )

    # ----------------------------
    # Formatting
    # ----------------------------
    ax.grid()
    ax.set_xlim(0, 40)
    ax.set_ylim(-2, 2)
    ax.set_xlabel('m', fontsize=20)
    ax.set_ylabel('f(m)', fontsize=20)
    ax.legend(fontsize=18, loc='lower right')

    # ----------------------------
    # Save PDF
    # ----------------------------
    return save_pdf(fig, "d_dm_ln_A047749")


# ======================================================================
# 4. Allow manual standalone execution
# ======================================================================

if __name__ == "__main__":
    figures_dir = ensure_figdir()
    generate_d_dm_ln_A047749(figures_dir)
