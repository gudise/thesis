from matplotlib.pyplot import figure as plt_figure
from numpy             import histogram as np_histogram,\
                              isnan as np_isnan
from multiprocessing   import Pool as _Pool



class BarraProgreso:
    """
    Esta clase instancia un objeto BarraProgreso.

    Parameters
    ----------
    total : int
        Número de iteraciones que hacen el 100% de la barra.
    long_barra : int, opcional
        Número de caracteres que componen la barra, por defecto 40.
    caracter : char, opcional
        Caracter que se utiliza para decorar la barra, por defecto '#'.

    Returns
    -------
    None
    """
    def __init__(self, total, long_barra=40, caracter='#'):
        self.total = total
        self.long_barra = long_barra
        self.caracter = caracter
        self.indice = 0
                    
        print(f" |{''.join(['.' for i in range(self.long_barra)])}| {0.00:.2f} %", end='\r')
        
    def __call__(self, incr=1):
        """
        Actualiza la barra. En función de las variables `total` y `long_barra`, esta actualización puede reflejarse o no en el dibujo.
        
        Parameters
        ----------        
        incr : int, opcional
            Este valor indica en cuantas unidades debe incrementarse el contador de iteraciones `indice`, por defecto `1`.
            
        Returns
        -------
        None
        """
        self.indice+=incr
        
        perone = self.indice/self.total
        n_puntos = int(self.long_barra*perone)
        
        if n_puntos==self.long_barra:
            print(f" |{''.join([self.caracter for i in range(self.long_barra)])}| {100.00:.2f} %")
        else:
            print(f" |{''.join([self.caracter for i in range(n_puntos)])+''.join(['.' for i in range(self.long_barra-n_puntos)])}| {100*perone:.2f} %", end='\r')
            
            
def export_legend_plt(ax, name='legend.pdf', pad_inches=0.1, **kwargs):
    """
    Exporta la leyenda de la figura actualmente activa.

    Esta función es útil para exportar la leyenda de un gráfico cuando se tienen
    múltiples gráficos que comparten la misma leyenda. Es importante tener en cuenta
    que se requiere haber creado una figura con 'plt.plot', la cual debe incluir una
    leyenda, y NO se debe haber ejecutado 'plt.show()'.

    Parameters
    ----------
    ax : `matplotlib.axes.Axes`
        Los ejes de la figura de los cuales se quiere extraer la leyenda. Si se utilizan
        comandos como 'plot', etc., se puede obtener los ejes activos del gráfico con
        ax=plt.gca().

    name : str, opcional
        Nombre de la imagen. Por defecto es "legend.pdf".

    pad_inches : float, opcional
        Esta opción se pasa directamente a la función "savefig". Por defecto es 0.1.

    kwargs : dict
        Las opciones introducidas aquí se pasan directamente a la función `ax.legend`.

    Returns
    -------
    None
    """

    # Create a legend without the plot lines in a new empty figure
    fig_legend = plt_figure(figsize=(3, 3))  # Adjust the size as needed
    legend = fig_legend.legend(*ax.get_legend_handles_labels(), frameon=False, **kwargs)

    # Save the legend as a standalone image (in this case, PNG format)
    fig_legend.savefig(name, bbox_inches='tight', pad_inches=pad_inches)
    
    
def set_size_plt(ax, x=4/3*4, y=4):
    """
    Esta función permite dibujar una figura fijando el tamaño del plot, y no de la figura completa. Toma los valores `x`, `y` que representan las dimensiones en dichos ejes de un plot, y construye la figura del tamaño que sea necesario para acomodar los ejes.
    
    Parameters
    ----------
    ax : `matplotlib.axes.Axes`
        Objeto que contiene la figura a redimensionar.
    x : float, opcional
        Nueva anchura, por defecto 16/3.
    y : float, opcional
        Nueva altura, por defecto 4.
        
    Returns
    -------
    None
    """
    l = ax.figure.subplotpars.left
    r = ax.figure.subplotpars.right
    t = ax.figure.subplotpars.top
    b = ax.figure.subplotpars.bottom
    figw = float(x)/(r-l)
    figh = float(y)/(t-b)
    ax.figure.set_size_inches(figw, figh)
    
    
def run_in_parallel(func, args):
    """
    Ejecuta la función en paralelo utilizando argumentos proporcionados.

    Esta función toma un método 'func' y una lista de argumentos 'args', y repite la ejecución
    de la función en paralelo. El método 'func' puede recibir un número arbitrario y tipos de argumentos,
    pero serán pasados en orden según aparezcan en 'args'.

    Parameters
    ----------
    func : function
        Procedimiento que puede tomar un número arbitrario y tipos de argumentos, pero son específicos
        en su posición.

    args : list
        Lista que contiene una lista con los argumentos a pasar a 'func' en cada proceso.

    Returns
    -------
    list
        Lista con los resultados de cada proceso.

    Warnings
    --------
    Esta función siempre debe ser ejecutada dentro de `if __name__=='__main__':`.
    """
    with _Pool() as pool:
        return pool.starmap(func, args)

    return results
