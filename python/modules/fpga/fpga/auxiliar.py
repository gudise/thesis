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