#! /usr/bin/env python

import os
import sys

sys.argv.append('-b')

from ROOT import TFile, TCanvas

if __name__ == '__main__':

    if len(sys.argv) < 4 or not os.path.isfile(sys.argv[1]):
        print 'Usage %s INPUT OUTPUT [ADDITIONALPLOTS [...]]' % sys.argv[0]
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

    for branch in [br.GetName() for br in input_tree.GetListOfBranches()] + sys.argv[3:-1]:
        if 'packedPuppi' in branch:
            continue

        file_name = ''
        canvas = TCanvas()
        plots = branch.split(',')
        cut = '' if len(plots) == 1 else plots[1].strip()
        opt = ''
        file_name = plots[0].replace('/', '__').replace('(', '__').replace(')', '__').replace(',', '__').replace('|', '__')
        for index, plot in enumerate(plots[0].split('|')):
            input_tree.SetLineColor(index + 1)
            input_tree.Draw(plot.strip(), cut, opt)
            opt = 'same'

        for ext in ['C', 'png', 'pdf']:
            canvas.SaveAs(os.path.join(output_dir, file_name + '.' + ext))
