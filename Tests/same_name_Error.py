#!/usr/bin/env python3

print("start test")

import g3logPython

print("g3logPython imported")

logger = g3logPython.get_ifaceLogWorker(False)
journaldSink_1 = logger.SysLogSinks.new_Sink("SinkName","id=g3log_1")
journaldSink_2 = logger.SysLogSinks.new_Sink("SinkName","id=g3log_2")

print("should have raised an exception...")



