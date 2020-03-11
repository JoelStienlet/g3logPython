#!/usr/bin/env python3

#
# full color terminal example
#

import g3logPython as log
import time

logger = log.get_ifaceLogWorker(False)
colorTermSink = logger.ClrTermSinks.new_Sink("color term")

log.debug("hello world! from python")
 # this is required to make sure the text was sent for display before muting:
time.sleep(1)

future = colorTermSink.mute()
future.join()

log.debug("This is not displayed in the terminal")
 # this is required to prevent the sink to be unmuted before the text in sent for display:
time.sleep(1)

future = colorTermSink.unmute()
future.join()

log.debug("this in displayed again")
