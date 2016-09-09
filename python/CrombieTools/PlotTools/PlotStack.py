import os
from .. import Load, DirFromEnv, Nminus1Cut

newStackPlotter = Load('PlotStack')
plotter         = newStackPlotter()


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


def ReadExceptionConfig(region,aPlotter = plotter):
    """ Reads an [exception configuation](@ref formatmc) file from the [environment](@ref envconfig).

    @param region is the region that needs a different configuration that's about to be plotted.
    @param aPlotter is the plotter that will be adjusted. The default is the plotter defined in this module.
    """
    from .. import LoadConfig
    if os.environ.get('CrombieExcept_' + region) == None:
        print 'Region ' + region + ' does not have an except config set!'
    else:
        aPlotter.ReadMCConfig(os.environ['CrombieExcept_' + region])


class ParallelStackContainer:
    """A class that holds a PlotStack and copies it.

    Makes the N minus 1 cuts on the object before calling PlotStack::MakeCanvas()
    """

    def __init__(self, plotter, overwrite):
        self.Plotter = plotter
        self.Overwrite = overwrite

    def Copy(self):
        return ParallelStackContainer(plotter.Copy(), self.Overwrite)

    def MakePlot(self, category, region, exprArg):
        """Adjusts cut to N minus 1 and plots.

        @param category is a string
        @param region is a string
        @param exprArg is a list of arguments for the plotter used in PlotStack.MakePlots()
        """

        SetCuts(category, region, self.Plotter)
        holdCut = self.Plotter.GetDefaultWeight()

        expr = []
        kwargs = {}

        if type(exprArg[0]) == dict:
            kwargs = dict(exprArg[0])
            expr = list(exprArg[1:])
        else:
            expr = list(exprArg)

        # Different expressions for data and MC
        self.Plotter.SetDataExpression(kwargs.get('data_expr',''))
        # List cut lines to draw in a plot
        for cut_line in kwargs.get('cut_lines', Nminus1Cut(holdCut, expr[0], True)):
            self.Plotter.AddCutLine(cut_line)

        self.Plotter.SetDefaultWeight(Nminus1Cut(holdCut, expr[0]))
        self.Plotter.SetDefaultExpr(expr[0])
        expr[0] = '_'.join([category, region, expr[0]])
        if (self.Overwrite or 
            not os.path.exists(str(self.Plotter.GetOutDirectory()) + expr[0] + '.pdf')):
            self.Plotter.MakeCanvas(*expr)
            self.Plotter.SetDefaultWeight(holdCut)
            # Reset and exceptional values for this plot
            self.Plotter.SetDataExpression('')
            self.Plotter.ResetCutLines()


def MakePlots(categories, regions, exprArgs, overwrite=True, parallel=True, aPlotter=plotter):
    """ Shortcut to make plots for multiple categories and regions with the same setup.

    @param categories is a list of categories to plot.
    @param regions is a list of the regions to plot.
    @param exprArgs is a list of lists of parameters to be used in PlotStack::MakeCanvas().
                    Normally, the first expression is the default expression to be plotted.
                    The basename of the output file is automatically set by this.
                    If the first argument is actually a dictionary, it will be passed basically as
                    key word arguments to ParallelStackContainer.MakePlot() instead.
                    Key words supported in dictionary and types:
                        - data_expr takes a string
                        - cut_lines takes a list of floats
    @param overwrite overwrites old plots with the same name if set to true.
                     Otherwise, those plots are skipped.
    @param parallel determines whether or not to run the plots in a Multiprocess fashion.
    @param aPlotter is the plotter to use to plot. The default is the plotter defined in this module.
    """

    if not type(categories) == list:
        MakePlots([categories], regions, exprArgs, aPlotter)

    elif not type(regions) == list:
        MakePlots(categories, [regions], exprArgs, aPlotter)

    else:
        passToParallel = []
        for category in categories:
            for region in regions:
                for exprArg in exprArgs:
                    passToParallel.append([category, region, exprArg])

        if parallel:
            from ..Parallelization import RunParallel
            RunParallel(ParallelStackContainer(aPlotter, overwrite), 'MakePlot', passToParallel)

        else:
            plotter = ParallelStackContainer(aPlotter, overwrite)
            for args in passToParallel:
                plotter.MakePlot(*args)
