#!/usr/bin/python3.8

import os
import sys
import math
from subprocess import run # esta función permite llamar a sub-rutinas de shell (e.g. programas en C), y pasar/recibir datos.

def N_bits_MUX(N):
	"""
	Función para calcular el numero de bits
	necesarios para especificar 'N' estados.
	"""
	if N<=0:
		return 1
	else:
		if math.log2(N)>int(math.log2(N)):
			return int(math.log2(N))+1
		else:
			return int(math.log2(N))
	
argv = sys.argv
argc = len(argv)

out_name	=	"garomatrix"
directriz	=	'y'
tipo		=	"lut3"
pinmap_opt	=	"no;"
minsel		=	0
N_inv		=	3
XOS_i		=	0
YOS_i		=	0
XOS_incr	=	1
YOS_incr	=	1
XOS_max		=	20
YOS_max		=	20
N_osciladores	=	10
resolucion		=	1000000
pos_oscilador_fijado	=	[[],[]]
for i, opt in enumerate(argv): # "i" es el indice que recorre la lista de opciones, "opt" es la opcion propiamente dicha
	if opt == "-help":
		with open(f"{os.environ['REPO_fpga']}/python/scripts/help/gen_garomatrix.help", "r") as f:
			print(f.read())
		exit()
		
	if opt == "-out":
		out_name = argv[i+1]
		
	if opt == "-tipo":
		tipo = argv[i+1]
	
	if opt == "-pinmap":
		pinmap_opt = argv[i+1]+";"
	
	if opt == "-minsel":
		minsel = 1
	
	if opt == "-directriz":
		if argv[i+1]=="x" or argv[i+1]=="X":
			directriz = 'x'
	
	if opt == "-Ninv":
		N_inv = int(argv[i+1])
	
	if opt == "-XOSincr":
		XOS_incr = int(argv[i+1])
	
	if opt == "-YOSincr":
		YOS_incr = int(argv[i+1])
	
	if opt == "-XOSmax":
		XOS_max = int(argv[i+1])
	
	if opt == "-YOSmax":
		YOS_max = int(argv[i+1])
	
	if opt == "-Nosc":
		N_osciladores = int(argv[i+1])
	
	if opt == "-posmap":
		j=i+1
		while True:
			if j < argc:
				if argv[j][0]=='-':
					break
				else:
					pos_oscilador_fijado[0].append(argv[j].split(',')[0])
					pos_oscilador_fijado[1].append(argv[j].split(',')[1])
			else:
				break
			j+=1
			
	if opt == "-resolucion":
		resolucion = int(argv[i+1])
		
N_osciladores_fijados = len(pos_oscilador_fijado[0])
if N_osciladores_fijados==0:
	pos_oscilador_fijado[0].append(XOS_i)
	pos_oscilador_fijado[1].append(YOS_i)

pos_oscilador = [[0 for i in range(N_osciladores)],[0 for i in range(N_osciladores)]]

for i in range(N_osciladores_fijados):
	pos_oscilador[0][i] = pos_oscilador_fijado[0][i]
	pos_oscilador[1][i] = pos_oscilador_fijado[1][i]

XOS = pos_oscilador[0][N_osciladores_fijados-1]
YOS = pos_oscilador[1][N_osciladores_fijados-1]
for i in range(N_osciladores_fijados, N_osciladores, 1):
	if directriz == 'x':
		XOS+=XOS_incr
	else:
		YOS+=YOS_incr
		
	if XOS >= XOS_max+1:
		XOS = pos_oscilador_fijado[0][N_osciladores_fijados-1]
		YOS+=N_inv//4+YOS_incr
		if YOS >= YOS_max+1:
			print("\nERROR: en funcion 'hwd_genROmatriz'. YOSmax excedida.\n")
			exit()
	if YOS >= YOS_max+1:
		YOS = pos_oscilador_fijado[1][N_osciladores_fijados-1]
		XOS+=XOS_incr
		if XOS >= XOS_max+1:
			print("\nERROR: en funcion 'hwd_genROmatriz'. XOSmax excedida.\n")
			exit()
			
	pos_oscilador[0][i]=XOS
	pos_oscilador[1][i]=YOS
	
