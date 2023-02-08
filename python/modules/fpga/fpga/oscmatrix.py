import math
import os
import sys
from subprocess import run # esta función permite llamar a sub-rutinas de shell (e.g. programas en C), y pasar/recibir datos.
import serial
import time
from numpy import reshape
from fpga import pinta_progreso
from fpga.interfazpcps import *
from mytensor import *


class OSCSUBMATRIX:
    def __init__(self, x0=2, y0=1, x1=10, y1=10, dx=2, dy=1, N_osc=10, directriz='y'):
        self.x0 = x0
        self.y0 = y0
        self.x1 = x1
        self.y1 = y1
        self.dx = dx
        self.dy = dy
        self.N_osc = N_osc
        self.directriz = directriz
        
        
def genOscCoord(lista_submatrices):
    lista_osciladores = ""
    for dominio in lista_submatrices:
        x = dominio.x0
        y = dominio.y0
        for i in range(dominio.N_osc):
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
    osc_coord = [[],[]]
    for i in lista_osciladores.split():
        osc_coord[0].append(int(i.split(',')[0]))
        osc_coord[1].append(int(i.split(',')[1]))
                
    return osc_coord
    
    
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
            
            
def genGaromatrix(out_name="garomatrix.v", N_inv=3, tipo="lut3", pinmap="no;", minsel=0, posmap="no"):
        
    pos_oscilador = [[],[]]
    for i in posmap.split():
        pos_oscilador[0].append(int(i.split(',')[0]))
        pos_oscilador[1].append(int(i.split(',')[1]))
        
    N_osciladores = len(pos_oscilador[0])

    with open(out_name, "w") as f:
        bel_ocupacion = ["BEL=\"A6LUT\"","BEL=\"B6LUT\"","BEL=\"C6LUT\"","BEL=\"D6LUT\""]
        
        pinmap_proc = []
        intaux = 0
        for i in range(len(pinmap)):
            if pinmap[i]==';':
                saux = pinmap[intaux:i]
                intaux+=i+1
                if saux == "no":
                    pinmap_proc.append(" ")
                else:
                    pinmap_proc.append(f", LOCK_PINS=\"{saux}\" ")
        for i in range(len(pinmap_proc),N_inv,1):
            pinmap_proc.append(pinmap_proc[-1])
            
        f.write(f"//N_osciladores de Galois: {N_osciladores}\n\n")
        
        f.write("module GAROMATRIX (\n")
        f.write("   input clock,\n")
        f.write("   input enable,\n")
        f.write("   input clock_s,\n")
        f.write(f"  input[{N_inv-2}:0] sel_poly,\n")
        
        if N_osciladores > 1:
            f.write(f"  input[{clog2(N_osciladores)-1}:0] sel_ro,\n")
            
        if tipo == "lut4":
            if not minsel:
                f.write(f"  input[{N_inv-1}:0] sel_pdl,\n")
            else:
                f.write(f"  input[0:0] sel_pdl,\n")
        elif tipo == "lut5":
            if not minsel:
                f.write(f"  input[{2*N_inv-1}:0] sel_pdl,\n")
            else:
                f.write(f"  input[1:0] sel_pdl,\n")
        elif tipo == "lut6":
            if not minsel:
                f.write(f"  input[{3*N_inv-1}:0] sel_pdl,\n")
            else:
                f.write(f"  input[2:0] sel_pdl,\n")
                
        f.write("   output out\n")
        f.write("   );\n\n")
        f.write(f"  wire[{N_osciladores-1}:0] out_ro;\n")
        f.write(f"  wire[{N_osciladores-1}:0] out_ro_sampled;\n")
        f.write("   (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *)\n")
        f.write(f"  wire[{N_inv-1}:0]\n")
        
        for i in range(1, N_osciladores, 1):
            f.write(f"      w_{i-1},\n")
        f.write(f"      w_{N_osciladores-1};\n")
        f.write("\n")

        if N_osciladores>1:
            f.write("   assign out = enable? out_ro_sampled[sel_ro] : clock;\n")
        else:
            f.write("   assign out = enable? out_ro_sampled[0] : clock;\n")
        f.write("\n\n")
        
        if tipo == "lut3":
            for i in range(N_osciladores):
                celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
                f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[0]}*) LUT3 #(8'h95) inv_{i}_0(.O(w_{i}[{0}]), .I0(w_{i}[{N_inv-1}]), .I1(1'b0), .I2(1'b0));\n")
                
                aux=1
                ocupacion_celda=1
                while True:
                    if ocupacion_celda==4:
                        if celda[0]%2 == 0:
                            celda[0]+=1
                        else:
                            celda[1]+=1
                        ocupacion_celda=0
                    if aux == N_inv:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) invout_{i}(.O(out_ro[{i}]), .I0(w_{i}[{N_inv-1}]));\n") # INV final
                        f.write(f"  (* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
                        
                        ocupacion_celda+=1
                        
                        break
                        
                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h95) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{aux-1}]));\n")
                    
                    ocupacion_celda+=1
                    aux+=1
                f.write("\n")

        elif tipo == "lut4":
            for i in range(N_osciladores):
                celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
                f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[0]}*) LUT4 #(16'h9595) inv_{i}_{0}(.O(w_{i}[{0}]), .I0(w_{i}[{N_inv-1}]), .I1(1'b0), .I2(1'b0), .I3(sel_pdl[0]));\n")
                
                contador=1
                aux=1
                ocupacion_celda=1
                while True:
                    if ocupacion_celda==4:
                        if celda[0]%2 == 0:
                            celda[0]+=1
                        else:
                            celda[1]+=1
                        ocupacion_celda=0
                    if aux == N_inv:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) invout_{i}(.O(out_ro[{i}]), .I0(w_{i}[{N_inv-1}]));\n") # INV final
                        f.write(f"  (* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
                        
                        ocupacion_celda+=1
                        
                        break
                            
                    if not minsel:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h9595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[{contador}]));\n")
                    else:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h9595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[0]));\n")
                            
                    contador+=1
                    ocupacion_celda+=1
                    aux+=1
                f.write("\n")

        elif tipo == "lut5":
            for i in range(N_osciladores):
                celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
                f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[0]}*) LUT5 #(32'h95959595) inv_{i}_{0}(.O(w_{i}[{0}]), .I0(w_{i}[{N_inv-1}]), .I1(1'b0), .I2(1'b0), .I3(sel_pdl[0]), .I4(sel_pdl[1]));\n")
                
                contador=2
                aux=1
                ocupacion_celda=1
                while True:
                    if ocupacion_celda==4:
                        if celda[0]%2 == 0:
                            celda[0]+=1
                        else:
                            celda[1]+=1
                        ocupacion_celda=0
                    if aux == N_inv:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) invout_{i}(.O(out_ro[{i}]), .I0(w_{i}[{N_inv-1}]));\n") # INV final
                        f.write(f"  (* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
                        
                        ocupacion_celda+=1
                        
                        break
                        
                    if not minsel:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h95959595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[{contador}]), .I4(sel_pdl[{contador+1}]));\n")
                    else:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h95959595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[0]), .I4(sel_pdl[1]));\n")
                        
                    contador+=2
                    ocupacion_celda+=1
                    aux+=1
                f.write("\n")

        elif tipo == "lut6":
            for i in range(N_osciladores):
                celda=[pos_oscilador[0][i],pos_oscilador[1][i]]
                f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[0]}*) LUT6 #(64'h9595959595959595) inv_{i}_{0}(.O(w_{i}[{0}]), .I0(w_{i}[{N_inv-1}]), .I1(1'b0), .I2(1'b0), .I3(sel_pdl[0]), .I4(sel_pdl[1]), .I5(sel_pdl[2]));\n")
                
                contador=3
                aux=1
                ocupacion_celda=1
                while True:
                    if ocupacion_celda==4:
                        if celda[0]%2 == 0:
                            celda[0]+=1
                        else:
                            celda[1]+=1
                        ocupacion_celda=0
                    if aux == N_inv:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) invout_{i}(.O(out_ro[{i}]), .I0(w_{i}[{N_inv-1}]));\n") # INV final
                        f.write(f"  (* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
                        
                        ocupacion_celda+=1
                        
                        break
                        
                    if not minsel:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h9595959595959595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[{contador}]), .I4(sel_pdl[{contador+1}]), .I5(sel_pdl[{contador+2}]));\n")
                    else:
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h9595959595959595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[0]), .I4(sel_pdl[1]), .I5(sel_pdl[2]));\n")
                        
                    contador+=3
                    ocupacion_celda+=1
                    aux+=1
                f.write("\n")
                
        f.write("endmodule\n")
        
        
