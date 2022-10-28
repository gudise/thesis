#!/usr/bin/python3.8

import sys


### FUNCIONES ###
def extract_substring(entrada, offset, inicio, fin):
	"""
	Esta función auxiliar devuelve un string con el contenido
	entre los caracteres 'inicio' y 'fin' (excluidos), comenzando a buscar
	desde la posición 'offset' (incluida). También devuelve la posición
	del último caracter añadido. Si la entrada se termina antes de alcanzar
	el fin, la función devuelve los caracteres que lleve hasta el momento, y
	position = -1.
	"""
	result = ""
	position = -1
	for i in range(offset, len(entrada), 1):
		if entrada[i] == inicio:
			for j in range(i+1, len(entrada), 1):
				if entrada[j] != fin:
					result+=entrada[j]
				else:
					position = j
					break
			break
	return result, position


def procesar_protocolo(protocolo):
	"""
	Esta función toma un protocolo y devuelve una lista
	de dos dimensiones, lista[i][j] donde i es el paso
	temporal del protocolo, j es el nodo, y contiene la sucesión
	de métodos que deben lanzarse para el protocolo. Si un nodo j
	para un paso i no ejecuta ninguna acción, el contenido de la matriz
	será el caracter nulo ''. Además, la función devuelve también
	el numero de pasos y el número de nodos.
	"""
	protocolo_n_steps = 0
	protocolo_n_nodes = 0
	for i,c in enumerate(protocolo):
		if c == 'T':
			if protocolo_n_steps < int(protocolo[i+1]):
				protocolo_n_steps = int(protocolo[i+1])

		if c == 'N':
			if protocolo_n_nodes < int(protocolo[i+1]):
				protocolo_n_nodes = int(protocolo[i+1])

	protocolo_n_steps+=1
	protocolo_n_nodes+=1

	protocolo_list = [['' for j in range(protocolo_n_nodes)] for i in range(protocolo_n_steps)]

	offset = 0
	while offset < len(protocolo) and offset>=0:
		x,offset = extract_substring(protocolo, offset, 'T', 'N')
		y,offset = extract_substring(protocolo, offset, 'N', ':')
		aux,offset = extract_substring(protocolo, offset, ':', 'T')
		protocolo_list[int(x)][int(y)] = aux

	return protocolo_list,protocolo_n_steps,protocolo_n_nodes


def ejecutar_protocolo(protocolo_formateado):
	"""
		Además, la función lee la condición del protocolo y lanza la función
	red.condicion_protocolo()
	"""
	condicion = protocolo_formateado.split('-')[0]
	remitente_legit,destinatarios_legit,mensaje_legit = procesar_condicion_protocolo(condicion, red.n_nodos)

	protocolo = protocolo_formateado.split('-')[1]
	protocolo_list, n_pasos, n_nodos = procesar_protocolo(protocolo)

	for i in range(n_pasos):
		for j in range(n_nodos):
			if protocolo_list[i][j] != '':
				errcode=red.nodo[j].method(protocolo_list[i][j])
				if errcode != '0':
					return errcode
		red.update()


	for i in destinatarios_legit:
		mensaje = red.nodo[i].memory
		if mensaje != mensaje_legit:
			return '-3' # puede ser un error del protocolo o un ataque DOS. En el futuro implementar una manera de distinguirlos.

	for i in range(red.n_nodos):
		if i not in destinatarios_legit:
			mensaje = red.nodo[i].memory
			if mensaje == mensaje_legit:
				return '-4'

	return '0'


def verbalize_errcode(err):
	"""
	Esta función toma un código de rror y devuelve una descripción del error.
	WORK IN PROGRESS: esto hay que ordenarlo, y los 'errcode' deben ser strings, para poder
	transmitir más información (e.g., qué nodo da errores, etc).
	"""
	if err.split()[0] == '0':
		print("OK: Protocolo ejecutado correctamente.")
	elif err.split()[0] == '-1':
		print(f"ERROR: El nodo {err.split()[1]} trata de escribir en el puerto {err.split()[2]} \(puerto max: {int(err.split()[3])-1}\)")
	elif err.split()[0] == '-2':
		print(f"ERROR: Tratando de ejecutar un metodo \({err.split()[1]}\) que no existe")
	elif err.split()[0] == '-3':
		print("ERROR: Protocolo fallido")
	elif err.split()[0] == '-4':
		print("ATTACK: Protocolo vulnerado")
	elif err.split()[0] == '-5':
		print("ERROR: Dos puertos tratan de escribir en el mismo link")


