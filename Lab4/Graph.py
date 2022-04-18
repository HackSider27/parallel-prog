import numpy as np
import matplotlib.pyplot as plt

f = open('data.txt', 'r')
x = []
y = []
z = []
v = []

x.append(int(f.readline()))
y.append(float((f.readline()[:-1])))
z.append(1)
v.append(1)
#print(y[0])
for i in range (1, 16):
    x.append(int(f.readline()))
    y.append((float(f.readline()[:-1])))
    z.append(y[0]/y[i])
    v.append(y[0]/(x[-1]*y[i]))
	#print(y[i])
f.close()

data = np.column_stack((x, y))

fig, (ax1, ax2, ax3) = plt.subplots(nrows=1, ncols=3, figsize=(16, 4))

ax1.scatter(x=x, y=y, c='b')
ax1.set_title('RESULT:')
ax1.set_xlabel('Threads')
ax1.set_ylabel('Average time')

ax2.scatter(x=x, y=z, c='b')
ax2.set_title('RESULT:')
ax2.set_xlabel('Threads')
ax2.set_ylabel('Accelarition')

ax3.scatter(x=x, y=v, c='b')
ax3.set_title('RESULT:')
ax3.set_xlabel('Threads')
ax3.set_ylabel('Efficiency')

plt.show()
