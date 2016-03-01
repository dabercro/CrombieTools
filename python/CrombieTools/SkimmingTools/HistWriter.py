""" @package CrombieTools.SkimmingTools.HistWriter
Submodule of CrombieTools.SkimmingTools.
Contains the constructor and default HistWriter object.
"""

from .. import Load

newHistWriter = Load('HistWriter')
histWriter    = newHistWriter()
