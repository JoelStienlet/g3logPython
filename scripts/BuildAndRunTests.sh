#!/usr/bin/env bash

pwd=`pwd`
echo "$pwd"
ls
sudo BUILD_WITH_COVERAGE=1 python3 setup.py install
ls -al build
cd Examples
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
whoami
sudo chown -R travis:travis build
./Two_Sinks.py