def procesar_condicion_protocolo(condicion, n_nodos):
	"""
	Esta función inicializa el primer mensaje:
	el símbolo '!' se utiliza para separar metadatos.
	El formato es como sigue:
	remitente!destinatarios!mensaje
	"""

	mensaje = condicion.split('!')[2]

	if condicion.split('!')[0] == '':
		remitente = -1 # el emisor del mensaje no esta identificado
	else:
		remitente = int(condicion.split('!')[0])

	if condicion.split('!')[1] == '':
		destinatarios = [i for i in range(n_nodos)] # mensaje esta destinado a cualquier nodo
	else:
		destinatarios = [int(i) for i in condicion.split('!')[1].split()]

	return remitente,destinatarios,mensaje


def filtrar_protocolos_equivalentes(lista_protocolos):
	"""
	Esta función filtra los protocolos equivalentes
	de una lista de entrada, en base a ciertos criterios.
	Por ahora, elimina:
		i)	Protocolos que se diferencien únicamente en
				las variables de métodos que no acepten variables
				(pueden surgir en el contexto de generación
				automática de protocolos).
		ii) Protocolos que se diferencian únicamente en el
				orden de ejecución de dos acciones cuando una de
				esas acciones es nula ('0'). 
	"""
	for i in range(len(lista_protocolos)):
		condicion_i = lista_protocolos[i].split('-')[0]
		aux_i = procesar_protocolo(lista_protocolos[i])[0]
		for j in range(len(aux_i)):
			for k in range(len(aux_i[0])):
				aux_j_k = aux_i[j][k].split(',')
				for l in reversed(range(len(aux_j_k))):
					if aux_j_k[l].split('/')[0] == '0':
						del aux_j_k[l] # Con esto eliminamos las acciones vacías
					elif aux_j_k[l].split('/')[0] == '1' or aux_j_k[l].split('/')[0] == '3' or aux_j_k[l].split('/')[0] == '4':
						aux_j_k[l]=f"{aux_j_k[l].split('/')[0]}/"
				aux_i[j][k] = ""
				for l in range(len(aux_j_k)-1):
					aux_i[j][k] += f"{aux_j_k[l]},"
				aux_i[j][k] += f"{aux_j_k[len(aux_j_k)-1]}"

		lista_protocolos[i] = f"{condicion_i}-"
		for j in range(len(aux_i)):
			for k in range(len(aux_i[0])):
				if aux_i[j][k] != '':
					lista_protocolos[i]+=f"T{j}N{k}:{aux_i[j][k]}"

	aux = lista_protocolos[:]
	protocolos_a_borrar = []
	for i in range(len(aux)):
		for j in range(i+1, len(aux), 1):
			if aux[i] == aux[j] and j not in protocolos_a_borrar:
				protocolos_a_borrar.append(j)

	protocolos_a_borrar.sort(reverse=True)
	for i in protocolos_a_borrar:
		del lista_protocolos[i]


def procesar_topol(topol):
	"""
	Esta función lee una topología de entrada en formato
	de texto, y devuelve una matriz que codifica dicha topología.
	El formato de entrada es: cada nodo conectado mediante
	los símbolos '<', '>' o '<>', y cada pareja separada 
	por comas. Los símbolos '<, >, <>' significan lo que 
	aparentan: el sentido de la conexión (in, out, inout).
	La matriz de salida es una relación de todos los nodos, y vale:
		-1 si NO están conectados.
		0 si la conexión es de entrada, i.e., si i<j, [i][j]=0
													  [j][i]=1
													  
		1 si la conexión es de salida, i.e., si i>j, [i][j]=1
													 [j][i]=0
													 
		2 si la conexión es bidireccional, i.e., si i<>j, [i][j]=2
														  [j][i]=2
	"""
	aux = topol.split(',')
	parejas = []
	for i in aux:
		if len(i) == 4:
			parejas.append((int(i[0]),int(i[3]),'-'))
		else:
			if i[1] == '<':
				parejas.append((int(i[0]),int(i[2]),'<'))
			else:
				parejas.append((int(i[0]),int(i[2]),'>'))
	aux = []
	for i in parejas:
		if i[0] not in aux:
			aux.append(i[0])
		if i[1] not in aux:
			aux.append(i[1])
	n_nodos = max(aux)+1
	topol_m = [[-1 for j in range(n_nodos)] for i in range(n_nodos)]
	for i in parejas:
		if i[2] == '-':
			topol_m[i[0]][i[1]] = 2
			topol_m[i[1]][i[0]] = 2
		elif i[2] == '<':
			topol_m[i[0]][i[1]] = 0
			topol_m[i[1]][i[0]] = 1
		elif i[2] == '>':
			topol_m[i[0]][i[1]] = 1
			topol_m[i[1]][i[0]] = 0
			
	return topol_m



