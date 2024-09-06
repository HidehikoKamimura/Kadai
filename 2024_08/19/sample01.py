import cvxpy as cp

x1 = cp.Variable()
x2 = cp.Variable()

obj = cp.Maximize(20*x1 + 60*x2)

cons = [
	5*x1 + 4*x2	<=	80,
	2*x1 + 4*x2	<=	40,
	2*x1 + 8*x2	<=	64,
	x1			>=	0,
	x2			>=	0,
]

p = cp.Problem(obj, cons)
p.solve(verbose=True)
print(x1.value, x2.value)
