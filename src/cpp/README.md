# Collatz Conjecture — Computational Validation Code

This directory contains the C++ implementation used to empirically validate structural and statistical claims underlying a formal proof of the Collatz Conjecture.

This README describes *code-level intent and design decisions*.  
- For build instructions, see `INSTALL.md` at the repository root.  
- For the exact computational environment used to produce validated results, see `ENVIRONMENT.md`.

This project was written not as a proof per se, but to provide *exhaustive computational validation* over user-defined bounds of constructs that appear in the accompanying
formal proof. Establishing the Collatz Conjecture requires manipulating very large integers — far beyond what 64-bit or even 128-bit integers can handle.

Tabulating certain parameters, such as the histogram distribution of the number of distinct convergent orbits and their associated path lengths, quickly outgrew manual calculation. High-performance computation was necessary to process hundreds of billions of integers to verify that they behave exactly as predicted.

This also motivated the development of novel data structures to maintain efficiency without compromising rigor. One of the primary optimizations was the development of an array of binary trees, where each element of a tree represents a convergent orbit of a fixed length. This allowed paths with the same total number of factors of 2 (equal overall length) — but arranged in different orders — to be compared efficiently.

Orbits are represented as the exponent of 2 that divides an integer after a 3n+1 Collatz step, guaranteeing an even result. The struct holding orbit elements is part of a 64-bit union, which facilitates extremely rapid integer comparison operations between orbits with the same total factors of 2.

Patterns predicted theoretically do indeed appear when examined exhaustively over a chosen subset of integers, which is gratifying to observe computationally.

## Code Supporting Collatz Convergence Analysis

The `path` class holds the data structure including the number of factors required for convergence and the complete orbital sequence. It can also compute the equivalence class with a default length representing the minimum class length predicted to guarantee convergence.

The default `path` object uses a 64-bit signed integer (`int64_t`) as defined in `<stdint.h>`. The `path` class is a typedef specialization of the `t_path<>` template, which can be replaced with a 32-bit variant if needed:

```cpp
typedef t_path<int64_t> path;
```

The build also supports the GNU Multiple Precision (GMP) library. Multiple precision compilation is enabled by default; comment out the following line in `common.hpp` to disable GMP:

```cpp
#define gnu_mp // Comment this out to compile without GMP
```

Enabling GMP creates the `mp_path` type:

```cpp
typedef t_path<mpz_class> mp_path;
```

### Note on Tasks and Linking

If you have an old `tasks.json` for VSCode, it may include linker flags for GMP:

```
"-lgmpxx",
"-lgmp",
```

These are only necessary if you are building with GMP. Remove or comment them out if not using GMP.

## OEIS

The [Online Encyclopedia of Integer Sequences](https://oeis.org) (OEIS) was heavily used to develop this proof and validate C++ implementations of relevant integer sequences.

Within `oeis.cpp` and `oeis.hpp` are class implementations of several Collatz-related sequences. Some sequences require GMP, as their integers exceed standard precision limits.

OEIS references are treated as formal specifications: sequence definitions are implemented directly and validated against known initial terms before being used in larger computations.

## Non-goals

This codebase is not intended to:
- Serve as a standalone mathematical proof
- Perform symbolic reasoning or theorem proving
- Replace formal analytic arguments
- Optimize for general-purpose Collatz exploration

Its purpose is to validate, at scale, that predicted equivalence classes,
orbit structures, and convergence properties behave exactly as required by
the theory.
