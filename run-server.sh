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
JEMALLOC=/usr/lib/x86_64-linux-gnu/libjemalloc.so.1

if [ ! -f $JEMALLOC ]
then
    echo "Error: libjemalloc does not exist, install libjemalloc1 package"
    exit 1
fi

export LD_PRELOAD=$JEMALLOC:$LD_PRELOAD
bin/server $1