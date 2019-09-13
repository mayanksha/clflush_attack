# Import the necessary packages and modules
import matplotlib.pyplot as plt
import numpy as np

import sys
# Prepare the data
N=5000
s = [0]*N
s_ticks = [0]*N
r = [0]*N
r_ticks = [0]*N
m = [0]*N
m_ticks = [0]*N

i = 0
k = 500
scount=0
rcount=0
mcount=0
with open("Task-2a.data", "r") as ifile:
	for line in ifile:
		if line.strip() and line != "\n":
			print(line)
			line = line.split('\n')[0]
			case = line.split(',')[2]
			# print(case)
			# print(line.split('\n')[0].split(','))
			time = int(line.split(',')[1])
			tick = line.split(',')[0]
			if(tick == ''):
				continue
			tick = int(tick)
			# print(case)
			if(case == 'S'):
				print(case)
				s[scount]=time
				s_ticks[scount]=tick
				scount = scount +1
			if(case == 'r'):
				r[rcount]=time
				r_ticks[rcount]=tick
				rcount = rcount +1
			if(case == 'M'):
				m[mcount]=time
				m_ticks[mcount]=tick
				mcount = mcount +1

N = 110000
miss_s = [0]*N
miss_s_ticks = [0]*N
miss_r = [0]*N
miss_r_ticks = [0]*N
miss_m = [0]*N
miss_m_ticks = [0]*N

miss_s_count=0
miss_m_count=0
miss_r_count=0
i = 0
with open("Task-2a-missdata.data", "r") as ifile:
	for line in ifile:
		if line.strip() and line != "\n":
			print(line)
			line = line.split('\n')[0]
			if(len(line.split(',')) < 3 or line.split(',')[0] == '' or line.split(',')[1] == '' or line.split(',')[2] == ''):
				continue
			case = line.split(',')[2]
			# print(case)
			# print(line.split('\n')[0].split(','))
			time = int(line.split(',')[1])
			tick = line.split(',')[0]
			tick = int(tick)
			if(time > 300):
				continue
			# print(case)
			if(case == 'S'):
				print(case)
				miss_s[miss_s_count]=time
				miss_s_ticks[miss_s_count]=tick
				miss_s_count = miss_s_count +1
			if(case == 'r'):
				miss_r[miss_r_count]=time
				miss_r_ticks[miss_r_count]=tick
				miss_r_count = miss_r_count +1
			if(case == 'M'):
				miss_m[miss_m_count]=time
				miss_m_ticks[miss_m_count]=tick
				miss_m_count = miss_m_count +1

# print(miss_m_count,miss_r_count,miss_s_count)
# # Plot the data
# print(s_ticks,m_ticks)
D = 400
L= 100
area = area = np.pi*20
plt.scatter(s_ticks[:2*L], s[:2*L],s= area,c='red', alpha=0.5,label = "Square")
plt.scatter(r_ticks[:L], r[:L],c='blue',s= area, alpha=0.5,label = "Reduce")
plt.scatter(m_ticks[:L], m[:L],c='green',s= area, alpha=0.5,label = "Multiply")
plt.scatter(miss_s_ticks[:D], miss_s[:D],c='red', alpha=0.5)
plt.scatter(miss_r_ticks[:D], miss_r[:D],c='blue', alpha=0.5)
plt.scatter(miss_m_ticks[:D], miss_m[:D],c='green', alpha=0.5)
x_val=[0]*600
i=0
for i in range(600):
	x_val[i]=i
y_val=[61]*600
# plt.scatter(x_val,y_val, c="black",label = "Threshold")
plt.plot(x_val,y_val, color="black", linestyle="--",label = "Threshold")
# plt.plot(s_ticks[:1000],m_ticks[:1000])
# plt.hist(x, bins=10)
plt.ylabel('Latency (in cycles)')
plt.xlabel('Iterations')
# Add a legend
plt.title('Cache hits for Square/Reduce/Multiply')
plt.legend()

# Show the plot
plt.show()
