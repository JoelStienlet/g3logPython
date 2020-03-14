# Technical aspects
## Sink types

Currently g3logpython provides 3 sink backends: logrotate, syslog, and a color-terminal output. One or more sinks can be used simultaneously. To use a sink, just add it to the logger (and optionnaly configure it to change the sink's default parameters).
Note that some sinks like logrotate can be instantiated multiple times (for example with logrotate: log locally + on a remote NFS simultaneously).

### logrotate
Writes the logs to compressed files. The number of files and the number of log entries per file can be adjusted.

### syslog
Logs to the system log (example: journald). With this sink your program will automatically inherit from the system-log settings (remote logging,...).

### Color terminal
This is a simple output to stderr, with colors.

### adding sink types
Adding a new sink type requires modifications to the code of this library, so cannot be done at runtime. Nevertheless, most parts of this wrapper are templated, so that adding a new sink type shouldn't be a major difficulty.

## joining g3log's threads

An interface for the std::future returned by g3log's call() is provided, and exported to python (see example 06). In practice, this is useful when you want to make sure a given configuration of a sink has taken effect before the next instructions are executed. You may safely ignore the future however, because the lifetime of the data passed to the sink's methods are managed by the bindings layer (safely: in the sense that no access to unallocated memory will occur).

## Thread safety
See g3log if you're not familiar with the call() to access sinks.
g3logPython calls the methods of the sinks through the call() of their respective g3log's handle. The thread safety coming from g3log is thus also inherited in 3glogPython.

## Object lifetime

### python strings

When calling a sink method, the strings sent by python are copied and stored in a global store, until the worker thread using them has finished (the store joins the thread before deleting the deep-copies).
The global store has a thread running in the background, periodically checking for data no longer in use.

### ifaceLogWorker
A flag can be passed when performing the first call to get_ifaceLogWorker(). This flag selects the lifetime of the LogWorker interface, that can either be the lifetime of the process, or managed by the user through shared_ptrs.

### Handles (LogRotateSnkHndl...)
in g3log, the worker will return a handle when adding a new sink. Here, this wrapper keeps ownership of g3log's handles, and provides its own wrapper-handle to python. In this wrapper, a sink is identified by its name, so you can easily access a handle using the sink's name (see example 06).


