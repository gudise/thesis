"""
Este módulo permite definir nombres para los ejes
de un numpyarray.

Clases:
-------
Tensor

Funciones:
----------
load_tensor(file_name) : <devuelve Tensor>
    Esta función devuelve un objeto Tensor guardado en un fichero de texto.
    
    file_name : <string>
        Nombre del fichero de texto donde se ha
        guardado un objeto Tensor utilizando la
        función Tensor.save()
    
    
"""

from numpy import array as np_array,\
                  compress as np_compress,\
                  reshape as np_reshape,\
                  ravel as np_ravel,\
                  savetxt as np_savetxt,\
                  loadtxt as np_loadtxt,\
                  mean as np_mean,\
                  std as np_std,\
                  append as _append


class Tensor:
    """
    Esta clase es un 'wrapper' de 'ndarray' con una lista 'axis' que mapea 
    cada eje con un nombre cualitativo. El i-ésimo elemento de 'axis' 
    contiene el nombre del i-ésimo eje.
    
    Parámetros:
    -----------
    array : <numpy array>
        Arreglo que contiene los datos y forma del tensor.
        
    axis : <lista de strings>
        Nombres de los ejes. El orden en el que se da es correlativo
        con el correspondiente eje del parámetro 'array'.
        
    Funciones:
    ----------
    size()
    
    item()
    
    take()
    
    slice()
    
    swap()
    
    mean()
    
    std()
    
    save(file_name, fmt)
        Esta función guarda el Tensor en un fichero de texto.
        
        Parámetros:
        -----------
        file_name : <string>
            Nombre del fichero
            
        fmt : <string, opcional, por defecto '%.18e'>
            Formato numérico de los datos. Sigue el mismo formato que el parámetro 'fmt'
            de numpy.savetxt
            
    squeeze()
            
    """
    def __init__(self, array, axis=False):
        """
        Esta función inicializa un objeto Tensor, que es esencialmente un 
        nparray junto con una variable 'self.axis' que permite indexar los ejes
        del array mediante un nombre natural, y no un índice entero. Si no se
        introduce una lista de nombres en esta variable, se asiganrán por 
        defecto como nombres el índice de cada eje (empezando en '0').
        """
        if type(array) == type([]):
            self.array = np_array(array)
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
        """
        return self.shape[self.axis.index(axis)]
            
    def item(self, **axes):
        """
        Esta función toma por argumento el nombre de cada eje igualado a un 
        índice, y devuelve el valor guardado en esa coordenada.
        """
        return self.array.item(*[int(axes[ax_name]) for ax_name in self.axis])
        
    def take(self, **kwargs):
        """
        Esta función toma una serie de ejes (por nombre), igualados a una lista de índices, y devuelve un Tensor
        reducido a los índices seleccionado.
        """
        axe=[]
        slicing_position=[]
        for key, value in kwargs.items():
            axe.append(key)
            slicing_position.append(value)
        aux = self.array[:]
        for i,a in enumerate(axe):
            aux = aux.take(slicing_position[i], axis=self.axis.index(a))
        return Tensor(aux, self.axis)
    
    def slice(self, **kwargs):
        """
        Esta función toma una serie de ejes (por nombre), igualados a una porción 'slice'. Esta porción puede ser
        un entero, una lista de dos enteros, o una lista de tres enteros, y devuelve un Tensor reducido al 'slice'
        indicado (usando la misma notación que un slice estándar en Python: [x] es la x-ésima entrada, [x,y] son
        todas entradas en [x,y), y [x,y,z] son todas las entradas en [x,y) tomadas en saltos de z). Esta función
        es en realidad un wrapper a 'take', usando la nomenclatura de Python para hacer slicing.
        """
        axe=[]
        slicing_position=[]
        for key, value in kwargs.items():
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
        Esta función devuelve un Tensor con el mismo "array" pero los ejes 
        "axe1" y "axe2" intercambiados.
        """
        result_array = self.array.swapaxes(self.axis.index(axe1), self.axis.index(axe2)).copy()
        result_axis = self.axis[:]
        result_axis[self.axis.index(axe1)],result_axis[self.axis.index(axe2)] = result_axis[self.axis.index(axe2)],result_axis[self.axis.index(axe1)]
        return Tensor(result_array, result_axis)
        
    def mean(self, axe):
        """
        Esta función devuelve un Tensor comprimido en el eje 'axis', el cual
        se suprime, de modo que cada punto del nuevo Tensor contiene el 
        promedio a lo largo del eje suprimido.
        """
        result_array = np_mean(self.array, axis=self.axis.index(axe))
        result_axis = self.axis[:]
        result_axis.remove(axe)
        return Tensor(result_array, result_axis)
        
    def std(self, axe):
        """
        Esta función devuelve un Tensor comprimido en el eje 'axis', el cual
        se suprime, de modo que cada punto del nuevo Tensor contiene la
        desviación típica a lo largo del eje suprimido.
        """
        result_array = np_std(self.array, axis=self.axis.index(axe))
        result_axis = self.axis[:]
        result_axis.remove(axe)
        return Tensor(result_array, result_axis)
        
    def save(self, file_name, fmt='%.18e'):
        """
        Esta función guarda el Tensor en un fichero de texto.
        """
        nfilas = self.shape[0]
        ncolumnas = 1
        for i in self.shape[1:]:
            ncolumnas*=i
        encabezado="#[axis] "+" ".join(self.axis)
        encabezado+="\n#[shape] "+" ".join([f"{i}" for i in self.shape])+"\n"

        np_savetxt(file_name, np_reshape(np_ravel(self.array), (nfilas, ncolumnas)), fmt=fmt, header=encabezado, comments="")
        
    def squeeze(self):
        """
        Esta función elimina todos los ejes del tensor cuya dimensión sea '1'.
        """
        aux = self.axis[:]
        for i in self.axis:
            if self.size(i)==1:
                aux.remove(i)
        return Tensor(self.array.squeeze(),aux)


def load_tensor(file_name):
    """
    Esta función devuelve un Tensor guardado en un fichero de texto.
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
    array = np_loadtxt(file_name)
    for i in range(len(shape)):
        shape[i]=int(shape[i])
    return Tensor(np_reshape(np_ravel(array), shape), axis)
    
    
def append(axe, *args):
    """Esta función es un 'wrapper' a numpy.append', pero permite especificar el eje por el que se concatenan dos objetos `Tensor` por el nombre. Todos los argumentos de entrada deben tener un parámetro `Tensor.axis` idéntico, así como las mismas dimensiones en todos los ejes excepto quizá en `axe`. El resultado es un nuevo `Tensor` que consiste en la concatenación de los argumentos en orden según se proporcionaron. Es obligatorio proporcionar al menos dos tensores de entrada.
    """
    try:
        result =  Tensor(array=_append(args[0].array,args[1].array,args[0].axis.index(axe)), axis=args[0].axis)
    except:
        print("ERROR: asegurarse de que se cumplen los requisitos de entrada (al menos dos tensores y 'axe' ")
        return -1
        
    for i in range(2,len(args),1):
        result = Tensor(array=_append(result.array, args[i].array, result.axis.index(axe)), axis=result.axis)
        
    return result
    
