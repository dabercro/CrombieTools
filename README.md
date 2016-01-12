# CrombieTools

## Installation

This package should be useable on a system with ROOT 6.05/02 or higher and Python 2.7 installed.
To install, first check that `~\.bashrc` or `~\.bash_profile` on MACOSX exsists.
If not type `touch ~\.bashrc` or `touch ~\.bash_profile`.
Then simply do the following:
```
git clone https://github.com/dabercro/CrombieTools.git
cd CrombieTools
./install.sh
```
The install script will add the necessary lines environment variables to your `~\.bashrc` or `~\.bash_profile`.

## Description of Tools

### Scripts Without Arguments or Configuration Files

There are a number of scripts in `CrombieTools/bin` which is included in your `PATH` after installation.

`CleanEmacsBackups` searches the existing directory and all subdirectories for `*~` files and removes them.

`CompileCrombieTools` is run during installation to compile all components of the package.
It can also be run after editting any files to recompile new libraries.

`CrombieClean` removes all files created by `CompileCrombieTools`.

`CrombieSetupWorkspace` copies a number of template files into the directory you call it from.
These files are used to configure the Skimming tools, described below.

### Scripts for Slimming

COMING LATER

### Other Scripts

`CrombieMakeWebPage` Makes a simple HTML document displaying plots in a directory with .pdf .png and .C files in it.
The arguments are first, the directory, then the number of plots per row.
A third optional argument is an expression to match for blinded plots.

### Analysis Tools

COMING LATER

### Plot Tools

COMING LATER

### Skimming Tools

COMING LATER
