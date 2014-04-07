#!/usr/bin/env bash
set -e
set -x

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. 
make -j4
cd ..
bin/tests

