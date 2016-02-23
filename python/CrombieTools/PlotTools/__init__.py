import os
from . import tdrStyle

tdrStyle.setTDRStyle()

__all__ = ['Plot2D','PlotFitParameters','PlotHists','PlotROC','PlotStack','PlotUtils']

if type(os.environ.get('CrombieOutPlotDir')) == str:
    if not os.path.exists(os.environ['CrombieOutPlotDir']):
        os.makedirs(os.environ['CrombieOutPlotDir'])
