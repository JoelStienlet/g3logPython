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
ls
mkdir -p build_bindings && cd  build_bindings 
cmake -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .. && make && sudo make install
check_error $? !!
cd $pwd
sudo BUILD_WITH_COVERAGE=1 python3 setup.py install
check_error $? !!
ls -al
whoami
sudo chown -R travis:travis build
ls -al
export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH
cd Examples
pwd
./04_simple_build.sh --coverage
check_error $? !!
echo "======= BUILD FINISHED ======="
echo "running examples..."
./00_simple.py
check_error $? !!
./01_journald.py
check_error $? !!
./02_logrotate.py
check_error $? !!
./03_Three_Sinks.py
check_error $? !!
./04_simple
check_error $? !!
cd $pwd
cd Tests
pwd
echo "running tests..."
./EXCEPT_journal_Sink_type_twice.py
check_error $? !!
./EXCEPT_same_name_Error.py
check_error $? !!
./Fatal.py || { echo "ERROR return value"; true; }

./journald_sink_check_journalctl.py
check_error $? !!
./logrotate_two_directories.py
check_error $? !!
#./valgrind_journald.sh
# TODO : install valgrind 
./WARNING_bad_level.py
check_error $? !!

echo "Tests Finished."
