# Plot a comparison of the natural logarithm of the covering numbers C(n)
# versus the natural logarithm of the binomial coefficients B(k) = C(k, k-2)

import os
import numpy as np
from numpy import log as ln
from scipy.special import binom
import matplotlib as mpl
import matplotlib.pyplot as plt

# ----------------------------
# 1. Configure LaTeX-style fonts
# ----------------------------
mpl.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
    "text.latex.preamble": r"\usepackage{amsmath}"
})

# ----------------------------
# 2. Set up directories
# ----------------------------
script_dir = os.path.dirname(__file__)
data_dir = os.path.join(script_dir, "../data")
figures_dir = os.path.join(script_dir, "../figures")
os.makedirs(figures_dir, exist_ok=True)

data_file = os.path.join(data_dir, "Covering-Limits-80000.txt")

# ----------------------------
# 3. Load and filter data
# ----------------------------
y = np.loadtxt(data_file, unpack=True)

# Remove zero values to avoid log(0)
y = y[y > 0]
count_y = len(y)

mean_y = np.mean(y)
median_y = np.median(y)

# ----------------------------
# 4. Compute binomial coefficients B(k) = C(k, k-2)
# ----------------------------
k_vals = np.arange(2, count_y)
y_bin = binom(k_vals, k_vals - 2)

# ----------------------------
# 5. Compute logarithms
# ----------------------------
lny = ln(y)
lny_bin = ln(y_bin)

# ----------------------------
# 6. Generate figure
# ----------------------------
fig, ax = plt.subplots(figsize=(7, 5))

ax.plot(lny_bin, label=r'$\ln B(k),\; B(k)=\binom{k}{k-2}$')
ax.plot(lny, label=r'$\ln C(n),\; \max(C(n)) \le \frac{2^{k}-1}{2^{k}}$')

ax.set_title(r'Natural logarithm comparison: $C(n)$ vs. $B(k)$')
ax.set_xlabel(r'$k$')
ax.set_ylabel(r'$\ln$ value')

ax.legend(fontsize=12)
ax.grid(True)

# ----------------------------
# 7. Save PDF
# ----------------------------
pdf_file = os.path.join(figures_dir, "Cn_vs_Bk.pdf")
plt.savefig(pdf_file, bbox_inches="tight")

# ----------------------------
# 8. Display
# ----------------------------
plt.show()

print(f"Figure saved to: {pdf_file}")
