import os
from .. import Load

newCutflowMaker = Load('CutflowMaker')
cutflowMaker = newCutflowMaker()

cutflowMaker.SetDebugLevel(int(os.environ.get('DEBUG', 1)))
