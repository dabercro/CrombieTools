# Crombie Tools

[![Build Status](https://travis-ci.org/dabercro/CrombieTools.svg?branch=master)](https://travis-ci.org/dabercro/CrombieTools)
[![Documentation](https://codedocs.xyz/dabercro/CrombieTools.svg)](https://codedocs.xyz/dabercro/CrombieTools/)

Welcome to the CrombieTools package.

This contains everything I use to analyze flat trees in ROOT.
After installation, it can basically be treated as a Python package
with some handy command line tools thrown in.
The backend is mostly classes written in C++.

## Installation

This package should be useable on a system with ROOT 6.05/02 or higher and Python 2.7 installed.
To install, first check that `~/.bashrc` or `~/.bash_profile` on MACOSX exsists.
If not type `touch ~/.bashrc` or `touch ~/.bash_profile`.
Then simply do the following:

    git clone https://github.com/dabercro/CrombieTools.git
    cd CrombieTools
    ./install.sh

The install script will add the necessary lines environment variables to your `~/.bashrc` or `~/.bash_profile`.
After it finishes, either source your  `~/.bashrc` or `~/.bash_profile` file again,
log out and back in, or just open a new terminal window.

After installation, it's recommended that you [run tests](test/README.md).

## Other Topics

[Running Tests](test/README.md) <br>
[Analysis Workspace](templates/README.md) <br>
[Command Line Tools Reference](bin/README.md) <br>

Use the tabs or sidebar of the reference website to explore different
classes available through CrombieTools.

## To do:

@todo Create a class that propogates systematic uncertainties based on correlations in a tree.
      For example, create a 2D histogram where you average the quadrature sums weighted based
      on contributions in one direction to get the total uncertainty in another direction.
      Make sure that when you end up adding multiple dimensions together that the order does not matter.
      i.e. do some math!
