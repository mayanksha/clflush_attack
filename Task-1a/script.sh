#!/bin/bash

echo "Compiling receiver!"
gcc -Wall -g -O0  ./sender.c ../lib/util.c -o sender -lm
if [ $? -ne 0 ]
then
    echo "Receiver compilation failed!"
    exit -1
fi
echo "Compiling sender!"
gcc -Wall -g -O0  ./receiver.c ../lib/util.c -o receiver -lm
if [ $? -ne 0 ]
then
    echo "Sender compilation failed!"
    exit -1
fi

for i in `seq 1 10 2000`
do
    times=$(( i*100 ))
    # times=$(( 350 ))

    (taskset 0x4 ./receiver ${times} > ./temp) &
    sender_out=`sleep 0.1 && taskset 0x1 ./sender`

    if [ $? -ne 0 ]
    then
        echo "Sender failed. Exiting!"
        exit -1
    fi

    for job in `jobs -p`
    do
        kill -s SIGINT ${job}
    done
    sleep 0.2
    accu=$(cat './temp' | grep -o 'Accuracy.*' | sed 's/Accuracy = //')
    bps=$(cat './temp' | grep -o 'Data receiving rate.*' | sed 's/Data receiving rate : //' | sed 's/bps//g')
    echo "${times},${accu},${bps}"
    # M=$(cat './temp' | head -2 | grep -o 'M' | wc -l)
    # r=$(cat './temp' | head -2 | grep -o 'r' | wc -l)

    # S_orig=$(cat './temp2' | grep -o 'S' | wc -l)
    # M_orig=$(cat './temp2' | grep -o 'M' | wc -l)
    # r_orig=$(cat './temp2' | grep -o 'r' | wc -l)
    # echo "S = ${S}, Orig = ${S_orig}"
    # echo "M = ${M}, Orig = ${M_orig}"
    # echo "r = ${r}, Orig = ${r_orig}"
    # echo `cat temp | head -n2 | wc -c`
    # echo ""
done