out_name+=".v"
with open(out_name, "w") as f:
	bel_ocupacion = ["BEL=\"A6LUT\"","BEL=\"B6LUT\"","BEL=\"C6LUT\"","BEL=\"D6LUT\""]
	
	pinmap = []
	intaux = 0
	for i in range(len(pinmap_opt)):
		if pinmap_opt[i]==';':
			saux = pinmap_opt[intaux:i]
			intaux+=i+1
			if saux == "no":
				pinmap.append(" ")
			else:
				pinmap.append(f", LOCK_PINS=\"{saux}\" ")
	for i in range(len(pinmap),N_inv,1):
		pinmap.append(pinmap[-1])
		
	f.write(f"//N_osciladores de Galois: {N_osciladores}\n\n")
		
	if tipo == "lut3":
		f.write("module GAROMATRIX (\n")
		f.write("	input clock,\n")
		if N_osciladores > 1:
			f.write(f"	input[{N_bits_MUX(N_osciladores)-1}:0] sel_ro,\n")

		f.write(f"	input[{N_inv-1}:0] polinomio,\n")
		f.write("	input enable,\n")
		f.write("	output out\n")
		f.write("	);\n\n")
		f.write(f"	(* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[{N_osciladores-1}:0] out_ro;\n")
		f.write(f"	wire[{N_osciladores-1}:0] out_ro_sampled;\n")
		f.write(f"	reg[{N_osciladores-1}:0] enable_ro;\n")
		for i in range(N_osciladores):
			f.write(f"\n	wire[{N_inv-1}:0] w_{i};")
		if N_osciladores>1:
			f.write("\n\n	assign out = enable? out_ro_sampled[sel_ro] : clock;\n")
			f.write("\n	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[sel_ro]=1;\n")
			f.write("	end\n")
		else:
			f.write("\n\n	assign out = enable? out_ro_sampled[0] : clock;\n")
			f.write("\n	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[0]=1;\n")
			f.write("	end\n")
		f.write("\n\n")
		
		for i in range(N_osciladores):
			celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
			f.write(f"    (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
			
			contador=0
			aux=0
			ocupacion_celda=1
			while True:
				if ocupacion_celda==4:
					celda[1]+=1
					ocupacion_celda=0
				if aux == N_inv-1:
					f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT3 #(8'h95) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(out_ro[{i}]), .I2(polinomio[{aux}]));\n") # ojo aquí.
					#f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT1 #(2'b01) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]));\n") # ojo aquí.
					f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
					
					ocupacion_celda+=1
					
					break
				
				f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT3 #(8'h95) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(out_ro[{i}]), .I2(polinomio[{aux}]));\n")
				
				contador+=2
				ocupacion_celda+=1
				aux+=1
				
			f.write("\n")
		f.write("endmodule\n")

	elif tipo == "lut4":
		f.write("module GAROMATRIX (\n")
		f.write("	input clock,\n")
		if N_osciladores > 1:
			f.write(f"	input[{N_bits_MUX(N_osciladores)-1}:0] sel_ro,\n")
		if not minsel:
			f.write(f"	input[{N_inv-1}:0] sel,\n")
		else:
			f.write(f"	input[0:0] sel,\n")
		f.write(f"	input[{N_inv-1}:0] polinomio,\n")
		f.write("	input enable,\n")
		f.write("	output out\n")
		f.write("	);\n\n")
		f.write(f"	(* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[{N_osciladores-1}:0] out_ro;\n")
		f.write(f"	wire[{N_osciladores-1}:0] out_ro_sampled;\n")
		f.write(f"	reg[{N_osciladores-1}:0] enable_ro;\n")
		for i in range(N_osciladores):
			f.write(f"\n	wire[{N_inv-1}:0] w_{i};")
		
		if N_osciladores>1:
			f.write("\n\n	assign out = enable? out_ro_sampled[sel_ro] : clock;\n")
			f.write("\n	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[sel_ro]=1;\n")
			f.write("	end\n")
		else:
			f.write("\n\n	assign out = enable? out_ro_sampled[0] : clock;\n")
			f.write("\n	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[0]=1;\n")
			f.write("	end\n")
		f.write("\n\n")
		
		for i in range(N_osciladores):
			celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
			f.write(f"    (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
			
			contador=0
			aux=0
			ocupacion_celda=1
			while True:
				if ocupacion_celda==4:
					celda[1]+=1
					ocupacion_celda=0
				if aux == N_inv-1:
					if not minsel:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel[{contador}]), .I2(polinomio[{aux}]), .I3(sel[{contador}]));\n\n")
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
					else:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel[0]), .I2(polinomio[{aux}]), .I3(sel[0]));\n\n")
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
					
					ocupacion_celda+=1
					
					break
						
				if not minsel:
					f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel[{contador}]), .I2(polinomio[{aux}]), .I3(sel[{contador}]));\n")
				else:
					f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel[0]), .I2(polinomio[{aux}]), .I3(sel[0]));\n")
						
				contador+=1
				ocupacion_celda+=1
				aux+=1
						
			f.write("\n")

		f.write("endmodule\n")
	
	elif tipo == "lut5":
		f.write("module GAROMATRIX (\n")
		f.write("	input clock,\n")
		if N_osciladores > 1:
			f.write(f"	input[{N_bits_MUX(N_osciladores)-1}:0] sel_ro,\n")
		if not minsel:
			f.write(f"	input[{2*N_inv-1}:0] sel,\n")
		else:
			f.write(f"	input[1:0] sel,\n")
		f.write(f"	input[{N_inv-1}:0] polinomio,\n")
		f.write("	input enable,\n")
		f.write("	output out\n")
		f.write("	);\n\n")
		f.write(f"	(* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[{N_osciladores-1}:0] out_ro;\n")
		f.write(f"	wire[{N_osciladores-1}:0] out_ro_sampled;\n")
		f.write(f"	reg[{N_osciladores-1}:0] enable_ro;\n")
		for i in range(N_osciladores):
			f.write(f"\n	wire[{N_inv-1}:0] w_{i};")
		
		if N_osciladores>1:
			f.write("\n\n	assign out = enable? out_ro_sampled[sel_ro] : clock;\n")
			f.write("\n	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[sel_ro]=1;\n")
			f.write("	end\n")
		else:
			f.write("\n\n	assign out = enable? out_ro_sampled[0] : clock;\n")
			f.write("\n	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[0]=1;\n")
			f.write("	end\n")
		f.write("\n\n")
		
		for i in range(N_osciladores):
			celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
			f.write(f"    (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
			
			contador=0
			aux=0
			ocupacion_celda=1
			while True:
				if ocupacion_celda==4:
					celda[1]+=1
					ocupacion_celda=0
				if aux == N_inv-1:
					if not minsel:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel[{contador}]), .I2(polinomio[{aux}]), .I3(sel[{contador}]), .I4(sel[{contador+1}]));\n\n")
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
					else:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel[0]), .I2(polinomio[{aux}]), .I3(sel[0]), .I4(sel[1]));\n\n")
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
					
					ocupacion_celda+=1
					
					break
						
				if not minsel:
					f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel[{contador}]), .I2(polinomio[{aux}]), .I3(sel[{contador}, .I4(sel[{contador+1}])]));\n")
				else:
					f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel[0]), .I2(polinomio[{aux}]), .I3(sel[0]), .I4(sel[1]));\n")
						
				contador+=2
				ocupacion_celda+=1
				aux+=1
						
			f.write("\n")

		f.write("endmodule\n")
	
	elif tipo == "lut6":
		f.write("module GAROMATRIX (\n")
		f.write("	input clock,\n")
		if N_osciladores > 1:
			f.write(f"	input[{N_bits_MUX(N_osciladores)-1}:0] sel_ro,\n")
		if not minsel:
			f.write(f"	input[{3*N_inv-1}:0] sel,\n")
		else:
			f.write(f"	input[2:0] sel,\n")
		f.write(f"	input[{N_inv-1}:0] polinomio,\n")
		f.write("	input enable,\n")
		f.write("	output out\n")
		f.write("	);\n\n")
		f.write(f"	(* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[{N_osciladores-1}:0] out_ro;\n")
		f.write(f"	wire[{N_osciladores-1}:0] out_ro_sampled;\n")
		f.write(f"	reg[{N_osciladores-1}:0] enable_ro;\n")
		for i in range(N_osciladores):
			f.write(f"\n	wire[{N_inv-1}:0] w_{i};")
		
		if N_osciladores>1:
			f.write("\n\n	assign out = enable? out_ro_sampled[sel_ro] : clock;\n")
			f.write("\n	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[sel_ro]=1;\n")
			f.write("	end\n")
		else:
			f.write("\n\n	assign out = enable? out_ro_sampled[0] : clock;\n")
			f.write("\n	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[0]=1;\n")
			f.write("	end\n")
		f.write("\n\n")
		
		for i in range(N_osciladores):
			celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
			f.write(f"    (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
			
			contador=0
			aux=0
			ocupacion_celda=1
			while True:
				if ocupacion_celda==4:
					celda[1]+=1
					ocupacion_celda=0
				if aux == N_inv-1:
					if not minsel:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel[{contador}]), .I2(polinomio[{aux}]), .I3(sel[{contador}]), .I4(sel[{contador+1}]), .I5(sel[{contador+2}]));\n\n")
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
					else:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel[0]), .I2(polinomio[{aux}]), .I3(sel[0]), .I4(sel[1]), .I5(sel[2]));\n\n")
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
						
					ocupacion_celda+=1
					
					break
						
				if not minsel:
					f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel[{contador}]), .I2(polinomio[{aux}]), .I3(sel[{contador}], .I4(sel[{contador+1}]), .I5(sel[{contador+2}])));\n")
				else:
					f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel[0]), .I2(polinomio[{aux}]), .I3(sel[0]), .I4(sel[1]), .I5(sel[2]));\n")
						
				contador+=3
				ocupacion_celda+=1
				aux+=1
						
			f.write("\n")

		f.write("endmodule\n")
	