#!/usr/bin/python3.8

## Este script está llamado a hacer la misma función que el programa 'analizar_puf.x', utilizando únicamente la programación en C para los cálculosrealmente intensivos (i.e., los histogramas de números muy grandes y los correspondientes cálculos de entropía). La idea es comunicar este script con esos programas mediante un pipe (función 'run'de la biblioteca 'subprocess').

import os
import sys
from subprocess import run
from numpy import loadtxt, reshape
from mytensor import *


# Variables con valores por defecto
in_name="rawdata.tsr"
# input=0
# out_r_name	=	{"rawdata_out.mtz"},
# out_d_name	=	{"dist_out.mtz"},
# out_x_name	=	{"pufexp_out.pex"},
# out_m_name	=	{"map.txt"},
# out_h_name	=	{"histograma.txt"},			
# output_r		=	0,
# output_d		=	0,
# output_x		=	0,
# output_m		=	0,
# output_h		=	0;
# hist_type		=	0,
# hist_norm		=	0,
# out_x_opt[4]	=	{'0','0','0','0'};
# hist_resolucion,
# trunc			=	-1,
# zeroes_padded	=	0,
# resize_retos	=	1,
# resize_inst 	=	1,
# resize_rep  	=	1,
# resize_y		=	1;
# intrad			=	0,
# intrad_h		=	0,
# intrad_a		=	0,
# intrad_d		=	0;
# intrad_ajuste_name[256] =	{"intrad_ajuste.txt"},
# intrad_dist_name[256]	=	{"intrad_dist.txt"},
# intrad_hist_name[256]	=	{"intrad_hist.txt"};
# caja_intrad		=	1.0,
# xmin_intrad		=	-1,
# xmax_intrad		=	-1,
# media_intrad	=	-1.0,
# desv_intrad, p_intrad;
# interd		=	0,
# interd_h	=	0,
# interd_a	=	0,
# interd_d	=	0;
# interd_ajuste_name[256] =	{"interd_ajuste.txt"},
# interd_dist_name[256]	=	{"interd_dist.txt"},
# interd_hist_name[256]	=	{"interd_hist.txt"};
# caja_interd		=	1.0,
# xmin_interd		=	-1,
# xmax_interd		=	-1,
# media_interd	=	-1.0,
# desv_interd,
# p_interd;
# far_name[256]	=	{"far.txt"},
# frr_name[256]	=	{"frr.txt"},
# roc_name[256]	=	{"roc.txt"};
# far	=	0,
# frr	=	0,
# eer	=	0,
# roc	=	0;
# eer_value;
# eer_index = -1;
# analisisv		=	0,
# analisisv_h		=	0,
# analisisv_a		=	0,
# analisisv_d		=	0;
# analisisv_ajuste_name[256]	=	{"analisisv_ajuste.txt"},
# analisisv_dist_name[256]	=	{"analisisv_dist.txt"},
# analisisv_hist_name[256]	=	{"analisisv_hist.txt"};
# goldenkey_name[256]			=	{"goldenkey.mtz"};
# caja_analisisv		=	1.0,
# xmin_analisisv		=	-1,
# xmax_analisisv		=	-1,
# media_analisisv		=	-1.0,
# desv_analisisv,
# p_analisisv;
# Nx		=	0,
# Ny		=	0;
# fcorr	=	0;
# x_coord,
# y_coord;
# frec_esperada			=	100.0,
# desv_frec_esperada  =	1.0,
# factorx			 	=	1.0,
# factory			 	=	1.0,
# ftemp				=	0;
# N_prec	=	1,
# N_rep	=	1,
# N_inst	=	1,
# N_retos	=	1,
# N_pdl	=	1,
# N_cells	=	0;
# promediar_rep=0;
# puftype	=	0,
# topol	=	one_out_of_2;
# k_mod	=	3,
# N_bits	=	0,
# N_claves,
# **index_array,
# contador,
# contador1;
# ntrials		=	1,
# N_exitos	=	0,
# bias_orden	=	1;
# umbral_id	=	-1.0;
# adversario	=	0,
# verbose		=	1,
# entrop		=	0,
# minentrop	=	0,
# gsort		=	0;
# topolfile[256]	=	{"topol.cmt"},
# helptxt[256];
# aux,
# *aux_array,
# aux_rand0,
# aux_rand1;
# daux,
# *daux_array,
# entropia,
# minentropia;
# MATRIZ	*rawdata,
# *rawdata_temp,
# *set_intrad,
# *set_interd,
# *set_analisisv,
# *frec_nominal,
# *bitwrd_bias,
# *goldenkey;
# MPFR_MATRIZ *data;
# PUFARRAY	*pufarray,
# *pufarray_temp;
# PUFEXP	*pufexp,
# *pufexp_ilegit,
# *pufexp_temp,
# *pufexp_aux0,
# *pufexp_aux1,
# *pufexp_aux2;
# DISTRI	*intrad_hist,
# *intrad_ajuste = NULL,
# *interd_hist,
# *interd_ajuste = NULL,
# *far_curve,
# *frr_curve,
# *roc_curve,
# *analisisv_hist,
# *analisisv_ajuste = NULL;
# MPFR_DISTRI *histograma;
# BINARIO **bitstring,
# *caux;
# CMTOPOL *topologia;
# mpfr_t	caja,
# fcaja,
# xmin,
# xmax;
# FILE *punte;
	
