import math


class osc_array:
	def __init__(self, x0, y0, x1, y1, dx, dy, nosc, directriz):
		self.x0 = x0
		self.y0 = y0
		self.x1 = x1
		self.y1 = y1
		self.dx = dx
		self.dy = dy
		self.nosc = nosc
		self.directriz = directriz
		
		
def genOscLocations(posmap, debug=0):
	osc_arrays = []
	for j in posmap.split():
		a = int(j.split(';')[0].split(',')[0]) # x0
		b = int(j.split(';')[0].split(',')[1]) # y0
		
		c = int(j.split(';')[1].split(',')[0]) # x1
		d = int(j.split(';')[1].split(',')[1]) # y1
		
		e = int(j.split(';')[2].split(',')[0]) # dx
		f = int(j.split(';')[2].split(',')[1]) # dy
		
		g = int(j.split(';')[3]) # nosc
		
		h = j.split(';')[4] # directriz
		
		osc_arrays.append(osc_array(a,b,c,d,e,f,g,h))
		
	lista_osciladores = ""
	for dominio in osc_arrays:
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
	if debug:
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
		
	return lista_osciladores[:-1]
	
	
def clog2(N):
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
			
			
def genRomatrix(out_name="romatrix.v", N_inv=3, tipo="lut1", pinmap_opt="no;", minsel=0, posmap="no", debug=0):
		
	pos_oscilador = [[],[]]
	for i in posmap.split():
		pos_oscilador[0].append(i.split(',')[0])
		pos_oscilador[1].append(i.split(',')[1])
		
	N_osciladores = len(pos_oscilador[0])

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
			
		f.write(f"//N_osciladores: {N_osciladores}\n\n")
		
		f.write("module ROMATRIX (\n")
		f.write("	input clock,\n")
		f.write("	input enable,\n")
		
		if N_osciladores > 1:
			f.write(f"	input[{clog2(N_osciladores)-1}:0] sel_ro,\n")
			
		if tipo != "lut1":
			if minsel:
				f.write(f"	input[0:0] sel_pdl,\n")
			else:
				f.write(f"	input[{N_inv-1}:0] sel_pdl,\n")
		
		f.write("	output out\n")
		f.write("	);\n\n")
		f.write("	(* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *)\n")
		f.write(f"	wire[{N_osciladores-1}:0] out_ro;\n")
		f.write(f"	reg[{N_osciladores-1}:0] enable_ro;\n")
		for i in range(N_osciladores):
			f.write(f"	wire[{N_inv-1}:0] w_{i};\n")
		f.write("\n")

		if N_osciladores>1:
			f.write("	assign out = enable? out_ro[sel_ro] : clock;\n")
			f.write("\n")
			f.write("	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[sel_ro]=1;\n")
			f.write("	end\n")
		else:
			f.write("	assign out = enable? out_ro[0] : clock;\n")
			f.write("\n")
			f.write("	always @(*) begin\n")
			f.write("		enable_ro = 0;\n")
			f.write("		if(enable) enable_ro[0]=1;\n")
			f.write("	end\n")
		f.write("\n\n")
		
		if not debug:
			if tipo == "lut1":
				for i in range(N_osciladores):
					celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
					f.write(f"	(* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
					
					aux=0
					ocupacion_celda=1
					while True:
						if ocupacion_celda==4:
							celda[1]+=1
							ocupacion_celda=0
						if aux == N_inv-1:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT1 #(2'b01) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]));\n\n")
							
							ocupacion_celda+=1
							
							break
							
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT1 #(2'b01) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]));\n")
						
						ocupacion_celda+=1
						aux+=1
						
					f.write("\n")

			elif tipo == "lut2":
				for i in range(N_osciladores):
					celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
					f.write(f"	(* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
					
					contador=0
					aux=0
					ocupacion_celda=1
					while True:
						if ocupacion_celda==4:
							celda[1]+=1
							ocupacion_celda=0
						if aux == N_inv-1:
							if not minsel:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]));\n\n")
							else:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]));\n\n")
								
							ocupacion_celda+=1
							
							break
								
						if not minsel:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]));\n")
						else:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]));\n")
								
						contador+=1
						ocupacion_celda+=1
						aux+=1
								
					f.write("\n")

			elif tipo == "lut3":
				for i in range(N_osciladores):
					celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
					f.write(f"	(* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
					
					contador=0
					aux=0
					ocupacion_celda=1
					while True:
						if ocupacion_celda==4:
							celda[1]+=1
							ocupacion_celda=0
						if aux == N_inv-1:
							if not minsel:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]));\n\n")
							else:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]));\n\n")
								
							ocupacion_celda+=1
							
							break
								
						if not minsel:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]));\n")
						else:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]));\n")
								
						contador+=2
						ocupacion_celda+=1
						aux+=1
								
					f.write("\n")

			elif tipo == "lut4":
				for i in range(N_osciladores):
					celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
					f.write(f"	(* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
					
					contador=0
					aux=0
					ocupacion_celda=1
					while True:
						if ocupacion_celda==4:
							celda[1]+=1
							ocupacion_celda=0
						if aux == N_inv-1:
							if not minsel:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]));\n\n")
							else:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]));\n\n")
								
							ocupacion_celda+=1
							
							break
								
						if not minsel:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]));\n")
						else:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]));\n")
								
						contador+=3
						ocupacion_celda+=1
						aux+=1
								
					f.write("\n")

			elif tipo == "lut5":
				for i in range(N_osciladores):
					celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
					f.write(f"	(* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
					
					contador=0
					aux=0
					ocupacion_celda=1
					while True:
						if ocupacion_celda==4:
							celda[1]+=1
							ocupacion_celda=0
						if aux == N_inv-1:
							if not minsel:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]), .I4(sel_pdl[{contador+3}]));\n\n")
							else:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]));\n\n")
								
							ocupacion_celda+=1
							
							break
								
						if not minsel:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}, .I4(sel_pdl[{contador+3}])]));\n")
						else:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]));\n")
								
						contador+=4
						ocupacion_celda+=1
						aux+=1
								
					f.write("\n")

			elif tipo == "lut6":
				for i in range(N_osciladores):
					celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
					f.write(f"	(* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
					
					contador=0
					aux=0
					ocupacion_celda=1
					while True:
						if ocupacion_celda==4:
							celda[1]+=1
							ocupacion_celda=0
						if aux == N_inv-1:
							if not minsel:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]), .I4(sel_pdl[{contador+3}]), .I5(sel_pdl[{contador+4}]));\n\n")
							else:
								f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]), .I5(sel_pdl[4]));\n\n")
								
							ocupacion_celda+=1
							
							break
								
						if not minsel:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}], .I4(sel_pdl[{contador+3}]), .I5(sel_pdl[{contador+4}])));\n")
						else:
							f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]), .I5(sel_pdl[4]));\n")
								
						contador+=5
						ocupacion_celda+=1
						aux+=1
								
					f.write("\n")

			f.write("endmodule\n")
		
		else:
			for i in range(N_osciladores):
				f.write(f"	CLOCK_DIVIDER cd_{i} (\n")
				f.write("		.clock_in(clock),\n")
				f.write(f"		.fdiv({i}),\n")
				f.write(f"		.clock_out(out_ro[{i}])\n")
				f.write("	);\n")
				f.write("\n")
			f.write("endmodule\n")
			

