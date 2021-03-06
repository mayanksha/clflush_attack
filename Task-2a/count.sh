#!/bin/bash

ENCYPTED_FILE="/home/vinayakt/Desktop/7thSem/SecureMemorySystems/assgn1/Task-1a/hello.txt.gpg"
GPG_BINARY="/home/vinayakt/Desktop/7thSem/SecureMemorySystems/assgn1/Task-1a/gnupg-1.4.13/g10/gpg"

gcc -Wall -g -O0  ./spy.c ../lib/util.c -o spy -lm
if [ $? -ne 0 ]
then
    echo "Compile failed!"
    exit -1
fi

for i in `seq 1`
do
    # times=$(( i*10 ))
    times=$(( 350 ))
    echo "Times = ${times}"

    (taskset 0x4 ./spy ${times} > ./temp) & (sleep 0.001 && taskset 0x1  ${GPG_BINARY} -d ${ENCYPTED_FILE} 2>&1 > /dev/null)

    for job in `jobs -p`
    do
        wait $job
    done
    S=$(cat './temp' | head -2 | grep -o 'S' | wc -l)
    M=$(cat './temp' | head -2 | grep -o 'M' | wc -l)
    r=$(cat './temp' | head -2 | grep -o 'r' | wc -l)

    S_orig=$(cat './temp2' | grep -o 'S' | wc -l)
    M_orig=$(cat './temp2' | grep -o 'M' | wc -l)
    r_orig=$(cat './temp2' | grep -o 'r' | wc -l)
    echo "S = ${S}, Orig = ${S_orig}"
    echo "M = ${M}, Orig = ${M_orig}"
    echo "r = ${r}, Orig = ${r_orig}"
    echo `cat temp | head -n2 | wc -c`
    echo ""
    sleep 1
done
