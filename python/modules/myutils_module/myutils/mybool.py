"""
Entran cantidades dtype=int, salen cantidades dtype=int, entre medias sólo existen cantidades dtype=bool.
"""

from numpy  import array as np_array,\
                   concatenate as np_concatenate,\
                   identity as np_identity,\
                   zeros as np_zeros,\
                   all as np_all,\
                   sort as np_sort,\
                   argsort as np_argsort,\
                   invert as np_invert,\
                   empty as np_empty
from itertools import product as it_product

def gauss_elimination_gf2(coeff_matrix_in, const_vector_in):
    """
    Esta función resuelve un sistema de ecuaciones booleanas en un cuerpo
    finito con las operaciones XOR ("+") y AND ("*").
    
    Parámetros:
    -----------
    coeff_matrix_in : <lista de lista de bool>
        Matriz que representa los coeficientes del sistema de ecuaciones.
        
    const_vector_in : <lista de bool>
        Vector que representa el término independiente de cada ecuación.
    """
    coeff_matrix = np_array(coeff_matrix_in).astype(bool) #[[bool(j) for j in i] for i in coeff_matrix_in]
    const_vector = np.array(const_vector_in).astype(bool) #[bool(i) for i in const_vector_in]

    n = len(coeff_matrix)

    # Combinar coeficientes y vector constante en una matriz aumentada
    augmented_matrix = np_concatenate((coeff_matrix,const_vector), axis=1)

    # Aplicar eliminación hacia adelante
    for i in range(n):
        if augmented_matrix[i][i] == 0:
            # Intercambiar filas si el pivote es cero
            for j in range(i + 1, n):
                if augmented_matrix[j][i] != 0:
                    augmented_matrix[i], augmented_matrix[j] = augmented_matrix[j], augmented_matrix[i]
                    break
            else:
                raise ValueError("El sistema no tiene solución única.")

        for j in range(i + 1, n):
            factor = augmented_matrix[j][i]
            for k in range(i, n + 1):
                augmented_matrix[j][k]^= factor & augmented_matrix[i][k]

    # Aplicar sustitución hacia atrás
    solution = np_zeros(n,dtype=bool)
    for i in range(n - 1, -1, -1):
        solution[i] = augmented_matrix[i][n]
        for j in range(i):
            augmented_matrix[j][n]^= augmented_matrix[j][i] & solution[i]

    return solution.astype(int)



def invert_matrix_gf2(matrix_in):
    """
    Esta función devuelve la inversa de una matriz booleana en un cuerpo
    finito con las operaciones XOR ("+") y AND ("*").
    
    Parámetros:
    ----------
    matrix_in : <Lista de lista de bool>
        Matriz a invertir.
    """
    matrix = np_array(matrix_in).astype(bool)
    n = len(matrix)

    # Crear una matriz identidad
    identity_matrix = np_identity(n, dtype=bool)

    # Combinar la matriz original y la matriz identidad en una matriz aumentada
    augmented_matrix = np_concatenate((matrix,identity_matrix), axis=1)

    # Aplicar eliminación hacia adelante
    for i in range(n):
        if augmented_matrix[i][i] == 0:
            for j in range(i + 1, n):
                if augmented_matrix[j][i] != 0:
                    augmented_matrix[i], augmented_matrix[j] = augmented_matrix[j], augmented_matrix[i]
                    break
            else:
                raise ValueError("La matriz no es invertible.")

        for j in range(i + 1, n):
            factor = augmented_matrix[j][i]
            for k in range(2 * n):
                augmented_matrix[j][k] ^= factor & augmented_matrix[i][k]

    # Aplicar sustitución hacia atrás
    for i in range(n - 1, -1, -1):
        for j in range(i - 1, -1, -1):
            factor = augmented_matrix[j][i]
            for k in range(2 * n):
                augmented_matrix[j][k] ^= factor & augmented_matrix[i][k]

    # Extraer la matriz inversa de la matriz aumentada
    inverse_matrix = augmented_matrix[:,n:]

    return inverse_matrix.astype(int)


