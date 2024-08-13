import matplotlib.pyplot as plt
import numpy as np
import math

from matplotlib import rc
rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
## for Palatino and other serif fonts use:
#rc('font',**{'family':'serif','serif':['Palatino']})
rc('text', usetex=True)

# del matplotlib.font_manager.weight_dict['roman']
# matplotlib.font_manager._rebuild()

def Sinc_Kernel(u):
    result = []
    for _u in u:
        if abs(_u) < 0.01:
            v = 1 / np.pi
        else:
            v = np.sin(_u) / ( np.pi * _u)
        result.append(v)
    return result

def Epanechnikov_Kernel(u):
    result = []
    for _u in u:
        v = 3 * max(0, 1 - pow(_u, 2)) / 4
        result.append(v)
    return result

def Silverman_Kernel(u):
    result = []
    for _u in u:
        v = np.exp(-np.abs(_u) / np.sqrt(2)) * np.sin(np.abs(_u) / np.sqrt(2) + np.pi / 4)
        result.append(v)
    return result

def Pinsker_Kernel(u):
    result = []
    for _u in u:
        if abs(_u) < 0.01:
            v = 2 / (3 * np.pi)
        else:
            v = 2 * ( np.sin(_u) - _u * np.cos(_u) ) / ( np.pi * pow(_u, 3) )
        result.append(v)
    return result


def main():
    plt.style.use('_mpl-gallery')

    x_min = -20
    x_max =  20
    x_pitch = 0.01

    u = np.arange(x_min, x_max, x_pitch)

    k_Epane = Epanechnikov_Kernel(u)
    k_Sync = Sinc_Kernel(u)
    k_Silverman = Silverman_Kernel(u)
    k_Pinsker = Pinsker_Kernel(u)

    fig, ax = plt.subplots()

    ax.set_title('Kernel', fontsize=18)
    ax.set_xlabel(r'$u$', fontsize=14)
    ax.set_ylabel(r'$K(u)$', fontsize=14)

    ax.plot(u, k_Epane, color='b', linewidth=3.0, label="Epanechnikov Kernel [p3]")
    ax.plot(u, k_Silverman, color='g', linewidth=3.0, label="Silverman Kernel [p]")
    ax.plot(u, k_Sync, color='r', linewidth=3.0, label="Sinc Kernel (1.32) [p19]")
    ax.plot(u, k_Pinsker, color='m', linewidth=3.0, label="Pinsker Kernel [p27]")

    x_tick = 1
    y_tick = 0.1

    k = k_Epane + k_Silverman + k_Sync + k_Pinsker

    y_min = math.floor(min(k)*10) / 10 - y_tick
    y_max = math.ceil(max(k)*10) / 10 + y_tick

    ax.set(xlim=(x_min, x_max), xticks=np.arange(x_min, x_max, x_tick), ylim=(y_min, y_max), yticks=np.arange(y_min, y_max, y_tick))
    plt.subplots_adjust(left=0.1, bottom=0.1, top=0.9)

    ax.axhline(0, color='k', linewidth=1.0)
    ax.axvline(0, color='k', linewidth=1.0)

    plt.legend(fontsize=14)

    anch_x = x_min + ( x_max - x_min) / 2 + 4 * x_tick
    anch_y = y_min + (y_max - y_min ) / 2

    ep_tex = r'\begin{eqnarray*}K(u) = \frac{3}{4}(1 - u^{2})_{+}\end{eqnarray*}'
    ax.text(anch_x, anch_y + y_tick, ep_tex, color="b", fontsize=20)

    slv_tex = r'\begin{eqnarray*}K(u) = \frac{1}{2}\exp(-|u|/\sqrt{2})\sin(|u|/\sqrt{2} + \pi/4)\end{eqnarray*}'
    ax.text(anch_x, anch_y, slv_tex, color="g", fontsize=20)

    sinc_tex = r'\begin{eqnarray*}K(u) = \frac{\sin u}{\pi u}\quad (u \neq 0), \quad \frac{1}{\pi}\quad (u=0)\end{eqnarray*}'
    ax.text(anch_x, anch_y - y_tick, sinc_tex, color="r", fontsize=20)

    pin_tex = r'\begin{eqnarray*}K(u) = \frac{2}{\pi u^{3}}(\sin u - u \cos u)\quad (u \neq 0), \quad\frac{2}{3\pi}\quad (u=0)\end{eqnarray*}'
    ax.text(anch_x, anch_y - 2 * y_tick, pin_tex, color="m", fontsize=20)

    plt.show()

if __name__ == "__main__":
    main()
