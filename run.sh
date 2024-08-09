#!/usr/bin/env bash
make clean build

for i in {0..9}
do
    make run ARGS="-input=Lena${i}.pgm"
done