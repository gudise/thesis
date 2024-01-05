"""
Este módulo permite definir nombres para los ejes
de un numpyarray.    
"""

from numpy import array as _array,\
                  savetxt as _savetxt,\
                  loadtxt as _loadtxt,\
                  append as _append


class Tensor:
    """
    Un objeto que es esencialmente un nparray junto con una variable 'self.axis' que permite indexar los ejes del array mediante un nombre natural, en lugar de un índice entero.

    Parameters
    ----------
    array : numpy.ndarray
        Arreglo que contiene los datos y la forma del tensor.

    axis : list of strings, optional
        Nombres de los ejes. El orden en que se proporcionan los nombres se correlaciona
        con el correspondiente eje del parámetro 'array'. Si no se introduce una lista de nombres en esta variable,
        se asignarán por defecto como nombres el índice de cada eje (empezando en '0').

    Attributes
    ----------
    array : numpy.ndarray
        Arreglo que contiene los datos y la forma del tensor.

    shape : tuple
        Forma del tensor (igual que en un nparray).

    axis : list of strings
        Nombres de los ejes. El i-ésimo elemento contiene el nombre del i-ésimo eje.
        Si no se proporciona, se asignan nombres predeterminados en forma de índices.
    """
    def __init__(self, array, axis=False):
        if type(array) == type([]):
            self.array = _array(array)
        else:
            self.array = array
        self.shape = self.array.shape
        if axis:
            self.axis = axis
        else:
            self.axis  = [f"{i}" for i in range(len(self.shape))]
            
    def size(self, axis):
        """
        Esta función toma el nombre de un eje y devuelve su dimensión.
        
        Parameters
        ----------
        axis : str
            Nombre del eje del que se desea obtener la dimensión.
        
        Returns
        -------
        int
            Dimensión del eje especificado.
        """
        return self.shape[self.axis.index(axis)]
            
    def item(self, **axes):
        """
        Esta función toma por argumento el nombre de cada eje igualado a un 
        índice, y devuelve el valor guardado en esa coordenada.
        
        Parameters
        ----------
        axes : dict
            Argumentos clave-valor donde las claves son los nombres de los ejes y
            los valores son los índices para esos ejes.
        
        Returns
        -------
        object
            Valor almacenado en las coordenadas especificadas.
        """
        return self.array.item(*[int(axes[ax_name]) for ax_name in self.axis])
        
    def take(self, **axes):
        """
        Toma una serie de ejes (por nombre), igualados a una lista de índices, y devuelve un Tensor
        reducido a los índices seleccionados.
        
        Parameters
        ----------
        axes : dict
            Argumentos clave-valor donde las claves son los nombres de los ejes y los valores
            son listas de índices a lo largo de esos ejes.

        Returns
        -------
        Tensor
            Tensor reducido a los índices seleccionados a lo largo de los ejes especificados.
        """
        axe=[]
        slicing_position=[]
        for key, value in axes.items():
            axe.append(key)
            slicing_position.append(value)
        aux = self.array[:]
        for i,a in enumerate(axe):
            aux = aux.take(slicing_position[i], axis=self.axis.index(a))
        return Tensor(aux, self.axis)
    
    def slice(self, **axes):
        """
        Toma una serie de ejes (por nombre), igualados a una porción 'slice'. Esta porción puede ser
        un entero, una lista de dos enteros, o una lista de tres enteros, y devuelve un Tensor reducido al 'slice'
        indicado (usando la misma notación que un slice estándar en Python: [x] es la x-ésima entrada, [x,y] son
        todas entradas en [x,y), y [x,y,z] son todas las entradas en [x,y) tomadas en saltos de z). Esta función
        es en realidad un wrapper a 'take', usando la nomenclatura de Python para hacer slicing.
        
        Parameters
        ----------
        axes : dict
            Argumentos clave-valor donde las claves son los nombres de los ejes y los valores
            son objetos 'slice' o listas de enteros para realizar la porción en esos ejes.

        Returns
        -------
        Tensor
            Tensor reducido al 'slice' indicado a lo largo de los ejes especificados.
        """
        axe=[]
        slicing_position=[]
        for key, value in axes.items():
            axe.append(key)
            if type(value)==type([]) or type(value)==type(()):
                if len(value)==1:
                    slicing_position.append([value[0]])
                elif len(value)==2:
                    slicing_position.append(list(range(value[0],value[1])))
                elif len(value)==3:
                    slicing_position.append(list(range(value[0],value[1],value[2])))
            else:
                slicing_position.append([value])        
        aux = self.array[:]
        for i,a in enumerate(axe):
            aux = aux.take(slicing_position[i], axis=self.axis.index(a))
        return Tensor(aux, self.axis)
    
    def swap(self, axe1, axe2):
        """
        Devuelve un Tensor con el mismo "array" pero los ejes "axe1" y "axe2" intercambiados.

        Parameters
        ----------
        axe1 : int
            Índice del primer eje a intercambiar.

        axe2 : int
            Índice del segundo eje a intercambiar.

        Returns
        -------
        Tensor
            Tensor con los ejes "axe1" y "axe2" intercambiados.
        """
        result_array = self.array.swapaxes(self.axis.index(axe1), self.axis.index(axe2)).copy()
        result_axis = self.axis[:]
        result_axis[self.axis.index(axe1)],result_axis[self.axis.index(axe2)] = result_axis[self.axis.index(axe2)],result_axis[self.axis.index(axe1)]
        return Tensor(result_array, result_axis)
        
    def mean(self, axe):
        """
        Devuelve un Tensor comprimido en el eje 'axe', suprimiéndolo y calculando el promedio a lo largo de dicho eje.

        Parameters
        ----------
        axe : int
            Índice del eje a comprimir y calcular el promedio.

        Returns
        -------
        Tensor
            Tensor con el eje 'axe' suprimido y cada punto contiene el promedio a lo largo de ese eje.
        """
        result_array = self.array.mean(axis=self.axis.index(axe))
        result_axis = self.axis[:]
        result_axis.remove(axe)
        return Tensor(result_array, result_axis)
        
    def std(self, axe):
        """
        Devuelve un Tensor comprimido en el eje 'axe', suprimiéndolo y calculando la desviación estándar a lo largo de dicho eje.

        Parameters
        ----------
        axe : int
            Índice del eje a comprimir y calcular la desviación estándar.

        Returns
        -------
        Tensor
            Tensor con el eje 'axe' suprimido y cada punto contiene la desviación estándar a lo largo de ese eje.
        """
        result_array = self.array.std(axis=self.axis.index(axe))
        result_axis = self.axis[:]
        result_axis.remove(axe)
        return Tensor(result_array, result_axis)
        
    def save(self, file_name, fmt='%.18e'):
        """
        Guarda el Tensor en un archivo de texto.

        Parameters
        ----------
        file_name : str
            Nombre del archivo donde se guardará el Tensor.

        fmt : str, optional
            Formato numérico de los datos. Sigue el mismo formato que el parámetro 'fmt'
            de numpy.savetxt.

        Returns
        -------
        None
        """
        nfilas = self.shape[0]
        ncolumnas = 1
        for i in self.shape[1:]:
            ncolumnas*=i
        encabezado="#[axis] "+" ".join(self.axis)
        encabezado+="\n#[shape] "+" ".join([f"{i}" for i in self.shape])+"\n"

        _savetxt(file_name, self.array.flatten().reshape(nfilas, ncolumnas), fmt=fmt, header=encabezado, comments="")
        
    def squeeze(self):
        """
        Elimina todos los ejes del tensor cuya dimensión sea igual a '1'.

        Returns
        -------
        Tensor
            Tensor resultante después de eliminar los ejes con dimensión '1'.
        """
        aux = self.axis[:]
        for i in self.axis:
            if self.size(i)==1:
                aux.remove(i)
        return Tensor(self.array.squeeze(),aux)


