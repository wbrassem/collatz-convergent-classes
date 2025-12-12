# General Build Guidelines

This repository can be built directly if you are using VS Code by using the `Run Build Task...` available under
the `Terminal` top level menu function.  There are both Windows and Mac OSX build instructions defined in the
tasks.json using g++ and clang++ respectively.

The exectuable can also be built using CMake with `cmake ..` from the build subdirectory:

    cd <repository>
    mkdir build
    cd build
    cmake ..
    make

The default make target is used to create the menu executable.

## GNU MP

The build can optionally include GNU Multiple Precision integers - this isn't strictly required, but it greatly enhances
what you'll get out of this.  The GNU MP libraries are included in the mingw (Minimalist GNU for Windows) but have to built
for some other operating systems.

## Doxygen and TeX

The documentation is Doxygen compliant and can be run directly by running `doxygen` from `<repo-path>/Collatz/build` directory
or by using the make target.

    make doxygen
    
Execute this command from tbe build directory as well.  The generated HTML documentation and LaTeX files with be placed under 
/build in separate directories.  To view generated docs point your browser to `file://<repo-path>/Collatz/build/html/index.html` file.

On Mac, it might be something like:

    file:///users/<username>/<repo-path>/Collatz/build/html/index.html 

On Windows, it might be something like:

    file:///C:/Users/<username>/<repo-path>/Collatz/build/html/index.html 

Doxygen can also process TeX markup language.  As a consequence, the documentation includes a number of embedded formulae
that are written in LaTeX format so that they can be converted into TeX symbols (PNG files) during documentation generation.
If you do not have a TeX installation these formulae will not render for you and they will appear as is in the generated HTML.

There are several TeX distributions available including a special one for Mac called MacTeX.  TeX Live is recommended.
TeX Live is a complete distribution capable of producing professional articles, books, thesis, etc. but for code
documentation it primarly about producing PDF output which looks good in generated documentation.

## Mac OSX Install

Install Visual Studio Code for Mac.  The git which comes with OSX should work just fine.

The MacTeX application installs in `/Applications/TeX/TeXShop.app` with all the goodies underneath the Contents
subdirectory.  The Doxygen TeX engine utilizes the exectutable epstopdf (Enhanced Postscript to PDF) in order
to convert any TeX markups in the code comments to PDF for use in the HTML pages.  TeX install includes epstopdf.

The installation also changes the default $PATH variable (defined in `/etc/paths`) so that the all of the TeX
components are directly available from the shell.  It does this by adding the TeX directory under `/etc/paths.d`.
The contents of this file contain /Library/TeX/texbin which is symbolic link to where the MacTeX binaries are.

MacTeX also installs TeX Live which places contents in `/usr/local/texlive/<year>/bin/x86_64-darwin` with all the 
executables in the distribution.

The Mac OSX install requires you to build the GNU MP libraries if you want to take advantage of the abilty to
generate arbitrarily large integers and utilize the OEIS sub-menu as discussed in README.md.

The easiest way I found to build the required GNU MP libraries for Mac is using GMP tarfile.  As an example this
was built using gmp-6.2.1.tar.xz.  You can find the link to the tarfile here: https://gmplib.org/#DOWNLOAD.
There is also a link to a very helpful PDF file with the documentation: https://gmplib.org/gmp-man-6.2.1.pdf.

## Windows Install

