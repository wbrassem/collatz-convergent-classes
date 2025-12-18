# ENVIRONMENT.md

This document records the computational and documentation environment used to
build, run, and generate documentation for the Global Descent Convergent Classes
Collatz project.

The goal is reproducibility, not immutability:
- Tools are specified at the semantic level where they affect results.
- Observed versions are recorded where behavior or mathematics may differ.
- Transitive or incidental dependencies are intentionally omitted.

--------------------------------------------------------------------------

## Windows Environment (Primary Development Platform)

### System Information

- OS: Windows 11 (64-bit)
- Processor: 12th Gen Intel(R) Core(TM) i5-12450H @ 2.00 GHz
- Architecture: x64-based processor
- Installed RAM: 16.0 GB
- System type: 64-bit operating system

--------------------------------------------------------------------------

### Development Toolchain (Windows)

MSYS2 / MinGW-w64
- Host system:
  - Windows NT kernel: 10.0.26100
- MSYS2 shell:
  - uname: MSYS_NT-10.0-26100
- MSYS2 runtime:
  - msys2-runtime 3.5.4-8
- Package manager:
  - pacman 6.1.0-10
- Toolchain: MinGW-w64 (x86_64)
- Compiler:
  - gcc: 14.2.0
  - g++: 14.2.0
- Debugger:
  - gdb: 16.1

Installed via MSYS2 pacman using the MinGW-w64 (mingw64) toolchain.

--------------------------------------------------------------------------

### GNU Multiple Precision Arithmetic Library (GMP)

- Version: 6.3.0
- Distribution: MSYS2 mingw-w64
- Header: C:\msys64\mingw64\include\gmp.h
- Static library: C:\msys64\mingw64\lib\libgmp.a
- Verified via __GNU_MP_VERSION macros

GMP is used for arbitrary-precision integer arithmetic and directly affects
numerical correctness for large Collatz iterations and OEIS-derived sequences.

--------------------------------------------------------------------------

### Build System

- CMake: 3.26.4
- Make: GNU Make 4.3
- Generator: Unix Makefiles (explicitly selected on Windows)

--------------------------------------------------------------------------

### Python Environment

- Python version: 3.13 (observed patch: 3.13.9)
- Virtual environment: /src/.venv

Declared dependencies (requirements.txt):

- numpy
- scipy
- matplotlib

Resolved package versions (Windows):

- numpy: 2.3.5
- scipy: 1.16.3
- matplotlib: 3.10.7

Only top-level numerical libraries are recorded; transitive dependencies are
intentionally omitted.

Exact wheel builds may vary by platform, but numerical behavior is expected to remain stable across minor updates of each library.

--------------------------------------------------------------------------

### Documentation Toolchain

Doxygen

- Doxygen version: 1.15.0
- Output format: HTML only
- LaTeX PDF generation: Disabled

Doxygen is used to generate API documentation from annotated C++ source code.

--------------------------------------------------------------------------

### Math Rendering (MathJax)

- Math rendering engine: MathJax
- Integration mode: HTML (JavaScript)
- Source:
  - Local copy preferred to avoid CDN blocking
  - Version: 2.7.9
- Doxygen configuration:
  USE_MATHJAX = YES
  MATHJAX_RELPATH = ../../../third_party/MathJax-2.7.9

**Note:** MathJax 3.x introduces a new module structure incompatible with Doxygen's HTML generation.  
To ensure formulas render correctly in the documentation, version 2.7.9 is used.

--------------------------------------------------------------------------

Graphviz

- Graphviz version: 14.1.0 (64-bit)
- Executable: dot
- Usage:
  - Class diagrams
  - Dependency graphs

Graphviz replaces legacy Ghostscript-based pipelines and is required for diagram
generation when HAVE_DOT = YES.

--------------------------------------------------------------------------

### Versioning Philosophy

This environment represents a known-good configuration under which:

- Numerical results are validated
- Documentation renders correctly
- Builds succeed without warnings

Future environments may differ, but deviations should be documented explicitly
when behavior, results, or rendered output changes.

--------------------------------------------------------------------------
