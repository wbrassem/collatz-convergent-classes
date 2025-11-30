# Plot of natural logarithm of even terms of discrete OEIS A186009 versus continuous
# representation of natural logarithm of OIES A047729 for even terms with n=2m
# These series have no positive roots and are convex

import os
import numpy as np
from scipy.special import gamma
import matplotlib as mpl
import matplotlib.pyplot as plt

# ----------------------------
# 1. Configure LaTeX-style fonts
# ----------------------------
mpl.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
})

# ----------------------------
# 2. Set up figures directory
# ----------------------------
figures_dir = os.path.join(os.path.dirname(__file__), "../figures")
os.makedirs(figures_dir, exist_ok=True)

# ----------------------------
# 3. Generate figure
# ----------------------------

# Discrete even terms from A186009
a186009 = np.array([1,1,3,12,85,476,2652,17637,108950,663535,5936673,39993895,
                    257978502,1899474678,12809477536,84141805077])

x_start = 0
x_end   = a186009.size
x = np.arange(x_start, x_end)
ln_a186009 = np.log(a186009)

fig, ax = plt.subplots(figsize=(16, 9))

# Scatter plot for discrete A186009
ax.scatter(x, ln_a186009, marker="o", s=20, label='A186009(n=2m)')

# Continuous representation using A047749
m = np.linspace(x_start, x_end, 4001)
numer  = 3*m + 1
denom1 = 2*m + 2
denom2 = m + 1

a047749 = gamma(numer) / (gamma(denom1) * gamma(denom2))
ln_a047749 = np.log(a047749)

# Plot continuous representation
ax.plot(m, ln_a047749, c='red', label='A047749(n=2m)')

# Grid, axis limits, labels
ax.grid()
ax.set_xlim(x_start, x_end)
ax.set_ylim(-5, 30)
ax.set_xlabel('m', fontsize=20)
ax.set_ylabel('f(m)', fontsize=20)

# Legend in lower right
ax.legend(fontsize=20, loc='upper left')

# ----------------------------
# 4. Save figure
# ----------------------------
pdf_file = os.path.join(figures_dir, "A047749_vs_A186009_2m.pdf")
plt.savefig(pdf_file, bbox_inches="tight")

# ----------------------------
# 5. Display interactively
# ----------------------------
plt.show()

print(f"Figure saved to: {pdf_file}")
