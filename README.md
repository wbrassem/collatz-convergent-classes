# Collatz Conjecture Project

This repository contains the LaTeX source, figures, Python analysis scripts, and C++ code used in the research paper:

“Collatz Conjecture Anti-Cyclicity and Convergent Class Coverage”

The project includes:

- High-precision C++ orbit exploration code (optional GMP support)
- Python scripts for coverage analysis, ratio studies, and figure generation
- A reproducible pipeline that produces all figures in PDF format
- LaTeX source for the formal paper

------------------------------------------------------------

## 🛠️ Canonical Build (LaTeX)

The official build uses `latexmk` for consistency.  
From the repository root, run:

```bash
latexmk -pdf -interaction=nonstopmode -output-directory=build src/collatz_paper.tex
```

### VS Code Build Task (Alternative)

A VS Code task is provided so you can build the paper by pressing:

Ctrl+Shift+B (Windows/Linux)  
Cmd+Shift+B (Mac)

This produces:

```bash
build/collatz_paper.pdf
```


------------------------------------------------------------

## 📄 Outputs

- Final paper (PDF): build/collatz_paper.pdf
- Generated plots: figures/
- Raw data files: data/

------------------------------------------------------------

## 📂 Directory Structure

| Directory | Contents |
|----------|----------|
| `/src` | `collatz_paper.tex`, `references.bib` |
| `/python` | `*.py` analysis + figure scripts |
| `/.venv` | Python virtual environment |
| `/figures` | `*.pdf` generated plots |
| `/data` | `*.txt`, `*.csv` raw data |
| `/cpp` | `collatz.cpp`, `Makefile` |
| `/build` | `collatz_paper.pdf`, intermediates |

If preferred, `.venv` may be moved into `/python` and `.gitignore` updated.

------------------------------------------------------------

## ⚙️ Python Setup

Create and activate the virtual environment.

Mac/Linux:

```bash
python3 -m venv .venv
source .venv/bin/activate
```

Windows (PowerShell):

```powershell
python -m venv .venv
Set-ExecutionPolicy -Scope Process -ExecutionPolicy RemoteSigned
.\.venv\Scripts\Activate.ps1
```

Verify Python and pip:


```bash
which python
pip list
```

Install dependencies:

```bash
pip install -r requirements.txt
```

------------------------------------------------------------

## 🚀 C++ Collatz Executable

The `/cpp` directory contains a high-precision C++ implementation with optional GMP support.

Build the GMP version:

```bash
make gmp
```

Build the standard version:

```bash
make
```

Output binaries appear under `/cpp/build` unless the Makefile specifies otherwise.

------------------------------------------------------------

## 📊 Reproducible Figure Generation

Most figures are generated using:

```bash
python python/generate_all_figures.py
```


This script:

- Loads data from `/data`
- Generates all plots using Matplotlib with LaTeX text rendering
- Writes the output PDFs to `/figures`
- Uses consistent naming conventions such as `11_Term_Coverage.pdf`

------------------------------------------------------------

## 📄 License

To be determined.  
(MIT is recommended for open mathematical/scientific work.)

------------------------------------------------------------

## 🔍 Status

The repository is ready for its initial commit.  
Further development will focus on the paper contents, citations, and new figures.
