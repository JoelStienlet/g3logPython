#!/usr/bin/env bash

echo "extra argument: $@"
set -x
g++ -g 08_file_descriptor_iface.cpp -o 08_fd_iface -lg3logBindings  -lg3logger "$@"
