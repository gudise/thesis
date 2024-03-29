###
## Nomenclatura:
##
##  bitstr -> bit-string (i.e., a list of 'u1' represented by integer = 0 or 1)
##  bytestr -> byte-string (i.e., a list of 'u8' represented by integer < 256)
###

OCT0 =  1           # 256^0
OCT1 =  256         # 256^1
OCT2 =  65536       # 256^2
OCT3 =  16777216    # 256^3

cmd_calc =          1
cmd_scan =          2


def int_to_bytestr(entrada):
    """Esta funcion convierte un numero `entrada` en su representacion en base 256 (lista de u8 'bytestr'). El número más a la dcha del bytestr (result[0]) es el menos significativo.
    
    Parameters
    ----------
    entrada : int
        Número a convertir en 'bytestr'.
        
    Returns
    -------
    lista de u8
        Lista que representa `entrada` en base 256.
    """
    result=[]
    if entrada == 0:
        result.append(0)
    else:
        aux = entrada
        while aux > 0:
            result.append(aux%256)
            aux = aux // 256
        
    return result
    
    
def bytestr_to_int(entrada):
    """Convierte una lista de bytes en su correspondiente número decimal (entero).
    
    Parameters
    ----------
    entrada : lista de u8
        Lista de `int` entre 0 y 255 que representa un valor en base 256.
        
    Returns
    -------
    int
        Valor entero (base 10) dado por la lista `entrada`.
    """
    result=0
    for i in range(len(entrada)):
        result = result + entrada[i]*(256**i)
        
    return result
    
    
def int_to_bitstr(entrada):
    """Esta funcion convierte un numero `entrada` en su representacion en base 2 ('bitstr', lista de 0/1). El número más a la dcha del bitstr (result[0]) es el menos significativo.
    
    Parameters
    ----------
    entrada : int
        Valor entero en base 10.
        
    Returns
    -------
    lista de {0,1}
        Representación binaria de `entrada`.
    """
    result=[]
    if entrada == 0:
        result.append(0)
    else:
        aux = entrada
        while aux > 0:
            result.append(aux%2)
            aux = aux // 2
        
    return result
    
    
def bitstr_to_int(entrada):
    """Convierte una lista de bits en su correspondiente número decimal (entero).
    
    Parameters
    ----------
    entrada : lista de {0,1}
        Representación binaria de un valor.
        
    Returns
    -------
    int
        Representación decimal de `entrada`.
    """
    result=0
    for i in range(len(entrada)):
        result = result + entrada[i]*(2**i)
        
    return result
        

def receive_u8(serialport):
    """Esta funcion implementa un bucle para leer 1 byte del puerto serie `serialport`, y devuelve dicho byte en formato u8 (un numero entre 0 y 255). Si no habia datos en el buffer de entrada a la hora de llamar a la funcion, devuelve -1.
    
    Parameters
    ----------
    serialport : pyserial.Serial
        Objeto `Serial` del paquete 'pyserial'.
        
    Returns
    -------
    int
        En caso de éxito, 'byte' leído del puerto serie en formato decimal. En otro caso, -1.
    """
    read=[0]
    if serialport.inWaiting() > 0:
        
        for i in range(1):
            read[i]=int.from_bytes(serialport.read(1), "big")

        return (read[0])
    
    else:
        return -1
    
    
def receive_u32(serialport):
    """Esta funcion implementa un bucle para leer 4 bytes del puerto serie `serialport`, y los devuelve en formato u32 (entero). Si no habia datos en el buffer de entrada a la hora de llamar a la funcion devuelve -1.
    
    Parameters
    ----------
    serialport : pyserial.Serial
        Objeto `Serial` del paquete 'pyserial'.
        
    Returns
    -------
    int
        En caso de éxito, entero leído de `serialport`. En caso contrario, -1.
    """
    read=[0, 0, 0, 0]
    if serialport.inWaiting() > 0:
        
        for i in range(4):
            read[i]=int.from_bytes(serialport.read(1), "big")

        return (OCT3*read[3]+OCT2*read[2]+OCT1*read[1]+read[0])
    
    else:
        return -1
    
    
def receive_bytestr(serialport, bytestr_size): 
    """Recibe `bytestr_size` bytes a traves del puerto serie `serialport`.
    
    Parameters
    ----------
    serialport : pyserial.Serial
        Objeto `Serial` del paquete 'pyserial'.
    bytestr_size : int
        Número de bytes a leer a través del puerto `serialport`.
        
    Returns
    -------
    lista de u8
        Lista de bytes leídos del puerto serie.
    """
    result=[]
    for i in range(bytestr_size):
        while 1:
            aux = receive_u8(serialport)
            if aux >= 0: # Esto puede dar problemas
                result.append(aux)
                break
            
    return result
    
    
def send_u8(serialport, number):
    """Esta funcion envia un numero en formato u8 a traves de `serialport`. Si `number` es mayor que 256, se enviará el resto: number%256.
    
    Parameters
    ----------
    serialport : pyserial.Serial
        Objeto `Serial` del paquete 'pyserial'.
    number : u8
        Byte (entero entre 0 y 255) a enviar a través del puerto serie.
        
    Returns
    -------
    0
    """
    serialport.write(bytes([number%256]))

    return 0


def send_bytestr(serialport, bytestr_out):
    """Envía la lista de bytes `bytestr_out` a traves del puerto serie `serialport`.
    
    Parameters
    ----------
    serialport : pyserial.Serial
        Objeto `Serial` del paquete 'pyserial'.
    bytestr_out : lista de u8
        Lista de bytes (entero entre 0 y 255) a enviar a través del puerto serie.
        
    Returns
    -------
    0
    """ 
    for i in bytestr_out:
        send_u8(serialport, i)
            
    return 0


