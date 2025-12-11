#!/bin/sh
[ -z $CONFIG ] && config=Release || config="$CONFIG"
[ -z $BUILD_SERVER ] && build_server=1 || build_server="$BUILD_SERVER"

cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_C_FLAGS=-m32 \
    -DCMAKE_CXX_FLAGS=-m32 \
    -DCMAKE_BUILD_TYPE=$config \
    -DSTATIC_STDCXX=true \
    -DBUILD_SERVER=$build_server \
&&
cmake \
    --build build \
    --config $config \
    --parallel $(nproc)
