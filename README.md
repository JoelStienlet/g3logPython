[![Build Status](https://travis-ci.org/JoelStienlet/g3logPython.svg?branch=master)](https://travis-ci.org/JoelStienlet/g3logPython)
[![GitHub license](https://img.shields.io/badge/license-Unlicense-brightgreen)](https://github.com/JoelStienlet/g3logPython/blob/master/LICENSE)
[![codecov](https://codecov.io/gh/JoelStienlet/g3logPython/branch/master/graph/badge.svg)](https://codecov.io/gh/JoelStienlet/g3logPython)
[![Percentage of issues still open](http://isitmaintained.com/badge/open/JoelStienlet/g3logPython.svg)](http://isitmaintained.com/project/JoelStienlet/g3logPython "Percentage of issues still open")

# g3logPython
Python bindings for [g3log](https://github.com/KjellKod/g3log.git)

This library provides python3 bindings for g3log + g3sinks (currently logrotate, syslog, and a color-terminal output).
Calls can be made both from python and C++, retaining the thread-safety of g3log.
The advantage is that you can use the same logger both in c++ and python parts of a multi-language project.
Note that this project is made of two parts:
- a binding layer in C++
- a python wrapper aroung the binding layer, using pybind11

The binding layer modifies the interface of g3log+sinks to make it easily wrappable with pybind11. Exported methods from the binding layer can also be called from C++, providing a different interface to g3log.
 
## License
Unlicense license

## Build and Install

For the C++ bindings:

```bash
mkdir build_bindings && cd build_bindings && cmake .. && make && sudo make install
```
Then build the python wrapper:

```bash
python3 setup.py install
```




## Example
See the Examples directory for more advanced usage, and examples of different sink backends.

```python
#!/usr/bin/env python3
import g3logPython as log
logger = log.get_ifaceLogWorker(False)
journaldSink = logger.SysLogSinks.new_Sink("journald","id=g3logPython")
journaldSink.echoToStderr()
log.debug("hello world!")
```

## Technical aspects
### Sink types

Currently g3logpython provides 3 sink backends: logrotate, syslog, and a color-terminal output. One or more sinks can be used simultaneously. To use a sink, just add it to the logger (and optionnaly configure it to change the sink's default parameters).
Note that some sinks like logrotate can be instantiated multiple times (for example with logrotate: log locally + on a remote NFS simultaneously).

#### logrotate
Writes the logs to compressed files. The number of files and the number of log entries per file can be adjusted.

#### syslog
Logs to the system log (example: journald). With this sink your program will automatically inherit from the system-log settings (remote logging,...).

#### Color terminal
This is a simple output to stderr, with colors.

#### adding sink types
Adding a new sink type requires modifications to the code of this library, so cannot be done at runtime. Nevertheless, most parts of this wrapper are templated, so that adding a new sink type shouldn't be a major difficulty.

### joining g3log's threads

An interface for the std::future returned by g3log's call() is provided, and exported to python (see example 06). In practice, this is useful when you want to make sure a given configuration of a sink has taken effect before the next instructions are executed. You may safely ignore the future however, because the lifetime of the data passed to the sink's methods are managed by the bindings layer (safely: in the sense that no access to unallocated memory will occur).

### Object lifetime

#### python strings

When calling a sink method, the strings sent by python are copied and stored in a global store, until the worker thread using them has finished (the store joins the thread before deleting the deep-copies).
The global store has a thread running in the background, periodically checking for data no longer in use.

#### ifaceLogWorker
A flag can be passed when performing the first call to get_ifaceLogWorker(). This flag selects the lifetime of the LogWorker interface, that can either be the lifetime of the process, or managed by the user through shared_ptrs.

#### Handles (LogRotateSnkHndl...)
in g3log, the worker will return a handle when adding a new sink. Here, this wrapper keeps ownership of g3log's handles, and provides its own wrapper-handle to python. In this wrapper, a sink is identified by its name, so you can easily access a handle using the sink's name.

### Thread safety
See g3log if you're not familiar with the call() to access sinks.
g3logPython calls the methods of the sinks through the call() of their respective g3log's handle. The thread safety coming from g3log is thus also inherited in 3glogPython.

## Similar projects:
[pyg3log](https://github.com/GreyDireWolf/pyg3log.git) is another wrapper for g3log. It's advantage is being more lightweight than g3logPython. It does not (yet in 02/2020) implement the call() method through g3log sink handles, nor does it include a data store as we do to extend the lifetime of python strings.  Note that this may not be required if the default sink parameters suit your application.

