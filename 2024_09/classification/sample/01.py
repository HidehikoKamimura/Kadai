import numpy as np

d = np.array(
	[
		[0,	0.34,	-1.81],
		[0,	0.45,	-1.74],
		[0,	-0.68,	-2.76],
		[0,	0.02,	-4.12],
		[0,	-1.80,	-0.88],
		[1,	2.32,	0.06],
		[1,	3.11,	2.97],
		[1,	1.06,	-0.41],
		[1,	2.99,	2.48],
		[1,	2.98,	-1.51],
	]
)

print(d[:5])
print(d[5:10])

d1 = d[:5]
d2 = d[5:10]

print(np.mean(d1, axis=0))
print(np.mean(d2, axis=0))

# x1 = d1[
cv_d = np.cov(d1, d2)

print('================')
print(cv_d)

cv_d_inv = np.linalg.inv(cv_d)

print('================')
print(cv_d_inv)

i = cv_d_inv * cv_d
print('================')
print(i)
