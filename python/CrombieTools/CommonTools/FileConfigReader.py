"""@package CrombieTools.CommonTools.FileConfigReader
Holds constructor and default FileConfigReader object.
@author Daniel Abercrombie <dabercro@mit.edu>
"""

from .. import Load

"""Function pointer to the constructor."""
newFileConfigReader = Load('FileConfigReader')
"""Object created by the default constructor."""
fileConfigReader    = newFileConfigReader()
