from numpy              import  unique as _unique,\
                                histogram as _histogram,\
                                array as _array
from scipy.stats        import  chisquare as _chisquare
from matplotlib.pyplot  import  bar as _bar,\
                                plot as _plot,\
                                annotate as _annotate


def intervalo_int(inf, sup):
    """
    Esta función auxiliar devuelve los extremos de números enteros dado un intervalo real.
    
    Parameters
    ----------
    inf : float
        Extremo inferior del intervalo.
    sup : float
        Extremo superior del intervalo.
        
    Returns
    -------
    list
        Rango de enteros entre los extremos `inf` y `sup`.
    """
    return list(range(int(inf)+(int(inf)<inf),int(sup)+1,1))
    
    
def chisq_gof_discrete(obs_data, model, alpha=0.05, plothist=False, **kwargs): # chi square goodness of fit discrete.
    """
    Realiza un test chi^2 de bondad del ajuste entre los datos observados y un modelo
    estocástico discreto (una función pmf -probability mass function-) que hipotéticamente
    genera dichos datos.

    Parameters
    ----------
    obs_data : array_like
        Vector de datos observados.

    model : callable
        Función pmf (probability mass function) que representa el modelo estocástico discreto.
        El primer argumento siempre debe ser la variable independiente (aleatoria).

    alpha : float, optional
        Nivel de significancia para el test chi^2. Por defecto es 0.05.

    plothist : bool, optional
        Indica si se debe graficar el histograma de los datos. Por defecto es False.

    kwargs : dict
        Opciones adicionales para pasar parámetros a la función pmf introducida como modelo.

    Returns
    -------
    dict
        Un diccionario con los campos:
        - 'chisq': Valor del estadístico chi^2.
        - 'pvalue': Valor tal que la probabilidad de obtener 'chisq' es al menos pvalue.
        - 'dof': Número de grados de libertad de la distribución chi^2.
        - 'testpass': Booleano que indica si el test ha sido superado en función de 'alpha'.

    Notes
    -----
    Esta función calcula un test chi^2 de bondad del ajuste entre los datos observados y un modelo
    estocástico discreto proporcionado como una función pmf. Se genera un histograma de los datos
    y se calculan los valores esperados de probabilidad para cada bin. Luego se aplica la función
    'chisquare' de scipy para realizar el test chi^2.

    El parámetro 'kwargs' se puede utilizar para pasar parámetros adicionales a la función pmf del modelo.
    """
    # Histograma de los valores observados
    obs_unique,obs = _unique(obs_data, return_counts=True)
    obs_edges = [0]+[i+0.1 for i in obs_unique] # sumamos 0.1 para que el histograma se construya
                                                # tomando ambos extremos del intervalo.
    sum_obs = sum(obs)
    if plothist:
        _bar(obs_unique,obs, edgecolor="white")

    # Generamos un modelo esperado: Bin(n=10,p=0.5), normalizado a la suma de los valores observados
    esp_hist = []
    for i in range(len(obs_edges[:-1])):
        esp_hist.append(sum([model(k, **kwargs) for k in intervalo_int(obs_edges[i], obs_edges[i+1])]))

    sum_esp_hist = sum(esp_hist)
    esp=[sum_obs*i/sum_esp_hist for i in esp_hist]
    if plothist:
        _plot(obs_unique,esp, '-ob')
    
    chisq,pvalue = _chisquare(obs,esp)
    if pvalue<alpha: # Se puede descartar la hipótesis con significancia 'alpha'
        testpass=True
    else:
        testpass=False
    
    return dict(chisq=chisq, pvalue=pvalue, dof=len(obs)-1, testpass=testpass)
            
            
def get_area_fraction(hist, p=0.9):
    """
    Encuentra los extremos 'xmin' y 'xmax' de una lista 'hist' de números (no necesariamente normalizada)
    de tal manera que el intervalo 'hist[xmin:xmax]' contenga al menos una fracción 'p' de la suma acumulada
    de los elementos de la lista.

    Parameters
    ----------
    hist : list or array_like
        Lista de números.

    p : float, optional
        Fracción mínima deseada. Por defecto es 0.9.

    Returns
    -------
    xmin : int
        Índice del extremo izquierdo del intervalo.

    xmax : int
        Índice del extremo derecho del intervalo.

    Notes
    -----
    Esta función busca los extremos 'xmin' y 'xmax' en la lista 'hist' de números, de tal manera que el intervalo
    'hist[xmin:xmax]' contenga al menos una fracción 'p' de la suma acumulada de los elementos de la lista. La búsqueda
    comienza desde el índice correspondiente al valor máximo de la lista y se desplaza simétricamente hacia ambos extremos.

    """
    xmin = hist.argmax()
    xmax=xmin
    suma_contenida=sum([hist[i] for i in range(xmin,xmax,1)])
    while suma_contenida<p:
        if xmin>0:
            xmin-=1
        if xmax<len(hist):
            xmax+=1
        suma_contenida=sum([hist[i] for i in range(xmin,xmax,1)])
    
    return xmin,xmax


