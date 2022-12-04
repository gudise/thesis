#!/usr/bin/python3.8

import os
import sys
import serial
import time
import numpy as np
import math
from fpga import pinta_progreso
from fpga.interfazpcbackend import *


out_name = "rawdata.mtz"
osc = range(1)
pdl = range(1)
sel_ro_width = 8
sel_pdl_width = 5
buffer_out_width = 32
N_osciladores_por_instancia = 0
N_repeticiones = 1
puerto = '/dev/ttyS1'
baudrate = 9600
for i, opt in enumerate(sys.argv):
	if opt == "-help":
		with open(f"{os.environ['REPO_fpga']}/python/scripts/help/medir_romatrix.help", "r") as f:
			print(f.read())
		exit()
		
	if opt == "-out":
		out_name = sys.argv[i+1]
		
	if opt == "-trama_selector" or opt == "-ts":
		sel_ro_width = int(sys.argv[i+1])
		sel_pdl_width = int(sys.argv[i+2])
		
	if opt == "-buffer_out_width" or opt=="-bow":
		buffer_out_width = int(sys.argv[i+1])

	if opt == "-osc_range" or opt == "-or":
		osc=[]
		for j in range(i+1, len(sys.argv), 1):
			if sys.argv[j][0] == '-':
				break
			else:
				osc.append(int(sys.argv[j]))
		if len(osc)>0 and len(osc)<4:
			if len(osc) == 1:
				osc = range(osc[0])
			elif len(osc) == 2:
				osc = range(osc[0], osc[1], 1)
			elif len(osc) == 3:
				osc = range(osc[0], osc[1], osc[2])

	if opt == "-pdl_range" or opt == "-pr":
		pdl=[]
		for j in range(i+1, len(sys.argv), 1):
			if sys.argv[j][0] == '-':
				break
			else:
				pdl.append(int(sys.argv[j]))
		if len(pdl)>0 and len(pdl)<4:
			if len(pdl) == 1:
				pdl = range(pdl[0])
			elif len(pdl) == 2:
				pdl = range(pdl[0], pdl[1], 1)
			elif len(pdl) == 3:
				pdl = range(pdl[0], pdl[1], pdl[2])
				
	if opt == "-osc_por_instancia" or opt=="-opi":
		N_osciladores_por_instancia = int(sys.argv[i+1])
		
	if opt=="-nrep":
		N_repeticiones = int(sys.argv[i+1])
		
	if opt=="-puerto":
		puerto = '/dev/ttyS'+sys.argv[i+1]
		
	if opt=="-baudrate":
		baudrate = int(sys.argv[i+1])


if N_osciladores_por_instancia==0:
	N_osciladores_por_instancia = len(osc)

buffer_in_width = sel_ro_width+sel_pdl_width
N_osciladores = len(osc)
N_pdl = len(pdl)
N_instancias = N_osciladores//N_osciladores_por_instancia

print(f"\n N_instancias = {N_instancias}\n N_repeticiones = {N_repeticiones}\n N_pdl = {N_pdl}\n N_osciladores = {N_osciladores}\n")

fpga = serial.Serial(port=puerto, baudrate=baudrate, bytesize=8)
time.sleep(.1)

buffer_sel_pdl=[]
for i in pdl:
	buffer_sel_pdl.append(resizeArray(intToBitstr(i), sel_pdl_width))

buffer_sel_ro=[]
for i in osc:
	buffer_sel_ro.append(resizeArray(intToBitstr(i), sel_ro_width))

contador=0
N_total = N_osciladores_por_instancia*N_instancias*N_repeticiones*N_pdl
pinta_progreso(0, N_total, barra=40)
medidas=[]
for i in range(N_instancias):
	for j in range(N_repeticiones):
		for k in range(N_pdl):
			for l in range(N_osciladores_por_instancia):
				buffer_in = buffer_sel_ro[i*N_osciladores_por_instancia+l] + buffer_sel_pdl[k]

				scan(fpga, buffer_in, buffer_in_width)
		
				medidas.append(bitstrToInt(calc(fpga, buffer_out_width)))
			
			contador+=N_osciladores_por_instancia
			pinta_progreso(contador, N_total, barra=40)

pinta_progreso(N_total, N_total, barra=40)
	
fpga.close()

medidas = np.reshape(medidas, (N_instancias*N_repeticiones*N_pdl,N_osciladores_por_instancia))

np.savetxt(out_name, medidas, header=f"#[N_filas] {N_instancias*N_repeticiones*N_pdl}\n#[N_columnas] {N_osciladores_por_instancia}\n#[matriz]\n", footer="#[fin]", comments="")

print("\n")