### CLASES ###
class NODO:
	def __init__(self, id, n_puertos):
		self.n_puertos = n_puertos
		self.puerto = ['z' for i in range(n_puertos)]
		self.id = id
		self.buffer = '' # memoria de entrada auxiliar que se interpone entre el puerto y self.memory
		self.memory = '!!'
		self.key = f"k{id}" # esta variable contiene la clave privada del nodo


	def set_mem(self, mensaje):
		"""
		Coloca un mensaje inicial. Los mensajes son strings formales con un significado sintáctico, no semántico.
		Los posibles mensajes son:
			0 --> este valor representa el 'no-mensaje'. Representa una memoria vacía, y en viar/recibir el mensaje '0' significa no enviar nada.
			p --> público. No es un problema que cualquier nodo conozca este mensaje.
			s --> secreto. Solo un subconjunto de nodos puede conocer este mensaje, en otro caso se dispara un error (ejecución / ataque).
		
		Los mensajes pueden tener modificadores, separados por un punto. Por ejemplo:
			k[id] --> el mensaje está encriptado por el nodo cuya id es 'id'.
		Estos modificadores son dinámicos: se pueden concatenar, y se pueden eliminar con el método (y posiblemente la variable, i.e., la clave) adecuada. 
		"""
		self.memory = str(mensaje)
		return '0'


	def set_key(self, key):
		self.key = str(key)
		return '0'


	def encrypt(self):
		self.memory = self.memory+'.'+self.key
		return '0'


	def decrypt(self):
		aux = self.memory.split('.')
		msg = aux[0]
		mod = aux[1:]
		if mod.count(self.key) > 0:
			mod.remove(self.key)
		self.memory = msg
		for i in range(len(mod)):
			self.memory+='.'+mod[i]
		return '0'


	def write_to_port(self, puerto):
		"""
		Vuelca el valor de la memoria al puerto correspondiente
		"""
		if puerto < self.n_puertos:
			self.puerto[puerto] = self.memory # garantizamos que no se sale de rango (condiciones periódicas en 'puerto')
			return '0'
		else:
			return f"-1 {self.id} {puerto} {self.n_puertos}"


	def read_from_buffer(self):
		"""
		Vuelca el valor del buffer de entrada en la memoria
		"""
		self.memory = self.buffer
		return '0'


	def method(self, comando):
		"""
		El comando que introduce el metodo a ejecutar es una string con el formato:
		[indice del metodo].[variables empleadas por el metodo]:[indice del metodo].[variables empleadas por el metodo]:...:[indice del metodo].[variables empleadas por el metodo]
		por ahora los metodos son:
			sm --> set_mem(mensaje)
			sk --> set_key(key) 
			0 --> indice especial que significa 'no hacer nada'. No dispara ningun metodo
			1 --> read_from_buffer
			2 --> write_to_port(numero de puerto)
			3 --> encrypt
			4 --> decrypt

		Varios metodos se pueden concatenar separándolos por dos puntos ':'. Al hacer esto, se ejecutaran los métodos en el orden indicado.
		Esto permite lanzar todos los métodos deseados en una sola función (incluido el método 'vacío' '0.', el cual no tiene función ninguna).
		"""
		methods_to_launch = comando.split(',')
		for i in methods_to_launch:
			aux = i.split('/')
			ind = aux[0]
			if ind == 'sm':
				result=self.set_mem(aux[1])
			elif ind == 'sk':
				result=self.set_key(aux[1])
			elif ind == '0':
				result='0'
			elif ind == '1':
				result=self.read_from_buffer()
			elif ind == '2':
				if aux[1]=='':
					return -1 # chequeamos que efectivamente este comando contenga una variable
				var = int(aux[1])
				result=self.write_to_port(var)
			elif ind == '3':
				result=self.encrypt()
			elif ind == '4':
				result=self.decrypt()
			else:
				result=f"-2 {ind}"

			if result != '0':
				return result

		return result


