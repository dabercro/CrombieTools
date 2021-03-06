# This first import is to load the sqlite3 library
import sqlite3

from .. import Load

newYieldDump = Load('YieldDump')
dumper = newYieldDump()

def SetupFromEnv(aDumper=dumper):
    """A function that sets up the yield dumper after sourcing a config"""

    from ..CommonTools.FileConfigReader import SetupConfigFromEnv

    SetupConfigFromEnv(aDumper)
