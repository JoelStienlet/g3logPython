#!/usr/bin/env bash

pwd=`pwd`

#sudo apt --fix-broken install
sudo apt-get update
sudo apt-get install -y libboost-all-dev python3-setuptools python3-pip python3-dev
#  python3-pybind11  <-- does not work: PYBIND11_MODULE causes syntax errors...
# see https://github.com/pybind/pybind11/issues/890#issuecomment-544160220

#
# \
# |-- g3log-install
# |
# |-- g3sinks-install
# |
# |-- pybind11-install
#

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
#git clone https://github.com/JoelStienlet/g3sinks.git -b top_level_cmake
git clone https://github.com/KjellKod/g3sinks.git -b master
cd g3sinks
mkdir -p  build_travis
cd build_travis
# change boost shared library option to use boost as a shared library (the distro fedora for example ships only the dynamic lib version)
sed -i -e '/Boost_USE_STATIC_LIBS/s/ON/OFF/' sink_logrotate/CMakeLists.txt
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j && sudo make install


cd $pwd
mkdir pybind11-install && cd pybind11-install
git clone https://github.com/pybind/pybind11.git
cd pybind11
sudo pip3 install .

# cleanup:
cd $pwd
sudo rm -rf g3log-install
sudo rm -rf g3sinks-install
sudo rm -rf pybind11-install

