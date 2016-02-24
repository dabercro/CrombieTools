#! /usr/bin/python

from CrombieTools.PlotTools.PlotStack import *
from array import array
import os

SetupFromEnv()

plotter.AddDataFile(os.environ['CrombieFileBase'] + '_Data.root')

categories = ['example']
regions    = ['signal']

Exprs = list()

def SetupArgs(theArray):
    Exprs = [
        ['example',len(theArray)-1,array('d',theArray),'X Label', 'Y Label',False]
        ['LogExample',10,0,100,,'X Label', 'Y Label',True]
        ]

xArray = [0,1,10,100]

SetupArgs(xArray)

MakePlots(categories,regions,Exprs)
