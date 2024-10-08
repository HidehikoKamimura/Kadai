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

	ax.set_xticks(np.arange(0, 10, 1), np.arange(1, 11, 1))
	ax.set_yticks(np.arange(0, 10, 1), np.arange(1, 11, 1))


	#
	# draw routes
	#
	route_np = ReadGraphFile('./path.txt')

	brk = []
	for i, poly in enumerate(route_np):
		ln = []
		broken = 0
		for j in range(0, len(poly)):
			if j < len(poly) - 1 :

				vertical_prv = -1

				if len(ln) > 0:
					prv0 = ln[len(ln)-1][0]
					prv1 = ln[len(ln)-1][1]

					if prv0[1] == prv1[1] :
						vertical_prv = 1
					else:
						vertical_prv = 0

				p0 = poly[j]
				p1 = poly[j+1]

				ln.append([p0, p1])

				vertical_cur = -1

				if p0[1] == p1[1] :
					vertical_cur = 1
				else :
					vertical_cur = 0

				if vertical_prv != -1 and vertical_cur != -1 :
					if vertical_prv != vertical_cur :
						broken += 1

		brk.append(broken)

	brk_min = min(brk)
	print('minimum broken count : ' + str(brk_min))

	for i, poly in enumerate(route_np):
		ln = []
		col = []

		ln_style = "solid"
		ln_width = 3
		ln_alpha = 1.0

		for j in range(0, len(poly)):
			if j < len(poly) - 1 :
				p0 = poly[j]
				p1 = poly[j+1]
				ln.append([p0, p1])
				if brk[i] == brk_min :
					col.append('lime')
				else:
					col.append('yellowgreen')
					ln_style = "dotted"
					ln_width = 1
					ln_alpha = 0.1

		lc = LineCollection(ln, colors=col, linewidths=ln_width, linestyle=ln_style, alpha=ln_alpha)
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
			ax.annotate('start', xy = (nd[0][0]-0.5, nd[0][1]-0.5))
			ax.annotate('end', xy = (nd[1][0], nd[1][1]+0.2))

	plt.show()

if __name__ == "__main__":
    main()
