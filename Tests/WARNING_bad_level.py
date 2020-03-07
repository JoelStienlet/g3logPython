#!/usr/bin/env python3

print("start test")

import g3logPython as log
import sys
print("g3logPython imported")

logger = log.get_ifaceLogWorker(False)
journaldSink = logger.SysLogSinks.new_Sink("journald","g3logPython_TEST")

print("loggers created")

log.debug("debug: hello world! from python")

bad_level_number = 1234

log.g3log(bad_level_number, "message sent to a bad log level")
print("This error will not raise an exception")
print("check journalctl -n50")
print("a warning should be displayed in the logs.")


