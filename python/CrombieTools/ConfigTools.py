""" @file ConfigTools.py
Defines the CrombieTools.ConfigTools package
@author Daniel Abercrombie <dabercro@mit.edu>

@package CrombieTools.ConfigTools
These are quick parsers for the FileConfigs for minor things that might be needed in Python.
@todo Use an actual FileConfigReader in here.
"""

import os
from collections import defaultdict

FileLists = defaultdict(lambda: [])

def TreeList(*args):
    """Get list of tree names from a config file

    @param args are the file locations of the Configuration file to read the tree lists from
    """

    output = set()

    for config in args:
        if not os.path.exists(config):
            print '%s does not exist!' % config
            exit(40)

        with open(config, 'r') as in_file:
            for in_line in in_file.readlines():

                split_line = in_line.split()

                if split_line:
                    raw = split_line[0]
                    treename = raw.lstrip('#')
                    if treename != '.':
                        lastname = treename
                    if raw[0] != '#':
                        output.add(lastname)
                        FileLists[lastname].append(split_line[1])

    return output
