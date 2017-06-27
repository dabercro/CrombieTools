import os
from .. import Load

newTriggerPlotter = Load('PlotTriggerEfficiency')
plotter           = newTriggerPlotter()

def SetupFromEnv(aPlotter = plotter):
    """
    Some hack lazy stuff to setup a plotter
    """

    plotter.SetDebugLevel(int(os.environ.get('DEBUG', 1)))

    from ..CommonTools.FileConfigReader import SetFunctionFromEnv
    SetFunctionFromEnv([
            (aPlotter.SetInDirectory, 'CrombieInFilesDir'),
            ])
