from numpy.random       import randint as np_randint,\
                               permutation as np_permutation
from numpy              import pi as np_pi,\
                               cos as np_cos,\
                               sin as np_sin,\
                               log as np_log,\
                               unique as np_unique,\
                               mean as np_mean,\
                               std as np_std,\
                               argmin as np_argmin
from scipy.stats        import binom as sp_binomial
                               #norm as sp_normal,\
                               #fit as sp_fit
from scipy.interpolate  import interp1d as sp_interp1d
from matplotlib.pyplot  import plot as plt_plot,\
                               bar as plt_bar,\
                               tick_params as plt_tick_params,\
                               gca as plt_gca,\
                               axis as plt_axis,\
                               show as plt_show,\
                               savefig as plt_savefig,\
                               yscale as plt_yscale
from mytensor           import *


def hamming(array1, array2, porciento=False):
    """
    Esta función devuelve la distancia de Hamming de dos arrays de
    símbolos.
    
    Parámetros:
    -----------
    array1 : <str>
        Primer array de entrada.
        
    array2 . <str>
        Segundo array de entrada.
        
    porciento : <bool, opcional, por defecto 'False'>
        Si 'True', devuelve la salida en tanto por ciento.
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
    """
    Este objeto contiene una función que devuelve una lista de
    valores enteros, la cual representa las parejas de celdas
    (osciladores) empleadas para construir una respuesta binaria.
    
    Parámetros:
    -----------
    N_osc : <int>
        Número de celdas de que consta la PUF.
    
    topol : <str, opcional>
        Este parámetro indica la topología de la PUF. Las opciones
        admitidas son:
            'all_pairs' o 'ap'
                todas las comparaciones posibles
            'n/2'
                comparaciones sin repetir osciladores
            'n-1'
                comparaciones repitiendo un oscilador cada vez
            'custom'
                comparaciones 'custom' tal y como se dan en el parámetro
                "custom"
                
    custom : <list, opcional>
        Lista de parejas. Cada elemento de 'custom' es una lista de dos
        osciladores a comparar.
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
        """
        Dibuja un grafo con la topología.
        
        Parámetros:
        -----------
        color_vert : <str, opcional, por defecto 'black'>
            Color de los vértices del grafo.
        
        size_vert : <float, opcional, por defecto 20>
            Tamaño de los vértices del grafo.
        
        color_link : <str, opcional, por defecto 'tab:gray'>
            Color de los link del grafo.
        
        size_link : <float, opcional, por defecto 1.5>
            Tamaño de los link del grafo.
        
        export_pdf : <str o bool, opcional, por defecto False>
            Nombre del fichero .pdf en el cual volcará el dibujo.
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
        """
        Método de llamada; devuelve una lista de parejas que
        conforman una iteración de la topología (i.e., después de
        permutar la lista de osciladores)
        
        Parámetros:
        -----------
        random : <bool, opcional, por defecto True>
            Si 'True' devuelve una iteración aleatoria de la topología,
            si 'False' devuelve el grafo original.
        """
        if not random:
            return [pair for pair in self.grafo]
        else:
            perm = np_permutation(self.N_osc)
            return [[perm[pair[0]], perm[pair[1]]] for pair in self.grafo]


class PufExp:
    """
    Estos objetos son esencialmente un TENSOR que solo almacena valores
    binarios, y cuyos ejes están predefinidos: axis=['retos', 'inst', 'rep']
    Además puede guardar una lista que relacione el índice del eje 'retos'
    con una representación de cada reto (en principio arbitraria, en la
    práctica será una lista de parejas de las celdas empleadas para obtener
    cada bit).
    
    Parámetros:
    -----------        
    instancias : <objeto Tensor o lista de objetos Tensor>
        Tensor tal y como es devuelto por la función
        myfpga.StdMatrix.medir(), i.e., con tres ejes 'rep', 'osc', 'pdl'.
        
    retos : <lista de lista de parejas de int>
        Lista que contiene una lista de parejas (i.e., listas de dos elementos),
        tal y como son devueltas por la llamada a un objeto PufTopol. El
        número de parejas que contenga este parámetro será el número de bits
        base (sin tener en cuenta PDL ni multibit) de las respuestas.
    
    d_pdl : <bool, opcional, por defecto False>
    
    multibit : <lista de int, opcional, por defecto [0]>
        Esta opción indica qué bits de las comparaciones indicadas en la
        opción 'retos' se utilizand para construir la respuesta. Esta
        consta de 32 bits, desde 0 (bit de signo) hasta 31 (bit menos
        significativo).
        
    Constantes:
    -----------
    N_inst
    N_pdl
    N_rep
    N_osc
    N_bits
    N_retos
    x
    intradist
    intradist_media
    intrdist_std
    intradist_p
    intradist_ajuste_binom
    intradist_ajuste_normal
    interdist
    interdist_media
    interdist_std
    interdist_p
    interdist_ajuste_binom
    interdist_ajuste_normal
    far
    frr
    t_eer
    eer
    
    Funciones:
    ----------
    print()
        Esta función pinta el experimento PUF completo.
        
        Parámetros:
        -----------
        print_retos : <bool, opcional, por defecto True>
            Si 'True' pinta también el reto asociado con cada respuesta.
        
    """
    def __init__(self, instancias, retos, d_pdl=False, multibit=[0]):
        """
        Función de inicialización.
        """
        if type(instancias)==type([]):
            self.instancias = instancias # lista de objetos TENSOR, cada uno de los cuales contiene la "medida" (quizá simulación) de una instancia PUF.
        else:
            self.instancias=[instancias]
            
        self.N_inst = len(self.instancias)
        self.N_pdl = self.instancias[0].size('pdl')
        self.N_rep = self.instancias[0].size('rep')
        self.N_osc = self.instancias[0].size('osc')
        self.N_bits_partial = len(retos[0]) # Número de bits sin tener en cuenta el "boost" PDL.
        self.N_bits = self.N_bits_partial*self.N_pdl*len(multibit) # Número de bits real de cada respuesta.
        self.x = list(range(self.N_bits+1)) # Eje de abscisas para las gráficas.
        self.x_pc = list(x*100/self.N_bits for x in self.x) # Eje de abscisas porcentual para las gráficas.
        
        self.retos = retos
        self.N_retos = len(self.retos)
        self.multibit = multibit
        
        # Experimento PUF
        self.pufexp=[[[] for j in self.instancias] for i in self.retos]
        for i_reto,reto in enumerate(self.retos): # retos
            
            for i_inst,inst in enumerate(self.instancias): # instancias
                
                for i_rep in range(self.N_rep): # repeticiones
                    
                    response_bin=""
                    for i_pdl in range(self.N_pdl):
                        
                        for bit in range(self.N_bits_partial):
                            
                            if d_pdl: # Usamos la primera erivada de la línea PDL para extraer la respuesta. 
                                diff = (inst.item(osc=reto[bit][0], rep=i_rep, pdl=(i_pdl+1)%self.N_pdl)-inst.item(osc=reto[bit][0], rep=i_rep, pdl=i_pdl)) - (inst.item(osc=reto[bit][1], rep=i_rep, pdl=(i_pdl+1)%self.N_pdl)-inst.item(osc=reto[bit][1], rep=i_rep, pdl=i_pdl))
                            else:
                                diff = inst.item(osc=reto[bit][0], rep=i_rep, pdl=i_pdl) - inst.item(osc=reto[bit][1], rep=i_rep, pdl=i_pdl)
                            
                            if diff<0:
                                diff_sign = '0'
                            else:
                                diff_sign = '1'
                            diff_abs_bin = format(int(abs(diff)), '031b') # Convierte 'abs(diff)' en un string binario de 31 dígitos con 'leading zeroes'.
                            diff_bin = diff_sign+diff_abs_bin
                            
                            response_bin+="".join([diff_bin[i] for i in self.multibit]) # Solo nos quedmos con los bit indicados en la opción 'multibit'.

                    self.pufexp[i_reto][i_inst].append(response_bin)
        
        ## Intra-distancia
        self.intradist_set=[]
        for i_reto in range(self.N_retos):
            
            for i_inst in range(self.N_inst):
                
                for i_rep in range(self.N_rep):
                    
                    for j_rep in range(i_rep+1,self.N_rep,1):
                        self.intradist_set.append(hamming(self.pufexp[i_reto][i_inst][i_rep],self.pufexp[i_reto][i_inst][j_rep]))
        
        self.intradist_media = np_mean(self.intradist_set)
        self.intradist_std = np_std(self.intradist_set)
        self.intradist_p = self.intradist_media/self.N_bits
        self.intradist = [0 for x in self.x]
        for x,y in zip(*np_unique(self.intradist_set, return_counts=True)):
            self.intradist[x]=y
        intradist_sum = sum(self.intradist)
        for x in self.x:
            self.intradist[x]/=intradist_sum
        self.intradist_ajuste_binom = sp_binomial.pmf(self.x, n=self.N_bits, p=self.intradist_p)
        #self.intradist_ajuste_normal = sp_normal.pdf(self.x, loc=self.intradist_media, scale=self.intradist_std)
        
        ## Inter-distancia
        self.interdist_set=[]
        for i_reto in range(self.N_retos):
            
            for i_inst in range(self.N_inst):
                
                for j_inst in range(i_inst+1,self.N_inst,1): 
                    
                    for i_rep in range(self.N_rep):
                        self.interdist_set.append(hamming(self.pufexp[i_reto][i_inst][i_rep],self.pufexp[i_reto][j_inst][i_rep]))
        
        self.interdist_media = np_mean(self.interdist_set)
        self.interdist_std = np_std(self.interdist_set)
        self.interdist_p = self.interdist_media/self.N_bits
        self.interdist = [0 for x in self.x]
        for x,y in zip(*np_unique(self.interdist_set, return_counts=True)):
            self.interdist[x]=y
        interdist_sum = sum(self.interdist)
        for x in self.x:
            self.interdist[x]/=interdist_sum
        self.interdist_ajuste_binom = sp_binomial.pmf(self.x, n=self.N_bits, p=self.interdist_p)
        #self.interdist_ajuste_normal = sp_normal.pdf(self.x, loc=self.interdist_media, scale=self.interdist_std)
        
        # Identificabilidad
        self.far = sp_binomial.cdf(self.x, n=self.N_bits, p=self.interdist_p)
        self.frr = sp_binomial.sf(self.x, n=self.N_bits, p=self.intradist_p) # 1-binomial.cdf
        self.t_eer = np_argmin([max(far,frr) for far,frr in zip(self.far,self.frr)])
        self.eer = max(self.far[self.t_eer],self.frr[self.t_eer])
        self.roc = (sp_binomial.logcdf(self.x, n=self.N_bits, p=self.interdist_p)/np_log(10), sp_binomial.logsf(self.x, n=self.N_bits, p=self.intradist_p)/np_log(10)) # dupla con los ejex x,y de la curva ROC.
        
                
    def print(self, print_retos=True):
        if print_retos:
            print("# reto,inst,rep = reto = respuesta\n")
        else:
            print("# reto,inst,rep = respuesta\n")
        for i_reto,reto in enumerate(self.retos):
            for i_inst,inst in enumerate(self.instancias):
                for i_rep in range(self.N_rep):
                    if print_retos:
                        print(f" {i_reto},{i_inst},{i_rep} = {reto} = {' '.join(str(bit) for bit in self.pufexp[i_reto][i_inst][i_rep])}")
                    else:
                        print(f" {i_reto},{i_inst},{i_rep} = {' '.join(str(bit) for bit in self.pufexp[i_reto][i_inst][i_rep])}")
                        
