This example shows how the file descriptor interface of g3logBindings allows you to use g3log in conjunction with other log systems that provide a feature to log to an open file. Of course, interoperability doesn't necessarily make use of the file descriptor interface, some loggers may use a callback (or other design choices), so which part of the interface is most suited for interoperability has to be defined on a case by case analysis.
In this example, we will redirect the log output from the 3d engine [panda3d](https://github.com/panda3d/panda3d.git) to g3log.
build_panda3d.sh
install_panda3d.sh
build_09.sh
09_main.py

