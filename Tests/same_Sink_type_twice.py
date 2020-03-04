#!/usr/bin/env python3

print("start test")

import g3logPython

print("g3logPython imported")

logger = g3logPython.get_ifaceLogWorker(False)
journaldSink_1 = logger.SysLogSinks.new_Sink("journald_1","id=g3log_1")
journaldSink_2 = logger.SysLogSinks.new_Sink("journald_2","id=g3log_2")

print("loggers created")

journaldSink_1.echoToStderr()
journaldSink_2.echoToStderr()

journaldSink_1.setLogHeader("========== TEST HEADER 1 ==========")
journaldSink_2.setLogHeader("========== TEST HEADER 2 ==========")


print("logger configured")

g3logPython.debug("debug: hello world! from python")
g3logPython.info("some info from python")
g3logPython.warning("important warning from python")

