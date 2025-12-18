# General Build Guidelines

This repository can be built directly if you are using VS Code by using the `Run Build Task...` available under
the `Terminal` top level menu function.  There are both Windows and Mac OSX build instructions defined in the
tasks.json using g++ and clang++ respectively.

The Windows + MSYS2 environment described in ENVIRONMENT.md is the reference platform under which all validated results were produced.

The executable can also be built using CMake with `cmake ..` from the build subdirectory:

    cd <repository>
    mkdir build
    cd build
    cmake -G "Unix Makefiles" ..
    make

Note that CMake support is provided for convenience; the Makefile-based workflow is the reference implementation used for validated results. The default make target is used to create the menu executable.

## GNU MP

GMP support is optional at build time but enabled by default and required for large-scale or OEIS-based computations.
The GNU MP libraries are included in the mingw (Minimalist GNU for Windows) but have to be built for some other operating systems.

## Doxygen and TeX

The documentation is Doxygen compliant and can be run directly by running `doxygen` from `<repo-path>/Collatz/build` directory
or by using the make target.

    make doxygen

Execute this command from the build directory as well.  The generated HTML documentation and LaTeX files will be placed under 
/build in separate directories.  To view generated docs point your browser to `file://<repo-path>/Collatz/build/html/index.html`.

On Mac, it might be something like:

    file:///users/<username>/<repo-path>/Collatz/build/html/index.html 

On Windows, it might be something like:

    file:///C:/Users/<username>/<repo-path>/Collatz/build/html/index.html 

Doxygen can also process TeX markup language.  As a consequence, the documentation includes a number of embedded formulae
that are written in LaTeX format so that they can be converted into TeX symbols (PNG files) during documentation generation.
If you do not have a TeX installation these formulae will not render for you and they will appear as is in the generated HTML.

There are several TeX distributions available including a special one for Mac called MacTeX.  TeX Live is recommended.
TeX Live is a complete distribution capable of producing professional articles, books, thesis, etc., but for code
documentation it is primarily about producing PDF output which looks good in generated documentation.

## Mac OSX Install

Install Visual Studio Code for Mac.  The git which comes with OSX should work just fine.

The MacTeX application installs in `/Applications/TeX/TeXShop.app` with all the goodies underneath the Contents
subdirectory.  The Doxygen TeX engine utilizes the executable epstopdf (Enhanced Postscript to PDF) in order
to convert any TeX markups in the code comments to PDF for use in the HTML pages.  TeX install includes epstopdf.

The installation also changes the default $PATH variable (defined in `/etc/paths`) so that all of the TeX
components are directly available from the shell.  It does this by adding the TeX directory under `/etc/paths.d`.
The contents of this file contain `/Library/TeX/texbin` which is symbolic link to where the MacTeX binaries are.

MacTeX also installs TeX Live which places contents in `/usr/local/texlive/<year>/bin/x86_64-darwin` with all the 
executables in the distribution.

The Mac OSX install requires you to build the GNU MP libraries if you want to take advantage of the ability to
generate arbitrarily large integers and utilize the OEIS sub-menu as discussed in README.md.

