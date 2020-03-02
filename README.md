[![Build Status](https://travis-ci.org/JoelStienlet/g3logPython.svg?branch=master)](https://travis-ci.org/JoelStienlet/g3logPython)
[![GitHub license](https://img.shields.io/github/license/Naereen/StrapDown.js.svg)](https://github.com/JoelStienlet/g3logPython/blob/master/LICENSE)

# g3logPython
Python bindings for [g3log](https://github.com/KjellKod/g3log.git)

This library provides python bindings for g3log and sinks (currently logrotate and syslog).
Calls can be made both from python and C++, retaining the thread-safety of g3log.

## License
Unlicense license

## Build
python3 setup.py install

## Example
See the Examples directory for more advanced usage.

```python
#!/usr/bin/env python3
import g3logPython as log
logger = log.get_ifaceLogWorker(False)
journaldSink = logger.SysLogSinks.new_Sink("journald","id=g3logPython")
journaldSink.echoToStderr()
log.debug("hello world!")
```

## Technical aspects
### Object lifetime
#### ifaceLogWorker
A flag can be passed when performing the first call to get_ifaceLogWorker(). This flag selects the lifetime of the LogWorker interface, that can either be the lifetime of the process, or managed by the user through shared_ptrs.

#### Handles (LogRotateSnkHndl...)
in g3log, the worker will return a handle when adding a new sink. Here, this wrapper keeps ownership of g3log's handles, and provides its own wrapper-handle to python. In this wrapper, a sink is identified by its name, so you can easily access a handle using the sink's name.

### About call() and sink methods:
See g3log if you're not familiar with the call() to access sinks.
g3logPython makes the sink handle methods go through the call() of their respective g3log handle. This guarantees thread-safe access. 

## Similar projects:
[pyg3log](https://github.com/GreyDireWolf/pyg3log.git) is another wrapper for g3log. It's advantage is being more lightweight than g3logPython. It does not (yet in 02/2020) implement the method call() through g3log sink handles. This may not be required if the default sink parameters suit your application.

