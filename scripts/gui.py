#! /usr/bin/env python

import sys
from ROOT import TMVA

if __name__ == '__main__':
    TMVA.TMVAGUI(sys.argv[1])
    raw_input('Press enter to end GUI session\n')
