#!/usr/bin/env bash

echo "extra argument: $@"
g++ 04_simple_pure_CPP.cpp -o 04_simple -lg3logBindings  -lg3logger "$@"
