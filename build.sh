#!/bin/bash

set -e

for dir in $(find ./ -mindepth 1 -maxdepth 1 -type d); do
    cd $dir
    if [ -f ./SConstruct ]; then
        echo "== BUILDING $dir =="
        scons -c
        scons
    fi
    cd -
done
