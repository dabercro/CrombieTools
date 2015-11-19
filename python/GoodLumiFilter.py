import ROOT
import json
from CrombieVars import skimSrc

ROOT.gROOT.LoadMacro(skimSrc + 'GoodLumiFilter.cc+')

def MakeFilter(jsonFileName):
    filter = ROOT.GoodLumiFilter()
    jsonFile = open(jsonFileName,'r')
    data = json.load(jsonFile)
    jsonFile.close()

    for run, lumisecs in data.items():
        for lumisec in lumisecs:
            for lumi in range(lumisec[0],lumisec[1]+1):
                filter.AddLumi(int(run),lumi)
            ##
        ##
    ##
    return filter
##
