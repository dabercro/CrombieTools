from .. import Load

newFitTools = Load('FitTools')
fitTools = newFitTools()


def SetupFromEnv(analyzer=fitTools):
    """Sets up a FitTools for the user by reading the CrombieAnalysisConfig.
    @param analyzer is the FitTools instance that will be modified.
    @todo rework these setup functions into a wrapper class
    """

    from ..CommonTools.FileConfigReader import SetupConfigFromEnv

    SetupConfigFromEnv(analyzer)