def matrix_multiply_gf2(matrix1_in, matrix2_in):
    """
    Esta función multiplica un par de matrices booleanas en un cuerpo
    finito con las operaciones XOR ("+") y AND ("*").
    Esta función puede utilizarse también para multiplicar vectores
    por matrices, y se encarga de gestionar automáticamente la disposición
    del vector: fila si va delante de la matriz, columna si va detrás. Así,
    si 'matrix1_in' y 'matrix2_in' son dos vectores, el resultado será el
    producto vectorial.
    
    Parámetros:
    -----------
    matrix1_in : <Lista de lista de bool>
        Primer multiplicando.
    
    matrix2_in : <Lista de lista de bool>
        Segundo multiplicando.
    """
    matrix1 = np_array(matrix1_in).astype(bool)
    if matrix1.ndim == 1:
        matrix1 = matrix1.reshape(1,matrix1.size)

    matrix2 = np_array(matrix2_in).astype(bool)
    if matrix2.ndim == 1:
        matrix2 = matrix2.reshape(matrix2.size,1)
    
    rows1,cols1 = matrix1.shape
    rows2,cols2 = matrix2.shape

    if cols1 != rows2:
        raise ValueError("El número de columnas de la primera matriz debe ser igual al número de filas de la segunda matriz.")

    result = np_zeros((rows1,cols2),dtype=bool)

    for i in range(rows1):
        for j in range(cols2):
            for k in range(cols1):
                result[i][j] ^= matrix1[i][k] & matrix2[k][j]

    return result.astype(int).squeeze()


def hamming(in1, in2):
    """
    Esta función calcula la distancia de Hamming (medida en bits) entre dos entradas.
    """
    if len(in1) != len(in2):
        raise ValueError("El tamaño de las entradas debe ser igual.")
    
    n = len(in1)
    result=0
    for i in range(n):
        result+=int(bool(in1[i])^bool(in2[i]))
    return result


class LinearSystECC:
    """
    Código de corrección de errores lineal sistemático
    """
    def __init__(self, dim=4, length=7, G=False, H=False):
        """Constructor"""
        if G!=False and H!=False:
            zero = matrix_multiply_gf2(G,H.T)
            if np_all(zero == 0) == False:
                raise ValueError("La matriz generadora y la matriz de paridad no cumplen GH^=0")
            else:
                self.G = np_array(G,dtype='B')
                self.H = np_array(H,dtype='B')
                self.dim,self.length = self.G.shape
                
        elif G!=False:
            self.G = np_array(G,dtype='B')
            self.dim,self.length = self.G.shape
            self.H = np_concatenate((self.G[:,self.dim:].T,np_identity(self.length-self.dim,dtype='B')),axis=1)
            
        elif H!=False:
            self.H = np_array(H,dtype='B')
            _,self.length = self.H.shape
            self.dim = self.length-_
            self.G = np_concatenate((np_identity(self.dim,dtype='B'),self.H[:,:self.dim].T),axis=1)
            
        else:
            self.dim = dim
            self.length = length
            self.G = np_empty((dim,0),dtype=bool) # Generamos la matriz G iterativamente
            for i in range(length//dim+1):
                if i%2==0:
                    self.G=np_concatenate((self.G,np_identity(dim,dtype=bool)),axis=1)
                else:
                    self.G=np_concatenate((self.G,np_invert(np_identity(dim,dtype=bool))),axis=1)
                self.G=self.G[:,:length].astype('B')
            self.H = np_concatenate((self.G[:,self.dim:].T,np_identity(self.length-self.dim,dtype='B')),axis=1)
            
            
    def encode(self, vector):
        """Codifica un mensaje de entrada de "dim" bits en un código de "length" bits"""
        if len(vector) != self.dim:
            raise ValueError("El vector de entrada debe tener tamaño 'dim'.")

        return matrix_multiply_gf2(vector,self.G)
    
    
    def decode(self, vector):
        """
        Decodifica un código de entrada de 'length' bits en un mensaje de 'dim' bits
        utilizando un método de mínima distancia de Hamming.
        Este método es absolutamente ineficiente, y no debería usarse con dimensiones
        mayores que 9.
        """
        if len(vector)!=self.length:
            raise ValueError("El vector de entrada debe tener tamaño 'length'.")
        
        # Calculamos todos los posibles códigos
        msg_set = list(it_product([0,1], repeat=self.dim))
        code_set = [matrix_multiply_gf2(msg,self.G) for msg in msg_set]
        
        # Calculamos el código con menor distancia al candidato
        dist_set = []
        for code in code_set:
            dist_set.append(hamming(vector,code))
        sort_set = np_sort(dist_set)
        sort_argset = np_argsort(dist_set)

        if sort_set[0]==sort_set[1]:
            raise ValueError("Demasiados errores para decodificar el vector de manera fiable.")
        return msg_set[sort_argset[0]]

