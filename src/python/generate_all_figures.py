#!/usr/bin/env python3
"""
generate_all_figures.py

Top-level script to generate all figures for the Collatz convergent-classes project.

This script invokes all individual figure generators:
- A047749, d_dm_ln_A047749, d_dm2_ln_A047749
- A047749_vs_A186009 (even & odd)
- Partial term sums
- Term coverage (Cn vs Bk)
- 31/43 best-fit
- Term group ratios

Supports both:
- Direct standalone execution
- Invocation from other scripts (if needed)
"""

# ======================================================================
# 1. Imports
# ======================================================================

from figure_utils import ensure_figdir
from pathlib import Path

from A047749 import generate_A047749
from d_dm_ln_A047749 import generate_d_dm_ln_A047749
from d_dm2_ln_A047749 import generate_d_dm2_ln_A047749
from A047749_vs_A186009_even import generate_A047749_vs_A186009_even
from A047749_vs_A186009_odd import generate_A047749_vs_A186009_odd
from term_brackets import generate_term_brackets
from Cn_vs_Bk import generate_Cn_vs_Bk
from term_group_ratios import generate_term_group_ratios
from best_fit_31_43 import generate_best_fit_31_43

# ======================================================================
# 2. Main figure-generation routine
# ======================================================================

def main():
    figures_dir = ensure_figdir()               # Path object
    data_dir = figures_dir.parent / "data"      # Path to repo-root/data
    data_dir.mkdir(parents=True, exist_ok=True) # ensure it exists

    print("=== Generating all Collatz figures ===")

    print("\nGenerating A047749 and logarithmic figures...")
    generate_A047749(figures_dir, data_dir)
    generate_d_dm_ln_A047749(figures_dir, data_dir)
    generate_d_dm2_ln_A047749(figures_dir, data_dir)

    print("\nGenerating A047749 vs A186009 for odd and even indices...")
    generate_A047749_vs_A186009_even(figures_dir, data_dir)
    generate_A047749_vs_A186009_odd(figures_dir, data_dir)

    print("\nGenerating partial term sums figures...")
    generate_term_brackets(figures_dir, data_dir)

    print("\nGenerating term coverage figure...")
    generate_Cn_vs_Bk(figures_dir, data_dir)

    print("\nGenerating 31/43 best-fit figure...")
    generate_best_fit_31_43(figures_dir, data_dir)

    print("\nGenerating term group ratios figures...")
    generate_term_group_ratios(figures_dir, data_dir)

    print("\n=== All figure generation complete ===")

# ======================================================================
# 3. Manual standalone execution
# ======================================================================

if __name__ == "__main__":
    main()
