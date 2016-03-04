import os
from .. import Load, DirFromEnv, Nminus1Cut

newStackPlotter = Load('PlotStack')
plotter         = newStackPlotter()

def SetupFromEnv(aPlotter = plotter):
    from .. import LoadConfig
    DirFromEnv('CrombieOutPlotDir')
    if os.path.exists('CrombieAnalysisConfig.sh'):
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
        print 'Could not find CrombieAnalysisConfig.sh'

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

def MakePlots(categories,regions,exprArgs,aPlotter = plotter):
    """ Shortcut to make plots for multiple categories and regions with the same setup.

    @param categories is a list of categories to plot.
    @param regions is a list of the regions to plot.
    @param aPlotter is the plotter to use to plot. The default is the plotter defined in this module.
    """
    if not (type(categories) == list and type(regions) == list):
        ## @todo Fix this to just call itself again with a list
        print 'Even if not using multiple regions or categories, must be a list!'
        return 0
    for category in categories:
        for region in regions:
            SetCuts(category,region,aPlotter)
            for exprArg in exprArgs:
                holdCut = aPlotter.GetDefaultWeight()
                expr = list(exprArg)
                aPlotter.SetDefaultWeight(Nminus1Cut(holdCut, expr[0]))
                aPlotter.SetDefaultExpr(expr[0])
                expr[0] = '_'.join([category,region,expr[0]])
                aPlotter.MakeCanvas(*expr)
                aPlotter.SetDefaultWeight(holdCut)
