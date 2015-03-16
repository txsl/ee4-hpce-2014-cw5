#!/bin/bash

LENS="8 16 128 1024 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608";

export HPCE_CL_SRC_DIR=`pwd`/../src

echo "function_name,width,height,bits,levels,time" >> output.csv

for w in $LENS; do
#    if [ `echo "${w}*1 % 64" | bc` -eq 0 ]
#   then
        size=$(( w * h * 1 /8 ))
        echo $size $w $h;
        realtime=$(( time ( cat /dev/zero | head -c ${size} | ../bin/process_old ${w} ${w} 1 8 ) > /dev/null ) 2>&1 > /dev/null | grep real );
        echo $realtime;
        echo "process_old,${w},${w},1,8,${realtime}" >> output.csv
#fi
done
