#!/usr/bin/env python3

import g3logPython as log
import os
import time

logger = log.get_ifaceLogWorker(False)
JournaldSink = logger.SysLogSinks.new_Sink("journald","g3logPython_Example")

log.debug("hello world! from python (not echoed)")
time.sleep(1)

future = JournaldSink.echoToStderr()
future.join() # optional: join() g3log's worker thread

log.debug("this string is echoed to stderr")
time.sleep(1)

future = JournaldSink.muteStderr()
future.join() # optional

log.debug("this string is NOT echoed to stderr")

print("now check your system log with: journalctl _PID=" + str(os.getpid()) )
