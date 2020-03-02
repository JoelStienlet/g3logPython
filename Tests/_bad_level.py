#!/usr/bin/env python3

print("start test")

import g3logPython

print("g3logPython imported")

logger = g3logPython.get_ifaceLogWorker(False)
journaldSink = logger.SysLogSinks.new_Sink("journald","id=g3log")

print("loggers created")

g3logPython.debug("debug: hello world! from python")
g3logPython.g3log(1234, "message sent to a bad log level")

