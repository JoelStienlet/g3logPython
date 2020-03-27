#!/usr/bin/env python3

import g3logPython as g3log
import logging

logger = g3log.get_ifaceLogWorker(False)
colorTermSink = logger.ClrTermSinks.new_Sink("color term")
# optional interoperability with pythons' builtin logger:
builtinLogger = logging.getLogger()
builtinLogger.addHandler(g3log.g3logHandler())
builtinLogger.setLevel(logging.DEBUG)

g3log.debug("hello world! from python, g3log used directly")
logging.info("this message is displayed using python's built-in logger")

