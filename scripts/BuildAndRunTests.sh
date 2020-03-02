#!/usr/bin/env bash

pwd=`pwd`
echo "$pwd"
ls
sudo BUILD_WITH_COVERAGE=1 python3 setup.py install
ls -al
whoami
sudo chown -R travis:travis build
ls -al
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
cd Examples
./Two_Sinks.py
cd $pwd
cd Tests
./_bad_level.py || {echo "ERROR return value"; true}
./same_Sink_type_twice.py || {echo "ERROR return value"; true}
./same_name_Error.py || {echo "ERROR return value"; true}
./Fatal.py || {echo "ERROR return value"; true}
