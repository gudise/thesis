from numpy              import  unique as _unique,\
                                histogram as _histogram,\
                                array as _array,\
                                linspace as _linspace
from scipy.stats        import  chisquare as _chisquare,\
                                skewnorm as _skewnorm
from scipy.optimize     import  curve_fit as _curve_fit,\
                                root_scalar as _root_scalar
from matplotlib.pyplot  import  bar as _bar,\
                                plot as _plot,\
                                annotate as _annotate


def intervalo_int(inf, sup):
    """Esta función auxiliar devuelve los extremos de números enteros dado un intervalo real."""
    return list(range(int(inf)+(int(inf)<inf),int(sup)+1,1))
    
    
def chisq_gof_discrete(obs_data, model, alpha=0.05, plothist=False, **kwargs): # chi square goodness of fit discrete.
    """
    chisq_gof_discrete(obs_data, model, alpha=0.05, verbose=True, plothist=False, **kwargs)
    
    Esta función toma un vector de datos observados y un modelo estocástico discreto
    (una función pmf -probability mass function-) que hipotéticamente genera dichos
    datos, y ejecuta un test chi^2 de bondad del ajuste. Para ello se realiza en primer
    lugar un histograma de los datos introducidos, con un binneado tal que no queda ninguna
    caja sin ningún dato; a continuación se calcula el valor de probabilidad esperado para
    cada caja como la distribución acumulada (i.e., la suma) de probabilidades
    correspondientes a cada caja. Finalmente se ejecuta la función 'chisquare' de
    scipy, y se presentan los resultados.
    La opción de diccionario 'kwargs' se puede utilizar para pasara parámetros a la función
    pmf introducida como modelo. El primer argumento de la función modelo siempre debe ser
    la variable independiente (aleatoria).
    La función devuelve un diccionario con los campos 'chisq' (valor del estadístico chi^2),
    'pvalue' (valor tal que la probabilidad de obtener 'chisq' es al menos pvalue), 'dof' con
    el número de grados de libertad de la distribución chi^2, y 'testpass' que contiene un
    booleano True o False en función de si el test ha sido superado o no, a la luz del valor
    'alpha' introducido.
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
    Esta función toma una lista 'hist' de numeros (no necesariamente normalizada)
    y devuelve los extremos 'xmin', 'xmax' tales que en el intervalo 'hist[xmin:xmax]' se encuentra
    contenido el 'p' por uno de la suma acumulada de los elementos de la lista. La búsqueda de tales extremos se
    lleva a cabo partiendo del índice que corresponde al valor máximo de la lista, y desplazándose simétricamente hacia ambos
    extremos.
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
    Esta función toma un conjunto de datos 'data' para representar en forma de histograma,
    y devuelve un histograma con el número de bins adecuado para que el histograma no
    presente máximos locales, lo cual redunda en un aspecto más suave. En ocasiones puede
    ser aceptable que el histograma presente máximos relativos en las colas del histograma, ya
    que esto no perjudica gravemente el aspeccto del mismo; para estos casos, se proporiona
    un parámetro 'p' que representa la fracción (tanto por uno) de área bajo la cual la
    función buscará que el histograma no presente máximos relativos, contando desde el máximo
    del histograma, e.g., si p=0.8, esta función devuelve el histograma que no presenta máximos
    relativos en el 80% del área del histograma, medido simétricamente desde el punto máximo
    del histograma. Los parámetros x0, x1 son opcionales, y se utilizan para definir respectivamente
    el extremo inferior y superior del rango del histograma; si no se pasan estas opciones, el rango se 
    calcula automáticamente a partir de los valores mínimo y máximo de 'data'.
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
    
    
def bin_rv_discrete(rv_discrete, bins_in, **kwargs):
    """
    Esta función toma un objeto 'rv_discrete' de scip y lista 'bins' conteniendo un bineado del eje de abscisas
    (i.e., una lista de 'bins' tal y como es devuelta por la función 'numpy.histogram',
    es decir una lista de N_bins+1 elementos, conteniendo los ectremos de cada bin),
    y devuelve una lista de tamaño 'N_bin' elementos conteniendo la probabilidad
    acumulada en los extremos dados por 'bins'. El argumento '**kwargs' se utiliza
    para pasar parámetros extra a 'rv_discrete'. El arreglo de salida está normalizado respecto de la suma
    de sus elementos.
    """
    bins = [i-1e-6 if i.is_integer() else i for i in bins_in] # Así nos aseguramos de que el binneado sea cerrado por la izda. y abierto por la dcha.
    
    data_exp = [rv_discrete.cdf(k=int(bins[1]),**kwargs)]
    for i in range(1,len(bins)-2,1):
        data_exp += [rv_discrete.cdf(k=int(bins[i+1]),**kwargs)-rv_discrete.cdf(k=int(bins[i]),**kwargs)]
    data_exp += [1-rv_discrete.cdf(k=int(bins[-2]),**kwargs)]

    return data_exp
    
    
def bin_rv_cont(rv_continuous, bins_in, **kwargs):
    """
    Esta función toma un objeto 'rv_continuous' de scipy y una lista 'bins' conteniendo un bineado del eje de abscisas
    (i.e., una lista de 'bins' tal y como es devuelta por la función 'numpy.histogram',
    es decir una lista de N_bins+1 elementos, conteniendo los ectremos de cada bin),
    y devuelve una lista de tamaño 'N_bin' elementos conteniendo la probabilidad
    acumulada en los extremos dados por 'bins'. El argumento '**kwargs' se utiliza
    para pasar parámetros extra a 'rv_continuous'. El arreglo de salida está normalizado respecto de la suma
    de sus elementos.
    """
    bins = bins_in[:]
    
    data_exp = [rv_continuous.cdf(x=bins[1],**kwargs)]
    for i in range(1,len(bins)-2,1):
        data_exp += [rv_continuous.cdf(x=bins[i+1],**kwargs)-rv_continuous.cdf(x=bins[i],**kwargs)]
    data_exp += [1-rv_continuous.cdf(x=bins[-2],**kwargs)]
    
    return data_exp
    
    
def Dks_montecarlo_discrete(model, fit, N, verbose=True, **kwargs):
    """Esta función calcula la distribución del estadístico KS de un modelo `model`que produce N valores aleatorios frente a
    una curve teórica `fit`, que se da como una lista de valores. La función devuelve una lista de `T` de estos índices KS.
    El método `model` admite una cantidad arbitraria de parámetros pasados mediante `kwargs`.
    
    :param model: Función aleatoria a contrastar. El primer parámetro que debe aceptar es `N`, seguido de un número arbitrario de parámetros que son pasados mediante `kwargs`. Devuelve un array de valores aleatorios.
    :type model: Método
    
    :param fit: Lista de valores que representa la distribución de probabilidad contra la cual se calcula la distribución de KS.
    :type fit: Lista de float

    :param N: Número de veces que se repite el cálculo de Dks
    :type N: int

    :param verbose: Si `True`pinta el progreso.
    :type verbose: bool, opcional.
    
    *kwargs : lista de parámetros que pasar a `model`.

    :return: Lista de valores Dks,
    :rtype: Lista de float.
    """
    fit = _array(fit)
    Dks=[]
    for i in range(N):
        data = model(**kwargs)
        hist_data,_ = _histogram(data, bins=fit.size, range=(0,fit.size), density=True)
        Dks.append(abs(hist_data.cumsum()-fit.cumsum()).max())
        
        if verbose:
            if (i+1)%100==0:
                print(f"{(i+1)/100:.0f} %",end='\r')
    return Dks
    
    
def fit_skewnorm(data, bins=10, alpha=False, plot=False):
    """Esta función toma una distribución `data`, ajusta una curva 'skew norm' al histograma, y calcula el 'valor p' correspondiente a una significancia `alpha`. Resulta útil para distribuciones obtenidas por simulación, que carecen de una función densidad teórica.
    
    :param data: Vector con los datos a ajustar.
    :type data: Lista de float.

    :param bins: Número de cajas para el histograma de `data`.
    :type bins: int, opcional.

    :param alpha: Valor de significancia 'alpha' para el cual se calcula el valor p, i.e., el valor tal que de p a infinito la proporción de área bajo la densidad es `alpha`.
    :type alpha: float, opcional.

    :param plot: Si `True` pinta el histograma y superpone la curva encontrada. La función no ejecuta 'show()', de forma que el usuario puede recabar la figura externamente con 'gca()' y 'gcf()', y editarla antes de representarla.
    :type plot: bool, opcional.

    :return: La función devuelve una lista que contiene los tres parámetros que ajustan la curva (a, loc, scale), y el histograma y bineado de los datos de entrada.
    """
    def _func(x,a,loc,scale,alpha):
        return _skewnorm.cdf(x=x,a=a,loc=loc,scale=scale)-(1-alpha)
    hist,edges = _histogram(data, density=True, bins=bins, range=(min(0,min(data)),max(data)))
    a,loc,scale = _curve_fit(_skewnorm.pdf, xdata=edges[:-1], ydata=hist, p0=[1,1,1])[0]
   
    if alpha:
        p_val = _root_scalar(_func, args=(a,loc,scale,alpha), bracket=(edges[0],edges[-1]))['root']
        print(f"alpha: {alpha} --> p val: {p_val}")

    if plot:
        _plot(_linspace(edges[0],edges[-1],100),_skewnorm.pdf(x=_linspace(edges[0],edges[-1],100),a=a,loc=loc,scale=scale),color='C1', label="Interpolación",lw=3)
        _bar(edges[:-1],hist, width=0.9*(edges[1]-edges[0]),color='C0', label="Simulación")
        if alpha:
            _annotate(text=f"${alpha*100:.0f} \%$", xy=(p_val,0), xytext=(p_val,0.95*max(hist)),
                         arrowprops=dict(width=1,headwidth=0,color='grey'),color='grey')

    return a,loc,scale,hist,edges  
