#!/bin/bash

mkdir build
cd build
cmake ..
make
mv server ../
rm -r build
