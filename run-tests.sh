#!/usr/bin/env bash
set -e
set -x

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG  .. 
make -j4
cd ..
bin/tests