class RED:
	def __init__(self, topol):
		self.n_nodos = len(topol[0])
		self.n_puertos = [0 for i in range(self.n_nodos)]
		# 'n_puertos' es una lista que indica el numero de puertos de cada nodo en la red
		
		self.mapa_puerto = [] 
		# 'mapa_puerto' relaciona cada puerto con el nodo al que pertenece.
		# Sus componentes son listas de dos elementos: el primer valor es el nodo al que pertenece, 
		# el segundo es el numero que identifica ese puerto dentro del nodo
		
		for i in range(self.n_nodos):
			for j in range(self.n_nodos):
				if topol[i][j] != -1:
					self.mapa_puerto.append((i, self.n_puertos[i]))
					self.n_puertos[i]+=1
		self.n_puertos_total = len(self.mapa_puerto)

		self.link = [[[-1, 0] for j in range(self.n_puertos_total)] for i in range(self.n_puertos_total)] 
		# matriz 2D que indica el estado de un par de puertos.
		# sus elementos link[i][j] (donde i, j son el i-esimo y j-esimo puerto) 
		# son a su vez listas de dos componentes:
		#	la primera [0] indica el tipo de conexion:
		# 		-1 -> no hay conexion
		# 		0 -> in
		# 		1 -> out
		# 		2 -> inout
		#	la segunda [1] indica el mensaje que contiene el link. Sera un string, salvo si esta vacio que valdra 0.
		
		puertos_asignados = []
		for i in range(self.n_puertos_total):
			for j in range(self.n_puertos_total):
				if topol[self.mapa_puerto[i][0]][self.mapa_puerto[j][0]] == 0 and i not in puertos_asignados and j not in puertos_asignados: 
					self.link[i][j][0] = 0
					self.link[j][i][0] = 1
					puertos_asignados.append(i)
					puertos_asignados.append(j)
					
				elif topol[self.mapa_puerto[i][0]][self.mapa_puerto[j][0]] == 1 and i not in puertos_asignados and j not in puertos_asignados: 
					self.link[i][j][0] = 1
					self.link[j][i][0] = 0
					puertos_asignados.append(i)
					puertos_asignados.append(j)
					
				elif topol[self.mapa_puerto[i][0]][self.mapa_puerto[j][0]] == 2 and i not in puertos_asignados and j not in puertos_asignados: 
					self.link[i][j][0] = 2
					self.link[j][i][0] = 2
					puertos_asignados.append(i)
					puertos_asignados.append(j)

		self.nodo=[]
		for i in range(self.n_nodos):
			self.nodo.append(NODO(i, self.n_puertos[i]))


	def print_link_status(self):
		"""
		Esta funcion pinta todos los link
		de la red con sus valores actuales.
		Básicamente para depuración.
		"""
		for i in range(len(self.link)):
			for j in range(i+1, len(self.link[i]), 1):
				if self.link[i][j][0] >=0:
					print(f"El link que conecta el puerto {self.mapa_puerto[i][1]} del nodo {self.mapa_puerto[i][0]} con el puerto {self.mapa_puerto[j][1]} del nodo {self.mapa_puerto[j][0]} contiene: {self.link[i][j][1]}")
		print()


	def update(self):
		for i in range(self.n_puertos_total):
			for j in range(i+1, self.n_puertos_total, 1):
				if self.link[i][j][0] == 2:

					if self.nodo[self.mapa_puerto[i][0]].puerto[self.mapa_puerto[i][1]]!='z' and self.nodo[self.mapa_puerto[j][0]].puerto[self.mapa_puerto[j][1]]!='z':
						#print(f"Error: Los puertos {self.mapa_puerto[i][1]} y {self.mapa_puerto[j][1]} de los nodos {self.mapa_puerto[i][0]} y {self.mapa_puerto[j][0]} estan tratando de colocar un mensaje en el bus compartido a la vez")
						return '-5'

					elif self.nodo[self.mapa_puerto[i][0]].puerto[self.mapa_puerto[i][1]]=='z' and self.nodo[self.mapa_puerto[j][0]].puerto[self.mapa_puerto[j][1]]!='z':
						self.link[i][j][1] = self.nodo[self.mapa_puerto[j][0]].puerto[self.mapa_puerto[j][1]]
						self.nodo[self.mapa_puerto[i][0]].buffer = self.link[i][j][1]

					elif self.nodo[self.mapa_puerto[i][0]].puerto[self.mapa_puerto[i][1]]!='z' and self.nodo[self.mapa_puerto[j][0]].puerto[self.mapa_puerto[j][1]]=='z':
						self.link[i][j][1] = self.nodo[self.mapa_puerto[i][0]].puerto[self.mapa_puerto[i][1]]
						self.nodo[self.mapa_puerto[j][0]].buffer = self.link[i][j][1]	



