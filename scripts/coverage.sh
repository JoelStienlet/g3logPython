#!/usr/bin/env bash

for filename in `find . | egrep '\.cpp'`;
do
  gcov -o . $filename;
  # > /dev/null;
done

export CODECOV_TOKEN="b5c50253-e6b1-4508-b7f3-800c25231eda"
bash <(curl -s https://codecov.io/bash)

