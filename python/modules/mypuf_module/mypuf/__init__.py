from numpy.random import randint as np_randint, permutation as np_permutation
from numpy import pi as np_pi, cos as np_cos, sin as np_sin, histogram as np_histogram, mean as np_mean, std as np_std
from scipy.stats import binom as sp_binomial, norm as sp_normal, fit as sp_fit
from scipy.interpolate import interp1d as sp_interp1d
from matplotlib.pyplot import plot as plt_plot, bar as plt_bar, tick_params as plt_tick_params, gca as plt_gca, axis as plt_axis, show as plt_show, savefig as plt_savefig, yscale as plt_yscale
from mytensor import *


def hamming(array1, array2, porciento=False):
    """Esta función calcula la distancia de Hamming de dos arrays de símbolos.
    
    :param array1: Lista de caracteres de entrada
    :type array1: str
    
    :param array2: Lista de caracteres de entrada
    :type array2: str
    
    :param porciento: Si `True` devuelve la salida en tanto por ciento., por defecto `False`
    :type porciento: bool, opcional
    
    :return: Número de símbolos diferentes entre las entradas
    :rtype: float
    
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


class PufTopol:
    """Este objeto contiene una función que devuelve una lista de valores enteros, la cual representa las parejas de
    celdas (osciladores) empleadas para construir una respuesta binaria
    
    :param N_osc: Número de celdas de que consta la PUF.
    :type N_osc: int
    
    :param topol: Este parámetro indica la topología de la PUF. Las opciones admitidas son:
        * `all_pairs` | `ap`, todas las comparaciones posibles
        * `n/2`, comparaciones sin repetir osciladores
        * `n-1`, comparaciones repitiendo un oscilador cada vez
        * `custom`, comparaciones 'custom' tal y como se dan en el parámetro "custom"
    :type topol: str, opcional
    
    :param custom: Lista de parejas; cada elemento de "custom" es una lista de dos osciladores a comparar
    :type custom: list, opcional
    """
    def __init__(self, N_osc, topol='all_pairs', custom=[[]]):
        """Método de inicialización
        """
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
        """Dibuja un grafo con la topología
        
        :param color_vert: Color de los vértices del grafo, por defecto `black`
        :type color_vert: str, opcional
        
        :parram size_vert: Tamaño de los vértices del grafo, por defecto `20`
        :type size_vert: float, opcional
        
        :param color_link: Color de los link del grafo, por defecto `tab:gray`
        :type color_link: str, opcional
        
        :param size_link: Tamaño de los link del grafo, por defecto `1.5`
        :type size_link: float, opcional
        
        :param export_pdf: Nombre del fichero .pdf en el cual volcará el dibujo 
        :type export_pdf: str, opcional
        """
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
        """Método de llamada.
        
        :param random: Si `True` devuelve una iteración aleatoria de la topología, si `False` devuelve el grafo original.
        :type random: bool, opcional
        
        :return: Lista de parejas que conforman una iteración de la topología (i.e., después de permutar la lista de osciladores)
        :rtype: list
        """
        if not random:
            return [pair for pair in self.grafo]
        else:
            perm = np_permutation(self.N_osc)
            return [[perm[pair[0]], perm[pair[1]]] for pair in self.grafo]


class PufExp:
    """
    Estos objetos son esencialmente un TENSOR que solo almacena valores binarios, y cuyos ejes están predefinidos:
    axis=['retos', 'inst', 'rep']. Además puede guardar una lista que relacione el índice del eje 'retos' con una
    representación de cada reto (en principio arbitraria, en la práctica será una lista de parejas de las celdas
    empleadas para obtener cada bit).
    """
    def __init__(self, topol, instancias, retos=[], d_pdl=False):
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
                            if d_pdl: # Usamos la primera erivada de la línea PDL para extraer la respuesta. 
                                if inst.item(osc=reto[bit][0], rep=i_rep, pdl=(i_pdl+1)%self.N_pdl)-inst.item(osc=reto[bit][0], rep=i_rep, pdl=i_pdl) > inst.item(osc=reto[bit][1], rep=i_rep, pdl=(i_pdl+1)%self.N_pdl)-inst.item(osc=reto[bit][1], rep=i_rep, pdl=i_pdl):
                                    aux.append(0)
                                else:
                                    aux.append(1)
                            else:
                                if inst.item(osc=reto[bit][0], rep=i_rep, pdl=i_pdl) > inst.item(osc=reto[bit][1], rep=i_rep, pdl=i_pdl):
                                    aux.append(0)
                                else:
                                    aux.append(1)
                    self.data[i_reto][i_inst].append(aux)
        self.intradist_set=[]
        self.intradist_ajuste = False
        self.interdist_set=[]
        self.interdist_ajuste = False
        self.far_curve = False
        self.frr_curve = False
        
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
            
    def intradist(self, ajuste=False):
        self.intradist_set=[]
        for i_reto in range(self.N_retos):
            for i_inst in range(self.N_inst):
                for i_rep in range(self.N_rep):
                    for j_rep in range(i_rep+1,self.N_rep,1):
                        self.intradist_set.append(hamming(self.data[i_reto][i_inst][i_rep],self.data[i_reto][i_inst][j_rep]))
        self.intradist_media = np_mean(self.intradist_set)
        self.intradist_std = np_std(self.intradist_set)
        self.intradist_p = self.intradist_media/self.N_bits
        self.intradist_hist = [[i for i in range(self.N_bits+1)], np_histogram(self.intradist_set, bins=self.N_bits+1, range=(0,self.N_bits+1), density=True)[0]]
        if ajuste == 'normal':
            def wrapper_normal(x):
                return sp_normal.pdf(x, self.intradist_media, self.intradist_std)
            def frr(x):
                return 1-sp_normal.cdf(x,self.intradist_media, self.intradist_std)
            self.intradist_ajuste = wrapper_normal
            self.frr_curve = frr
        else:
            def frr(x):
                return 1-sp_binomial.cdf(x, n=self.N_bits+1, p=self.intradist_p)
            self.intradist_ajuste = sp_interp1d([i for i in range(self.N_bits+1)] ,[sp_binomial.pmf(i, n=self.N_bits+1, p=self.intradist_p) for i in range(self.N_bits+1)])
            self.frr_curve = sp_interp1d([i for i in range(self.N_bits+1)] ,[frr(i) for i in range(self.N_bits+1)])
                        
    def interdist(self, ajuste=False):
        self.interdist_set=[]
        for i_reto in range(self.N_retos):
            for i_inst in range(self.N_inst):
                for j_inst in range(i_inst+1,self.N_inst,1):            
                    for i_rep in range(self.N_rep):
                        self.interdist_set.append(hamming(self.data[i_reto][i_inst][i_rep],self.data[i_reto][j_inst][i_rep]))
        self.interdist_media = np_mean(self.interdist_set)
        self.interdist_std = np_std(self.interdist_set)
        self.interdist_p = self.interdist_media/self.N_bits
        self.interdist_hist = [[i for i in range(self.N_bits+1)], np_histogram(self.interdist_set, bins=self.N_bits+1, range=(0,self.N_bits+1), density=True)[0]]
        if ajuste == 'normal':
            def wrapper_normal(x):
                return sp_normal.pdf(x, self.interdist_media, self.interdist_std)
            def far(x):
                return sp_normal.cdf(x, self.interdist_media, self.interdist_std)
            self.interdist_ajuste = wrapper_normal
            self.far_curve = far
        else:
            self.interdist_ajuste = sp_interp1d([i for i in range(self.N_bits+1)] ,[sp_binomial.pmf(i, n=self.N_bits+1, p=self.interdist_p) for i in range(self.N_bits+1)])
            self.far_curve = sp_interp1d([i for i in range(self.N_bits+1)] ,[sp_binomial.cdf(i, n=self.N_bits+1, p=self.interdist_p) for i in range(self.N_bits+1)])
                        
    def plot_hist(self):
        if len(self.intradist_set)>0:
            plt_bar(*self.intradist_hist)
        if len(self.interdist_set)>0:
            plt_bar(*self.interdist_hist)
        if self.intradist_ajuste:
            plt_plot(self.intradist_hist[0], [self.intradist_ajuste(x) for x in self.intradist_hist[0]], 'b')
        if self.interdist_ajuste:
            plt_plot(self.interdist_hist[0], [self.interdist_ajuste(x) for x in self.interdist_hist[0]], 'r')
        plt_show()
        
    def plot_curves(self, x0=False, x1=False, dx=1):
        if not x0:
            x0 = 0
        if not x1:
            x1 = self.N_bits
        ax = [x0+i*dx for i in range(int((x1-x0)/dx)+1)]
        if self.frr_curve:
            plt_plot(ax, [self.frr_curve(x) for x in ax])
        if self.far_curve:
            plt_plot(ax, [self.far_curve(x) for x in ax])
        plt_yscale('log')
        plt_show()