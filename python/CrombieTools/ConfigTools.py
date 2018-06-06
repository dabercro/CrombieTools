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

class Line(object):
    lasttree = ''
    lastentry = ''
    lastcolor = ''
    def __init__(self, in_line):
        split_line = in_line.lstrip('#').split()
        self.saved = not in_line.startswith('#')
        self.treename = split_line[0] if split_line[0] != '.' else Line.lasttree
        self.file = split_line[1]
        self.xs = split_line[2]
        self.entry = split_line[3] if split_line[3] != '.' else Line.lastentry
        self.color = ' '.join(split_line[4:]) if split_line[4] != '.' else Line.lastcolor
        self.raw = in_line

        Line.lasttree = self.treename
        Line.lastentry = self.entry
        Line.lastcolor = self.color


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
