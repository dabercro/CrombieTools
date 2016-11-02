import os
from .. import Load, DirFromEnv, Nminus1Cut

newQuickPlotter = Load('QuickPlot')
plotter         = newQuickPlotter()


def SetupFromEnv(aPlotter = plotter):
    """A function that sets up a plotter after sourcing a config file.

    @param aPlotter is the plotter to setup. Defaults to plotter in this module.
    """
    from ..CommonTools.FileConfigReader import SetupConfigFromEnv, SetFunctionFromEnv

    SetupConfigFromEnv(aPlotter)

    DirFromEnv('CrombieOutPlotDir')
    SetFunctionFromEnv([
            (aPlotter.SetOutDirectory, 'CrombieOutPlotDir'),
            ])


def SetCuts(category, region, aPlotter = plotter):
    """ Sets cuts based on category and region.

    @param category is the category of the analysis being used.
    @param region is the region of the plot being set.
    @param aPlotter is the plotter that is having its cuts set.
                    Default is the plotter defined in this module.
    """
    from ..LoadConfig import cuts
    aPlotter.SetDefaultWeight(cuts.cut(category, region))
    aPlotter.SetMCWeight(cuts.dataMCCuts(region, False))
    aPlotter.SetDataWeight(cuts.dataMCCuts(region, True))
