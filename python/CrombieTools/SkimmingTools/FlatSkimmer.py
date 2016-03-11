""" @package CrombieTools.SkimmingTools.FlatSkimmer
Submodule of CrombieTools.SkimmingTools
Contains the constructor and default object for FlatSkimmer.
Also contains the constructor for and a function to return a filled GoodLumiFilter.
"""

import json
from .. import Load

newFlatSkimmer = Load('FlatSkimmer')
flatSkimmer    = newFlatSkimmer()

newGoodLumiFilter = Load('GoodLumiFilter')

def MakeFilter(jsonFileName):
    """ Create a filled GoodLumiFilter
    @param jsonFileName is the name of the good runs JSON to use.
    @returns a GoodLumiFilter object that is filled with that JSON file.
    """
    aFilter = newGoodLumiFilter()
    if jsonFileName != '':
        jsonFile = open(jsonFileName,'r')
        data = json.load(jsonFile)
        jsonFile.close()

        for run, lumisecs in data.items():
            for lumisec in lumisecs:
                for lumi in range(lumisec[0],lumisec[1]+1):
                    aFilter.AddLumi(int(run),lumi)

    return aFilter