def get_hist_smooth(data, p=0.9, x0=float('nan'), x1=float('nan')):
    """
    Genera un histograma suavizado a partir de un conjunto de datos 'data'.

    Parameters
    ----------
    data : array_like
        Conjunto de datos para representar en forma de histograma.

    p : float, optional
        Fracción (tanto por uno) de área bajo la cual se busca que el histograma no presente máximos relativos.
        Por defecto es 0.9.

    x0 : float, optional
        Extremo inferior del rango del histograma. Por defecto es NaN (se calcula automáticamente a partir de 'data').

    x1 : float, optional
        Extremo superior del rango del histograma. Por defecto es NaN (se calcula automáticamente a partir de 'data').

    Returns
    -------
    numpy.ndarray
        Histograma suavizado generado.

    Notes
    -----
    Esta función devuelve un histograma con el número de bins adecuado para que no presente máximos locales,
    lo cual resulta en un aspecto más suave. El parámetro 'p' representa la fracción de área bajo la cual
    se busca que el histograma no presente máximos relativos, medido simétricamente desde el punto máximo
    del histograma.

    Los parámetros 'x0' y 'x1' son opcionales y se utilizan para definir el rango del histograma; si no se
    proporcionan, se calculan automáticamente a partir de los valores mínimo y máximo de 'data'.
    """
    if np_isnan(x0):
        x0 = min(data)
    if np_isnan(x1):
        x1 = max(data)
        
    num_bins = 10
    num_bins_old = 0
    while 1:
        hist,bins = np_histogram(data, bins=num_bins, density=True, range=(x0,x1))
        hist_aux = hist[:]/hist.sum()
        xmin,xmax = get_area_fraction(hist_aux, p)
        num_max = 0
        for i in range(xmin+1, xmax-1,1):
            if hist_aux[i]>hist_aux[i-1] and hist_aux[i]>hist_aux[i+1]:
                num_max+=1
            if num_max==2:
                num_bins_old=num_bins
                num_bins-=1
                break
        if num_max==1:
            if num_bins+1==num_bins_old:
                break
            else:
                num_bins_old = num_bins
                num_bins+=1
    
    return hist,bins
    
    
def bin_rv_discrete(rv_discrete, bins_in, *args):
    """
    Calcula la probabilidad acumulada de un objeto 'rv_discrete' de scipy en los extremos dados por 'bins'.

    Parameters
    ----------
    rv_discrete : scipy.stats.rv_discrete
        Objeto 'rv_discrete' de scipy.

    bins_in : array_like
        Bineado del eje de abscisas. Debe ser una lista de 'N_bins+1' elementos, conteniendo los extremos de cada bin.

    args : list, optional
        Parámetros extra para pasar a 'rv_discrete', distintos del primer argumento ('k').

    Returns
    -------
    numpy.ndarray
        Arreglo de tamaño 'N_bin' elementos que contiene la probabilidad acumulada en los extremos dados por 'bins'.
        El arreglo de salida está normalizado respecto a la suma de sus elementos.

    Notes
    -----
    Esta función toma un objeto 'rv_discrete' de scipy y una lista 'bins_in' que contiene el bineado del eje de abscisas.
    Calcula la probabilidad acumulada en los extremos dados por 'bins' y devuelve un numpy array normalizado con las probabilidades acumuladas.

    El argumento 'args' se utiliza para pasar parámetros extra distintos del primero ('k') a 'rv_discrete'.
    """
    bins = [i-1e-6 if i.is_integer() else i for i in bins_in] # Así nos aseguramos de que el binneado sea cerrado por la izda. y abierto por la dcha.
    
    data_exp = [rv_discrete.cdf(int(bins[1]),*args)]
    for i in range(1,len(bins)-2,1):
        data_exp += [rv_discrete.cdf(int(bins[i+1]),*args)-rv_discrete.cdf(int(bins[i]),*args)]
    data_exp += [1-rv_discrete.cdf(int(bins[-2]),*args)]

    return _array(data_exp)
    
    
def bin_rv_cont(rv_continuous, bins_in, *args):
    """
    Calcula la probabilidad acumulada de un objeto 'rv_continuous' de scipy en los extremos dados por 'bins'.

    Parameters
    ----------
    rv_continuous : scipy.stats.rv_continuous
        Objeto 'rv_continuous' de scipy.

    bins_in : array_like
        Bineado del eje de abscisas. Debe ser una lista de 'N_bins+1' elementos, conteniendo los extremos de cada bin.

    args : list, optional
        Parámetros extra para pasar a 'rv_continuous.cdf', distintos del primer argumento ('x').

    Returns
    -------
    numpy.ndarray
        Arreglo de tamaño 'N_bin' elementos que contiene la probabilidad acumulada en los extremos dados por 'bins'.
        El arreglo de salida está normalizado respecto a la suma de sus elementos.

    Notes
    -----
    Esta función toma un objeto 'rv_continuous' de scipy y una lista 'bins_in' que contiene el bineado del eje de abscisas.
    Calcula la probabilidad acumulada en los extremos dados por 'bins' utilizando 'rv_continuous.cdf' y devuelve un numpy array
    normalizado con las probabilidades acumuladas.

    El argumento 'args' se utiliza para pasar parámetros extra distintos al primero ('x') a 'rv_continuous.cdf'.
    """
    bins = bins_in[:]
    
    data_exp = [rv_continuous.cdf(bins[1],*args)]
    for i in range(1,len(bins)-2,1):
        data_exp += [rv_continuous.cdf(bins[i+1],*args)-rv_continuous.cdf(bins[i],*args)]
    data_exp += [1-rv_continuous.cdf(bins[-2],*args)]
    
    return _array(data_exp)
    
    
