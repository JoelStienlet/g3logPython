#!/usr/bin/env bash

echo "extra argument: $1"
g++ 04_simple_pure_CPP.cpp -o 04_simple -lg3logBindings  -lg3logger $1
