#! /usr/bin/env python

import os
import sys

sys.argv.append('-b')

from ROOT import TFile, TCanvas

if __name__ == '__main__':

    if len(sys.argv) != 4 or not os.path.isfile(sys.argv[1]):
        print 'Usage %s INPUT OUTPUT' % sys.argv[0]
        print '\nPlaces the branches in the "events" tree in file INPUT'
        print 'in plots that show up in directory OUTPUT.'
        print '\nCan be viewed with various PHP scripts floating around like in'
        print '/home/dabercro/public_html/plotviewer'

        exit(1)

    input_file = TFile(sys.argv[1])
    input_tree = input_file.events

    output_dir = sys.argv[2]
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for branch in [br.GetName() for br in input_tree.GetListOfBranches()]:
        if 'packedPuppi' in branch:
            continue

        file_name = os.path.join(output_dir, branch)
        canvas = TCanvas()
        input_tree.Draw(branch)
        for ext in ['C', 'png', 'pdf']:
            canvas.SaveAs(file_name + '.' + ext)
