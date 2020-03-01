#!/usr/bin/env bash

pwd=`pwd`
echo "$pwd"
ls
sudo python3 setup.py install
cd Examples
./Two_Sinks.py



