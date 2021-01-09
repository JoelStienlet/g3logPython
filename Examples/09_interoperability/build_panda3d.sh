#!/usr/bin/env bash

#sudo pip3 install panda3d
# -> won't install the C++ header files (used here)

# on fedora: dnf -y install rpm-build fakeroot mesa-libGL-devel
# ( mesa-libGLU-devel )
# debian: dpkg-deb


pwd
git clone https://github.com/panda3d/panda3d.git
ls
cd panda3d
# d39d40b1fc4dee02ad913e033790a5a6a591c659  --> 1.10.8 
git checkout d39d40b1fc4dee02ad913e033790a5a6a591c659
python3 makepanda/makepanda.py --threads 3 --everything --installer --no-opencv 
# --no-egl --no-gles --no-gles2

