import sys
import subprocess
command_pref = "(gcc -g -O0 sender.c ../lib/util.c -o s.out && taskset 0x4 ./s.out) & (gcc -g -O0 receiver.c ../lib/util.c -o r.out && taskset 0x1 ./r.out "
command_suf = " )"

for i in range(0,10000,100):
	command_temp = command_pref+str(i)+command_suf
	output,error = subprocess.Popen(command_temp, shell=True, executable="/bin/bash", stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
	acc = (len(output)-1)/100
	print(i,acc)