# Lectura de opciones
argv = sys.argv
argc = len(argv)
for i, opt in enumerate(argv): # "i" es el indice que recorre la lista de opciones, "opt" es la opcion propiamente dicha
	if opt == "-help":
		with open(f"{os.environ['REPO_puf']}/python/scripts/help/analizar_puf.help", "r") as f:
			print(f.read())
		exit()			
				
	if opt == "-in":
		for j in range(i+1, argc):
			if argv[j][0] == "-":
				break
			else:
				if argv[j]=="raw" or argv[j]=="r":
					input = 'r'
					in_name = "rawdata.mtz"
					if j+1 < argc:
						if argv[j+1][0]!='-':
							in_name=argv[j+1]
							
				elif argv[j]=="dist" or argv[j]=="d":
					input = 'd'
					in_name = "dist.mtz"
					if j+1 < argc:
						if argv[j+1][0]!='-':
							in_name=argv[j+1]
				
				elif argv[j]=="pufexp" or argv[j]=="x":
					input = 'x'
					in_name = "pufexp.pex"
					if j+1 < argc:
						if argv[j+1][0]!='-':
							in_name=argv[j+1]
	
	if opt == "-out":
		for j in range(i+1, argc):
			if argv[j][0] == "-":
				break
			else:
				if argv[j]=="raw" or argv[j]=="r":
					ouput_r=1
					if j+1<argc:
						if argv[j+1][0]!='-' and argv[j+1] not in ["raw","r","dist","d","pufexp","x","map","m","hist","h"]:
							out_r_name=argv[j+1]
				
				if argv[j]=="dist" or argv[j]=="d":
					ouput_d=1
					if j+1<argc:
						if argv[j+1][0]!='-' and argv[j+1] not in ["raw","r","dist","d","pufexp","x","map","m","hist","h"]:
							out_d_name=argv[j+1]
				
				if argv[j]=="pufexp" or argv[j]=="x":
					ouput_x=1
					if j+1<argc:
						if argv[j+1][0]!='-' and argv[j+1] not in ["raw","r","dist","d","pufexp","x","map","m","hist","h","resize","truncate","promedio","zeropad"]:
							out_x_name=argv[j+1]
					for k in range(j+1,argc):
						if argv[k][0]=='-' or argv[k] in ["raw","r","dist","d","pufexp","x","map","m","hist","h"]:
							break
						else:
							if argv[k]=="resize":
								for l in range(4):
									if out_x_opt[l]=='0':
										out_x_opt[l]='r'
										break
								resize_retos=int(argv[k+1].split(',')[0])
								resize_inst=int(argv[k+1].split(',')[1])
								resize_rep=int(argv[k+1].split(',')[2])
								resize_y=int(argv[k+1].split(',')[3])
							
							if argv[k]=="truncate":
								for l in range(4):
									if out_x_opt[l]=='0':
										out_x_opt[l]='t'
										break
								trunc=int(argv[k+1])
								
							if argv[k]=="promedio":
								for l in range(4):
									if out_x_opt[l]=='0':
										out_x_opt[l]='p'
										break
										
							if argv[k]=="zeropad":
								for l in range(4):
									if out_x_opt[l]=='0':
										out_x_opt[l]='z'
										break
								zeroes_padded=int(argv[k+1])
								
				
				if argv[j]=="map" or argv[j]=="m":
					ouput_m=1
					if j+1<argc:
						if argv[j+1][0]!='-' and argv[j+1] not in ["raw","r","dist","d","pufexp","x","map","m","hist","h"]:
							out_m_name=argv[j+1]
				
				if argv[j]=="hist" or argv[j]=="h":
					ouput_h=1
					if j+1<argc:
						if argv[j+1][0]!='-' and argv[j+1] not in ["raw","r","dist","d","pufexp","x","map","m","hist","h","caja","xmin","xmax","resolucion","sspace","norm"]:
							out_h_name=argv[j+1]
					for k in range(j+1,argc):
						if argv[k][0]=='-' or argv[k] in ["raw","r","dist","d","pufexp","x","map","m","hist","h"]:
							break
						else:
							if argv[k]=="caja":
								if argv[k+1]=="f":
									fcaja=float(argv[k+2])
								else:
									caja=float(argv[k+1])
							
							if argv[k]=="xmin":
								xmin=float(argv[k+1])
							
							if argv[k]=="xmax":
								xmax=float(argv[k+1])
							
							if argv[k]=="resolucion":
								hist_resolucion=int(argv[k+1])
							
							if argv[k]=="sspace":
								hist_type=1
							
							if argv[k]=="norm":
								if k+1<argc:
									if argv[k+1]=="alt":
										hist_norm=1
	
	if opt == "-geom":
		if i+1<argc:
			if argv[i+1][0]!='-':
				for j in range(i+1,argc):
					if argv[j]=="nx":
						if j+1<argc:
							if argv[j+1][0]!='-' and argv[j+1] not in ["nx","ny"]:
								Nx=int(argv[j+1])
								
					if argv[j]=="ny":
						if j+1<argc:
							if argv[j+1][0]!='-' and argv[j+1] not in ["nx","ny"]:
								Ny=int(argv[j+1])
	
	if opt == "-sim":
		for j in range(i,argc):
			if argv[j][0]=='-':
				break
			else:
				if argv[j]=="frec":
					if j+1<argc:
						if argv[j+1][0]!='-' and argv[j+1] not in ["frec","desv","fcorr","ftemp",]:
							frec_esperada=float(argv[j+1])
				
				if argv[j]=="desv":
					if j+1<argc:
						if argv[j+1][0]!='-' and argv[j+1] not in ["frec","desv","fcorr","ftemp",]:
							desv_frec_esperada=float(argv[j+1])
							
				if argv[j]=="fcorr":
					fcorr=1
					for k in range(j+1,argc):
						if argv[k][0]!='-' or argv[k] in ["fre","desv","fcorr","ftemp"]:
							break
						else:
							if argv[k]=="lineal_x":
								fcorr=1
								if k+1<argc:
									if argv[k+1][0]!='-' and argv[k+1] not in ["frec","desv","fcorr","ftemp"]:
										factorx=float(argv[k+1])
								break
							
							elif argv[k]=="lineal_xy":
								fcorr=2
								if k+1<argc:
									if argv[k+1][0]!='-' and argv[k+1] not in ["frec","desv","fcorr","ftemp"]:
										factorx=float(argv[k+1])
								if k+2<argc:
									if argv[k+2][0]!='-' and argv[k+2] not in ["frec","desv","fcorr","ftemp"]:
										factory=float(argv[k+2])
								break
								
							if argv[k]=="producto_xy":
								fcorr=3
								if k+1<argc:
									if argv[k+1][0]!='-' and argv[k+1] not in ["frec","desv","fcorr","ftemp"]:
										factorx=float(argv[k+1])
								break
								
				if argv[j]=="ftemp":
					if j+1<argc:
						if argv[j+1][0]!='-' and argv[j+1] not in ["frec","desv","fcorr","ftemp"]:
							ftemp=float(argv[j+1])
									
	
	if opt == "-nretos":
		N_retos=int(argv[i+1])
	
	if opt == "-ninst":
		N_inst=int(argv[i+1])
	
	if opt == "-nrep":
		N_rep=int(argv[i+1])
		if i+2<argc:
			if argv[i+2]=="promedio":
				promediar_rep=1
	
	if opt == "-npdl":
		N_pdl=int(argv[i+1])
	
	if opt == "-nprec":
		N_prec=int(argv[i+1])
	
	# if opt == "-puf":
	
	# if opt == "-pufanalisis" or opt == "-pa":
	
	# if opt == "-adversario" or opt == "-adv":
	
	# if opt == "-entropia":
	
	# if opt == "-minentropia":
	
	# if opt == "-nv":
	
	# if opt == "-gsort":		


# Script:

## Leemos la matriz de datos de entrada 'rawdata'
rawdata = readTensor(in_name)

# ### Analizamos los metadatos y asignamos ejes (numpyarray)
# for i,nombre_metadato in enumerate(metadatos[0]):
	# if nombre_metadato == 'tipo':
		# tipo = metadatos[1][i]
	# elif nombre_metadato == 'ejes':
		# ejes = [[], []] # contiene: nombre del eje, tamaño del eje
		# for j in metadatos[1][i].split():
			# ejes[0].append(j.split('(')[0])
			# ejes[1].append(int(j.split('(')[1][:-1]))
		# N_ejes = len(ejes[0])
# data_in = reshape(rawdata, ejes[1])

# #### Chequear consistencia de los ejes (si 'tipo' existe y es conocido)
# if tipo == 'ro':
	# if not N_ejes == 4:
		# print(f"Error: ")


# elif tipo == 'garo':
	# vfd

