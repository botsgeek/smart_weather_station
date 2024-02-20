#! /bin/bash

set -e
CONTAINER_NAME=smart_weather_station:0.1
SOURCE_DIR=/usr/src/myapp
if [ -z "$1" ]; then
    echo "Build type not specified"
    exit 1
fi
echo "pwd is `pwd`"
# COMMAND="docker run --rm -v `pwd`:/usr/src/myapp $CONTAINER_NAME bash -c 'cd $SOURCE_DIR && make && ls'"
# echo "docker command is: $COMMAND"
if [ "$1" == "debug" ]; then  
        docker run --rm -v `pwd`:/usr/src/myapp   $CONTAINER_NAME bash -c make
fi

if [ "$1" == "clean" ]; then  
        docker run --rm -v `pwd`:/usr/src/myapp  $CONTAINER_NAME bash -c "make clean"
fi

if [ "$1" == "release" ]; then
    echo "release build not avaialble yet"
fi