"""
figure_utils.py

Utility functions for figure management:
- ensure_figdir(): ensures the figure output directory exists
- save_pdf(): saves a matplotlib figure to PDF and returns its path

Intended for use by all figure-generation scripts in the Collatz project.
"""

# ======================================================================
# 1. Imports
# ======================================================================

import os
import matplotlib.pyplot as plt

# ======================================================================
# 2. Default figure directory
# ======================================================================

DEFAULT_FIG_DIR = os.path.join(os.path.dirname(__file__), "..", "figures")

# ======================================================================
# 3. Utility functions
# ======================================================================

def ensure_figdir(fig_dir=DEFAULT_FIG_DIR):
    """
    Ensure the figure directory exists and return its absolute path.

    Parameters
    ----------
    fig_dir : str
        Path to the desired figure directory.

    Returns
    -------
    str
        Absolute path to the ensured figure directory.
    """
    os.makedirs(fig_dir, exist_ok=True)
    return fig_dir


def save_pdf(fig, name, fig_dir=DEFAULT_FIG_DIR):
    """
    Save a matplotlib figure as a PDF into the specified directory.

    Parameters
    ----------
    fig : matplotlib.figure.Figure
        The figure to save.
    name : str
        Base filename (without extension).
    fig_dir : str
        Directory in which to save the PDF.

    Returns
    -------
    str
        Full path to the saved PDF.
    """
    path = os.path.join(ensure_figdir(fig_dir), f"{name}.pdf")
    fig.savefig(path, bbox_inches='tight')
    plt.close(fig)
    print(f"[OK] Saved {path}")
    return path
