import cvxpy as cp
import numpy as np

m, n = 5, 10
np.random.seed(1)

A, b = np.random.randn(m,n), np.random.randn(m)

x, z = cp.Variable(n), cp.Variable(n)
obj = cp.Minimize(np.ones([1,n]) @ z)
cons = [A@x == b, z >= x, z >= -x]

P = cp.Problem(obj, cons)
P.solve(verbose=True)

print("===== x =====")
print(x.value)

print("===== A =====")
print(A)

print("===== A * x =====")
print(A@x)

print("===== b =====")
print(b)

