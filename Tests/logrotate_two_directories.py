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
logrotateSink_1 = logger.LogRotateSinks.new_Sink("log first dir","py_g3logTest_1",logdir1)
logrotateSink_2 = logger.LogRotateSinks.new_Sink("log second dir","py_g3logTest_2",logdir2)

log.debug("two dir test")




