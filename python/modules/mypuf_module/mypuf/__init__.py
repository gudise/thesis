from numpy.random import randint as np_randint, permutation as np_permutation
from numpy import pi as np_pi, cos as np_cos, sin as np_sin, histogram as np_histogram, mean as np_mean
from scipy.stats import binom as sp_binom
from matplotlib.pyplot import plot as plt_plot, bar as plt_bar, tick_params as plt_tick_params, gca as plt_gca, axis as plt_axis, show as plt_show, savefig as plt_savefig
from mytensor import *



def hamming(array1, array2, porciento=False):
    """
    Esta función calcula la distancia de Hamming de dos arrays de símbolos
    """
    if len(array1) != len(array2):
        print("ERROR en 'hamming': arrays de distinto tamaño")
        return -1
    result=0
    for i in range(len(array1)):
        if array1[i] != array2[i]:
            result+=1
    if porciento:
        return result/len(array1)*100
    else:
        return result

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
    def __init__(self, topol, instancias, retos=[]):
        """
        """
        self.topol = topol
        if type(instancias)==type([]):
            self.instancias = instancias # lista de objetos TENSOR, cada uno de los cuales contiene la "medida" (quizá simulación) de una instancia PUF.
        else:
            self.instancias=[instancias]
            
        self.N_inst = len(self.instancias)
        self.N_pdl = self.instancias[0].shape[self.instancias[0].axis.index('pdl')]
        self.N_rep = self.instancias[0].shape[self.instancias[0].axis.index('rep')]
        self.N_osc = topol.N_osc
        self.N_bits_partial = len(topol.grafo) # Número de bits sin tener en cuenta el "boost" PDL.
        self.N_bits = self.N_bits_partial*self.N_pdl # Número de bits real de cada respuesta.
        
        if len(retos)>0:
            self.retos = retos
            self.N_retos = len(self.retos)
        self.data=[[[] for j in self.instancias] for i in self.retos]
        for i_reto,reto in enumerate(self.retos):
            for i_inst,inst in enumerate(self.instancias):
                for i_rep in range(self.N_rep):
                    aux=[]
                    for i_pdl in range(self.N_pdl):
                        for bit in range(self.N_bits_partial):
                            if inst.item(osc=reto[bit][0], rep=i_rep, pdl=i_pdl) > inst.item(osc=reto[bit][1], rep=i_rep, pdl=i_pdl):
                                aux.append(0)
                            else:
                                aux.append(1)
                    self.data[i_reto][i_inst].append(aux)
        self.intradist_set=[]
        self.interdist_set=[]
        
    def addreto(self, reto):
        self.N_retos+=1
        self.retos.append(reto)
        self.data.append([[] for j in self.instancias])
        for i_inst,inst in enumerate(self.instancias):
            for i_rep in range(self.N_rep):
                aux=[]
                for i_pdl in range(self.N_pdl):
                    for bit in range(self.N_bits_partial):
                        if inst.item(osc=reto[bit][0], rep=i_rep, pdl=i_pdl) > inst.item(osc=reto[bit][1], rep=i_rep, pdl=i_pdl):
                            aux.append(0)
                        else:
                            aux.append(1)
                self.data[-1][i_inst].append(aux)
                
    def print(self, print_retos=True):
        if print_retos:
            print("# reto,inst,rep = reto = respuesta\n")
        else:
            print("# reto,inst,rep = respuesta\n")
        for i_reto,reto in enumerate(self.retos):
            for i_inst,inst in enumerate(self.instancias):
                for i_rep in range(self.N_rep):
                    if print_retos:
                        print(f" {i_reto},{i_inst},{i_rep} = {reto} = {' '.join(str(bit) for bit in self.data[i_reto][i_inst][i_rep])}")
                    else:
                        print(f" {i_reto},{i_inst},{i_rep} = {' '.join(str(bit) for bit in self.data[i_reto][i_inst][i_rep])}")
            
    def intradist(self):
        for i_reto in range(self.N_retos):
            for i_inst in range(self.N_inst):
                for i_rep in range(self.N_rep):
                    for j_rep in range(i_rep+1,self.N_rep,1):
                        self.intradist_set.append(hamming(self.data[i_reto][i_inst][i_rep],self.data[i_reto][i_inst][j_rep]))
        self.intradist_hist = np_histogram(self.intradist_set, bins=self.N_bits, range=(0,self.N_bits), density=True)[0]
        self.intradist_media = np_mean(self.intradist_set)
                        
    def interdist(self):
        for i_reto in range(self.N_retos):
            for i_inst in range(self.N_inst):
                for j_inst in range(i_inst+1,self.N_inst,1):            
                    for i_rep in range(self.N_rep):
                        self.interdist_set.append(hamming(self.data[i_reto][i_inst][i_rep],self.data[i_reto][j_inst][i_rep]))
        self.interdist_hist = np_histogram(self.interdist_set, bins=self.N_bits, range=(0,self.N_bits), density=True)[0]
        self.interdist_media = np_mean(self.interdist_set)
        
    def dibujar_hist(self, ajuste=True):
        if len(self.intradist_set)>0:
            plt_bar([i for i in range(self.N_bits)], self.intradist_hist)
            if ajuste:
                plt_plot([i for i in range(self.N_bits)], [sp_binom.pmf(i, self.N_bits, self.intradist_media/self.N_bits) for i in range(self.N_bits)], 'b')
        if len(self.interdist_set)>0:
            plt_bar([i for i in range(self.N_bits)], self.interdist_hist)
            if ajuste:
                plt_plot([i for i in range(self.N_bits)], [sp_binom.pmf(i, self.N_bits, self.interdist_media/self.N_bits) for i in range(self.N_bits)], 'b')
        plt_show()