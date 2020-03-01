#!/usr/bin/env bash

pwd=`pwd`

#sudo apt --fix-broken install
sudo apt-get update
sudo apt-get install -y libboost-all-dev python3-dev python3-pybind11 python3-setuptools


mkdir g3log-install && cd g3log-install
git clone https://github.com/KjellKod/g3log.git -b master
cd g3log
#unzip -o 3rdParty/gtest/gtest-1.7.0.zip -d 3rdParty/gtest
mkdir -p  build_travis
cd build_travis
#cmake  -DADD_G3LOG_UNIT_TEST=ON ..
cmake ..
make -j && sudo make install

cd $pwd
mkdir g3sinks-install && cd g3sinks-install
git clone https://github.com/JoelStienlet/g3sinks.git -b top_level_cmake
cd g3sinks
mkdir -p  build_travis
cd build_travis
cmake ..
make -j && sudo make install

