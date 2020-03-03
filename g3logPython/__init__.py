
import sys
import inspect
from functools import partial
from _g3logPython import *

def g3log(level, message):
    try:
        callstack = inspect.stack()[1]
        receivelog( callstack.filename, callstack.lineno, callstack.function, level, message)
    except Exception as e:
        print("ERROR in g3log python: " + str(e), file=sys.stderr )


debug   = partial(g3log, g3DEBUG)
info    = partial(g3log, g3INFO)
warning = partial(g3log, g3WARNING)
fatal   = partial(g3log, g3FATAL)
#TODO : the fatal callstack should also display the python callstack 
# (g3log will now display the interpreter's callstack, not that useful for python)

