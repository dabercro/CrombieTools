import os
from .. import Load, DirFromEnv, Nminus1Cut

newStackPlotter = Load('PlotStack')
plotter         = newStackPlotter()

def SetupFromEnv(aPlotter = plotter):
    from .. import LoadConfig
    DirFromEnv('CrombieOutPlotDir')
    if os.path.exists('CrombieAnalysisConfig.sh') or os.path.exists('CrombiePlotterConfig.sh'):
        def readMC(config):
            aPlotter.ReadMCConfig(config,aPlotter.kBackground)
        def readSignal(config):
            aPlotter.ReadMCConfig(config,aPlotter.kSignal)

        targets = [[aPlotter.SetLuminosity, 'CrombieLuminosity'],
                   [aPlotter.SetInDirectory, 'CrombieInFilesDir'],
                   [aPlotter.SetOutDirectory, 'CrombieOutPlotDir'],
                   [aPlotter.SetLimitTreeDir, 'CrombieOutLimitTreeDir'],
                   [readMC,'CrombieMCConfig']
                   ]
        for target in targets:
            if os.environ.get(target[1]) == None:
                print 'Cannot find ' + target[1] + ' in config'
            else:
                try:
                    target[0](os.environ[target[1]])
                except:
                    target[0](float(os.environ[target[1]]))

    else:
        print 'Could not find CrombieAnalysisConfig.sh or CrombiePlotterConfig.sh'

def SetCuts(category,region,aPlotter = plotter):
    """ Sets cuts based on category and region.

    @param category is the category of the analysis being used.
    @param region is the region of the plot being set.
    @param aPlotter is the plotter that is having its cuts set.
    Default is the plotter defined in this module.
    """
    from .. import LoadConfig
    cuts = LoadConfig.cuts
    aPlotter.SetDefaultWeight(cuts.cut(category,region))
    aPlotter.SetMCWeights(cuts.dataMCCuts(region,False))
    aPlotter.SetDataWeights(cuts.dataMCCuts(region,True))

def ReadExceptionConfig(region,aPlotter = plotter):
    """ Reads an [exception configuation](@ref md_docs_FORMATMC) file from the [environment](@ref md_docs_ENVCONFIG).

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

    def __init__(self,plotter):
        self.Plotter = plotter

    def Copy(self):
        return ParallelStackContainer(plotter.Copy())

    def MakePlot(self,category,region,exprArg):
        """Adjusts cut to N minus 1 and plots.

        @param category is a string
        @param region is a string
        @param exprArg is a list of arguments for the plotter used in PlotStack.MakePlots()
        """

        SetCuts(category,region,self.Plotter)
        holdCut = self.Plotter.GetDefaultWeight()
        expr = list(exprArg)
        self.Plotter.SetDefaultWeight(Nminus1Cut(holdCut, expr[0]))
        self.Plotter.SetDefaultExpr(expr[0])
        expr[0] = '_'.join([category,region,expr[0]])
        self.Plotter.MakeCanvas(*expr)
        self.Plotter.SetDefaultWeight(holdCut)


def MakePlots(categories,regions,exprArgs,aPlotter = plotter):
    """ Shortcut to make plots for multiple categories and regions with the same setup.

    @param categories is a list of categories to plot.
    @param regions is a list of the regions to plot.
    @param exprArgs is a list of lists of parameters to be used in PlotStack::MakeCanvas.
    @param aPlotter is the plotter to use to plot. The default is the plotter defined in this module.
    """

    if not type(categories) == list:
        MakePlots([categories],regions,exprArgs,aPlotter)

    elif not type(categories) == list:
        MakePlots(categories,[regions],exprArgs,aPlotter)

    else:
        from ..Parallelization import RunParallel

        passToParallel = []
        for category in categories:
            for region in regions:
                for exprArg in exprArgs:
                    passToParallel.append([category,region,exprArg])

        RunParallel(ParallelStackContainer(aPlotter),'MakePlot',passToParallel)
