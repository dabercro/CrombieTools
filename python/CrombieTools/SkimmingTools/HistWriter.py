""" @package CrombieTools.SkimmingTools.HistWriter
Submodule of CrombieTools.SkimmingTools.
Contains the constructor and default HistWriter object.
"""

from .. import Load

newHistWriter = Load('HistWriter')
histWriter    = newHistWriter()

def MakeHist(inputFile, outputFile='', outputHist='', writer=histWriter):
    """A convenience function for HistWriter assuming the text file is named something you like.

    @param inputFile is the text file where the histogram is written out.
    @param outputFile is the location where the root histogram will be saved.
           If left blank, the name is just the inputFile replaced with .txt changed to .root
    @param outputHist is the name of the histogram saved. If left blank, it uses the
           basename of the inputFile. (e.g. a/place/hist.txt -> 'hist').
    @param writer specifies a HistWriter to use. This is probably rarely, if ever, needs to be changed.
    """

    writer.SetFileName(outputFile)
    writer.SetHistName(outputHist)

    import os.path as op

    if outputFile == '':
        outputFile = op.splitext(inputFile)[0] + '.root'

    if outputHist == '':
        outputHist = op.splitext(op.basename(inputFile))[0]

    # Check the configuration file to see if it's 2D
    numX = 0
    numY = 0
    with open(inputFile, 'r') as check_input:
        for line in check_input:
            if not numX:
                numX = len(line.split())
                if numX == 2:
                    break
            else:
                numY += 1

    if numY:
        writer.Make2DHist(inputFile, numX, numY)
    else:
        writer.MakeHist(inputFile)
