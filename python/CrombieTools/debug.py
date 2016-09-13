"""@package CrombieTools.debug

Activates debugging symbols and gives a debugging hook.

@author: Daniel Abercrombie <dabercro@mit.edu>
"""

import CrombieTools


CrombieTools.loadAddOn = '++g'


def debug(debugger='gdb'):
    """Hooks a debugger to a script.

    See https://root.cern.ch/phpBB3/viewtopic.php?t=10227.

    @param debugger is type of debugger
    """
    import os
    pid = os.spawnvp(os.P_NOWAIT,
                     debugger, [debugger, '-q', 'python', str(os.getpid())])

    # give debugger some time to attach to the python process
    import time
    time.sleep(1)

    # verify the process' existence (will raise OSError if failed)
    os.waitpid(pid, os.WNOHANG)
    os.kill(pid, 0)
