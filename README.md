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

Technical aspects are described on [this page](https://github.com/JoelStienlet/g3logPython/blob/master/doc/technical.md).

## Other ressources

[links to similar projects](https://github.com/JoelStienlet/g3logPython/blob/master/doc/links.md)
