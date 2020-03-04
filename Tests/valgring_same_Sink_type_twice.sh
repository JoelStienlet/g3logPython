#!/usr/bin/env bash

# https://github.com/python/cpython/blob/master/Misc/README.valgrind
#export PYTHONMALLOC=malloc

valgrind --suppressions=/usr/share/doc/python3-devel/valgrind-python.supp python3 ./same_Sink_type_twice.py

