"""@package CrombieTools.CommonTools.TreeContainer
Holds constructor and default TreeContainer object.
@author Daniel Abercrombie <dabercro@mit.edu>
"""

from .. import Load

"""Function pointer to the constructor."""
newTreeContainer = Load('TreeContainer')
"""Object created by the default constructor."""
treeContainer    = newTreeContainer()