The easiest way to build the required GNU MP libraries for Mac is using GMP tarfile.  For example, gmp-6.2.1.tar.xz
can be downloaded here: [GMP Download](https://gmplib.org/#DOWNLOAD).  Documentation is available here: [GMP Manual](https://gmplib.org/gmp-man-6.2.1.pdf).

## Windows Install

First install Visual Studio Code if you want to utilize some of the project files which are included in the Collatz repository.  
It can be downloaded from [VS Code](https://code.visualstudio.com/download).  The 64 bit executable will look like `VSCodeUserSetup-x64-1.79.2.exe`.

Next install Git for Windows ([Git Downloads](https://git-scm.com/downloads)) if you don't already have it.  
For 64 bit machines it will be something like `Git-2.41.0-64-bit.exe`.  It will install in `C:\Program Files\Git` by default.  
Git Bash Here is also a really nice shell tool you can access from the file explorer right-click menu.  

The development environment on Windows 11 was MSys2 to install MinGW for gcc, g++ and gdb.  This can be obtained from [MSys2](https://www.msys2.org).  
This can be installed using an installer like `msys2-x86_64-20230526.exe`.  The installer defaults to `C:\msys64`.

After installing MSys2, open the MSys2 application.  Optionally you can right click on the icon in the Task bar and "Pin to taskbar".  

Run the following in MSys2 window to update the installation:

    pacman -Syuu

Run this repeatedly until there is nothing left to update.  

For 64-bit systems install the development toolchain:

    pacman -S --needed base-devel mingw-w64-x86_64-toolchain

This toolchain installs the GNU MP libraries and headers for you.

Add the mingw bin directory to the PATH, for example: `C:\msys64\mingw64\bin`.

Check that gcc, g++, gdb are available:

- **Git Bash / Linux-style shell:**

    ```bash
    $ which gcc
    /c/msys64/mingw64/bin/gcc
    ```

- **PowerShell:**

    ```powershell
    PS> Get-Command gcc
    ```

Install the CMake executable from [CMake](https://cmake.org/download/).  Verify it is in PATH:

- **Git Bash / Linux-style shell:**

    ```bash
    $ which cmake
    /c/Program Files/CMake/bin/cmake
    ```

- **PowerShell:**

    ```powershell
    PS> Get-Command cmake
    ```

For make, you can install it via Chocolatey: [Chocolatey](https://chocolatey.org/install).  
After installation:

    choco install make

Verify `make` is in PATH using a new Git Bash or PowerShell terminal.

Clone the Collatz repository from [GitHub](https://github.com/wbrassem/Collatz).

Open VS Code, ensure the C/C++ and CMake Tool extensions are installed.

If CMake complains about compilers, scan for kits:

    <CTRL-SHIFT-P> → "CMake: Scan for Kits"

Select the mingw64 GCC compiler, e.g.,

    GCC 13.1.0 x86_64-w64-mingw32 (mingw64)

Sometimes Windows defaults to nmake; switch to Unix Makefiles:

    cmake .. -G "Unix Makefiles"

Download Doxygen from [Doxygen](https://www.doxygen.nl/download.html) and add `C:\Program Files\doxygen\bin` to PATH.

Optionally install TeX Live 2023 from [TeX Live](https://www.tug.org/texlive/) which places binaries in `C:\texlive\2023\bin\windows`.

Install Graphviz to replace Ghostscript for LaTeX formula rendering: [Graphviz Download](https://graphviz.org/download/).  
Ensure the `bin` directory (e.g., `C:\Program Files\Graphviz\bin`) is added to PATH.

To build the executable:

    make

To generate HTML documentation:

    make doxygen

## Troubleshooting Doxygen Documentation

If your HTML documentation does not render LaTeX formulae or MathJax correctly, check the following:

### 1. Doxygen Version
- Ensure **Doxygen ≥ 1.15.0** is installed. Older versions may fail to process MathJax or LaTeX.
- Verify in PowerShell / Terminal:

```powershell
doxygen --version
```

### 2. PATH Setup
- Doxygen, TeX binaries, and Graphviz must be in your PATH.
- Windows example (PowerShell):

```powershell
Get-Command doxygen
Get-Command pdflatex
Get-Command dot
```

- macOS example:

```bash
which doxygen
which pdflatex
which dot
```

### 3. TeX Installation and Rendering
- Ensure a full TeX distribution is installed (TeX Live or MacTeX).
- HTML output uses an unaltered local MathJax v2 for rendering formulas, and is included in the repo.

### 4. Browser Cache
- Sometimes formulae appear as raw LaTeX due to caching. Clear the browser cache or open in a private window.

### 5. Rebuild Documentation
- If formulae fail, try a **clean rebuild**:

```bash
cd build
rm -rf html latex
make doxygen
```

- This ensures MathJax and LaTeX images are regenerated correctly.