def resize_array(array_old, array_size):
    """Esta funcion toma un `array_old` de tamaño arbitrario y transfiere los elementos a un array de tamaño `array_size`, empezando por el dígito menos significativo (a la dcha.). Si `array_size` no es suficiente para contener todo el `array_old`, la salida estará truncada. Si `array_size` es excesivo para contener `array_old`, los huecos que sobren se rellenan con 0.
    
    Parameters
    ----------
    array_old : lista
        Array a redimensionar (tipo de datos arbitrario).
    array_size : int
        Tamaño del nuevo array.
        
    Returns
    -------
    lista
        Lista del mismo tipo que `array_old` redimensionada.
    """
    split_string = [c for c in array_old]
    result=[]
    for i in range(array_size):
        if i < len(split_string):
            result.append(split_string[i])
        else:
            result.append(0)
        
    return result


def bitstr_to_bytestr(bitstr_in, bitstr_width):
    """Toma una lista de bits `bitstr_in` y la convierte a una lista de bytes del tamaño mínimo necesario para alojar `bitstr_width` bits.
    
    Parameters
    ----------
    bitstr_in : lista de {0,1}
        Lista de bits.
    bitstr_width : int
        Número de bits mínimos acomodados a la salida.
        
    Returns
    -------
    lista de u8
        Lista de bytes.
    """
    split_string = [c for c in bitstr_in]
    size = bitstr_width//8+(bitstr_width%8>0)
    
    result=[]
    index_split_string=0
    for i in range(size):
        aux=0
        for j in range(8):
            if index_split_string < len(split_string):
                if split_string[index_split_string] == 1:
                    aux = aux + 2**j
                index_split_string = index_split_string+1
            else:
                break
        result.append(aux)

    return result


def bytestr_to_bitstr(bytestr, bitstr_width):
    """Esta función toma una lista de bytes `bytestr` y los aloja en una lista de bits de tamaño `bitstr_width`. Si `bitstr_width` es insuficiente, la lista de bits se truncará.
    
    Parameters
    ----------
    bytestr : lista de u8
        Lista de enteros entre 0 y 255 (bytes).
    bitstr_width : int
        Tamaño de la lista de bits resultante.
        
    Returns
    -------
    lista de {0,1}
        Lista de bits.
    """
    bitstr_aux=[]
    for i in range(8*len(bytestr)):
        bitstr_aux.append(0)
        
    j=0
    for i in bytestr:
        aux = i
        bitstr_aux[j] = aux%2
        
        aux = aux//2
        bitstr_aux[j+1] = aux%2
        
        aux = aux//2
        bitstr_aux[j+2] = aux%2
        
        aux = aux//2
        bitstr_aux[j+3] = aux%2
        
        aux = aux//2
        bitstr_aux[j+4] = aux%2
        
        aux = aux//2
        bitstr_aux[j+5] = aux%2
        
        aux = aux//2
        bitstr_aux[j+6] = aux%2
        
        aux = aux//2
        bitstr_aux[j+7] = aux%2
        
        j = j+8
        
    result=[]
    for i in range(bitstr_width):
        if i < len(bitstr_aux):
            result.append(bitstr_aux[i])
        else:
            result.append(0)
            
    return result


def print_bitstr(bitstr_in):
    """Esta función imprime una lista de bits como una 'string'.
    
    Parameters
    ----------
    bitstr_in : lista de {0,1}
        Lista de bits a imprimir.
    """
    
    bin_string = ''.join([str(item) for item in reversed(bitstr_in)])
    print(bin_string, end='')
    
    
def print_bytestr(bytestr_in):
    """Esta función imprime una lista de bytes como una 'string'.
    
    Parameters
    ----------
    bytestr_in : lista de u8
        Lista de bytes a imprimir.
    """
    aux=[i for i in reversed(bytestr_in)]
    print(aux, end='')


## Aquí empiezan las funciones 'ciclo'
def calc(serialport, buffer_out_width):
    """Esta funcion dispara un ciclo RDY->CALC->PRINT->RDY en una máquina de estados finitos conectada a traves de `serialport` y devuelve un bitstr de tamaño `buffer_out_width`.
    
    Parameters
    ----------
    serialport : pyserial.Serial
        Objeto `Serial` del paquete 'pyserial'.
    buffer_out_width : int
        Tamaño del 'bitstr' devuelto.
        
    Returns
    -------
    lista de {0,1}
        Respuesta binaria devuelta por la máquina de estados finitos después de realizar el procesamiento en FPGA.
    """
    bytestr_size = buffer_out_width//8+(buffer_out_width%8>0)
    send_u8(serialport, cmd_calc)
    bytestr = receive_bytestr(serialport, bytestr_size)
    return bytestr_to_bitstr(bytestr, buffer_out_width)


def scan(serialport, bitstr_in, buffer_in_width):
    """Esta funcion dispara un ciclo RDY->SCAN->RDY en una máquina de estados finitos conectada a traves de `serialport` y carga a la máquina un bitstr de tamaño `buffer_in_width`.
    
    Parameters
    ----------
    serialport : pyserial.Serial
        Objeto `Serial` del paquete 'pyserial'.
    bitstr_in : lista de {0,1}
        Lista de bits a cargar en la máquina de estados finitos, conteniendo datos de entrada necesarios para la operación de la FPGA.
    buffer_in_width : int
        Número de bits escritos en la máquina de estados finitos.    
    """
    send_u8(serialport, cmd_scan)
    send_bytestr(serialport, bitstr_to_bytestr(bitstr_in, buffer_in_width))
