from numpy.random       import randint as np_randint,\
                               permutation as np_permutation
from numpy              import pi as np_pi,\
                               cos as np_cos,\
                               sin as np_sin,\
                               log as np_log,\
                               sqrt as np_sqrt,\
                               histogram as np_histogram,\
                               mean as np_mean,\
                               std as np_std,\
                               argmin as np_argmin,\
                               argsort as np_argsort,\
                               flip as np_flip,\
                               array
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
        
        
def sesgo_data_bin(data):
    """Esta función extrae el sesgo de cada bit a partir de una lista (data) de vectores binarios"""
    N_data = len(data)
    N_bits = len(data[0])
    
    result = [0 for i in range(N_bits)]
    for i in range(N_bits):
        for j in range(N_data):
            if data[j][i]=='1':
                result[i]+=1
        result[i]/=N_data
        
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
            self.grafo = custom[:]
            
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
            
    def __call__(self, random=False):
        """
        Método de llamada; devuelve una lista de parejas que
        conforman una iteración de la topología (i.e., después de
        permutar la lista de osciladores)
        
        Parámetros:
        -----------
        random : <bool, opcional, por defecto False>
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
    Este objeto contiene un experimento PUF. Además puede guardar una lista 
    que relacione el índice del eje 'retos' con una representación de cada 
    reto (en principio arbitraria, en la práctica será una lista de parejas 
    de las celdas empleadas para obtener cada bit).
    
    Parámetros:
    -----------        
    instancias : <objeto Tensor o lista de estos>
        Tensor tal y como es devuelto por la función
        myfpga.StdMatrix.medir(), i.e., con tres ejes 'rep', 'pdl', 'osc'.
        
    retos : <Lista de parejas de int o lista de estas>
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
        
    n_rep_code : <int, opcional, por defecto 1>
        Esta opción indica el número de repeticiones que se utilizarán para
        corregir la respuesta PUF una vez digitalizada, utilizando un código
        de repetición, i.e., un esquema de corrección de errores por 
        repetición: el número de repeticiones introducido se divide en grupos
        de n_rep_code respuestas binarias; cada bit posicional de la respuesta
        se calcula como la moda de bits del grupo. Notar que esto reduce el 
        número de repeticiones efectivo introducido en un factor 1/n_rep_code.
        Es recomendable que esta cantidad sea impar.
        
    del_bits : <lista de int, opcional, por defecto []>
        Esta opción indica los canales de bits que se eliminarán de la respuesta
        final (el primer canal corresponde al índice 0). Esto permite eliminar
        bits que son muy ruidosos.
        
    Constantes:
    -----------
    pufexp : lista de tres dimensiones que almacena los valores binarios de la 
             respuesta PUF, y cuyos ejes están predefinidos y dados en el orden: 
             [retos][inst][rep]
    N_inst
    N_pdl
    N_rep
    N_osc
    N_bits
    N_retos
    x
    sesgo_bit
    intradist
    intradist_media
    intradist_std
    intradist_error_media
    intradist_p
    intradist_ajuste_binom
    intradist_ajuste_normal
    interdist
    interdist_media
    interdist_std
    interdist_error_media
    interdist_p
    interdist_ajuste_binom
    interdist_ajuste_normal
    Dks : estadístico de Kolmogorov-Smirnov (bondad del ajuste binomial frente a la suma acumulada de interdistancias).
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
    def __init__(self, instancias, retos, d_pdl=False, multibit=[0], n_rep_code=1, del_bits=[]):
        """
        Función de inicialización.
        """
        if type(instancias)==type([]):
            self.instancias = instancias[:] # lista de objetos TENSOR, cada uno de los cuales contiene la "medida" (quizá simulación) de una instancia PUF.
        else:
            self.instancias=[instancias]
        
        try: # Si podemos hacer esto es que la entrada es una lista de lista de parejas.
            retos[0][0][0]
            self.retos = retos[:]
        except:
            self.retos = [retos[:]]
            
        self.N_inst = len(self.instancias)
        self.N_pdl = self.instancias[0].size('pdl')
        self.N_rep = self.instancias[0].size('rep')//n_rep_code # N. total de repticiones.
        self.N_osc = self.instancias[0].size('osc')
        self.N_bits_partial = len(self.retos[0]) # Número de bits sin tener en cuenta el "boost" PDL.
        self.N_bits = self.N_bits_partial*self.N_pdl*len(multibit)-len(del_bits) # Número de bits real de cada respuesta.
        self.x = list(range(self.N_bits+1)) # Eje de abscisas para las gráficas.
        self.x_pc = list(x*100/self.N_bits for x in self.x) # Eje de abscisas porcentual para las gráficas.
        
        self.N_retos = len(self.retos)
        self.multibit = multibit[:]
        
        # Experimento PUF
        self.pufexp=[[[] for j in self.instancias] for i in self.retos]
        for i_reto,reto in enumerate(self.retos): # retos
            
            for i_inst,inst in enumerate(self.instancias): # instancias
                
                for i_rep in range(self.N_rep): # repeticiones
                    
                    lista_rep_code=[] # lista de respuestas de un lote de corrección de errores
                    for i_n_rep_code in range(n_rep_code):
                        
                        rep_code="" # respuesta de corrección de errores
                        for i_pdl in range(self.N_pdl):
                            
                            for bit in range(self.N_bits_partial):
                                
                                if d_pdl: # Usamos la primera erivada de la línea PDL para extraer la respuesta. 
                                    diff = (inst.item(osc=reto[bit][0], rep=n_rep_code*i_rep+i_n_rep_code, pdl=(i_pdl+1)%self.N_pdl)-inst.item(osc=reto[bit][0], rep=n_rep_code*i_rep+i_n_rep_code, pdl=i_pdl)) - (inst.item(osc=reto[bit][1], rep=n_rep_code*i_rep+i_n_rep_code, pdl=(i_pdl+1)%self.N_pdl)-inst.item(osc=reto[bit][1], rep=n_rep_code*i_rep+i_n_rep_code, pdl=i_pdl))
                                else:
                                    diff = inst.item(osc=reto[bit][0], rep=n_rep_code*i_rep+i_n_rep_code, pdl=i_pdl) - inst.item(osc=reto[bit][1], rep=n_rep_code*i_rep+i_n_rep_code, pdl=i_pdl)
                                
                                if diff<0:
                                    diff_sign = '0'
                                else:
                                    diff_sign = '1'
                                diff_abs_bin = format(int(abs(diff)), '031b') # Convierte 'abs(diff)' en un string binario de 31 dígitos con 'leading zeroes'.
                                diff_bin = diff_sign+diff_abs_bin
                                
                                rep_code+="".join([diff_bin[i] for i in self.multibit]) # Solo nos quedmos con los bit indicados en la opción 'multibit'.
                        lista_rep_code.append(rep_code)
                        
                    response_bin="" # respuesta corregida por un método de votación.
                    for i_bit,bit in enumerate(sesgo_data_bin(lista_rep_code)):
                        if i_bit not in del_bits: # Si el canal de bits no ha sido vetado calculamos el bit mediano...
                            if round(bit)>0.5:
                                response_bin+='1'
                            else:
                                response_bin+='0'
                        else:
                            response_bin+='0' # ... en caso contrario escribimos '0' por convenio.

                    self.pufexp[i_reto][i_inst].append(response_bin)
                    
        ## Sesgo de cada bit
        self.sesgo_bit = [[sesgo_data_bin(self.pufexp[i_reto][i_inst]) for i_inst in range(len(self.instancias))] for i_reto in range(len(self.retos))]
        
        ## Intra-distancia: solo si N_rep > 1:
        if self.N_rep>1:
            intradist_set=[]
            for i_reto in range(self.N_retos):
                
                for i_inst in range(self.N_inst):
                    
                    for i_rep in range(self.N_rep):
                        
                        for j_rep in range(i_rep+1,self.N_rep,1):
                            intradist_set.append(hamming(self.pufexp[i_reto][i_inst][i_rep],self.pufexp[i_reto][i_inst][j_rep]))
            self.intradist_set = array(intradist_set).reshape(self.N_retos,self.N_inst,(self.N_rep*(self.N_rep-1))//2)
            
            self.intradist_media = np_mean(self.intradist_set)
            self.intradist_std = np_std(self.intradist_set)
            self.intradist_error_media = self.intradist_std/np_sqrt(self.intradist_set.flatten().size) # desviación estándar de la media (teorema del límite central)
            self.intradist_p = self.intradist_media/self.N_bits
            self.intradist = np_histogram(self.intradist_set, bins=self.N_bits+1, range=(0,self.N_bits))[0]
            self.intradist = self.intradist/self.intradist.sum()            
            self.intradist_ajuste_binom = sp_binomial.pmf(self.x, n=self.N_bits, p=self.intradist_p)
            #self.intradist_ajuste_normal = sp_normal.pdf(self.x, loc=self.intradist_media, scale=self.intradist_std)
        
        ## Inter-distancia: solo si N_inst > 1:
        if self.N_inst>1:
            interdist_set=[]
            for i_reto in range(self.N_retos):
            
                for i_rep in range(self.N_rep):
                
                    for i_inst in range(self.N_inst):
                    
                        for j_inst in range(i_inst+1,self.N_inst,1): 
                            interdist_set.append(hamming(self.pufexp[i_reto][i_inst][i_rep],self.pufexp[i_reto][j_inst][i_rep]))
            self.interdist_set = array(interdist_set).reshape(self.N_retos,self.N_rep,(self.N_inst*(self.N_inst-1))//2)
                            
            self.interdist_media = self.interdist_set.mean()
            self.interdist_std = self.interdist_set.std()
            self.interdist_error_media = self.interdist_std/np_sqrt(self.interdist_set.flatten().size) # desviación estándar de la media (teorema del límite central)
            self.interdist_p = self.interdist_media/self.N_bits
            self.interdist = np_histogram(self.interdist_set, bins=self.N_bits+1, range=(0,self.N_bits))[0]
            self.interdist = self.interdist/self.interdist.sum()
            self.interdist_ajuste_binom = sp_binomial.pmf(self.x, n=self.N_bits, p=self.interdist_p)
            #self.interdist_ajuste_normal = sp_normal.pdf(self.x, loc=self.interdist_media, scale=self.interdist_std)
            
        ## Identificabilidad: solo si N_rep > 1 y N_inst > 1:
        if self.N_rep>1 and self.N_inst>1:
            self.far = sp_binomial.cdf(self.x, n=self.N_bits, p=self.interdist_p)
            self.frr = sp_binomial.sf(self.x, n=self.N_bits, p=self.intradist_p) # 1-binomial.cdf
            self.t_eer = np_argmin([max(far,frr) for far,frr in zip(self.far,self.frr)])
            self.eer = max(self.far[self.t_eer],self.frr[self.t_eer])
            self.roc = (sp_binomial.logcdf(self.x, n=self.N_bits, p=self.interdist_p)/np_log(10), sp_binomial.logsf(self.x, n=self.N_bits, p=self.intradist_p)/np_log(10)) # dupla con los ejex x,y de la curva ROC.
            
            
    def Dks(self):
        """
        Esta función calcula el estadístico de Kolmogorov-Smirnov; devuelve una dupla con dos cantidades:
        la primera es un 'ndarray' 'Dks_set' con el conjunto de todos los N_retos*N_rep valores de Kolmogorov-Smirnov
        correspondientes a cada histograma de interdistancias; la segunda es un 'float' con el estadístico Dks
        calculado para el conjunto histograma de interdistancia completo. Es tentador utilizar esta última 
        cantidad como estadístico de KS, sin embargo no tengo claro que no sea más correcto utilizar por
        ejemplo el valor máximo 'Dks_set.max()', o algo así.
        """
        Dks_set=[]
        for i in range(self.N_retos):
            for j in range(self.N_rep):
                hist = np_histogram(self.interdist_set[i][j], bins=self.N_bits+1, range=(0,self.N_bits))[0]
                hist = hist/hist.sum()
                p = self.interdist_set[i][j].mean()/self.N_bits
                Dks_set.append( max( [abs( hist.cumsum()[k]-sp_binomial.cdf(k=k,n=self.N_bits,p=p) ) for k in range(self.N_bits+1)] ) )
        
        Dks = max([abs(self.interdist.cumsum()[k]-sp_binomial.cdf(k=k,n=self.N_bits,p=self.interdist_p)) for k in range(self.N_bits+1)])
        
        return array(Dks_set),Dks

                        
    def noisy_bits(self, metodo_calc='flip'):
        """
        Esta función devuelve una lista de canales bit ordenada en orden decreciente de variabilidad. Utilizar para decidir qué canales
        bit suprimir como parte del 'helper_data'. La función devuelve de hecho una lista [i][j][k] donde:
            i --> Reto
            j --> Instancia
            k --> k-ésimo canal bit
        El método de cálculo (metodo_calc) admite dos opciones: 'flip' (por defecto) y 'std'. La primera calcula los peores canales como
        aquellos que flipan un mayor número de veces, i.e., prima el hecho de que un bit tenga una baja entropía. En en segundo caso, tiene
        más importancia el hecho de que el sesgo del bit se aleje de los valores 0/1.
        """
        result = [[ [] for j in range(self.N_inst)] for i in range(self.N_retos)]
        for i in range(self.N_retos):
            for j in range(self.N_inst):
                for k in range(self.N_bits):
                    if metodo_calc == 'flip':
                        flips=0 # contador de flips
                        for l in range(1,self.N_rep,1):
                            if self.pufexp[i][j][l][k] != self.pufexp[i][j][l-1][k]:
                                flips+=1
                        result[i][j].append(flips)
                    else:
                        flips=[]
                        for l in range(self.N_rep):
                            if self.pufexp[i][j][l][k]=='0':
                                flips.append(0)
                            else:
                                flips.append(1)
                        result[i][j].append(np_std(flips))
                        
            
        return np_flip(np_argsort(result, axis=2), axis=2)
        
        
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
                        
                        
class PufExpAmpliado:
    """
    Esta clase contiene un experimento PUF ampliado, que es un conjunto de objetos 'PufExp' realizados en diferentes
    condiciones ambientales. Cada condición ambiental se identifica internamente mediante un índice entero.

    Parámetros:
    -----------
    experimentos : <lista de objetos 'PufExp'>
        Este parámetro introduce la lista de experimentos PUF realizados en diferentes condiciones ambientales.
        
    cond_amb : <lista de 'float', opcional, por defecto range(len(experimentos))>
        Esta opción es un alias para asignar un valor real al índice identificador de cada condición ambiental
        introducida. Por ejemplo, para asignar temperaturas reales medidas con unidades.
        
    cond_ref : <int, opcional, por defecto 0>
        Permite identificar cuál es la condición ambiental de referencia, dada la lista 'experimentos'. Notar que
        aunque es opcional, generalmente el valor por defecto (0) será incorrecto.
        
    Constantes:
    -----------
    experimentos
    cond_amb
    cond_ref
    N_amb
    intradist_amb_set
    intradist_amb
    curva_v
    curva_v_error

    """

    def __init__(self, experimentos, cond_amb=False, cond_ref=0):
        """ Función de inicialización."""
        self.experimentos = experimentos[:]
        if cond_amb != False:
            self.cond_amb = cond_amb[:]
        else:
            self.cond_amb = list(range(len(experimentos)))
        self.cond_ref = cond_ref
        self.N_retos = experimentos[0].N_retos
        self.N_inst = experimentos[0].N_inst
        self.N_rep = experimentos[0].N_rep
        self.N_amb = len(experimentos)
        self.N_bits = experimentos[0].N_bits
        
        self.intradist_amb_set=[]
        for i_reto in range(self.N_retos):
            
            for i_inst in range(self.N_inst):
                
                for i_rep in range(self.N_rep):
                
                    for j_rep in range(self.N_rep): # Repeticiones de la condición de referencia.
                
                        for i_amb,amb in enumerate(experimentos):
                            self.intradist_amb_set.append(hamming(amb.pufexp[i_reto][i_inst][i_rep],experimentos[cond_ref].pufexp[i_reto][i_inst][j_rep]))
        
        self.intradist_amb_set = np_reshape(self.intradist_amb_set, (self.N_retos,self.N_inst,self.N_rep**2,self.N_amb))
        
        self.curva_v = np_mean(self.intradist_amb_set, axis=(0,1,2)) # Promedio sobre N_retos, N_inst y N_rep.
        self.curva_v_error = np_std(self.intradist_amb_set, axis=(0,1,2))/np_sqrt(self.N_retos*self.N_inst*self.N_rep) # Desviación estándar de cada promedio.
        
        
def preliminar(medidas_in):
    """
    Esta función realiza un análisis rápido y automático de
    un conjunto de medidas experimentales tal y como son devueltas
    por una o varias funciones 'medir()' (ver 'myfpga.ring_osc').
    El análisis es estándar, utilizando una topología 'n/2'.
    
    Parámetros:
    ----------
    medidas : <lista de Tensor>
        Este parámetro contiene una lista de medidas tal y como son devueltas por 
        las funciones 'medir()' del módulo 'myfpga.ring_osc'.
    """
    from matplotlib.pyplot import plot,bar,show
    
    try:
        N_osc = medidas_in.size('osc')
        N_inst = 1
        medidas=[medidas_in]
    except:
        N_osc = medidas_in[0].size('osc')
        N_inst = len(medidas_in)
        medidas = medidas_in[:]
    
    topol = PufTopol(N_osc, topol='n/2')
    pufexp = PufExp(instancias = medidas, retos=[topol()])
    
    if N_inst>1:
        bar(pufexp.x, pufexp.intradist, color='tab:blue')
        plot(pufexp.x, pufexp.intradist_ajuste_binom, color='tab:orange')
        bar(pufexp.x,pufexp.interdist, color='tab:blue')
        plot(pufexp.x, pufexp.interdist_ajuste_binom, color='tab:green')
        show()
        print(f"Intra-dist media: {pufexp.intradist_media/pufexp.N_bits*100}")
        print(f"Inter-dist media: {pufexp.interdist_media/pufexp.N_bits*100}")
    else:
        bar(pufexp.x, pufexp.intradist, color='tab:blue')
        plot(pufexp.x, pufexp.intradist_ajuste_binom, color='tab:orange')
        show()
        print(f"Intra-dist media: {pufexp.intradist_media/pufexp.N_bits*100}")
