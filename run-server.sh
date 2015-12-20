#!/usr/bin/env bash
set -e
set -x

if [ -z "$NOCOMPILE" ];
then
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. 
    make -j4
    cd ..
fi

export GTEST_COLOR=yes
bin/server $1
