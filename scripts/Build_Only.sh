#!/usr/bin/env bash

check_error() {
    ret_code=$1
    full_command=${@:2}
    if [ $ret_code -ne 0 ]; then
        echo "\"${full_command}\""
        echo "ERROR Code: ${ret_code}."
        exit $ret_code
    fi
}

# enable !!
set -o history -o histexpand

curdir=${PWD##*/}
if [ $curdir != "g3logPython" ]; then
  echo "not in the right directory. Should be in g3logPython. abord."
  pwd
  exit 1
fi

pwd=`pwd`
echo "$pwd"

sudo rm -rf build_bindings
sudo rm -rf build

mkdir -p build_bindings && cd  build_bindings 
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .. && make && sudo make install
check_error $? !!
cd $pwd
sudo BUILD_WITH_COVERAGE=0 python3 setup.py install
check_error $? !!


