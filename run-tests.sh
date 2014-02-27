#!/usr/bin/env bash
set -e
set -x

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. 
make
cd ..
bin/tests

