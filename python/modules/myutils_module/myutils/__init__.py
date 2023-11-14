from matplotlib.pyplot import figure as plt_figure
from numpy             import histogram as np_histogram,\
                              isnan as np_isnan
from multiprocessing   import Pool as _Pool



class BarraProgreso:
    """Esta clase instancia un objeto :class:`BarraProgreso`
    
    :param total: Número de iteraciones que hacen el 100% de la barra.
    :type total: int
    
    :param long_barra: Número de caracteres que componen la barra, por defecto 40
    :type long_barra: int, opcional
    
    :param caracter: Caracter que se utiliza para decorar la barra, por defecto `#`
    :type caracter: char, opcional
    """
    def __init__(self, total, long_barra=40, caracter='#'):
        """Constructor
        """
        self.total = total
        self.long_barra = long_barra
        self.caracter = caracter
        self.indice = 0
                    
        print(f" |{''.join(['.' for i in range(self.long_barra)])}| {0.00:.2f} %", end='\r')
        
    def __call__(self, incr=1):
        """Actualiza la barra; en función de las variables `total`
        y `long_barra`, esta actualización puede reflejarse o
        no en el dibujo.
        
        :param incr: Este valor indica en cuantas unidades debe incrementarse el contador
                de iteraciones `indice`, por defecto `1`
        :type incr: int, opcional
        """
        self.indice+=incr
        
        perone = self.indice/self.total
        n_puntos = int(self.long_barra*perone)
        
        if n_puntos==self.long_barra:
            print(f" |{''.join([self.caracter for i in range(self.long_barra)])}| {100.00:.2f} %")
        else:
            print(f" |{''.join([self.caracter for i in range(n_puntos)])+''.join(['.' for i in range(self.long_barra-n_puntos)])}| {100*perone:.2f} %", end='\r')
            
            
def export_legend_plt(ax, name='legend.pdf', pad_inches=0.1, **kwargs):
    """Esta función permite exportar la leyenda la figura actualmente
    activa; muy útil para hacer múltiples plot en los cuales la
    leyenda es compartida. Notar que para usarse, debe haberse 
    creado una figura con 'plt.plot', la cual debe tener una leyenda,
    y NO debe haberse realizado un 'plt.show()'.
    
    :param ax: Ejes de la figura cuya leyenda se quiere extraer. Si usamos
        los comandos 'plot' etc, podemos obtener el eje del plot
        activo como ax=plt.gca()
    :type ax: `pyplot.Axe`
    
    :param name: Nombre de la imagen, por defecto "legend.pdf"
    :type name: str, opcional
    
    :param pad_inches: Esta opción se pasa tal cual a la función "savefig", por defecto 0.1
    :type pad_inches: float, opcional
    
    **kwargs: Las opciones introducidas aquí son pasadas tal cual a la función `ax.legend`
    """

    # Create a legend without the plot lines in a new empty figure
    fig_legend = plt_figure(figsize=(3, 3))  # Adjust the size as needed
    legend = fig_legend.legend(*ax.get_legend_handles_labels(), frameon=False, **kwargs)

    # Save the legend as a standalone image (in this case, PNG format)
    fig_legend.savefig(name, bbox_inches='tight', pad_inches=pad_inches)
    
    
def run_in_parallel(func, args):
    """Esta función toma un método 'func' y una lista argumentos de 'args', y repite la ejecución de la función en paralelo.
    El método 'func' puede reibir argumentos arbitrarios, pero serán pasados en orden según aparezcan en 'args'.
    
    :param func: Procedimiento, el cual puede tomar un número y tipo de argumentos arbitrario, pero posición-específicos.
    :type func: Función de Python
    
    :param args: Lista que contiene una lista con los argumentos a pasar a 'func' en cada proceso.
    :type args: Lista de lista.
    
    :return: Lista con los resultados de cada proceso.
    :rtype: Lista.
    
    .. warnings: Esta función siempre bede ser ejecutada dentro de `if __name__=='__main__':`
    """
    with _Pool() as pool:
        return pool.starmap(func, args)

    return results
