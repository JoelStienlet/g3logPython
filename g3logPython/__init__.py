
import sys
import inspect
from functools import partial
import logging
from _g3logPython import *

def g3log(level, message):
    try:
        callstack = inspect.stack()[1]
        receivelog( callstack.filename, callstack.lineno, callstack.function, level, message)
    except Exception as e:
        print("ERROR in g3log python: " + str(e), file=sys.stderr )

# https://docs.python.org/3.8/library/logging.html
# https://docs.python.org/3/howto/logging-cookbook.html
# https://docs.python.org/3.8/library/logging.html#logging.LogRecord
class g3logHandler(logging.Handler):
    def __init__(self, *args, **kwargs):
        super(g3logHandler, self).__init__(*args, **kwargs)
    def emit(self, record):
        try:
            msg = self.format(record)
            receivelog( record.filename, 
                        record.lineno, 
                        record.funcName, 
                        self.pylevel_to_g3level(record.levelno), 
                        msg)
        except Exception as e:
            print("ERROR in g3log python: " + str(e), file=sys.stderr )
    def pylevel_to_g3level(self, pylevel):
        if pylevel == logging.DEBUG:
            return g3DEBUG
        elif pylevel == logging.INFO:
            return g3INFO
        elif pylevel == logging.WARNING:
            return g3WARNING
        elif pylevel == logging.ERROR:
            return g3WARNING
        elif pylevel == logging.CRITICAL:
            return g3FATAL
        else:
            return g3FATAL
        
debug   = partial(g3log, g3DEBUG)
info    = partial(g3log, g3INFO)
warning = partial(g3log, g3WARNING)
fatal   = partial(g3log, g3FATAL)
#TODO : the fatal callstack should also display the python callstack 
# (g3log will now display the interpreter's callstack, not that useful for python)

