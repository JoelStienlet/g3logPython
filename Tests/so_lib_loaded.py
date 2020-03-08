#!/usr/bin/env python3

import g3logPython as log

import os
import subprocess

pid = os.getpid()
subprocess.run(['lsof','-p',str(pid)])


