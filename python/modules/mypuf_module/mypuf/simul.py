from numpy                  import array as _array,\
                                   sqrt as _sqrt
from numpy.random           import choice as _choice

"""Este módulo contiene rutinas para smular el comportamiento de una PUF desde un punto de vista 'behavioural' (no físico).
"""

def sim_hamming_distrib(N_iter=1, N_resp=10, N_bits=100, p=0.5):
    """Esta función recibe un valor `p` promedio (e.g., resultado de un experimento PUF) y `N_bits` número de bits, y simula una serie de `N_resp` respuestas PUF (técnicamente, diferencias con respecto a una 'clave dorada') siguiendo un modelo binomial ideal, i.e., cada respuesta PUF se construye como una palabra binomial de parámetros `N_bits`, `p_resp`, donde `p_resp` es el valor tal que, si el modelo es ideal y produce respuestas binomiales de parámetros `N_bits`, `p_resp`, entonces la distribución de distancias Hamming correspondiente se distribuye como una binomial de parámetros `N_bits`, `p` (introducidos en la función); esta cantidad `p_resp` se calcula teniendo en cuenta que la prob. de que el i-ésimo bit de un par de respuestas contribuya a la distancia Hamming es la probabilidad de que el primero sea '1' y el segundo '0', más la prob. de que el primero sea '0' y el segundo '1', y es igual a p_resp=1/2-sqrt(1-2*p)/2. La función simula `N` respuestas y devuelve un array con todas las distancias posibles, i.e., un array de tamaño N*(N-1)/2.
    
    :param N_iter: Número de veces (iteraciones) que se repite la simulación.
    :type N_iter: int.
    
    :param N_resp: Número de respuestas a simular.
    :type N_resp: int
    
    :param N_bits: Número de bits de las respuestas.
    :type N_bits: int
    
    :param p: Parámetro `p` promedio de la distribución de distancias Hamming a simular.
    :type p: float
    
    :return: Lista de todas las N_iter*N_resp*(N_resp-1)/2 distancias Hamming (medidas en número de bits) realizadas en el pool de `N` respuestas PUF simuladas.
    :rtype: Lista de int.
    """
    p_resp = 1/2-_sqrt(1-2*p)/2
    set_prim = _choice([1,0],size=(N_iter,N_resp,N_bits),p=[p_resp,1-p_resp])
    result=[]
    for i in range(N_iter):
        for j in range(N_resp):
            for k in range(j+1,N_resp,1):
                result.append((set_prim[i][j]^set_prim[i][k]).sum())
    return _array(result)