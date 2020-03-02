#!/usr/bin/env python3

print("start test")

import g3logPython

print("g3logPython imported")

logger = g3logPython.get_ifaceLogWorker(False)
journaldSink = logger.SysLogSinks.new_Sink("journald","id=g3log")

print("loggers created")

g3logPython.debug("debug: hello world! from python")
g3logPython.info("some info from python")
g3logPython.warning("important warning from python")
g3logPython.fatal("We messed up in python")