First install Visual Studio Code if you want to utilize some of the project files which are are included in the Collatz repository.  It can
be downloaded from https://code.visualstudio.com/download.  The 64 bit executable will look like VSCodeUserSetup-x64-1.79.2.exe.  By default,
VS Code is installed locally under `C:\Users\<user-id>\AppData\Local\Programs\Microsoft VS Code\`.  The "Open with Code" options are helpful
if you want the abiity to right-click on a file to open with VS Code.

Next install Git for Windows (https://git-scn.com/downlaods) if you don't already have it.  For 64 bit machines it will be something like
Git-2.41.0-64-bit.exe.  It will install in `C:\Program Files\Git` by default.  Git Bash Here is also a really nice shell tool you can access
from the file explorer right-click menu.  If you plan to utilize Visual Studio Code as the IDE then you can optionally select VS Code as the
default editor for Git.

The development environment on Windows 11 was MSys2 to install MinGW for gcc, g++ and gdb.  This can be obtained from https://www.msys2.org.
This can be installed using an installer like msys2-x86_64-20230526.exe.  The installer defaults to `C:\msys64` which is a good choice because
the path doesn't have any annoying whitespace such as that in the middle of "Program Files" type paths.

After installing MSysy2, find the msys2 application in `C:\msys64` and open it if it doesn't automatically.  This will open a terminal window.
Optionally you can right click on the icon in the Task ban and "Pin to taskbar".  The pacman package manager is used at this point.

Run the following in msys2 window to update the installation:

    pacman -Syuu

Run this repeatedly until there is nothing left to update.  

By default, the MSys2 32 and 64 bit mingw installations do not include the development kit.  You are going to want this.  Here's how to go
about it for 64-bit systems:

    pacman -S --needed base-devel mingw-w64-x86_64-toolchain

This toolchain is really nice because it installs the GNU MP libraries and headers for you.

The only thing you need to do now is add the mingw directory to the PATH settings.  Open settings and search for "environment" under
"Find a setting" and select "Edit the system enviroment variables".  The System Properties should pop up then click "Environment Variables".
Select Path and then "Edit...". Click New to Add the Msys mingw bin director which will be something like `C:\msys64\mingw64\bin` for 64-bit
host machines.  Then click Ok (repeatedly) to close out Settings.

To double check all of this open a new terminal (to pick up the new Path settings) and check that gcc, g++ and gdb all work fine using
"which" Linux commands.  If you installed GitBash then this is a great time to use it as it will tell you if the components you need are
in the PATH or not.  The response should look something like this:

    $ which gcc
    /c/msys64/mingw64/bin/gcc

Install the CMake executable program from https://cmake.org/download/.  The installer will be something like cmake-3.27.0-rc4-windows-x86_64.msi
for 64 bit machines.  You can elect for this to update the Path environment variable for you, but double check to be sure it worked because
sometimes it didn't take.  If it is not there then add `C:\Program Files\CMake\bin` to the PATH (or wherever you installed CMake).  Again verify
that cmake is available using a new GitBash window:

    $ which cmake
    /c/Program Files/CMake/bin/cmake

For make itself, there are a number of ways, but I opted for the Chocolatey Windows package manager.  The trick here is you need to install it
using adminstrator level so you have to open your terminal window with elevated provileges in order to succeed.  Search for PowerShell using
"terminal" and should be found.  The default execution policy is Restricted:

    PS C:\Users\{id}> Get-ExecutionPolicy
    Restricted

This isn't sufficient for Chocolatey.  The first step is to start PowerShell with "Run as administrator".  There are detailed instructions
at https://chocolatey.org/install to set the execution policy in order to install the package manager - including a convenient cut and paste
command you run in the shell to allow Chocolatey to install.  As Administrator, you should be able to run "Set-ExecutionPolicy AllSigned".

Cut and paste the long execution policy as given at the chololatey link above.  Chcolatey gets installed in `C:\ProgramData\chocolatey\bin`.
Check that the install worked:

    choco -?

This should return a lot of output.  To actually install `make` enter `choco install make` from the same window.  The make.exe is installed
alongside choco.exe in the bin directory.  Close the admin PowerShell and from a new GitBash shell you can confirm that make is now available:

    $ which make
    /c/ProgramData/chocolatey/bin/make

I am not entirely sure how it works though because `make` is available even though I don't see any new directories in the Path environment.
Hopefully it "just works" for you.
If you haven't already, it's time to clone the Collatz repository in Visual Studio Code which is at https://github.com/wbrassem/Collatz

Once CMake is installed Visual Studio often detects this and prompts you to add CMake and CMake Tool extension which is not only nice but
may be required later on.  If not then search in the Visual Studio Extension and install them both.  Also install the C/C++ Extension Pack.

When you first run CMake it might complain about not having CMAKE_C_COMPILER and CMAKE_CXX_COMPILER not set.  This is technically true because
you need to scan for compiler kits and pick one to use.  Open up command palette `<CTRL-SHFT-P>` and look for "CMake: Scan for Kits".  This should
find the MSys2 mingw compiler(s).  Type `<CTRL-SHIFT-P>` again to select one of the ones you just installed.  Probably looks something like:
    
    GCC 13.1.0 x86_64-w64-mingw32 (mingw64)

To step through the code select the "Run and Debug" option on left-hand side of VS Code and then from the "Start Debugging" drop down (green
triangle run button) select the "Windows gdb debug".  If all goes well you should be able to not only build but debug the executable.

Sometimes Window defaults to selecting their own nmake build utility.  To change to Unix makefile try:

    cmake .. -G "Unix Makefiles"

Download Doxygen (not required, but worth it) from https://www.doxygen.nl/download.html.  It will look something like doxygen-1.9.7-setup.exe
and defaults installs in `C:\Program Files\doxygen`.  Manually add `C:\Program Files\doxygen\bin` to the settings PATH environment variable.
To build the executable from the build directory:

    make

Optionally install TeX Live 2023 from https://www.tug.org/texlive/ which places files in `C:\textlive\2023\bin\windows\`.  It puts all the
binaries there and also conveniently adds the `C:\textlive\<year>\bin\windows` to the Path environment variable where epstopdf resides.

Despite it's prodigious 8G size, the TeX Live 2023 installation did not actually render the Doxygen embedded formulae, but instead complained
that it was unable to run `gswin32c.exe`.  The `make doxygen` calls epstopdf as is done on Mac, but Windows installations seem to require this
additional component.

The `gswin32c.exe` executable is separately downloadable from `ghostscript.com` using the `gs10012w32.exe` 32-bit install download which also
installs a few other things.  The install goes into `C:\Program Files (x86)\gs\gs<xx.xx.x>\bin`.  This executable also became available to TeX Live
without changing the Path variable.  After `gswin32c.exe` was installed, the embedded TeX formulae in the Doxygen code comments rendered.

Note: It really looked like installing the full TeX Live was not a hard requirement.  However when I uninstalled TeX Live and reran doxygen
fresh it said the `'latex' is not recognized as an internal or external command`.  So it's needed in some way to do that actual creation of
the *.png file resources which HTML uses.  Once I reinstalled TeX Live, the formulae for the HTML pages regenerated.

To generate the HTML documentation from the build directory:

    make doxygen