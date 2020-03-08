#!/usr/bin/env python3

import g3logPython as log
import os

logger = log.get_ifaceLogWorker(False)
colorTermSink = logger.SysLogSinks.new_Sink("journald","g3logPython_Example")

log.debug("hello world! from python")

print("now check your system log with: journalctl _PID=" + str(os.getpid()) )
