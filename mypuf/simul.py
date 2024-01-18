from numpy                  import array as _array,\
                                   sqrt as _sqrt,\
                                   histogram as _histogram
from numpy.random           import choice as _choice
from scipy.stats            import binom as _binom

"""Este módulo contiene rutinas para smular el comportamiento de una PUF desde un punto de vista 'behavioural' (no físico).
"""

def sim_hamming_cideal(N_inst, N_rep, N_bits, p_intra, p_inter, return_intra=True, return_inter=True, verbose=False):
    """Esta función simula el resultado de intra/inter-distancia de Hamming para un experimento PUF, utilizando un modelo cuasi-ideal. El modelo recibe las cantidades `p_intra` y `p_inter` observadas experimentalmente, y calcula los parámetros `p_inst` que determina la probabilidad de que un bit sea '1' en una instancia, y `p_rep` que determina la probabilidad de que un bit sea '1' en una repetición (técnicamente, que la diferencia con respecto de una 'clave dorada' sea '1'), tales que las distribuciones de intra/inter-distancias Hamming se distribuyan como binomiales de parámetros `p_intra` y `p_inter` respectivamente.
    
    :param N_inst: Número de instancias del experimento.
    :type N_inst: int.
    
    :param N_rep: Número de repeticiones del experimento.
    :type N_rep: int.
    
    :param N_bits: Número de bits de las respuestas PUF.
    :type N_bits: int.
    
    :param p_intra: Parámetro `p` del histograma de intra-distancias a simular.
    :type p_intra: float positivo menor que 1.
    
    :param p_inter: Parámetro `p` del histograma de inter-distancias a simular.
    :type p_inter: float positivo menor que 1.
    
    :param return_intra: Si `True` devuelve el conjunto de intra-distancias.
    :type return_intra: bool.
    
    :param return_inter: Si `True` devuelve el conjunto de inter-distancias.
    :type return_inter: bool.
    
    :param verbose: Si `True` pinta los valores de `p_rep`, `p_inst` calculados.
    :type verbose: bool.
    
    :return: Devuelve una dupla de listas `intra_set`,`inter_set`.
    :rtype: Dupla de lista de float.
    """
    p_rep = 1/2-_sqrt(1-2*p_intra)/2
    p_inst = 1/2-1/2*_sqrt((2*p_inter-1)/(2*p_intra-1))
    
    if verbose:
        print(f'p_rep: {p_rep}\np_inst: {p_inst}')
    
    ## Generamos los datos:
    key=[]
    data=[]
    for i in range(N_inst):
        key.append(_choice([1,0],p=[p_inst,1-p_inst],size=N_bits))
        for j in range(N_rep):
            data.append(key[i]^_choice([1,0],p=[p_rep,1-p_rep],size=N_bits))
    data = _array(data).reshape(N_inst,N_rep,N_bits)
    
    result=[]
    if return_intra:
    ## Cálculo de intra-distancia:
        intra_set=[]
        for i in range(N_inst):
            for j in range(N_rep):
                for k in range(j+1,N_rep,1):
                    intra_set.append((data[i][j]^data[i][k]).sum())
        result.append(_array(intra_set))    
    
    if return_inter:
        ## Cálculo de inter-distancia:
        inter_set=[]
        for i in range(N_rep):
            for j in range(N_inst):
                for k in range(j+1,N_inst,1):
                    inter_set.append((data[j][i]^data[k][i]).sum())
        result.append(_array(inter_set))

    return tuple(result)
    
    
def Dks_montecarlo_cideal(N, N_inst, N_rep, N_bits, p_intra, p_inter, verbose=True):
    """Esta función calcula las distribuciones del estadístico KS de un modelo cuasi-ideal frente a las dsitribuciones binomiales de parámetros n=N_bits, p=p_intra, p=p_inter. La función devuelve una dupla de listas de tamaño `N` de estos índices KS.
    """
    N_bins = N_bits+1
    fit_intra = _binom.pmf(k=range(N_bins),n=N_bits,p=p_intra)
    fit_inter = _binom.pmf(k=range(N_bins),n=N_bits,p=p_inter)
    
    Dks_intra=[]
    Dks_inter=[]
    for i in range(N):
        data_intra,data_inter = sim_hamming_cideal(N_inst, N_rep, N_bits, p_intra, p_inter)
        
        hist_intra,_ = _histogram(data_intra, bins=N_bins, range=(0,N_bins), density=True)
        Dks_intra.append(abs(hist_intra.cumsum()-fit_intra.cumsum()).max())
        
        hist_inter,_ = _histogram(data_inter, bins=N_bins, range=(0,N_bins), density=True)
        Dks_inter.append(abs(hist_inter.cumsum()-fit_inter.cumsum()).max())        
        
        if verbose:
            if (i+1)%100==0:
                print(f"{(i+1)/100:.0f}/{N/100:.0f}",end='\r')
                
    return Dks_intra,Dks_inter