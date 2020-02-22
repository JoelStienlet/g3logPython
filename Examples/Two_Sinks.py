#!/usr/bin/env python3

print("start test")

import g3logPython

print("g3logPython imported")

logger = g3logPython.get_ifaceLogWorker(False)
journaldSink = logger.SysLogSinks.new_Sink("journald","id=g3log")
logrotateSink = logger.LogRotateSinks.new_Sink("log rotate","py_g3logTest","/tmp/")

print("loggers created")

journaldSink.setLogHeader("========== TEST HEADER ==========")

journaldSink.echoToStderr()

logrotateSink.setMaxArchiveLogCount(10)

print("logger configured")

g3logPython.debug("debug: hello world! from python")
g3logPython.info("some info from python")
g3logPython.warning("important warning from python")
#g3logPython.fatal("We messed up in python")

# note: sink method calls and logging are asynchronous: the relative chronology
# of their actual execution may not correspond to the relative order in the code.
journaldSink.setIdentity("toto")
g3logPython.debug("now with new id")
