#!/usr/bin/env bash

pwd=`pwd`
echo "$pwd"
ls
sudo python3 setup.py install
cd Examples
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
./Two_Sinks.py

