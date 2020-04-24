#! /usr/bin/env python

import os
import sys
argv = sys.argv
sys.argv = [sys.argv[0]]

import ROOT
import CrombieTools  # Needed for KinematicFunctions.h

import logging

logging.basicConfig(level=logging.INFO)

def main(in_file, out_file, cut, *args, **kwargs):
    dump = args or ['run', 'luminosityBlock', 'event']
    to_scan = ':'.join(dump)

    logging.info('Input file %s', in_file)
    logging.info('Output file %s', out_file)
    logging.info('Cut %s', cut)
    logging.info('Scanning %s', to_scan)

    fh = ROOT.TFile(in_file)
    in_tree = fh.Get(kwargs.get('tree', 'events'))

    in_tree.GetPlayer().SetScanRedirect(True)
    in_tree.GetPlayer().SetScanFileName(out_file)

    in_tree.Scan(to_scan, cut, 'colsize=20')

    fh.Close()


if __name__ == '__main__':
    if len(argv) < 4:
        print 'USAGE %s INFILE OUTFILE CUT [BRANCH [BRANCH ...]]' % argv[0]
        exit(0)

    main(*argv[1:], tree=os.environ.get('tree', 'events'))