### SCRIPT ###
a = 1.0
b = 0
c = [1, 2]
for i, opt in enumerate(sys.argv): # "i" es el indice que recorre la lista de opciones, "opt" es la opcion propiamente dicha
	if opt == "-help":
		print("\nEste programa toma una serie de protocolos (separados por saltos de linea) y los resuelve (i.e., realiza la simulacion de cada protocolo y determina si termina con exito, falla o es vulnerado por un adversario. Por defecto, este programa implementa un protocolo (naif)")
		print("----------------------------\n")
		print("-help,\n\tpinta esta ayuda\n")
		print("-topol(t) 0<>1,\n\testa opcion permite introducir la topologia del protocolo (i.e., la relacion de nodos que estan interconectados entre si). La topologia se introduce como un string de parejas separadas por coma (,). Cada elemento de la pareja sera un nodo de la red, y ambos elementos de la pareja estaran conectados por uno de los tres simbolos: <, >, <> que representan el flujo de informacion.\nPor ejemplo:\n\t0>1,1<>2\n\testa topologia representa una red de tres nodos (0, 1 y 2) conectados de forma que '0' puede enviar mensajes a '1', y '1' y '2' pueden intercambiar mensajes entre si.\n")
		print("-protocolos , si esta opcion esta presente b valdra 1\n")
		print("-c [1 2 ...], esta opcion permite cambiar la lista c (por defecto [1, 2]), con tantos numeros como se desee\n")
		
		exit()
		
	if opt=="-topol" or opt=="-t":
		a=float(sys.argv[i+1])
	
	if opt == "-protocolos" or opt=="-p":
		lista_protocolos=[]
		for j in range(i+1, len(sys.argv)):
			if sys.argv[j][0] == "-":
				break
			else:
				lista_protocolos.append(int(sys.argv[j]))
		
	if opt == "-c":
		c=[]
		for j in range(i+1, len(sys.argv)):
			if sys.argv[j][0] == "-":
				break
			else:
				c.append(int(sys.argv[j]))
		


topol = procesar_topol("0<>1,1<>2")
red = RED(topol)

protocolo = f"0!2!s-T0N0:sm/sT0N2:sk/k0T1N0:3/,2/0T2N1:1/,2/1T3N2:1/,4/"

errcode = ejecutar_protocolo(protocolo)

verbalize_errcode(errcode)



lista_protocolos = [f"0!2!s-T0N0:sm/s,sk/kT0N2:sk/kT0N1:sk/kT1N0:3/,2/0T2N1:{i}/{j},{k}/{l},2/1T3N2:1/,4/" for i in range(5) for j in range(5) for k in range(5) for l in range(5)]

filtrar_protocolos_equivalentes(lista_protocolos)

protocolos_fallidos = []
protocolos_vulnerables = []
protocolos_exitosos = []
for i in lista_protocolos:
	red = RED(topol)
	errcode = ejecutar_protocolo(i)
	if errcode.split()[0] == '0':
		protocolos_exitosos.append(i)
	elif errcode.split()[0] == '-1' or errcode.split()[0] == '-2' or errcode.split()[0] == '-3' or errcode.split()[0] == '-5':
		protocolos_fallidos.append(i)
	elif errcode.split()[0] == '-4':
		protocolos_vulnerables.append(i)

print("Protocolos exitosos:\n-----------------")
for i,p in enumerate(protocolos_exitosos):
	print(p)
print("\n")

print("Protocolos vulnerados:\n-----------------")
for i,p in enumerate(protocolos_vulnerables):
	print(p)
print("\n")
