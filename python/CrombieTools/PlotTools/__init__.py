import os
from datetime import datetime

from . import tdrStyle
from .. import DirFromEnv

tdrStyle.setTDRStyle()

__all__ = ['Plot2D', 'PlotBrazil', 'PlotFitParameters', 'PlotROC', 'PlotStack', 'PlotUtils', 'QuickPlot', 'RatioComparison']

def AddOutDir(basename, default_suff='plots'):
    """A function to prepend the correct output directory and make the directory, if needed

    @param basename is the raw basename that is to be given to the output plot
    @param default is the location given to the script if the environment variable CrombieOutPlotDir is not set
    """

    default = '/home/dabercro/public_html/plots/%s_%s' % (datetime.today().strftime('%y%m%d'), default_suff)
    DirFromEnv('CrombieOutPlotDir', default)
    return os.path.join(os.environ.get('CrombieOutPlotDir', default), basename)


def SetupFromEnv(plotter):
    """A function that sets up a plotter after sourcing a config file.

    @param plotter is the plotter to setup. Defaults to plotter in this module.
    """
    from ..CommonTools.FileConfigReader import SetupConfigFromEnv, SetFunctionFromEnv

    SetupConfigFromEnv(plotter)

    if not os.environ.get('blind'):
        DirFromEnv('CrombieOutPlotDir')

    SetFunctionFromEnv([
            (plotter.SetOutDirectory, 'CrombieOutPlotDir'),
            ])
