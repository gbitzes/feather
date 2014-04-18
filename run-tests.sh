#!/usr/bin/env bash
set -e
set -x

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release  .. 
make -j4
cd ..

# If an argument has been supplied, run only those tests
if [ $# == 1 ] 
then
        bin/tests --gtest_filter="*$1*"
else
        # Otherwise, run everything
        bin/tests
fi

