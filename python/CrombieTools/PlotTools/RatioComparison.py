from .. import Load, DirFromEnv, Nminus1Cut

newRatioComparer = Load('RatioComparison')
plotter          = newRatioComparer()


def SetupFromEnv(aPlotter=plotter):
    """A function that sets up a plotter after sourcing a config file.

    @param aPlotter is the plotter to setup. Defaults to plotter in this module.
    """
    from ..CommonTools.FileConfigReader import SetupConfigFromEnv, SetFunctionFromEnv

    SetupConfigFromEnv(aPlotter)

    DirFromEnv('CrombieOutPlotDir')
    SetFunctionFromEnv([
            (aPlotter.SetOutDirectory, 'CrombieOutPlotDir'),
            ])


def SetCuts(category1, region1, region2, category2=None, aPlotter=plotter):
    """ Sets cuts based on category and region.

    @param category1 is the category of the analysis being used.
    @param region1 is the region of the numerator.
    @param region2 is the region of the denominator.
    @param category2 if set, is the category of the denominator.
                     Otherwise, the category is assumed to be the same
    @param aPlotter is the plotter that is having its cuts set.
                    Default is the plotter defined in this module.
    """

    category2 = category2 or category1

    from ..LoadConfig import cuts

    numerator_cut = cuts.cut(category1, region1)
    denominator_cut = cuts.cut(category2, region2)

    aPlotter.AddRatioCuts(numerator_cut + ' && ' + cuts.dataMCCuts(region1, True),
                          denominator_cut + ' && ' + cuts.dataMCCuts(region2, True),
                          aPlotter.kData, 'Data', 1)

    aPlotter.AddRatioCuts(numerator_cut + ' * ' + cuts.dataMCCuts(region1, False),
                          denominator_cut + ' * ' + cuts.dataMCCuts(region2, False),
                          aPlotter.kBackground, 'MC', 2)

    aPlotter.SetDataIndex(0)
    aPlotter.SetRatioIndex(1)
