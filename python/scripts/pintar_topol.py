#!/usr/bin/python3.8

import sys
import os
import numpy as np
import matplotlib.pyplot as plt


n_vertices = 3
topol = "0,1;0,2;1,2"
colorv = 'black'
colorl = 'tab:gray'
sizev = 20
sizel = 1.5
pdf = 0
pdf_name = "topol.pdf"
for i, opt in enumerate(sys.argv): # "i" es el indice que recorre la lista de opciones, "opt" es la opcion propiamente dicha
	if opt == "-help":
		with open(f"{os.environ['REPO_puf']}/python/scripts/help/pintar_topol.help", "r") as f:
			print(f.read())
		exit()
		
	if opt == "-nvert":
		n_vertices=int(sys.argv[i+1])
	
	if opt == "-topol":
		topol = sys.argv[i+1]

	if opt == "-colorv":
		colorv = sys.argv[i+1]

	if opt == "-colorl":
		colorl = sys.argv[i+1]

	if opt == "-sizev":
		sizev = int(sys.argv[i+1])

	if opt == "-sizel":
		sizel = int(sys.argv[i+1])
	
	if opt == "-pdf":
		pdf = 1
		if i+1 < len(sys.argv):
			if sys.argv[i+1][0] != '-':
				pdf_name = sys.argv[i+1]
		
		
class vertice:
	def __init__(self, conexiones):
		self.conectividad = conexiones

class arista:
	def __init__(self, a, b):
		if a<b:
			self.link = [a,b]
		else:
			self.link = [b,a]

class grafo:
	def __init__(self, n_vertices, aristas):
		self.n_vertices=n_vertices
		self.aristas=aristas
		self.vertices = []
		for i in range(n_vertices):
			con_aux=[]
			for arista in self.aristas:
				if arista[0] == i:
					con_aux.append(arista[1])
				elif arista[1] == i:
					con_aux.append(arista[0])
			self.vertices.append(vertice(con_aux))
		aux=[]
		for i,vert in enumerate(self.vertices):
			aux.append(len(vert.conectividad))
		aux.sort()
		self.indice=0
		for i in range(len(aux)):
			self.indice+=aux[i]*(self.n_vertices-1)**i

def all_pairs(n_vertices):
	result=[]
	for i in range(n_vertices):
		for j in range(i+1,n_vertices,1):
			result.append([i,j])
	return result

def n_menos_uno(n_vertices):
	result=[]
	for i in range(1,n_vertices,1):
		result.append([i-1,i])
	return result

def n_medios(n_vertices):
	result=[]
	for i in range(0, n_vertices, 2):
		result.append([i,i+1])
	return result


if topol == "all_pairs":
	aristas = all_pairs(n_vertices)
elif topol == "n-1":
	aristas = n_menos_uno(n_vertices)
elif topol == "n_2":
	aristas = n_medios(n_vertices)
else:
	topol_split = topol.split(';')
	aristas=[]
	for i in topol_split:
		aristas.append([int(i.split(',')[0]),int(i.split(',')[1])])

g = grafo(n_vertices, aristas)

angulos=[2*np.pi/g.n_vertices*i for i in range(g.n_vertices)]
x_coords=[np.cos(angulo) for angulo in angulos]
y_coords=[np.sin(angulo) for angulo in angulos]

for arista in g.aristas:
	plt.plot([x_coords[arista[0]], x_coords[arista[1]]],[y_coords[arista[0]], y_coords[arista[1]]], color=colorl, linewidth=sizel)

plt.plot(x_coords,y_coords,'o',color=colorv,markersize=sizev) # el parámetro 'ms' da cuenta del tamaño de los círculos.

# Selecting the axis-X making the bottom and top axes False.
plt.tick_params(axis='x', which='both', bottom=False, top=False, labelbottom=False)
  
# Selecting the axis-Y making the right and left axes False
plt.tick_params(axis='y', which='both', right=False, left=False, labelleft=False)
  
# Iterating over all the axes in the figure
# and make the Spines Visibility as False
for pos in ['right', 'top', 'bottom', 'left']:
	plt.gca().spines[pos].set_visible(False)

if pdf == 0:
	plt.show()
else:
	plt.savefig(pdf_name, transparent=True)
