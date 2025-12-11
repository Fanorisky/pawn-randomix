#!/bin/bash

# Available configs: Debug, [RelWithDebInfo], Release
[[ -z "$CONFIG" ]] \
&& config=RelWithDebInfo \
|| config="$CONFIG"
# Available options: [true], false
[[ -z "$BUILD_SERVER" ]] \
&& build_server=1 \
|| build_server="$BUILD_SERVER"

docker build \
    -t omp-easing-functions/build:ubuntu-18.04 \
    build_ubuntu-18.04/ \
|| exit 1

folders=('build')
for folder in "${folders[@]}"; do
    if [[ ! -d "./${folder}" ]]; then
        mkdir ${folder} &&
        chown 1000:1000 ${folder} || exit 1
    fi
done

docker run \
    --rm \
    -t \
    -w /code \
    -v $PWD/..:/code \
    -v $PWD/build:/code/build \
    -e CONFIG=${config} \
    -e BUILD_SERVER=${build_server} \
    omp-easing-functions/build:ubuntu-18.04
