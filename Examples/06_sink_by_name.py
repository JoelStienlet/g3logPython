#!/usr/bin/env python3

import g3logPython as log
import os
import time

#
# For a small example the benefits of getting the sink's handle by name is not obvious,
# but for a larger project, it may be convenient not having to store the handle somewhere.
#
#

def basic_log_init():
    logger = log.get_ifaceLogWorker(False)
    logger.SysLogSinks.new_Sink("my journald","g3logPython_Example")

def echo_to_stderr():
    logger = log.get_ifaceLogWorker() # get access to the singleton
    hndl = logger.SysLogSinks.new_SinkHndl("my journald") # get a temporary handle from the sink's name
    future = hndl.echoToStderr()
    future.join() # optional

def mute_stderr():
    logger = log.get_ifaceLogWorker() # get access to the singleton
    hndl = logger.SysLogSinks.new_SinkHndl("my journald") # get a temporary handle from the sink's name
    future = hndl.muteStderr()
    future.join() # optional


basic_log_init()

log.debug("hello world! from python (not echoed)")
time.sleep(1)

echo_to_stderr()

log.debug("this string is echoed to stderr")
time.sleep(1)

mute_stderr()

log.debug("this string is NOT echoed to stderr")

print("now check your system log with: journalctl _PID=" + str(os.getpid()) )
