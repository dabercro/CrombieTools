#!/usr/bin/env python

import sys

if len(sys.argv) > 1:
    from CrombieTools import Load
    for tool in sys.argv[1:]:
        Load(tool)
else:
    # Otherwise, creates most .so, .pcm, and .pyc files
    from CrombieTools.CommonTools import *
    from CrombieTools.SkimmingTools import *
    from CrombieTools.PlotTools import *
    from CrombieTools.AnalysisTools import *
    from CrombieTools import *
