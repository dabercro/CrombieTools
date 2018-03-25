#! /usr/bin/env python

import sys
from ROOT import TMVA

if __name__ == '__main__':
    if 'reg' in sys.argv:
        TMVA.TMVARegGui(sys.argv[1])
    else:
        TMVA.TMVAGUI(sys.argv[1])
    raw_input('Press enter to end GUI session\n')
