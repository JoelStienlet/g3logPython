#!/usr/bin/env python3

print("start test")

import g3logPython
import sys
print("g3logPython imported")

logger = g3logPython.get_ifaceLogWorker(False)
try:
    journaldSink_1 = logger.SysLogSinks.new_Sink("journald_1","id=g3log_1")
    journaldSink_2 = logger.SysLogSinks.new_Sink("journald_2","id=g3log_2")

    print("loggers created")
    print("ERROR: this should have failed!")
    sys.exit(1) # if no exception raised -> error
    
except Exception as inst:
    print(type(inst))
    print(inst.args)
    print(inst)
    print("error triggered as expected")
    sys.exit(0)



