# Plot of the continuous function representation of OIES A047729
# for even and odd terms with n=2m and n=2m+1
# This function has no positive roots

import os
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
from scipy.special import gamma

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
# 3. Generate figure (example)
# ----------------------------
fig, ax = plt.subplots(figsize=(16, 9))

# Generate 4001 datapoints between limits of m
m = np.linspace(0, 10, 4001)

# Define arguments for even gamma function based on range for m
enumer  = 3*m + 1
edenom1 = 2*m + 2
edenom2 =   m + 1

# Define arguments for odd gamma function based on range for m
onumer  = 3*m + 2
odenom1 = 2*m + 2
odenom2 =   m + 2

# Even sequence
ax.plot(m, gamma(enumer)/(gamma(edenom1)*gamma(edenom2)),
        c='blue', label=r'Even: f(m)=$\Gamma$(3m+1)/($\Gamma$(2m+2)*$\Gamma$(m+1))')

# Odd sequence
ax.plot(m, gamma(onumer)/(gamma(odenom1)*gamma(odenom2)),
        c='red', label=r'Odd: f(m)=$\Gamma$(3m+2)/($\Gamma$(2m+2)*$\Gamma$(m+2))')

ax.grid()
ax.set_xlim(0, 6)
ax.set_ylim(0, 1000)
ax.set_xlabel('m', fontsize=20)
ax.set_ylabel('f(m)', fontsize=20)
ax.legend(fontsize=20, loc='upper left')

# ----------------------------
# 4. Save figure
# ----------------------------
pdf_file = os.path.join(figures_dir, "A047729.pdf")
plt.savefig(pdf_file, bbox_inches="tight")

# ----------------------------
# 5. Display figure interactively
# ----------------------------
plt.show()

print(f"Figure saved to: {pdf_file}")
