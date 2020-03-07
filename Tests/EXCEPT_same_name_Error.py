#!/usr/bin/env python3

print("start test")

import g3logPython as log
import os
import sys

print("g3logPython imported")

logdir_base = "/tmp/g3logPython/"
if not os.path.exists(logdir_base):
    os.mkdir(logdir_base)
logdir1 = logdir_base + "rotate1"
if not os.path.exists(logdir1):
    os.mkdir(logdir1)
logdir2 = logdir_base + "rotate2"
if not os.path.exists(logdir2):
    os.mkdir(logdir2)

logger = log.get_ifaceLogWorker(False)

# new_Sink( g3logPython_sink_name  log_prefix  log_directory )
logrotateSink_1 = logger.LogRotateSinks.new_Sink("log rotate unique name","py_g3logTest_1",logdir1)
# -> the first one shouldn't raise
try:
    logrotateSink_2 = logger.LogRotateSinks.new_Sink("log rotate unique name","py_g3logTest_2",logdir2)
    
    print("ERROR: this should have failed!")
    sys.exit(1) # if no exception raised -> error
    
except Exception as inst:
    print(type(inst))
    print(inst.args)
    print(inst)
    print("error triggered as expected")
    sys.exit(0)




