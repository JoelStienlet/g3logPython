#!/usr/bin/env bash

# https://github.com/python/cpython/blob/master/Misc/README.valgrind
#export PYTHONMALLOC=malloc

#valgrind --error-exitcode=1 python3 ./valgrind_journald.py
valgrind --error-exitcode=1 --suppressions=/usr/share/doc/python3-devel/valgrind-python.supp python3 ./valgrind_journald.py
vg_return=$?
echo $vg_return
exit $vg_return
