from numpy.random import randint as np_randint, permutation as np_permutation
from numpy import pi as np_pi, cos as np_cos, sin as np_sin
from matplotlib.pyplot import plot as plt_plot, tick_params as plt_tick_params, gca as plt_gca, axis as plt_axis, show as plt_show, savefig as plt_savefig
from mytensor import *


class PUFTOPOL:
    """
    Este objeto contiene una función que devuelve una lista de valores enteros, la cual representa las parejas de
    celdas (osciladores) empleadas para construir una respuesta binaria
    """
    def __init__(self, N_osc, topol='all_pairs', custom=[[]]):
        self.topol = topol
        self.N_osc = N_osc
        self.grafo = []
        if topol=='all_pairs' or topol=='ap':
            for i in range(self.N_osc):
                for j in range(i+1, self.N_osc, 1):
                    self.grafo.append([i,j])
        elif topol=='n/2':
            for i in range(0, self.N_osc, 2):
                self.grafo.append([i,i+1])
        elif topol=='n-1':
            for i in range(0, self.N_osc-1, 1):
                self.grafo.append([i,i+1])
        elif topol=='custom':
            self.grafo = custom
            
    def dibujar(self, color_vert='black', size_vert=20, color_link='tab:gray', size_link=1.5, export_pdf=False):
        angulos=[2*np_pi/self.N_osc*i for i in range(self.N_osc)]
        x_coords=[np_cos(angulo) for angulo in angulos]
        y_coords=[np_sin(angulo) for angulo in angulos]
        for arista in self.grafo:
            plt_plot([x_coords[arista[0]], x_coords[arista[1]]],[y_coords[arista[0]], y_coords[arista[1]]], color=color_link, linewidth=size_link)
        plt_plot(x_coords,y_coords,'o', color=color_vert, markersize=size_vert) # el parámetro 'ms' da cuenta del tamaño de los círculos.
        # Selecting the axis-X making the bottom and top axes False.
        plt_tick_params(axis='x', which='both', bottom=False, top=False, labelbottom=False)
        # Selecting the axis-Y making the right and left axes False
        plt_tick_params(axis='y', which='both', right=False, left=False, labelleft=False)
        # Iterating over all the axes in the figure
        # and make the Spines Visibility as False
        for pos in ['right', 'top', 'bottom', 'left']:
            plt_gca().spines[pos].set_visible(False)
        plt_axis('square')
        if export_pdf:
            plt_savefig(export_pdf, transparent=True)
        else:
            plt_show()
            
    def __call__(self, random=True):
        """
        Al llamar a un objeto "PUFTOPOL" se devuelve una lista de parejas que conforman una iteración aleatoria de la
        topología (i.e., después de permutar la lista de osciladores). Si se utiliza random=False se devuelve el grafo
        original.
        """
        if not random:
            return [pair for pair in self.grafo]
        else:
            perm = np_permutation(self.N_osc)
            return [[perm[pair[0]], perm[pair[1]]] for pair in self.grafo]


class PUFEXP:
    """
    Estos objetos son esencialmente un TENSOR que solo almacena valores binarios, y cuyos ejes están predefinidos:
    axis=['retos', 'inst', 'rep']. Además puede guardar una lista que relacione el índice del eje 'retos' con una
    representación de cada reto (en principio arbitraria, en la práctica será una lista de parejas de las celdas
    empleadas para obtener cada bit).
    """
    def __init__(self):
        """
        """