#!/usr/bin/env python3

print("start test")

import g3logPython as log
print("g3logPython imported")

logger = log.get_ifaceLogWorker(False)
journaldSink = logger.SysLogSinks.new_Sink("journald","g3logPython_TEST_journald")

print("loggers created")

journaldSink.echoToStderr()

journaldSink.setLogHeader("========== g3logPython TEST journald + journalctl ==========")


print("logger configured")

debug_string = "debug: hello world! from python"
info_string = "some info from python"
warning_string = "important warning from python"

log.debug(debug_string)
log.info(info_string)
log.warning(warning_string)

print("echek journalctl -n50")
