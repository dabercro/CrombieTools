#! /usr/bin/env python

import ROOT

import sys
import logging

logging.basicConfig(level=logging.INFO)

if __name__ == '__main__':
    in_file = sys.argv[1]
    out_file = sys.argv[2]
    cut = sys.argv[3]

    dump = sys.argv[4:] if len(sys.argv) > 4 else ['runNumber', 'lumiNumber', 'eventNumber']
    to_scan = ':'.join(dump)

    logging.info('Input file %s', in_file)
    logging.info('Output file %s', out_file)
    logging.info('Cut %s', cut)
    logging.info('Scanning %s', to_scan)

    fh = ROOT.TFile(in_file)
    in_tree = fh.Get('events')

    in_tree.GetPlayer().SetScanRedirect(True)
    in_tree.GetPlayer().SetScanFileName(out_file)

    in_tree.Scan(to_scan, cut, 'colsize=20')

    fh.Close()