def genGaromatrix(out_name="garomatrix.v", N_inv=3, tipo="lut3", pinmap_opt="no;", minsel=0, posmap="no"):
		
	pos_oscilador = [[],[]]
	for i in posmap.split():
		pos_oscilador[0].append(i.split(',')[0])
		pos_oscilador[1].append(i.split(',')[1])
		
	N_osciladores = len(pos_oscilador[0])

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
		
		f.write("module GAROMATRIX (\n")
		f.write("	input clock,\n")
		f.write("	input enable,\n")
		f.write("	input clock_s,\n")
		f.write(f"	input[{N_inv-1}:0] sel_poly,\n")
		
		if N_osciladores > 1:
			f.write(f"	input[{clog2(N_osciladores)-1}:0] sel_ro,\n")
			
		if tipo != "lut3":
			if not minsel:
				f.write(f"	input[{N_inv-1}:0] sel_pdl,\n")
			else:
				f.write(f"	input[0:0] sel_pdl,\n")
				
		f.write("	output out\n")
		f.write("	);\n\n")
		f.write(f"	wire[{N_osciladores-1}:0] out_ro;\n")
		f.write(f"	wire[{N_osciladores-1}:0] out_ro_sampled;\n")
		f.write("	(* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *)\n")
		f.write(f"	wire[{N_inv-1}:0]\n")
		
		for i in range(1, N_osciladores, 1):
			f.write(f"		w_{i-1},\n")
		f.write(f"		w_{N_osciladores-1};\n")
		f.write("\n")

		if N_osciladores>1:
			f.write("	assign out = enable? out_ro_sampled[sel_ro] : clock;\n")
		else:
			f.write("	assign out = enable? out_ro_sampled[0] : clock;\n")
		f.write("\n\n")
		
		if tipo == "lut3":
			for i in range(N_osciladores):
				celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
				f.write(f"	(* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[0]}*) LUT3 #(8'h95) inv_{i}_{0}(.O(w_{i}[{0}]), .I0(w_{i}[{N_inv-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{0}]));\n")
				aux=1
				ocupacion_celda=1
				while True:
					if ocupacion_celda==4:
						celda[1]+=1
						ocupacion_celda=0
					if aux == N_inv:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) inv_{i}(.O(out_ro[{i}]), .I0(w_{i}[{N_inv-1}]));\n") # INV final
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
						
						ocupacion_celda+=1
						
						break
						
					f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT3 #(8'h95) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{aux}]));\n")
					
					ocupacion_celda+=1
					aux+=1
				f.write("\n")

		elif tipo == "lut4":
			for i in range(N_osciladores):
				celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
				contador=0
				aux=0
				ocupacion_celda=0
				while True:
					if ocupacion_celda==4:
						celda[1]+=1
						ocupacion_celda=0
					if aux == N_inv-1:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) inv_{i}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]));\n") # INV final
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
						
						ocupacion_celda+=1
						
						break
							
					if not minsel:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_poly[{aux}]), .I3(sel_pdl[{contador}]));\n")
					else:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_poly[{aux}]), .I3(sel_pdl[0]));\n")
							
					contador+=1
					ocupacion_celda+=1
					aux+=1
				f.write("\n")

		elif tipo == "lut5":
			for i in range(N_osciladores):
				celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
				contador=0
				aux=0
				ocupacion_celda=0
				while True:
					if ocupacion_celda==4:
						celda[1]+=1
						ocupacion_celda=0
					if aux == N_inv-1:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) inv_{i}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]));\n") # INV final
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
						
						ocupacion_celda+=1
						
						break
						
					if not minsel:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_poly[{aux}]), .I3(sel_pdl[{contador}, .I4(sel_pdl[{contador+1}])]));\n")
					else:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_poly[{aux}]), .I3(sel_pdl[0]), .I4(sel_pdl[1]));\n")
							
					contador+=2
					ocupacion_celda+=1
					aux+=1
				f.write("\n")

		elif tipo == "lut6":
			for i in range(N_osciladores):
				celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
				contador=0
				aux=0
				ocupacion_celda=0
				while True:
					if ocupacion_celda==4:
						celda[1]+=1
						ocupacion_celda=0
					if aux == N_inv-1:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) inv_{i}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]));\n") # INV final
						f.write(f"	(* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
							
						ocupacion_celda+=1
						
						break
						
					if not minsel:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_poly[{aux}]), .I3(sel_pdl[{contador}], .I4(sel_pdl[{contador+1}]), .I5(sel_pdl[{contador+2}])));\n")
					else:
						f.write(f"	(* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_poly[{aux}]), .I3(sel_pdl[0]), .I4(sel_pdl[1]), .I5(sel_pdl[2]));\n")
							
					contador+=3
					ocupacion_celda+=1
					aux+=1
				f.write("\n")
				
		f.write("endmodule\n")