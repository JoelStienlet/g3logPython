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
./00_simple.py
./01_journald.py
./02_logrotate.py
./03_Three_Sinks.py
cd $pwd
cd Tests
./journald_sink_check_journalctl.py
./logrotate_two_directories.py
# TODO : add valgrind here
#valgrind --error-exitcode=1 --suppressions=/usr/share/doc/python3-devel/valgrind-python.supp python3 ./valgrind_journald.py
./WARNING_bad_level.py
./EXCEPT_same_name_Error.py
./EXCEPT_journal_Sink_type_twice.py
./Fatal.py || { echo "ERROR return value"; true; }

echo "Tests Finished."