class ROMATRIX:
    def __init__(self, N_inv=3, submatrices=OSCSUBMATRIX()):
        self.submatrices=[]
        if type(submatrices) == type([]) or type(submatrices) == type(()):
            self.submatrices[:]=submatrices[:]
        else:
            self.submatrices.append(submatrices)
        self.osc_coord = genOscCoord(self.submatrices)
        self.N_inv = N_inv
        self.N_osc = len(self.osc_coord[0])
        
    def plotSubmatrices(self):
        import matplotlib.pyplot as plt

        max_x = max(self.osc_coord[0])
        min_x = min(self.osc_coord[0])
        max_y = max(self.osc_coord[1])
        min_y = min(self.osc_coord[1])

        mapa = [[0 for j in range(min_y,max_y+1,1)] for i in range(min_x,max_x+1,1)]

        for i,x in enumerate(range(min_x,max_x,1)):
            for j,y in enumerate(range(min_y,max_y,1)):
                if f"{x},{y}" in self.osc_coord:
                    mapa[i][j]=1
        plt.imshow(mapa, interpolation='none', origin='lower')
        plt.show()

    def genRomatrix(self, out_name="romatrix.v", tipo_lut="lut1", pinmap="no;", minsel=False, debug=False):
        N_inv = self.N_inv
        N_osciladores = self.N_osc

        with open(out_name, "w") as f:
            bel_ocupacion = ["BEL=\"A6LUT\"","BEL=\"B6LUT\"","BEL=\"C6LUT\"","BEL=\"D6LUT\""]
            
            pinmap_proc = []
            intaux = 0
            for i in range(len(pinmap)):
                if pinmap[i]==';':
                    saux = pinmap[intaux:i]
                    intaux+=i+1
                    if saux == "no":
                        pinmap_proc.append(" ")
                    else:
                        pinmap_proc.append(f", LOCK_PINS=\"{saux}\" ")
            for i in range(len(pinmap_proc),N_inv,1):
                pinmap_proc.append(pinmap_proc[-1])
                
            f.write(f"//N_osciladores: {N_osciladores}\n\n")
            
            f.write("module ROMATRIX (\n")
            f.write("   input clock,\n")
            f.write("   input enable,\n")
            
            if N_osciladores > 1:
                f.write(f"  input[{clog2(N_osciladores)-1}:0] sel_ro,\n")
                
            if tipo_lut == "lut2":
                if minsel:
                    f.write(f"  input[0:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{N_inv-1}:0] sel_pdl,\n")
            elif tipo_lut == "lut3":
                if minsel:
                    f.write(f"  input[1:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{2*N_inv-1}:0] sel_pdl,\n")
            elif tipo_lut == "lut4":
                if minsel:
                    f.write(f"  input[2:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{3*N_inv-1}:0] sel_pdl,\n")
            elif tipo_lut == "lut5":
                if minsel:
                    f.write(f"  input[3:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{4*N_inv-1}:0] sel_pdl,\n")
            elif tipo_lut == "lut6":
                if minsel:
                    f.write(f"  input[4:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{5*N_inv-1}:0] sel_pdl,\n")
            
            f.write("   output out\n")
            f.write("   );\n\n")
            f.write("   (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *)\n")
            f.write(f"  wire[{N_osciladores-1}:0] out_ro;\n")
            f.write(f"  reg[{N_osciladores-1}:0] enable_ro;\n")
            for i in range(N_osciladores):
                f.write(f"  wire[{N_inv-1}:0] w_{i};\n")
            f.write("\n")

            if N_osciladores>1:
                f.write("   assign out = enable? out_ro[sel_ro] : clock;\n")
                f.write("\n")
                f.write("   always @(*) begin\n")
                f.write("       enable_ro = 0;\n")
                f.write("       if(enable) enable_ro[sel_ro]=1;\n")
                f.write("   end\n")
            else:
                f.write("   assign out = enable? out_ro[0] : clock;\n")
                f.write("\n")
                f.write("   always @(*) begin\n")
                f.write("       enable_ro = 0;\n")
                f.write("       if(enable) enable_ro[0]=1;\n")
                f.write("   end\n")
            f.write("\n\n")
            
            if not debug:
                if tipo_lut == "lut1":
                    for i in range(N_osciladores):
                        celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                        f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
                        
                        aux=0
                        ocupacion_celda=1
                        while True:
                            if ocupacion_celda==4:
                                if celda[0]%2 == 0:
                                    celda[0]+=1
                                else:
                                    celda[1]+=1
                                ocupacion_celda=0
                            if aux == N_inv-1:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT1 #(2'b01) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]));\n\n")
                                
                                ocupacion_celda+=1
                                
                                break
                                
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT1 #(2'b01) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]));\n")
                            
                            ocupacion_celda+=1
                            aux+=1
                            
                        f.write("\n")

                elif tipo_lut == "lut2":
                    for i in range(N_osciladores):
                        celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                        f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
                        
                        contador=0
                        aux=0
                        ocupacion_celda=1
                        while True:
                            if ocupacion_celda==4:
                                if celda[0]%2 == 0:
                                    celda[0]+=1
                                else:
                                    celda[1]+=1
                                ocupacion_celda=0
                            if aux == N_inv-1:
                                if not minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]));\n")
                                    
                            contador+=1
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                elif tipo_lut == "lut3":
                    for i in range(N_osciladores):
                        celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                        f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
                        
                        contador=0
                        aux=0
                        ocupacion_celda=1
                        while True:
                            if ocupacion_celda==4:
                                if celda[0]%2 == 0:
                                    celda[0]+=1
                                else:
                                    celda[1]+=1
                                ocupacion_celda=0
                            if aux == N_inv-1:
                                if not minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]));\n")
                                    
                            contador+=2
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                elif tipo_lut == "lut4":
                    for i in range(N_osciladores):
                        celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                        f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
                        
                        contador=0
                        aux=0
                        ocupacion_celda=1
                        while True:
                            if ocupacion_celda==4:
                                if celda[0]%2 == 0:
                                    celda[0]+=1
                                else:
                                    celda[1]+=1
                                ocupacion_celda=0
                            if aux == N_inv-1:
                                if not minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]));\n")
                                    
                            contador+=3
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                elif tipo_lut == "lut5":
                    for i in range(N_osciladores):
                        celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                        f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
                        
                        contador=0
                        aux=0
                        ocupacion_celda=1
                        while True:
                            if ocupacion_celda==4:
                                if celda[0]%2 == 0:
                                    celda[0]+=1
                                else:
                                    celda[1]+=1
                                ocupacion_celda=0
                            if aux == N_inv-1:
                                if not minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]), .I4(sel_pdl[{contador+3}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}, .I4(sel_pdl[{contador+3}])]));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]));\n")
                                    
                            contador+=4
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                elif tipo_lut == "lut6":
                    for i in range(N_osciladores):
                        celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                        f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\", LOCK_PINS=\"I1:A1\" *) LUT2 #(4'b1000) AND_{i}(.O(w_{i}[0]), .I0(enable_ro[{i}]), .I1(out_ro[{i}]));\n") # AND inicial
                        
                        contador=0
                        aux=0
                        ocupacion_celda=1
                        while True:
                            if ocupacion_celda==4:
                                if celda[0]%2 == 0:
                                    celda[0]+=1
                                else:
                                    celda[1]+=1
                                ocupacion_celda=0
                            if aux == N_inv-1:
                                if not minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]), .I4(sel_pdl[{contador+3}]), .I5(sel_pdl[{contador+4}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]), .I5(sel_pdl[4]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}], .I4(sel_pdl[{contador+3}]), .I5(sel_pdl[{contador+4}])));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]), .I5(sel_pdl[4]));\n")
                                    
                            contador+=5
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                f.write("endmodule\n")
            
            else:
                for i in range(N_osciladores):
                    f.write(f"  CLOCK_DIVIDER cd_{i} (\n")
                    f.write("       .clock_in(clock),\n")
                    f.write(f"      .fdiv({i}),\n")
                    f.write(f"      .clock_out(out_ro[{i}])\n")
                    f.write("   );\n")
                    f.write("\n")
                f.write("endmodule\n")
        
    def implement(self, proj_name="project_romatrix", proj_dir="./", board="pynqz2", n_jobs=4, qspi=False, detail_routing=False, tipo_lut="lut1", pinmap="no;", minsel=False, pblock="no", data_width=32, buffer_out_width=32, wstyle=True, debug=False):
        if board=='cmoda7_15t':
            fpga_part="xc7a15tcpg236-1"
            board_part="digilentinc.com:cmod_a7-15t:part0:1.1"
            memory_part="mx25l3233"
            clk_name="/clk_wiz_1/clk_out1 (100 MHz)"
        elif board=='cmoda7_35t':
            fpga_part="xc7a35tcpg236-1"
            board_part="digilentinc.com:cmod_a7-35t:part0:1.1"
            memory_part="mx25l3233"
            clk_name="/clk_wiz_1/clk_out1 (100 MHz)"
        elif board=='zybo':
            fpga_part="xc7z010clg400-1"
            board_part="digilentinc.com:zybo:part0:1.0"
            memory_part="?"
            clk_name = "/processing_system7_0/FCLK_CLK0 (100 MHz)"
        elif board=='pynqz2':
            fpga_part="xc7z020clg400-1"
            board_part="tul.com.tw:pynq-z2:part0:1.0"
            memory_part="?"
            clk_name = "/processing_system7_0/FCLK_CLK0 (100 MHz)"
        
        posmap = self.osc_coord
        N_osc = self.N_osc
        N_bits_osc = clog2(N_osc)
        N_inv = self.N_inv
        
        if minsel:
            if tipo_lut == "lut1":
                N_bits_pdl = 0
            elif tipo_lut == "lut2":
                N_bits_pdl = 1
            elif tipo_lut == "lut3":
                N_bits_pdl = 2
            elif tipo_lut == "lut4":
                N_bits_pdl = 3
            elif tipo_lut == "lut5":
                N_bits_pdl = 4
            elif tipo_lut == "lut6":
                N_bits_pdl = 5
        else:
            if tipo_lut == "lut1":
                N_bits_pdl = 0
            elif tipo_lut == "lut2":
                N_bits_pdl = N_inv
            elif tipo_lut == "lut3":
                N_bits_pdl = 2*N_inv
            elif tipo_lut == "lut4":
                N_bits_pdl = 3*N_inv
            elif tipo_lut == "lut5":
                N_bits_pdl = 4*N_inv
            elif tipo_lut == "lut6":
                N_bits_pdl = 5*N_inv
                
        buffer_in_width = N_bits_osc+N_bits_pdl+5
        
        if buffer_in_width%8 == 0:
            octeto_in_width=buffer_in_width//8
        else:
            octeto_in_width=buffer_in_width//8+1

        if buffer_out_width%8 == 0:
            octeto_out_width=buffer_out_width//8
        else:
            octeto_out_width=buffer_out_width//8+1

        if buffer_in_width%data_width == 0:
            words_in_width=buffer_in_width//data_width
        else:
            words_in_width=buffer_in_width//data_width+1

        if buffer_out_width%data_width == 0:
            words_out_width=buffer_out_width//data_width
        else:
            words_out_width=buffer_out_width//data_width+1

        dw_ge_biw = ""
        biw_aligned_dw = ""
        biw_misaligned_dw = ""

        if data_width >= buffer_in_width:
            dw_ge_biw='`define DW_GE_BIW'
        elif buffer_in_width%data_width == 0:
            biw_aligned_dw='`define BIW_ALIGNED_DW'
        else:
            biw_misaligned_dw='`define BIW_MISALIGNED_DW'

        dw_ge_bow = ""
        bow_aligned_dw = ""
        bow_misaligned_dw = ""

        if data_width >= buffer_out_width:
            dw_ge_bow='`define DW_GE_BOW'
        elif buffer_out_width%data_width == 0:
            bow_aligned_dw='`define BOW_ALIGNED_DW'
        else:
            bow_misaligned_dw='`define BOW_MISALIGNED_DW'
            
        ## proj_dir (directorio actual)
        if wstyle:
            proj_dir = run(["wslpath","-w",os.environ['PWD']], capture_output=True, text=True).stdout.replace("\\","/").replace("\n","")
        else:
            proj_dir=os.environ['PWD']


        ## block design
        run(["mkdir","./block_design"])
        if board == "cmoda7_15t":
            if qspi:
                run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])
            else:
                run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])

        elif board == "cmoda7_35t":
            if qspi:
                run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])
            else:
                run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])

        elif board == "zybo":
            if qspi:
                run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_zybo.tcl","./block_design/bd_design_1.tcl"])
            else:
                run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_zybo.tcl","./block_design/bd_design_1.tcl"])

        elif board == "pynqz2":
            if qspi:
                run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_pynqz2.tcl","./block_design/bd_design_1.tcl"])
            else:
                run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_pynqz2.tcl","./block_design/bd_design_1.tcl"])


        ## partial flows (tcl)
        run(["mkdir","./partial_flows"])

        vivado_files=f"{proj_dir}/vivado_src/top.v {proj_dir}/vivado_src/interfaz_pspl.cp.v {proj_dir}/vivado_src/romatrix.v {proj_dir}/vivado_src/medidor_frec.cp.v {proj_dir}/vivado_src/interfaz_pspl_config.vh"
        if debug:
            vivado_files+=f" {proj_dir}/vivado_src/clock_divider.cp.v"
        if pblock != "no":
            vivado_files+=f" {proj_dir}/vivado_src/pblock.xdc"

        with open("./partial_flows/setupdesign.tcl", "w") as f:
            f.write(f"""
create_project {proj_name} {proj_dir}/{proj_name} -part {fpga_part}

set_property board_part {board_part} [current_project]

source {proj_dir}/block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

update_compile_order -fileset sources_1

add_files -norecurse {{ {vivado_files} }}

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

startgroup
set_property -dict [list CONFIG.C_GPIO_WIDTH {data_width} CONFIG.C_GPIO2_WIDTH {data_width}] [get_bd_cells axi_gpio_data]
endgroup

connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {{Clk {clk_name} }}  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files {proj_dir}/{proj_name}/{proj_name}.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse {proj_dir}/{proj_name}/{proj_name}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

set_property top design_1_wrapper [current_fileset]

update_compile_order -fileset sources_1

set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]

""")

        with open("./partial_flows/genbitstream.tcl", "w") as f:
            f.write(f"""
if {{[file exists {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc
}}

if {{[file exists {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc
}}

if {{[file exists {proj_dir}/{proj_name}/{proj_name}.runs/synth_1]==1}} {{
    reset_run synth_1
}}

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

launch_runs synth_1 -jobs {n_jobs}

wait_on_run synth_1

""")
            if qspi and board == "cmoda7_15t":
                f.write(f"""
file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1

file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new

close [ open {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc]

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

open_run synth_1 -name synth_1

startgroup

set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]

set_property config_mode SPIx4 [current_design]

endgroup

save_constraints

close_design
""")
            elif qspi and board == "cmoda7_35t":
                f.write(f"""
file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1

file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new

close [ open {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc]

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

open_run synth_1 -name synth_1

startgroup

set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]

set_property config_mode SPIx4 [current_design]

endgroup

save_constraints

close_design
""")

            if detail_routing:
                f.write("""
        open_run synth_1 -name synth_1

        startgroup
        """)
                for i in range(N_osc):
                    f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_0]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_0 }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_{i} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_{i} }}]
