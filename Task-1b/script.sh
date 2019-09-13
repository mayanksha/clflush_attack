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
    for j in `seq 5`
    do
        (taskset 0x4 ./receiver ${times} > ./temp) &
        sender_out=`sleep 0.1 && taskset 0x1 ./sender`
        if [ $? -ne 0 ]
        then
            echo "Sender failed. Exiting!"
            exit -1
        fi

        for i in `seq 1 10 200000`; do
            echo "" > /dev/null
        done

        for job in `jobs -p`
        do
            kill -s SIGINT ${job}
            wait ${job}
        done
        accu=$(cat './temp' | grep -o 'Bytes received : .*' | sed 's/Bytes received : //' | sed 's/ bytes//')
        bps=$(cat './temp' | grep -o 'Data receiving rate.*' | sed 's/Data receiving rate : //' | sed 's/bps//g')
        echo "${times},${accu},${bps}"
    done
done
