#!/usr/bin/env python3

print("test 05: mixing python and c++")

import g3logPython as log
import cppLib

logger = log.get_ifaceLogWorker(False)
colorTermSink = logger.ClrTermSinks.new_Sink("color term")

log.debug("here we use the logger in python")

cppLib.log_something()

cppLib.add_syslog()

log.debug("from the logger in python again")

cppLib.log_something()

