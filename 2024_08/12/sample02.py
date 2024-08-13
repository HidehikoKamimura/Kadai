import matplotlib.pyplot as plt
import numpy as np
from matplotlib.collections import LineCollection

def ReadGraphFile(iFilePath):
	cxt = []

	with open(iFilePath) as f:
		for s_line in f:
			pt = s_line.split('\t')
			poly = []
			for p in pt:
				xy = p.split(',')
				if( len(xy) == 2 ) :
					poly.append([int(xy[0]), int(xy[1])])
			cxt.append(poly)

	return np.array(cxt)


def main():

	fig, ax = plt.subplots()
	ax.set_xlim([-1, 10])
	ax.set_ylim([-1, 10])

	#
	# draw routes
	#
	route_np = ReadGraphFile('./path.txt')

	for poly in route_np:
		ln = []
		col = []
		for i in range(0, len(poly)):
			if i < len(poly) - 1 :
				p0 = poly[i]
				p1 = poly[i+1]
				ln.append([p0, p1])
				col.append('lime')
		lc = LineCollection(ln, colors=col, linewidths=1)

		ax.add_collection(lc)

	#
	# draw obstacles
	#
	obstacle_np = ReadGraphFile('./obst.txt')
	for obst in obstacle_np:
		ax.scatter(obst[:, 0], obst[:, 1], marker="o", color='k')

	#
	# draw nodes with weight = 1
	#
	node_w1_np = ReadGraphFile('./w1.txt')
	if node_w1_np.shape[1] > 0 :
		for nd in node_w1_np:
			ax.scatter(nd[:, 0], nd[:, 1], marker="o", c='white', edgecolors='blue')

	#
	# draw nodes with weight = 2
	#
	node_w2_np = ReadGraphFile('./w2.txt')

	if node_w2_np.shape[1] > 0 :
		for nd in node_w2_np:
			ax.scatter(nd[:, 0], nd[:, 1], marker="o", c='white', edgecolors='magenta')

	#
	# draw nodes with weight = 3
	#
	node_w3_np = ReadGraphFile('./w3.txt')
	if node_w3_np.shape[1] > 0 :
		for nd in node_w3_np:
			ax.scatter(nd[:, 0], nd[:, 1], marker="o", c='white', edgecolors='red')

	#
	# draw start, end nodes.
	#
	start_end_np = ReadGraphFile('./start_end.txt')
	if start_end_np.shape[1] > 0 :
		for nd in start_end_np:
			ax.scatter(nd[:, 0], nd[:, 1], marker="o", c='blue')


	plt.show()

if __name__ == "__main__":
    main()
