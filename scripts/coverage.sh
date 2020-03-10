#!/usr/bin/env bash

pwd
# /home/travis/build/JoelStienlet/g3logPython
ls -al
ls build/temp.linux-x86_64-3.6/
cp build/temp.linux-x86_64-3.6/g3logPython/*.gcda .
cp build/temp.linux-x86_64-3.6/g3logBindings/*.gcda .
ls -al

export CODECOV_TOKEN="b5c50253-e6b1-4508-b7f3-800c25231eda"
bash <(curl -s https://codecov.io/bash)

