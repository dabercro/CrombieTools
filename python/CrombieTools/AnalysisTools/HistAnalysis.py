from .. import Load

newHistAnalysis = Load('HistAnalysis')
histAnalysis = newHistAnalysis()

def SetupFromEnv(analyzer=histAnalysis):
    """Sets up a HistAnalysis for the user by reading the CrombieAnalysisConfig.
    @param analyzer is the HistAnalysis instance that will be modified.
    """
    
    from .. import LoadConfig
    import os

    analyzer.ReadMCConfig(os.env.get('CrombieMCConfig'))