def load_tensor(file_name):
    """
    Devuelve un Tensor almacenado en un archivo de texto.

    Parameters
    ----------
    file_name : str
        Nombre del archivo que contiene el Tensor.

    Returns
    -------
    Tensor
        Tensor cargado desde el archivo de texto.
    """
    axis=False
    with open(file_name, "r") as f:
        while 1:
            aux = f.readline()
            if aux[0]=='#':
                if aux.split('[')[1].split(']')[0] == 'axis':
                    axis=aux.split('[')[1].split(']')[1].split()
                elif aux.split('[')[1].split(']')[0] == 'shape':
                    shape=aux.split('[')[1].split(']')[1].split()
            else:
                break
    array = _loadtxt(file_name)
    for i in range(len(shape)):
        shape[i]=int(shape[i])
    return Tensor(array.flatten().reshape(shape), axis)
    
    
def append(axe, *args):
    """
    Concatena dos objetos 'Tensor' a lo largo de un eje especificado por su nombre.

    Parameters
    ----------
    axe : str
        Nombre del eje a lo largo del cual se realizará la concatenación.

    args : Tensor
        Tensores que se concatenarán. Se deben proporcionar al menos dos tensores de entrada.
        Todos los tensores deben tener un parámetro 'Tensor.axis' idéntico y las mismas
        dimensiones en todos los ejes excepto posiblemente en 'axe'.

    Returns
    -------
    Tensor
        Nuevo Tensor que consiste en la concatenación de los argumentos en el orden proporcionado.

    Raises
    ------
    ValueError
        Si no se cumplen los requisitos de entrada (al menos dos tensores y 'axe').
    """
    try:
        result =  Tensor(array=_append(args[0].array,args[1].array,args[0].axis.index(axe)), axis=args[0].axis)
    except:
        raise ValueError("asegurarse de que se cumplen los requisitos de entrada (al menos dos tensores y 'axe').")
        
    for i in range(2,len(args),1):
        result = Tensor(array=_append(result.array, args[i].array, result.axis.index(axe)), axis=result.axis)
        
    return result
    