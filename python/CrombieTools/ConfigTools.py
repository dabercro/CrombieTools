""" @file ConfigTools.py
Defines the CrombieTools.ConfigTools package
@author Daniel Abercrombie <dabercro@mit.edu>

@package CrombieTools.ConfigTools
These are quick parsers for the FileConfigs for minor things that might be needed in Python.
@todo Use an actual FileConfigReader in here.
"""

import os

def TreeList(configs):
    """Get list of tree names from a config file

    @param configs are the file locations of the Configuration file to read the tree lists from
    """

    if not isinstance(configs, list):
        return TreeList([configs])

    output = []

    for config in configs:
        if not os.path.exists(config):
            print '%s does not exist!' % config
            exit 40

        with open(config, 'r') as in_file:
            for in_line in in_file.readlines():

                split_line = in_line.split()

                if split_line:
                    treename = split_line[0].lstrip('#')
                    if treename != '.':
                        output.append(treename)

    return output
