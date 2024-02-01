#!/bin/bash

mkdir -p build
cd build
cmake -DENABLE_TESTS=On -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release .. && make -j
cd -
