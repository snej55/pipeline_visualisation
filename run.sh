#!/usr/bin/bash
cmake -S . -B build -G Ninja
cmake --build build/ -j14
cd build; ./main # run from binary directory