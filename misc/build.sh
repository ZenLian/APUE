#!/usr/bin/env bash

basepath=$(cd `dirname $0`; pwd)

cd ${basepath}
rm -rf build
mkdir -p build
cd build
cmake .. && make
