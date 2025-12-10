import os
import matplotlib.pyplot as plt

# Default figures directory (relative to utils.py)
DEFAULT_FIG_DIR = os.path.join(os.path.dirname(__file__), "..", "figures")

def ensure_figdir(fig_dir=DEFAULT_FIG_DIR):
    """Ensure the figure directory exists and return the absolute path."""
    os.makedirs(fig_dir, exist_ok=True)
    return fig_dir

def save_pdf(fig, name, fig_dir=DEFAULT_FIG_DIR):
    """
    Save a matplotlib figure as a PDF into the specified directory.
    Returns the full path to the saved PDF.
    """
    path = os.path.join(ensure_figdir(fig_dir), f"{name}.pdf")
    fig.savefig(path, bbox_inches='tight')
    plt.close(fig)
    print(f"[OK] Saved {path}")
    return path
