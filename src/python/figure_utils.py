"""
figure_utils.py

Utility functions for figure management:
- ensure_figdir(): ensures the figure output directory exists
- save_pdf(): saves a matplotlib figure to PDF and returns its path

Paths are now computed relative to the repository root.
"""

# ======================================================================
# 1. Imports
# ======================================================================

from pathlib import Path
import matplotlib.pyplot as plt

# ======================================================================
# 2. Repository root and default figure directory
# ======================================================================

# Compute repo root dynamically: src/python -> src -> repo root
REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_FIG_DIR = REPO_ROOT / "figures"

# ======================================================================
# 3. Utility functions
# ======================================================================

def ensure_figdir(fig_dir=DEFAULT_FIG_DIR):
    """
    Ensure the figure directory exists and return its absolute path.

    Parameters
    ----------
    fig_dir : Path
        Path to the desired figure directory.

    Returns
    -------
    Path
        Absolute path to the ensured figure directory.
    """
    fig_dir.mkdir(parents=True, exist_ok=True)
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
    fig_dir : Path
        Directory in which to save the PDF.

    Returns
    -------
    Path
        Full path to the saved PDF.
    """
    path = ensure_figdir(fig_dir) / f"{name}.pdf"
    fig.savefig(path, bbox_inches='tight')
    plt.close(fig)
    print(f"[OK] Saved {path}")
    return path
