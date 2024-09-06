import cvxpy as cp
import numpy as np

m, n = 5, 10
np.random.seed(1)

A, b = np.random.randn(m,n), np.random.randn(m)

x = cp.Variable(n)

obj = cp.Minimize( cp.norm(x, 1) )
cons = [ A @ x == b]

P = cp.Problem(obj, cons)
P.solve(verbose=False)

print("====x=====");
print(x.value)

print("====A=====");
print(A)

print("====A*x=====");
for a in A:
	v = 0.0
	for i, e in enumerate(a):
		v += e * x[i].value
	print(v)

print("====b=====");
print(b)
