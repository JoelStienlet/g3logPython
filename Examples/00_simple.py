#!/usr/bin/env python3

import g3logPython as log

logger = log.get_ifaceLogWorker(False)
colorTermSink = logger.ClrTermSinks.new_Sink("color term")

log.debug("hello world! from python")
