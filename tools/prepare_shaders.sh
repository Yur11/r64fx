#!/bin/bash

#Run from src/ directory.

if [ ! -x ./makestring ]
then
    g++ -std=c++11 -O0 ../tools/makestring.cpp -o makestring
fi

for i in `find . -name "*.vert" -o -name "*.frag"`
do
    ./makestring < $i > $i.h
    echo "$i.h"
done

rm ./makestring
