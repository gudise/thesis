#!/usr/bin/python3.8

import os
import sys

class dominio:
	def __init__(self, x0, y0, x1, y1, dx, dy, nosc, directriz):
		self.x0 = x0
		self.y0 = y0
		self.x1 = x1
		self.y1 = y1
		self.dx = dx
		self.dy = dy
		self.nosc = nosc
		self.directriz = directriz

debug = 0
dominios = []
for i, opt in enumerate(sys.argv): # "i" es el indice que recorre la lista de opciones, "opt" es la opcion propiamente dicha
	if opt == "-help":
		with open(f"{os.environ['REPO_fpga']}/python/scripts/help/gen_romatrix_osc_locations.help", "r") as f:
			print(f.read())
		exit()
		
	if opt == "-posmap":
		for j in sys.argv[i+1].split():
			a = int(j.split(';')[0].split(',')[0]) # x0
			b = int(j.split(';')[0].split(',')[1]) # y0
			
			c = int(j.split(';')[1].split(',')[0]) # x1
			d = int(j.split(';')[1].split(',')[1]) # y1
			
			e = int(j.split(';')[2].split(',')[0]) # dx
			f = int(j.split(';')[2].split(',')[1]) # dy
			
			g = int(j.split(';')[3]) # nosc
			
			h = j.split(';')[4] # directriz
			
			dominios.append(dominio(a,b,c,d,e,f,g,h))
	
	if opt == "-debug":
		debug=1
	
	
lista_osciladores = ""
for dominio in dominios:
	x = dominio.x0
	y = dominio.y0
	for i in range(dominio.nosc):
		if x > dominio.x1:
			if y==dominio.y1 or y+dominio.dy > dominio.y1:
				break
			else:
				x=dominio.x0
				y+=dominio.dy
				
		if y > dominio.y1:
			if x==dominio.x1 or x+dominio.dx > dominio.x1:
				break
			else:
				x+=dominio.dx
				y=dominio.y0
		lista_osciladores += f"{x},{y} "
		if dominio.directriz == 'y':
			y+=dominio.dy
		else:
			x+=dominio.dx
		
print(lista_osciladores[:-1])


## DEBUG:
if debug == 1:
	import matplotlib.pyplot as plt

	todos_x = [int(i.split(',')[0]) for i in lista_osciladores.split()]
	todos_y = [int(i.split(',')[1]) for i in lista_osciladores.split()]

	max_x = max(todos_x)
	min_x = min(todos_x)
	max_y = max(todos_y)
	min_y = min(todos_y)

	mapa = [[0 for j in range(min_y,max_y,1)] for i in range(min_x,max_x,1)]

	for i,x in enumerate(range(min_x,max_x,1)):
		for j,y in enumerate(range(min_y,max_y,1)):
			if f"{x},{y}" in lista_osciladores.split():
				mapa[i][j]=1

	plt.imshow(mapa, interpolation='none', origin='lower')
	plt.show()
