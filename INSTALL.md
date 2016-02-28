# Installation

This package should be useable on a system with ROOT 6.05/02 or higher and Python 2.7 installed.
To install, first check that `~/.bashrc` or `~/.bash_profile` on MACOSX exsists.
If not type `touch ~/.bashrc` or `touch ~/.bash_profile`.
Then simply do the following:

    git clone https://github.com/dabercro/CrombieTools.git
    cd CrombieTools
    ./install.sh

The install script will add the necessary lines environment variables to your `~/.bashrc` or `~/.bash_profile`.
After it finishes, either source your  `~/.bashrc` or `~/.bash_profile` file again, log out and back in, or just open a new terminal window.

After installation, it's recommended that you [run tests](test/README.md).