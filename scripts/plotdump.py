#! /usr/bin/env python

import os
import sys

sys.argv.append('-b')

import ROOT

if __name__ == '__main__':

    if len(sys.argv) < 4 or not os.path.isfile(sys.argv[1]):
        print 'Usage %s INPUT OUTPUT [ADDITIONALPLOTS [...]]' % sys.argv[0]
        print '\nPlaces the branches in the "events" tree in file INPUT'
        print 'in plots that show up in directory OUTPUT.'
        print '\nCan be viewed with various PHP scripts floating around like in'
        print '/home/dabercro/public_html/plotviewer'

        exit(1)

    input_file = ROOT.TFile(sys.argv[1])
    input_tree = input_file.events

    output_dir = sys.argv[2]
    if not output_dir.startswith('/'):
        output_dir = os.path.join(os.environ['HOME'], 'public_html', 'plots', output_dir)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    print 'Making plots in', output_dir
    ROOT.gErrorIgnoreLevel = ROOT.kWarning

    branches = [] if os.environ.get('quick') else [br.GetName() for br in input_tree.GetListOfBranches()]
    default_cut = os.environ.get('cut', '')

    for branch in branches + sys.argv[3:-1]:
        if 'packedPuppi' in branch:
            continue

        file_name = ''
        canvas = ROOT.TCanvas()
        plots = branch.split(';')
        cut = default_cut if len(plots) == 1 else plots[1].strip()
        opt = ''
        file_name = branch.replace('/', '_').replace('(', '_').replace(')', '_').replace(',', '_').replace('|', '__').replace(' ', '_').replace(';', '__').replace('!', '_').replace('<', '_').replace('&', '_')
        index = 1
        for plot in plots[0].split('|'):
            for x in cut.split('|'):
                input_tree.SetLineColor(index)
                input_tree.Draw(plot.strip(), x, opt)
                opt = 'same'
                index += 1

        for ext in ['C', 'png', 'pdf']:
            canvas.SaveAs(os.path.join(output_dir, file_name + '.' + ext))
