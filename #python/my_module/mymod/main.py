###
## Nomenclatura:
##
##  bitstr -> bit-string (i.e., a list of 'u1' represented by integer = 0 or 1)
##  bytestr -> byte-string (i.e., a list of 'u8' represented by integer < 256)
###

OCT0 =	1			# 256^0
OCT1 =	256			# 256^1
OCT2 =	65536		# 256^2
OCT3 =	16777216	# 256^3
	
	
def int_to_bytestr(entrada):
	"""
	Esta funcion convierte un numero 'entrada' en un bytestr
	(i.e. en su representacion en base 256). Como siempre,
	el numero mas a la dcha del bytestr (result[0])
	es el menos significativo
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
	"""
	Convierte una lista de bytes en su
	correspondiente numero decimal (entero)
	"""
	result=0
	for i in range(len(entrada)):
		result = result + entrada[i]*(256**i)
		
	return result
	
	
def int_to_bitstr(entrada):
	"""
	Esta funcion convierte un numero 'entrada' en un bitstr
	(i.e. en su representacion en base 2). Como siempre, el
	numero mas a la dcha del bitstr (result[0]) es el menos
	significativo
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
	"""
	Convierte una lista de bits en su
	correspondiente numero decimal (entero)
	"""
	result=0
	for i in range(len(entrada)):
		result = result + entrada[i]*(2**i)
		
	return result
		

def receive_u8(serialport):
	"""
	Esta funcion implementa un bucle para leer 1 byte del puerto serie
	'serialport', y devuelve dicho byte en formato u8 (un numero entre 0 y 255).
	Si no habia datos en el buffer de entrada a la hora de llamar a la
	funcion devuelve -1
	"""
	read=[0]
	if serialport.inWaiting() > 0:
		
		for i in range(1):
			read[i]=int.from_bytes(serialport.read(1), "big")

		return (read[0])
	
	else:
		return -1
	
	
def receive_u32(serialport):
	"""
	Esta funcion implementa un bucle para leer 4 bytes del puerto serie
	'serialport', y los devuelve en formato u32 (entero). Si no habia
	datos en el buffer de entrada a la hora de llamar a la funcion
	devuelve -1
	"""
	read=[0, 0, 0, 0]
	if serialport.inWaiting() > 0:
		
		for i in range(4):
			read[i]=int.from_bytes(serialport.read(1), "big")

		return (OCT3*read[3]+OCT2*read[2]+OCT1*read[1]+read[0])
	
	else:
		return -1
	
	
def receive_bytestr(serialport, bytestr_size): #aqui hay algun tipo de error
	"""
	Recibe 'bytestr_size' bytes a traves del puerto serie 'serialport'
	"""
	
	result=[]
	for i in range(bytestr_size):
		while 1:
			aux = receive_u8(serialport)
			if aux >= 0: #esto puede dar problemas
				result.append(aux)
				break
			
	return result
	
	
def send_u8(serialport, number):
	"""
	Esta funcion envia un numero en formato u8
	a traves de 'serialport'. Si 'number' es
	mayor que 256, se enviará el resto: number%256
	"""
	serialport.write(bytes([number%256]))

	return 0


def send_bytestr(serialport, bytestr_out):
	"""
	Envía 'bytestr_out' bytes a traves del puerto serie 'serialport'
	"""	
	
	for i in bytestr_out:
		send_u8(serialport, i)
			
	return 0


def resize_array(array_old, array_size):
	"""
	Esta funcion toma un array_old de tamano arbitrario y transfiere
	los caracteres a un array de tamano nbits, empezando por el
	caracter menos significativo (a la dcha.). Si nbits no es
	suficiente para contener todo el array_old, la salida estara
	truncada. Si nbits es excesivo para contener array_old,
	los huecos que sobren se rellenan con ceros.
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
	"""
	Toma una lista de bits (bitstr_in) de tamaño 'bitstr_width'
	y la convierte a una lista de bytes del tamaño mínimo
	necesario para alojar la entrada.
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
	"""
	Toma una lista de bytes y los aloja en una lista de bits
	de tamaño 'bitstr_width'. Si 'bitstr_width' es insuficiente,
	la lista de bits se truncará.
	
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
	"""
	"""
	
	bin_string = ''.join([str(item) for item in reversed(bitstr_in)])
	print(bin_string, end='')
	
	
def print_bytestr(bytestr_in):
	"""
	"""
	aux=[i for i in reversed(bytestr_in)]
	print(aux, end='')
	
	
def pinta_progreso(indice, total, barra=40):
	"""
	Esta funcion pinta una barra de progreso para el valor 'indice'
	de un total de 'total' iteraciones. La variable (opcional) 'barra'
	determina el tamaño total de la barra de progreso (por defecto 40
	items). Ojo! 'barra' es opcional, pero si se utiliza debe respetarse
	el orden de lso argumentos de la función! (i.e., llamarse en último lugar).
	"""
	percent = 100*indice/total
	n_puntos = int(percent/(100/barra))
	
	dibujo_lleno=""
	for i in range(n_puntos):
		dibujo_lleno=dibujo_lleno+"#"
		
	dibujo_vacio=""
	for i in range(barra-n_puntos):
		dibujo_vacio=dibujo_vacio+"."
		
	dibujo = "|"+dibujo_lleno+dibujo_vacio+"|"
	
	print(f" {dibujo} {percent:.2f} %", end='\r')

