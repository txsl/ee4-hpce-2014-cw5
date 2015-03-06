#!/bin/bash

LENS="8 16 128 1024 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608";

for w in $LENS; do
    for h in $LENS; do
        size=$(( w * h * 1 /8 ))
        echo $size; 
        /usr/bin/time -f %e cat /dev/zero | head -c $size | ../bin/process $w $h 1 8 > /dev/null
    done
done