import numpy as np
import matplotlib.pyplot as plt
import ot

a = np.array([
	0.2,
	0.5,
	0.2,
	0.1,
])

b = np.array([
	0.3,
	0.3,
	0.4,
	0.0
])

C = np.array([
	[0, 2, 2, 2],
	[2, 0, 1, 2],
	[2, 1, 0, 2],
	[2, 2, 2, 0],
])

P = ot.emd(a, b, C)

print('P*=')
print(P)

print('optimal transport cost = {:.1f}'.format((C*P).sum()))

categories = ['dog','cat','tiger','bird']

fig, (ax1, ax2) = plt.subplots(nrows=1, ncols=2, sharey = True)

ax1.bar(categories, a, width=0.5, color='tab:orange')
ax2.bar(categories, b, width=0.5, color='tab:blue')
plt.show()