def Dks_montecarlo_discrete(model, fit, N, verbose=True, **kwargs):
    """
    Calcula la distribución del estadístico KS de un modelo discreto en comparación con una distribución teórica.

    Parameters
    ----------
    model : callable
        Función aleatoria a contrastar. El primer parámetro que debe aceptar es 'N', seguido de un número arbitrario de
        parámetros que son pasados mediante 'kwargs'. Devuelve un array de valores aleatorios.

    fit : list of float
        Lista de valores que representa la distribución de probabilidad contra la cual se calcula la distribución de KS.

    N : int
        Número de veces que se repite el cálculo de Dks.

    verbose : bool, optional
        Si es True, imprime el progreso. Por defecto es True.

    kwargs : dict
        Parámetros a pasar a 'model'.

    Returns
    -------
    list of float
        Lista de valores Dks.

    Notes
    -----
    Esta función calcula la distribución del estadístico KS de un modelo 'model' que produce 'N' valores aleatorios frente a
    una curva teórica 'fit', representada como una lista de valores. Devuelve una lista de estos índices KS.

    El método 'model' admite una cantidad arbitraria de parámetros pasados mediante 'kwargs'.
    """
    fit = _array(fit)
    Dks=[]
    for i in range(N):
        data = model(**kwargs)
        hist_data,_ = _histogram(data, bins=fit.size, range=(0,fit.size), density=True)
        Dks.append(abs(hist_data.cumsum()-fit.cumsum()).max())
        
        if verbose:
            if (i+1)%100==0:
                print(f"{(i+1)/100:.0f}/{N/100:.0f}",end='\r')
    return Dks
    
    
def fit_rv_cont(data, rv_cont, bins=10, alpha=0.05, plot=False):
    """
    Encuentra la distribución 'rv_continuous' que mejor ajusta un conjunto de datos 'data' y calcula el 'valor p' correspondiente a una significancia 'alpha'.

    Parameters
    ----------
    data : list of float
        Vector con los datos a ajustar.

    rv_cont : scipy.stats.rv_continuous
        Objeto 'scipy.stats.rv_continuous' que define una distribución de probabilidad continua que se ajustará a los datos 'data'.

    bins : int, optional
        Número de cajas para el histograma de 'data'. Por defecto es 10.

    alpha : float, optional
        Valor de significancia 'alpha' para el cual se calcula el valor p. Por defecto es 0.05.

    plot : bool, optional
        Si es True, pinta el histograma y superpone la curva encontrada. La función no ejecuta 'show()', de forma que el usuario puede recabar la figura externamente
        con 'gca()' y 'gcf()', y editarla antes de representarla. Por defecto es False.

    Returns
    -------
    list
        Una lista que contiene tres elementos: 
        - El primero es una lista de los parámetros que mejor ajustan la distribución (en el mismo orden en que se definen en la función 'rv_cont.pdf').
        - El segundo es un nparray con el histograma.
        - El tercero es un nparray con el bineado de los datos de entrada.

    Notes
    -----
    Esta función es un wrapper para encontrar la distribución 'rv_continuous' que mejor ajusta un conjunto de valores 'data' utilizando la máxima verosimilitud.
    Calcula el 'valor p' correspondiente a una significancia 'alpha'. Resulta útil para distribuciones obtenidas por simulación, que carecen de una función
    densidad teórica.
    """
    hist,edges = _histogram(data, density=True, bins=bins, range=(min(0,min(data)),max(data)))
    params = rv_cont.fit(data)
   
    p_val = rv_cont.ppf(1-alpha, *params)
    print(f"alpha: {alpha} --> p val: {p_val}")

    if plot:
        _plot(edges[:-1],hist.sum()*bin_rv_cont(rv_cont, edges, *params),color='C1', label="Interpolación",lw=3)
        _bar(edges[:-1],hist, width=0.9*(edges[1]-edges[0]),color='C0', label="Simulación")
        if alpha:
            _annotate(text=f"${alpha*100:.0f} \%$", xy=(p_val,0), xytext=(p_val,0.95*max(hist)),
                         arrowprops=dict(width=1,headwidth=0,color='grey'),color='grey')

    return params,hist,edges
   
