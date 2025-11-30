# Plot of the first derivative (0th order polygamma) of the natural logarithm of OIES A047729
# for even and odd terms with n=2m and n=2m+1
# This function has no positive roots and approaches ln(27/4) at infinity from below

import os
import numpy as np
from scipy.special import polygamma
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
# Assumes this script lives in python/ and figures/ is at repo root
figures_dir = os.path.join(os.path.dirname(__file__), "../figures")
os.makedirs(figures_dir, exist_ok=True)

# ----------------------------
# 3. Generate figure
# ----------------------------
fig, ax = plt.subplots(figsize=(16, 9))

# Generate datapoints
m = np.linspace(0, 40, 4001)

# Even terms
eterm1 = 3*m + 1
eterm2 = 2*m + 2
eterm3 = m + 1

epoly1 =  3 * polygamma(0, eterm1)
epoly2 = -2 * polygamma(0, eterm2)
epoly3 = -1 * polygamma(0, eterm3)

# Odd terms
oterm1 = 3*m + 2
oterm2 = 2*m + 2
oterm3 = m + 2

opoly1 =  3 * polygamma(0, oterm1)
opoly2 = -2 * polygamma(0, oterm2)
opoly3 = -1 * polygamma(0, oterm3)

# Plot
ax.plot(m, epoly1 + epoly2 + epoly3,
        c='blue',
        label=r'Even: f(m)=3$\psi^{0}$(3m+1)-2$\psi^{0}$(2m+2)-$\psi^{0}$(m+1)')

ax.plot(m, opoly1 + opoly2 + opoly3,
        c='red',
        label=r'Odd: f(m)=3$\psi^{0}$(3m+2)-2$\psi^{0}$(2m+2)-$\psi^{0}$(m+2)')

# Grid, axis limits, labels, legend
ax.grid()
ax.set_xlim(0, 40)
ax.set_ylim(-2, 2)
ax.set_xlabel('m', fontsize=20)
ax.set_ylabel('f(m)', fontsize=20)
ax.legend(fontsize=20, loc='lower right')

# ----------------------------
# 4. Save figure
# ----------------------------
pdf_file = os.path.join(figures_dir, "d_dm_ln_A047749.pdf")
plt.savefig(pdf_file, bbox_inches="tight")

# ----------------------------
# 5. Display interactively
# ----------------------------
plt.show()

print(f"Figure saved to: {pdf_file}")
