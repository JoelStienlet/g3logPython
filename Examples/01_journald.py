#!/usr/bin/env python3

import g3logPython as log
import os
import time

logger = log.get_ifaceLogWorker(False)
JournaldSink = logger.SysLogSinks.new_Sink("journald","g3logPython_Example")

log.debug("hello world! from python")


time.sleep(1)

JournaldSink.echoToStderr()

#TODO join() the worker thread in python
time.sleep(1) # until we can join()

log.debug("this string is echoed to stderr")

time.sleep(1)

JournaldSink.muteStderr()

#TODO join() the worker thread in python
time.sleep(1) # until we can join()

log.debug("this string is NOT echoed to stderr")

print("now check your system log with: journalctl _PID=" + str(os.getpid()) )
