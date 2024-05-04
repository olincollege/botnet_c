#!/bin/bash

mkdir build
cd build
cmake ..
make
mv auto_client ../
rm -r build
