import numpy as np
from matplotlib import pyplot as plt
from textwrap import wrap

a = np.loadtxt(open("./data.csv", "rb"), delimiter=",", skiprows=2)

avgd_out = [np.mean(a[i:i+5], axis=0) for i in range(0, len(a), 5)]
avgd_out = np.array(avgd_out)
clocks = avgd_out[:,0:1]
accu = avgd_out[:,1:2] / 8
bps = avgd_out[:,2:] / 8

fig1, ax1 = plt.subplots(figsize=(10,8))
ax1.scatter(clocks, avgd_out[:,1:2])
ax1.plot(clocks, avgd_out[:,1:2])
ax1.set_title("\n".join(wrap("Number of bytes received vs Number of Cycles waited (sending 60 different characters, total 50 Bytes)", 50)))
ax1.set_xlabel("Number of Cycles waited before another probe")
ax1.set_ylabel("Number of bytes received")
#  fig1.savefig('./accuracy_50.png')

fig2, ax2 = plt.subplots(figsize=(10,8))
ax2.scatter(clocks, bps)
ax2.plot(clocks, bps)
ax2.set_title("\n".join(wrap("Bandwidth vs Number of Cycles waited (sending 60 different characters, total 50 Bytes)", 50)))
ax2.set_xlabel("Number of Cycles waited before another probe")
ax2.set_ylabel("Bandwidth (Bytes/sec) ")
#  fig2.savefig('./bandwidth_50.png')
plt.show()
