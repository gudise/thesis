from numpy import array, unique, log2
from numpy.random import permutation
from scipy.stats import entropy
from sympy import Symbol, symbols, integrate, factor, simplify


"""
Este módulo contiene una serie de clases y funciones para estudiar
PUF generales de medida compensada.

NOTA: a día de hoy es un 'cajón desastre' para una serie de funcionalidades
      presentes originalmente en el proyecto 'W-entropia-CM'
"""

def entropia(topol, N_osc=3, N_rep=1000, hist=False):
    """
    Esta función aproxima la entropía de 'topol' para
    'Nosc' osciladores, utilizando 'N_rep' muestras.
    La función devuelve una lista con tres números:
        (entropía, entropía/oscilador, entropía/bit).
    Si hist=True la función también pinta el histograma
    """
    muestras = [int(topol(permutation(N_osc)),base=2) for i in range(N_rep)]
    N_bits = len(topol(list(range(N_osc))))
    x,y = unique(muestras, return_counts=True)
    entrop = entropy(y, base=2)
    
    if hist==True:
        return x,y
    
    return entrop,entrop/N_osc,entrop/N_bits

def minentropia(topol, N_osc=3, N_rep=1000, hist=False):
    """
    Esta función aproxima la entropía de 'topol' para
    'Nosc' osciladores, utilizando 'N_rep' muestras.
    La función devuelve una lista con tres números:
        (minentropía, minentropía/oscilador, minentropía/bit).
    """
    muestras = [int(topol(permutation(N_osc)),base=2) for i in range(N_rep)]
    N_bits = len(topol(list(range(N_osc))))
    x,y = unique(muestras, return_counts=True)
    max_prob = y.max()/len(muestras)
    minentrop =  -log2(max_prob)
    
    if hist==True:
        plt.bar(x, y)
        plt.show()
    
    return minentrop,minentrop/N_osc,minentrop/N_bits
    
    
def allpairs(entrada):
    """
    Esta función toma una lista de 'N' flotantes, y devuelve 
    la respuesta binaria correspondiente a una topología
    'allpairs'.
    """
    result = ''
    for i in range(len(entrada)):
        for j in range(i+1, len(entrada), 1):
            if entrada[i] > entrada[j]:
                result+='1'
            else:
                result+='0'
    return result
    
    
def N_2(entrada):
    """
    Esta función toma una lista de 'N' flotantes, y devuelve 
    la respuesta binaria correspondiente a una topología
    'N/2'.
    """
    result = ''
    for i in range(0, len(entrada), 2):
        if entrada[i] > entrada[i+1]:
            result+='0'
        else:
            result+='1'
    return result    
    
    
def N_1(entrada):
    """
    Esta función toma una lista de 'N' flotantes, y devuelve 
    la respuesta binaria correspondiente a una topología
    'N - 1'.
    """    
    result = ''
    for i in range(len(entrada)-1):
        if entrada[i] > entrada[i+1]:
            result+='0'
        else:
            result+='1'
    return result
    
    
def k_3(entrada):
    """
    Esta función toma una lista de 'N' flotantes, y devuelve 
    la respuesta binaria correspondiente a una topología
    '3 modular'.
    """
    result=''
    for i in range(len(entrada)//3):
        result+=allpairs(entrada[3*i:3*i+3])
    return result
    
    
def k_4(entrada):
    """
    Esta función toma una lista de 'N' flotantes, y devuelve 
    la respuesta binaria correspondiente a una topología
    '4 modular'.
    """
    result=''
    for i in range(len(entrada)//4):
        result+=allpairs(entrada[4*i:4*i+4])
    return result
    
    
class Binary(Symbol):
    """
    clase 'Binary' que extiende (i.e., hereda) la clase 'Symbol',
    sobreescribiendo la función '_eval_power' de 'Symbol' por una
    apropiada para variables binarias.
    """
    def _eval_power(self, other):
        return self
        
    
class TeoN1:
    """
    Objeto utilizado para analizar teóricamente la topología 'N-1'
    """
    def __init__(self, N):
        """
        Esta función devuelve la distribución de probabilidad
        para una topología N-1; los argumentos de la función
        devuelta son valores binarios.
        """
        self.N = N
        self.x = symbols(f'x:{N}')
        self.b = [Binary(f'b{i+1}') for i in range(N-1)]

        self.int_iterativas = [integrate(1,(self.x[0],self.b[0]*self.x[1],self.x[1]+self.b[0]*(1-self.x[1])))]
        for i in range(1,N-1,1):
            self.int_iterativas.append(integrate(self.int_iterativas[-1],(self.x[i],self.b[i]*self.x[i+1],self.x[i+1]+self.b[i]*(1-self.x[i+1]))))
        self.int_iterativas.append(factor(integrate(self.int_iterativas[-1], (self.x[N-1],0,1))))
        
    def eval(self, **kwargs):
        """
        Esta función toma las variables opcionales 'bi' con i entre 0, N-1
        y devuelve la expresión (Sympy) substituida
        """
        lista_subs = []
        for i in kwargs:
            lista_subs.append((self.b[int(i[1:])],kwargs[i]))
        return simplify(self.int_iterativas[-1].subs(lista_subs))
    
    def distrib(self):
        """
        Esta función devuelve una lista con
        la distribución de probabilidad ordenada
        por índices: [b_{N-1}, ..., b_1], desde 'todos 1' a 'todos 0'
        (i.e., el histograma respecto de las entradas en numeración decimal).
        """
        # Construimos el diccionario de entrada correspondiente a cada entrada:
        result = []
        for entrada_int in range(int(2**(self.N-1))):
            entrada_dict = {}
            entrada_bin = format(entrada_int, f'0{self.N-1}b')
            for ind,caracter in enumerate(entrada_bin):
                entrada_dict[f'b{ind}'] = caracter
            result.append(self.eval(**entrada_dict))
        return array(result)
        
        
def TeoN1max(N):
    """
    Esta función busca iterativamente la respuesta de max
    probabilidad, devolviendo tanto el vector como el valor
    de prob.
    """
    x = symbols(f'x:{N}')
    b = [Binary(f'b{i+1}') for i in range(N-1)]

    resp = []
    maxprob = []
    int_resp=1
    for i in range(N-1):
        cand_0 = integrate(int_resp,(x[i],0,x[i+1]))
        cand_1 = integrate(int_resp,(x[i],x[i+1],x[i+1]+(1-x[i+1])))

        prob_0 = integrate(cand_0, (x[i+1],0,1))
        prob_1 = integrate(cand_1, (x[i+1],0,1))

        if prob_0<prob_1:
            int_resp = cand_1
            resp.append(1)
            maxprob.append(prob_1)
        else:
            int_resp = cand_0
            resp.append(0)
            maxprob.append(prob_0)
            
        print(f'{i+1}/{N-1}', end='\r')

    return maxprob,resp