import numpy as np
from matplotlib import pyplot as plt

a = np.loadtxt(open("./data.csv", "rb"), delimiter=",", skiprows=2)
accu = a[:,:2]
print(accu)
plt.scatter(accu[:,:1], accu[:,1:2])
plt.plot(accu[:,:1], accu[:,1:2])
plt.title("Accuracy (%) vs cycles waited in loop (parameter)")
plt.show()
