#!/usr/bin/env bash


# /home/travis/build/JoelStienlet/g3logPython
echo "=================="
pwd
ls -al
echo "=================="
echo "build/temp.linux-x86_64-3.6/"
ls build/temp.linux-x86_64-3.6/
echo "=================="
echo "build_bindings/"
ls build_bindings/
echo "=================="
echo "build_bindings/temp.linux-x86_64-3.6/"
ls build_bindings/temp.linux-x86_64-3.6/
echo "=================="
cp build/temp.linux-x86_64-3.6/g3logPython/*.gcda .
cp build_bindings/temp.linux-x86_64-3.6/g3logBindings/*.gcda .
echo "=================="
echo "check if the .gcda files are in the upper directory:"
ls -al
echo "=================="

export CODECOV_TOKEN="b5c50253-e6b1-4508-b7f3-800c25231eda"
bash <(curl -s https://codecov.io/bash)

