#!/usr/bin/python3.8

import os
import sys
import serial
import time
import numpy as np
import math
from fpga import pinta_progreso
from fpga.interfazpcps import *
from mytensor import *


out_name            = "rawdata.tsr"
out_fmt             = False
osc_list            = range(1)
pdl_list            = range(1)
resol_list          = range(16,17,1)
autoconfig          = True
sel_ro_width        = 4
sel_pdl_width       = 0
sel_resol_width     = 5
buffer_out_width    = 32
N_rep               = 1
f_ref               = False
log                 = False
puerto              = '/dev/ttyS1'
baudrate            = 9600

for i, opt in enumerate(sys.argv):
    if opt == "-help":
        with open(f"{os.environ['REPO_fpga']}/python/scripts/help/medir_romatrix.help", "r") as f:
            print(f.read())
        exit()
        
    if opt == "-out":
        out_name = sys.argv[i+1]
        
    if opt == "-fmt":
        out_fmt = sys.argv[i+1]
        
    if opt == "-config":
        autoconfig = False
        for j in range(i+1, len(sys.argv), 1):
            if sys.argv[j][0] == '-':
                break
            else:
                if sys.argv[j]=='sel_ro_width' or sys.argv[j]=='sow':
                    sel_ro_width = int(sys.argv[j+1])
                    
                if sys.argv[j]=='sel_pdl_width' or sys.argv[j]=='spw':
                    sel_pdl_width = int(sys.argv[j+1])
                    
                if sys.argv[j]=='sel_resol_width' or sys.argv[j]=='srw':
                    sel_resol_width = int(sys.argv[j+1])
                    
                if sys.argv[j]=='buffer_out_width' or sys.argv[j]=='bow':
                    buffer_out_width = int(sys.argv[j+1])
                    
    if opt == "-osc_range" or opt == "-or":
        osc_list=[]
        for j in range(i+1, len(sys.argv), 1):
            if sys.argv[j][0] == '-':
                break
            else:
                osc_list.append(int(sys.argv[j]))
        if len(osc_list)>0 and len(osc_list)<4:
            if len(osc_list) == 1:
                osc_list = range(osc_list[0])
            elif len(osc_list) == 2:
                osc_list = range(osc_list[0], osc_list[1], 1)
            elif len(osc_list) == 3:
                osc_list = range(osc_list[0], osc_list[1], osc_list[2])
                
    if opt == "-pdl_range" or opt == "-pr":
        pdl_list=[]
        for j in range(i+1, len(sys.argv), 1):
            if sys.argv[j][0] == '-':
                break
            else:
                pdl_list.append(int(sys.argv[j]))
        if len(pdl_list)>0 and len(pdl_list)<4:
            if len(pdl_list) == 1:
                pdl_list = range(pdl_list[0])
            elif len(pdl_list) == 2:
                pdl_list = range(pdl_list[0], pdl_list[1], 1)
            elif len(pdl_list) == 3:
                pdl_list = range(pdl_list[0], pdl_list[1], pdl_list[2])
                
    if opt == "-resol_range" or opt == "-rr":
        resol_list=[]
        for j in range(i+1, len(sys.argv), 1):
            if sys.argv[j][0] == '-':
                break
            else:
                resol_list.append(int(sys.argv[j]))
        if len(resol_list)>0 and len(resol_list)<4:
            if len(resol_list) == 1:
                resol_list = range(resol_list[0])
            elif len(resol_list) == 2:
                resol_list = range(resol_list[0], resol_list[1], 1)
            elif len(resol_list) == 3:
                resol_list = range(resol_list[0], resol_list[1], resol_list[2])
                
    if opt=="-nrep":
        N_rep = int(sys.argv[i+1])
        
    if opt=="-puerto":
        puerto = '/dev/tty'+sys.argv[i+1]
        
    if opt=="-baudrate":
        baudrate = int(sys.argv[i+1])
        
    if opt=="-fref":
        f_ref = float(sys.argv[i+1])
        
    if opt=="-log":
        log = True
        
        
if autoconfig:
    with open("medir_romatrix.config", "r") as f:
        texto = f.read().split('\n')
        for linea in texto:
            if linea.split('=')[0].replace(' ','') == 'sel_ro_width':
                sel_ro_width = int(linea.split('=')[1])
                
            if linea.split('=')[0].replace(' ','') == 'sel_pdl_width':
                sel_pdl_width = int(linea.split('=')[1])
                
            if linea.split('=')[0].replace(' ','') == 'sel_resol_width':
                sel_resol_width = int(linea.split('=')[1])
                
            if linea.split('=')[0].replace(' ','') == 'buffer_out_width':
                buffer_out_width = int(linea.split('=')[1])
        
buffer_in_width = sel_ro_width+sel_pdl_width+sel_resol_width

N_osc = len(osc_list) # Número de osciladores
N_pdl = len(pdl_list) # Número de pdl_list
N_resol = len(resol_list) # Número de medidas a repetir con distinta resolución

## Info log
if log:
    print(f"""
 INFO LOG:
     
 Número de osciladores  = {N_osc}
 Número de repeticiones = {N_rep}
 Número de pdl_list = {N_pdl}
 Número de resol    = {N_resol}
""")
        
    print("{osc:^8} {pdl:^8} {resol:^8} {fdiv:^8} {poly:^8}".format(osc='osc', pdl='pdl', resol='resol', fdiv='fdiv', poly='poly'))
    for i_resol in range(N_resol):
        for i_pdl in range(N_pdl):
            for i_osc in range(N_osc):
                print("{osc:^8} {pdl:^8} {resol:^8}".format(osc=osc_list[i_osc], pdl=pdl_list[i_pdl], resol=resol_list[i_resol]))
    print()

buffer_sel_ro=[]
for i in osc_list:
    buffer_sel_ro.append(resizeArray(intToBitstr(i), sel_ro_width))
    
buffer_sel_pdl=[]
for i in pdl_list:
    buffer_sel_pdl.append(resizeArray(intToBitstr(i), sel_pdl_width))
    
buffer_sel_resol=[]
for i in resol_list:
    buffer_sel_resol.append(resizeArray(intToBitstr(i), sel_resol_width))
    
fpga = serial.Serial(port=puerto, baudrate=baudrate, bytesize=8)
time.sleep(.1)

contador=0
N_total = N_osc*N_rep*N_pdl*N_resol
pinta_progreso(0, N_total, barra=40)

medidas=[]
for rep in range(N_rep):
    for resol in range(N_resol):
        for pdl in range(N_pdl):
            for osc in range(N_osc):
                buffer_in = buffer_sel_ro[osc]+buffer_sel_pdl[pdl]+buffer_sel_resol[resol]
                scan(fpga, buffer_in, buffer_in_width)
                medida = bitstrToInt(calc(fpga, buffer_out_width))
                if f_ref:
                    medida*=f_ref/2**resol_list[resol]
                medidas.append(medida)
                
            contador+=N_osc
            
            pinta_progreso(contador, N_total, barra=40)
            
pinta_progreso(N_total, N_total, barra=40)

fpga.close()

tensor_medidas = TENSOR(np.reshape(medidas, (N_rep,N_resol,N_pdl,N_osc)), metadatos=[['axis'], [['rep','resol','pdl','osc']]])

if not out_fmt:
    out_fmt = '%.18e'
tensor_medidas.write(out_name, fmt=out_fmt)

print("\n")
