import os
from . import tdrStyle

tdrStyle.setTDRStyle()

__all__ = ['Plot2D', 'PlotBrazil', 'PlotFitParameters', 'PlotHists', 'PlotROC', 'PlotStack', 'PlotUtils', 'QuickPlot']

def AddOutDir(basename):
    """A function to prepend the correct output directory and make the direcotry, if needed

    @param basename is the raw basename that is to be given to the output plot
    """

    from .. import DirFromEnv
    DirFromEnv('CrombieOutPlotDir', 'plots')
    return os.path.join(os.environ.get('CrombieOutPlotDir', 'plots'), basename)