""")
                    for j in range(1,N_inv,1):
                        f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j}]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j} }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
""") 
                f.write(f"""
endgroup

file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new

close [ open {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc w ]

add_files -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc

set_property target_constrs_file {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc [current_fileset -constrset]

save_constraints -force

close_design

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0
""") # Aqui termina detailed routing

            f.write(f"""
if {{[file exists {proj_dir}/{proj_name}/{proj_name}.runs/synth_1/__synthesis_is_complete__]==1}} {{
    reset_run synth_1
}}

launch_runs impl_1 -to_step write_bitstream -jobs {n_jobs}

wait_on_run impl_1
""")

        with open("./partial_flows/launchsdk.tcl", "w") as f:
            f.write(f"""
file mkdir {proj_dir}/{proj_name}/{proj_name}.sdk

file copy -force {proj_dir}/{proj_name}/{proj_name}.runs/impl_1/design_1_wrapper.sysdef {proj_dir}/{proj_name}/{proj_name}.sdk/design_1_wrapper.hdf

launch_sdk -workspace {proj_dir}/{proj_name}/{proj_name}.sdk -hwspec {proj_dir}/{proj_name}/{proj_name}.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform

""")

        with open("mkhwdplatform.tcl", "w") as f:
            f.write(f"""
source {proj_dir}/partial_flows/setupdesign.tcl

source {proj_dir}/partial_flows/genbitstream.tcl

source {proj_dir}/partial_flows/launchsdk.tcl

""")


        ## vivado sources
        run(["mkdir","./vivado_src"])
        run(["cp",f"{os.environ['REPO_fpga']}/verilog/interfaz_pspl.v","./vivado_src/interfaz_pspl.cp.v"])
        run(["cp",f"{os.environ['REPO_fpga']}/verilog/medidor_frec.v","./vivado_src/medidor_frec.cp.v"])
        if debug:
            run(["cp",f"{os.environ['REPO_fpga']}/verilog/clock_divider.v","./vivado_src/clock_divider.cp.v"])

        with open("vivado_src/interfaz_pspl_config.vh", "w") as f:
            f.write(f"""{dw_ge_biw}
        {biw_aligned_dw}
        {biw_misaligned_dw}
        {dw_ge_bow}
        {bow_aligned_dw}
        {bow_misaligned_dw}
        """)
        
        self.genRomatrix(out_name="vivado_src/romatrix.v", tipo_lut=tipo_lut, pinmap=pinmap, minsel=minsel, debug=debug)

        with open("vivado_src/top.v", "w") as f:
            if N_osc==1:
                aux=""
            else:
                aux=f".sel_ro(buffer_in[{N_bits_osc-1}:0]),"
            if tipo_lut == "lut1":
                aux1=""
            else:
                aux1=f".sel_pdl(buffer_in[{buffer_in_width-5-1}:{N_bits_osc}]),"

            f.write(f"""`timescale 1ns / 1ps


module TOP (
    input           clock,
    input[7:0]      ctrl_in,
    output[7:0]     ctrl_out,
    input[{data_width-1}:0] data_in,
    output[{data_width-1}:0]    data_out
);

    wire[{buffer_in_width-1}:0] buffer_in;
    wire[{buffer_out_width-1}:0] buffer_out;
    wire sync;
    wire ack;
    wire out_ro;
    reg enable_medidor=0;

    always @(posedge clock) begin
        case ({{sync,ack}})
            2'b10:
                enable_medidor <= 1;

            default:
                enable_medidor <= 0;
        endcase
    end

    INTERFAZ_PSPL #(
        .DATA_WIDTH({data_width}),
        .BUFFER_IN_WIDTH({buffer_in_width}),
        .BUFFER_OUT_WIDTH({buffer_out_width})
    ) interfaz_pspl (
        .clock(clock),
        .ctrl_in(ctrl_in),
        .ctrl_out(ctrl_out),
        .data_in(data_in),
        .data_out(data_out),
        .sync(sync),
        .ack(ack),
        .buffer_in(buffer_in),
        .buffer_out(buffer_out)
    );

    ROMATRIX romatrix (
        .clock(clock),
        .enable(1'b1),
        {aux}
        {aux1}
        .out(out_ro)
    );

    MEDIDOR_FREC #(
        .OUT_WIDTH({buffer_out_width})
    ) medidor_frec (
        .clock(clock),
        .enable(enable_medidor),
        .clock_u(out_ro),
        .resol(buffer_in[{buffer_in_width-1}:{buffer_in_width-5}]),
        .lock(ack),
        .out(buffer_out)
    );

endmodule
""")


        if pblock!="no":
            PBLOCK_CORNERS=pblock.split()
            PBLOCK_CORNER_0=PBLOCK_CORNERS[0].split(',')
            PBLOCK_CORNER_1=PBLOCK_CORNERS[1].split(',')
            with open("./vivado_src/pblock.xdc", "w") as f:
                f.write(f"""create_pblock pblock_TOP_0
add_cells_to_pblock [get_pblocks pblock_TOP_0] [get_cells -quiet [list design_1_i/TOP_0]]
resize_pblock [get_pblocks pblock_TOP_0] -add {{SLICE_X{PBLOCK_CORNER_0[0]}Y{PBLOCK_CORNER_0[1]}:SLICE_X{PBLOCK_CORNER_1[0]}Y{PBLOCK_CORNER_1[1]}}}
        """)


        ## sdk sources
        run(["mkdir","./sdk_src"])
        run(["cp",f"{os.environ['REPO_fpga']}/c-xilinx/sdk/interfaz_pcps-pspl.c","./sdk_src/interfaz_pcps-pspl.cp.c"])

        with open("./sdk_src/interfaz_pcps-pspl_config.h", "w") as f:
            if board == "cmoda7_15t" or board == "cmoda7_35t":
                f.write("#include \"xuartlite.h\"\n\n")
            elif board == "zybo" or board == "pynqz2":
                f.write("#include \"xuartps.h\"\n\n")

            f.write(f"""#define DATA_WIDTH          {data_width}
#define BUFFER_IN_WIDTH     {buffer_in_width}
#define BUFFER_OUT_WIDTH    {buffer_out_width}
#define OCTETO_IN_WIDTH     {octeto_in_width}
#define OCTETO_OUT_WIDTH    {octeto_out_width}
#define WORDS_IN_WIDTH      {words_in_width}
#define WORDS_OUT_WIDTH     {words_out_width}
""")

        ## config file
        with open("medir_romatrix.config","w") as f:
            f.write(f""" sel_ro_width = {N_bits_osc}
 sel_pdl_width = {N_bits_pdl}
 sel_resol_width = {5}
 buffer_out_width = {buffer_out_width}
""")


        ## log info
        print(f"""
 N_osc = {N_osc}
 N_pdl = {2**N_bits_pdl}

 Trama del selector(ts): {N_bits_osc} {N_bits_pdl} 5

    (0) <---sel_ro({N_bits_osc})---><---sel_pdl({N_bits_pdl})---><---sel_resol(5)---> ({buffer_in_width-1})


 sel_ro_width = {N_bits_osc}
 sel_pdl_width = {N_bits_pdl}
 dw  = {data_width}
 biw = {buffer_in_width}
 bow = {buffer_out_width}

 fpga part: {fpga_part}

 sdk source files: {proj_dir}/sdk_src/

""")
        if qspi:
            print(f"q-spi part: {memory_part}")

    def medir(self, osc=range(1), pdl=range(1), resol=range(16,17,1), N_rep=1, f_ref=False, config='auto', out_fmt='%.18e', log=False, puerto='S1', baudrate=9600):
        sel_ro_width        = 4
        sel_pdl_width       = 0
        sel_resol_width     = 5
        buffer_out_width    = 32
        osc_list=list(osc)
        pdl_list=list(pdl)
        resol_list=list(resol)
        N_osc = len(osc_list) # Número de osciladores
        N_pdl = len(pdl_list) # Número de pdl_list
        N_resol = len(resol_list) # Número de medidas a repetir con distinta resolución

        if config == 'auto':
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
        else:
            if 'sel_ro_width' in config:
                sel_ro_width = config['sel_ro_width']
            elif 'sow' in config:
                sel_ro_width = config['sow']
                
            if 'sel_pdl_width' in config:
                sel_pdl_width = config['sel_pdl_width']
            elif 'spw' in config:
                sel_pdl_width = config['spw']
                
            if 'sel_resol_width' in config:
                sel_resol_width = config['sel_resol_width']
            elif 'srw' in config:
                sel_resol_width = config['row']
                
            if 'buffer_out_width' in config:
                buffer_out_width = config['buffer_out_width']
            elif 'bow' in config:
                buffer_out_width = config['bow']
                
        buffer_in_width = sel_ro_width+sel_pdl_width+sel_resol_width

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
            
        fpga = serial.Serial(port='/dev/tty'+puerto, baudrate=baudrate, bytesize=8)
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

        tensor_medidas = TENSOR(reshape(medidas, (N_rep,N_resol,N_pdl,N_osc)), metadatos={'shape':[N_rep,N_resol,N_pdl,N_osc], 'axis':['rep','resol','pdl','osc']})
        
        return tensor_medidas