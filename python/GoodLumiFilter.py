import ROOT
import json
from CrombieVars import skimSrc

if not 'GoodLumiFilter' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'GoodLumiFilter.cc+')

newGoodLumiFilter = ROOT.GoodLumiFilter

def MakeFilter(jsonFileName):
    filter = newGoodLumiFilter()
    if jsonFileName != '':
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
    ##
    return filter
##
