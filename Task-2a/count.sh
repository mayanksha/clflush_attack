#!/bin/bash

ENCYPTED_FILE="/home/msharma/build/gnupg-1.4.13/testconf/testdir/hello.txt.gpg"
GPG_BINARY="/home/msharma/build/gnupg-1.4.13/g10/gpg"

gcc -Wall -g -O0  ./spy.c ../lib/util.c -o spy -lm
if [ $? -ne 0 ]
then
    echo "Compile failed!"
    exit -1
fi

for i in `seq 20 2 50`
do
    times=$(( i*10 ))
    echo "Times = ${times}"

    (taskset 0x4 ./spy ${times} > ./temp) & (sleep 0.001 && taskset 0x1  ${GPG_BINARY} -d ${ENCYPTED_FILE} 2>&1 > /dev/null)

    for job in `jobs -p`
    do
        wait $job
    done
    S=$(cat './temp' | grep -o 'S' | wc -l)
    M=$(cat './temp' | grep -o 'M' | wc -l)
    r=$(cat './temp' | grep -o 'r' | wc -l)

    S_orig=$(cat './temp2' | grep -o 'S' | wc -l)
    M_orig=$(cat './temp2' | grep -o 'M' | wc -l)
    r_orig=$(cat './temp2' | grep -o 'r' | wc -l)
    echo "S = ${S}, Orig = ${S_orig}"
    echo "M = ${M}, Orig = ${M_orig}"
    echo "r = ${r}, Orig = ${r_orig}"
    echo `cat temp | wc -c`
    echo ""
done
