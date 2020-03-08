#!/usr/bin/env python3

import g3logPython as log

logger = log.get_ifaceLogWorker(False)
colorTermSink = logger.LogRotateSinks.new_Sink("log rotate","py_g3logTest","/tmp/")

log.debug("hello world! from python")

