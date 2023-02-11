"""
Este módulo contiene una serie de clases y funciones para implementar en FPGA y medir una matriz de osciladores de 
anillo, tanto estándar como de Galois.
"""

from os import environ as os_environ
from subprocess import run as subprocess_run
from pickle import dump as pickle_dump, load as pickle_load
from serial import Serial as serial_Serial
from time import sleep as time_sleep
from numpy import reshape as np_reshape, log2 as np_log2
from matplotlib.pyplot import plot as plt_plot, show as plt_show, imshow as plt_imshow
from fpga import pinta_progreso
from fpga.interfazpcps import *
from mytensor import *
        
        
def genOscCoord(lista_submatrices):
    """
    Esta función toma una lista de objetos 'OSC_SUBMATRIX' y genera una lista 'osc_coord' de dimensiones
    shape=(2, N_osc), tal que 'osc_coord[0][i]' contiene la coordenada X del i-ésimo oscilador, y 'osc_coord[1][i]'
    la coordenada Y del i-ésimo oscilador.
    """
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
    Función para calcular el numero de bits necesarios para especificar 'N' estados.
    """
    if N<=0:
        return 1
    else:
        if np_log2(N)>int(np_log2(N)):
            return int(np_log2(N))+1
        else:
            return int(np_log2(N))
            
            
def loadOscmatrix(file_name):
    """
    Esta función es un 'wrapper' para la función 'load' del módulo
    'pickle', que permite cargar un objeto guardado serializado (de
    cualquier clase, lo llamo 'Oscmatrix' por sentido semántico
    en este módulo).
    """
    with open(file_name, "rb") as f:
        result = pickle_load(f)
    return result       
            
            
class OSC_SUBMATRIX:
    """
    Este objeto contiene las variables necesarias para que la función 'genOscCoord' produzca una lista de
    localizaciones de osciladores para ser implementados en FPGA.
    """
    def __init__(self, x0=2, y0=1, x1=10, y1=10, dx=2, dy=1, N_osc=10, directriz='y'):
        """
        Esta función se llama automáticamente al crear un objeto de esta clase. Los parámetros de la función son:
        
        PARÁMETROS:
        
            x0 = 2
            
                Coordenada X del primer oscilador de la matriz.
                
                
            y0 = 1
            
                Coordenada Y del primer oscilador de la matriz.
                
             
            x1 = 10

                Coordenada X máxima de la matriz (no se sobrepasará).
                
                
            y1 = 10
            
                Coordenada Y máxima de la matriz (no se sobrepasará).
                
                
            dx = 2
            
                Incremento de la coordenada X.
                
                
            dy = 1
            
                Incremento de la coordenada Y.
                
                
            nosc = 10
            
                Número de osciladores del dominio.
             
             
            dir = 'y' | 'x'
             
                Dirección de crecimiento de la matriz (hasta llegar a la coordenada máxima).
        """
        self.x0 = x0
        self.y0 = y0
        self.x1 = x1
        self.y1 = y1
        self.dx = dx
        self.dy = dy
        self.N_osc = N_osc
        self.directriz = directriz
        
        
class ROMATRIX:
    """
    Objeto que contiene una matriz de osciladores de anillo estándar.
    """
    def __init__(self, N_inv=3, submatrices=OSC_SUBMATRIX()):
        """
        Esta función inicializa un objeto 'ROMATRIX'; se llama automáticamente al crear un nuevo objeto de esta clase.
        
        PARÁMETROS:
        -----------
        
            N_inv 3
            
                Número de inversores de cada oscilador.
            
            
            submatrices OSC_SUBMATRIX(x0=2, y0=1, x1=10, y1=10, dx=2, dy=1, N_osc=10, directriz='y')
            
                Osciladores que forman la matriz. Se construye como una lista de objetos OSC_SUBMATRIX.
                Si solo pasamos un dominio de osciladores podemos pasar un objeto OSC_SUBMATRIX, en lugar
                de una lista.
        """
        self.submatrices=[]
        if type(submatrices) == type([]) or type(submatrices) == type(()):
            self.submatrices[:]=submatrices[:]
        else:
            self.submatrices.append(submatrices)
        self.osc_coord = genOscCoord(self.submatrices)
        self.N_inv = N_inv
        self.N_osc = len(self.osc_coord[0])
        
    def help(self):
        """
        Esta función es un atajo para "help('fpga.oscmatrix.ROMATRIX')"
        """
        help('fpga.oscmatrix.ROMATRIX')
        
    def save(self, file_name):
        """
        Esta función es un 'wrapper' para guardar objetos serializados con el módulo 'pickle'.
        """
        with open(file_name, "wb") as f:
            pickle_dump(self, f)
        
    def plotSubmatrices(self):
        """
        Esta función pinta una representación esquemática de las submatrices introducidas durante la creación de un
        objeto. Se trata de una herramienta de depuración, y la disposicón geométrica que muestra no tiene por qué 
        corresponderse con la que tendrán las matrices de osciladores dispuestas sobre la FPGA.
        """
        max_x = max(self.osc_coord[0])
        min_x = min(self.osc_coord[0])
        max_y = max(self.osc_coord[1])
        min_y = min(self.osc_coord[1])
        mapa = [[0 for j in range(min_y,max_y+1,1)] for i in range(min_x,max_x+1,1)]
        for i,x in enumerate(range(min_x,max_x,1)):
            for j,y in enumerate(range(min_y,max_y,1)):
                if f"{x},{y}" in self.osc_coord:
                    mapa[i][j]=1
        plt_imshow(mapa, interpolation='none', origin='lower')
        plt_show()

    def genRomatrix(self, out_name="romatrix.v", debug=False):
        """
        Esta función genera un diseño 'out_name' en formato Verilog con la implementación de las submatrices de 
        osciladores introducidas durante la creación del objeto. El principal uso de esta función es dentro de la
        función 'implement()'.'
        """
        with open(out_name, "w") as f:
            bel_ocupacion = ["BEL=\"A6LUT\"","BEL=\"B6LUT\"","BEL=\"C6LUT\"","BEL=\"D6LUT\""]
            
            pinmap_proc = []
            intaux = 0
            for i in range(len(self.pinmap)):
                if self.pinmap[i]==';':
                    saux = self.pinmap[intaux:i]
                    intaux+=i+1
                    if saux == "no":
                        pinmap_proc.append(" ")
                    else:
                        pinmap_proc.append(f", LOCK_PINS=\"{saux}\" ")
            for i in range(len(pinmap_proc),self.N_inv,1):
                pinmap_proc.append(pinmap_proc[-1])
                
            f.write(f"//N_osciladores: {self.N_osc}\n\n")
            
            f.write("module ROMATRIX (\n")
            f.write("   input clock,\n")
            f.write("   input enable,\n")
            
            if self.N_osc > 1:
                f.write(f"  input[{clog2(self.N_osc)-1}:0] sel_ro,\n")
                
            if self.tipo_lut == "lut2":
                if self.minsel:
                    f.write(f"  input[0:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{self.N_inv-1}:0] sel_pdl,\n")
            elif self.tipo_lut == "lut3":
                if self.minsel:
                    f.write(f"  input[1:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{2*self.N_inv-1}:0] sel_pdl,\n")
            elif self.tipo_lut == "lut4":
                if self.minsel:
                    f.write(f"  input[2:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{3*self.N_inv-1}:0] sel_pdl,\n")
            elif self.tipo_lut == "lut5":
                if self.minsel:
                    f.write(f"  input[3:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{4*self.N_inv-1}:0] sel_pdl,\n")
            elif self.tipo_lut == "lut6":
                if self.minsel:
                    f.write(f"  input[4:0] sel_pdl,\n")
                else:
                    f.write(f"  input[{5*self.N_inv-1}:0] sel_pdl,\n")
            
            f.write("   output out\n")
            f.write("   );\n\n")
            f.write("   (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *)\n")
            f.write(f"  wire[{self.N_osc-1}:0] out_ro;\n")
            f.write(f"  reg[{self.N_osc-1}:0] enable_ro;\n")
            for i in range(self.N_osc):
                f.write(f"  wire[{self.N_inv-1}:0] w_{i};\n")
            f.write("\n")

            if self.N_osc>1:
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
                if self.tipo_lut == "lut1":
                    for i in range(self.N_osc):
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
                            if aux == self.N_inv-1:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT1 #(2'b01) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]));\n\n")
                                
                                ocupacion_celda+=1
                                
                                break
                                
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT1 #(2'b01) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]));\n")
                            
                            ocupacion_celda+=1
                            aux+=1
                            
                        f.write("\n")

                elif self.tipo_lut == "lut2":
                    for i in range(self.N_osc):
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
                            if aux == self.N_inv-1:
                                if not self.minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not self.minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT2 #(4'h5) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]));\n")
                                    
                            contador+=1
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")
                        
                elif self.tipo_lut == "lut3":
                    for i in range(self.N_osc):
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
                            if aux == self.N_inv-1:
                                if not self.minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not self.minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h55) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]));\n")
                                    
                            contador+=2
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                elif self.tipo_lut == "lut4":
                    for i in range(self.N_osc):
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
                            if aux == self.N_inv-1:
                                if not self.minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not self.minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]));\n")
                                    
                            contador+=3
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                elif self.tipo_lut == "lut5":
                    for i in range(self.N_osc):
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
                            if aux == self.N_inv-1:
                                if not self.minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]), .I4(sel_pdl[{contador+3}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not self.minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}, .I4(sel_pdl[{contador+3}])]));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h5555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]));\n")
                                    
                            contador+=4
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                elif self.tipo_lut == "lut6":
                    for i in range(self.N_osc):
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
                            if aux == self.N_inv-1:
                                if not self.minsel:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}]), .I4(sel_pdl[{contador+3}]), .I5(sel_pdl[{contador+4}]));\n\n")
                                else:
                                    f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(out_ro[{i}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]), .I5(sel_pdl[4]));\n\n")
                                    
                                ocupacion_celda+=1
                                
                                break
                                    
                            if not self.minsel:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[{contador}]), .I2(sel_pdl[{contador+1}]), .I3(sel_pdl[{contador+2}], .I4(sel_pdl[{contador+3}]), .I5(sel_pdl[{contador+4}])));\n")
                            else:
                                f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h55555) inv_{i}_{aux}(.O(w_{i}[{aux+1}]), .I0(w_{i}[{aux}]), .I1(sel_pdl[0]), .I2(sel_pdl[1]), .I3(sel_pdl[2]), .I4(sel_pdl[3]), .I5(sel_pdl[4]));\n")
                                    
                            contador+=5
                            ocupacion_celda+=1
                            aux+=1
                                    
                        f.write("\n")

                f.write("endmodule\n")
            
            else:
                for i in range(self.N_osc):
                    f.write(f"  CLOCK_DIVIDER cd_{i} (\n")
                    f.write("       .clock_in(clock),\n")
                    f.write(f"      .fdiv({i}),\n")
                    f.write(f"      .clock_out(out_ro[{i}])\n")
                    f.write("   );\n")
                    f.write("\n")
                f.write("endmodule\n")
        
    def implement(self, projname="project_romatrix", projdir="./", njobs=4, linux=False, debug=False, print_files=True, **kwargs):
        """
        Esta función copia/crea en el directorio 'projdir' todos los archivos necesarios para implementar una matriz
        de osciladores de anillo con medición de la frecuencia y comunicación:

        pc (.py) <-> microprocesador (.c) <-> FSM en FPGA (.v),

        PARÁMETROS:
        -----------
            
        projname = 'project_romatrix'

            Nombre del proyecto de Vivado.

        projdir = './'

            Directorio donde se creará el proyecto de Vivado y las fuentes.
            
            
        board = 'pynqz2' | 'zybo' | 'cmoda7_15t' | 'cmoda7_35t'

            Placa de desarrollo utilizada en el proyecyo.
            
            
        njobs = 4

            Número de núcleos que utiilizará Vivado paralelamente para la síntesis/implementación.
            
            
        qspi = False

            Si esta opción se introduce con valor True, el flujo de diseño incluirá el guardado del bitstream en la memoria
            flash de la placa para que se auto-programe al encenderse.
            
            
        detailr = False

            Si esta opción se introduce con valor True el flujo .tcl incluirá el cableado de los inversores después de la
            síntesis. Esto aumenta las probabilidades de que la herramienta haga un cableado idéntico, pero es recomendable
            comprobarlo. (NOTA: no tengo garantías de que esta opción sea del todo compatible con -qspi).
            
            
        tipo_lut = 'lut1' | 'lut2' | 'lut3' | 'lut4' | 'lut5' | 'lut6'
        
            Esta opción especifica el tipo de LUT que utilizar (y con ello el número de posibles PDL).
            
            
        pinmap = 'no;'

            Esta opción permite cambiar la asignación de puertos de cada LUT que componen los anillos. Esta opción debe darse
            entre conmillas, y las asignaciones de cada inveror separadas por punto y coma ';'. por otro lado, cada
            asignación debe hacerse respetando el lenguaje XDC: 'I0:Ax,I1:Ay,I2:Az,...'. Si se dan menos asignaciones que
            inversores componen cada anillo, los inversores restantes se fijarán igual que el último inversor especificado. 
            Alternativamente, puede darse como asignación la palabra 'no', en cuyo caso el correspondiente inversor se dejará
            sin fijar (a elección del software de diseño). Recordar que el numero de entradas 'Ix' depende del tipo de LUT 
            utilizado. La nomenclatura de los pines de cada LUT es A1,A2,A3,A4,A5,A6. El puerto 'I0' corresponde al
            propagador de las oscilaciones. El resto de puertos ('I1' a 'I5') se utilizan o no en función del tipo de LUT 
            (ver opción "tipo_lut"). Algunos ejemplos son:

            pinmap = 'no;'
                Esta es la opción por defecto, y deja que el software elija qué pines utilizar para cada inversor.
                
            pinmap = 'I0:A5;I0:A6;I0:A4'
                Con esta opción fijamos el pin propagador de la oscilación para tres inversores. Este iseño corresponde 
                al ruteado más compacto en una fpga Zynq7000.
            
            pinmap = 'I0:A5;no;I0:A4'
                Parecido al anterior, pero ahora no fijamos el segundo inversor.
            
            pinmap = 'I0:A1,I1:A2,I2:A3,I3:A4,I5:A6'
                Fijamos todos los inversores igual, utilizando el puerto A1 para propagar la señal y el resto para la 
                selección de PDL (en una lut tipo LUT6).
            
            
        minsel = False

            Si esta opción está presente se realizará una selección mínima (únicamente 5 bits), que comparten todos
            los inversores de cada oscilador. (NOTA: por ahora solo esta implementada en tipo=lut6).


        pblock =  False

            Si esta opción está presente, el script añade una restricción PBLOCK para el módulo TOP (i.e., todos los
            elementos auxiliares de la matriz de osciladores se colocarán dentro de este espacio). Esta opción debe ir
            acompañada de dos puntos que representan respectivamente las esquinas inferior izda. y superior dcha. del
            rectángulo de restricción, separados por un espacio:

                'X0,Y0 X1,Y1'
                
            El diseñador es responsable de que la fpga utilizada disponga de recursos suficientes en el bloque descrito.
            Esta opción debe darse entre comillas.


        data_width | dw = 32
        
            Esta opción especifica la anchura del canal de datos PS<-->PL.
            

        buffer_out_width | bow = 32
        
            Esta opción especifica la anchura de la palabra de respuesta (i.e., de la medida).
            
            
        linux = False
        
            Marcar esta opción como True si la versión de Vivado que se va a utilizar está instalado en una máquina
            linux (de forma que los PATH se gestionen correctamente).


        debug = False

            Si esta opción está presente, se implementará una matriz de divisores de reloj de frecuencia conocida, lo
            que permite depurar el diseño al conocer qué resultados deben salir.
        """
        if 'board' in kwargs:
            self.board = kwargs['board']
        else:
            self.board = 'pynqz2'
        
        if 'qspi' in kwargs:
            self.qspi = kwargs['qspi']
        else:
            self.qspi = False
        
        if 'detail_routing' in kwargs:
            self.detail_routing = kwargs['detail_routing']
        elif 'detailr' in kwargs:
            self.detail_routing = kwargs['detailr']         
        else:
            self.detail_routing = False
        
        if 'tipo_lut' in kwargs:
            self.tipo_lut = kwargs['tipo_lut']
        else:
            self.tipo_lut = 'lut1'
        
        if 'pinmap' in kwargs:
            self.pinmap = kwargs['pinmap']
        else:
            self.pinmap = 'no;'
        
        if 'minsel' in kwargs:
            self.minsel = kwargs['minsel']
        else:
            self.minsel = False
        
        if 'pblock' in kwargs:
            self.pblock = kwargs['pblock']
        else:
            self.pblock = False
        
        if 'data_width' in kwargs:
            self.data_width = kwargs['data_width']
        elif 'dw' in kwargs:
            self.data_width = kwargs['dw']            
        else:
            self.data_width = 32
        
        if 'buffer_out_width' in kwargs:
            self.buffer_out_width = kwargs['buffer_out_width']
        elif 'bow' in kwargs:
            self.buffer_out_width = kwargs['bow']            
        else:
            self.buffer_out_width = 32
            
        self.N_bits_osc = clog2(self.N_osc)
        
        self.N_bits_resol = 5

        if self.board=='cmoda7_15t':
            self.fpga_part="xc7a15tcpg236-1"
            self.board_part="digilentinc.com:cmod_a7-15t:part0:1.1"
            self.memory_part="mx25l3233"
            self.clk_name="/clk_wiz_1/clk_out1 (100 MHz)"
        elif self.board=='cmoda7_35t':
            self.fpga_part="xc7a35tcpg236-1"
            self.board_part="digilentinc.com:cmod_a7-35t:part0:1.1"
            self.memory_part="mx25l3233"
            self.clk_name="/clk_wiz_1/clk_out1 (100 MHz)"
        elif self.board=='zybo':
            self.fpga_part="xc7z010clg400-1"
            self.board_part="digilentinc.com:zybo:part0:1.0"
            self.memory_part="?"
            self.clk_name = "/processing_system7_0/FCLK_CLK0 (100 MHz)"
        elif self.board=='pynqz2':
            self.fpga_part="xc7z020clg400-1"
            self.board_part="tul.com.tw:pynq-z2:part0:1.0"
            self.memory_part="?"
            self.clk_name = "/processing_system7_0/FCLK_CLK0 (100 MHz)"
            
        if self.minsel:
            if self.tipo_lut == "lut1":
                self.N_bits_pdl = 0
            elif self.tipo_lut == "lut2":
                self.N_bits_pdl = 1
            elif self.tipo_lut == "lut3":
                self.N_bits_pdl = 2
            elif self.tipo_lut == "lut4":
                self.N_bits_pdl = 3
            elif self.tipo_lut == "lut5":
                self.N_bits_pdl = 4
            elif self.tipo_lut == "lut6":
                self.N_bits_pdl = 5
            else:
                print("ERROR: 'tipo_lut' introducido incorrecto\n")
        else:
            if self.tipo_lut == "lut1":
                self.N_bits_pdl = 0
            elif self.tipo_lut == "lut2":
                self.N_bits_pdl = self.N_inv
            elif self.tipo_lut == "lut3":
                self.N_bits_pdl = 2*self.N_inv
            elif self.tipo_lut == "lut4":
                self.N_bits_pdl = 3*self.N_inv
            elif self.tipo_lut == "lut5":
                self.N_bits_pdl = 4*self.N_inv
            elif self.tipo_lut == "lut6":
                self.N_bits_pdl = 5*self.N_inv
            else:
                print("ERROR: 'tipo_lut' introducido incorrecto\n")
                
        self.buffer_in_width = self.N_bits_osc+self.N_bits_pdl+self.N_bits_resol
        
        if print_files:
            if self.buffer_in_width%8 == 0:
                octeto_in_width=self.buffer_in_width//8
            else:
                octeto_in_width=self.buffer_in_width//8+1

            if self.buffer_out_width%8 == 0:
                octeto_out_width=self.buffer_out_width//8
            else:
                octeto_out_width=self.buffer_out_width//8+1

            if self.buffer_in_width%self.data_width == 0:
                words_in_width=self.buffer_in_width//self.data_width
            else:
                words_in_width=self.buffer_in_width//self.data_width+1

            if self.buffer_out_width%self.data_width == 0:
                words_out_width=self.buffer_out_width//self.data_width
            else:
                words_out_width=self.buffer_out_width//self.data_width+1

            dw_ge_biw = ""
            biw_aligned_dw = ""
            biw_misaligned_dw = ""

            if self.data_width >= self.buffer_in_width:
                dw_ge_biw='`define DW_GE_BIW'
            elif self.buffer_in_width%self.data_width == 0:
                biw_aligned_dw='`define BIW_ALIGNED_DW'
            else:
                biw_misaligned_dw='`define BIW_MISALIGNED_DW'

            dw_ge_bow = ""
            bow_aligned_dw = ""
            bow_misaligned_dw = ""

            if self.data_width >= self.buffer_out_width:
                dw_ge_bow='`define DW_GE_BOW'
            elif self.buffer_out_width%self.data_width == 0:
                bow_aligned_dw='`define BOW_ALIGNED_DW'
            else:
                bow_misaligned_dw='`define BOW_MISALIGNED_DW'
                
            ## projdir (directorio actual)
            if not linux:
                projdir = subprocess_run(["wslpath","-w",os_environ['PWD']], capture_output=True, text=True).stdout.replace("\\","/").replace("\n","")
            else:
                projdir=os_environ['PWD']

            ## block design
            subprocess_run(["mkdir","./block_design"])
            if self.board == "cmoda7_15t":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])

            elif self.board == "cmoda7_35t":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])

            elif self.board == "zybo":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_zybo.tcl","./block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_zybo.tcl","./block_design/bd_design_1.tcl"])

            elif self.board == "pynqz2":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_pynqz2.tcl","./block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_pynqz2.tcl","./block_design/bd_design_1.tcl"])


            ## partial flows (tcl)
            subprocess_run(["mkdir","./partial_flows"])

            vivado_files=f"{projdir}/vivado_src/top.v {projdir}/vivado_src/interfaz_pspl.cp.v {projdir}/vivado_src/romatrix.v {projdir}/vivado_src/medidor_frec.cp.v {projdir}/vivado_src/interfaz_pspl_config.vh"
            if debug:
                vivado_files+=f" {projdir}/vivado_src/clock_divider.cp.v"
            if self.pblock:
                vivado_files+=f" {projdir}/vivado_src/pblock.xdc"

            with open("./partial_flows/setupdesign.tcl", "w") as f:
                f.write(f"""
create_project {projname} {projdir}/{projname} -part {self.fpga_part}

set_property board_part {self.board_part} [current_project]

source {projdir}/block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

update_compile_order -fileset sources_1

add_files -norecurse {{ {vivado_files} }}

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

startgroup
set_property -dict [list CONFIG.C_GPIO_WIDTH {self.data_width} CONFIG.C_GPIO2_WIDTH {self.data_width}] [get_bd_cells axi_gpio_data]
endgroup

connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {{Clk {self.clk_name} }}  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files {projdir}/{projname}/{projname}.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse {projdir}/{projname}/{projname}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

set_property top design_1_wrapper [current_fileset]

update_compile_order -fileset sources_1

set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]
    """)

            with open("./partial_flows/genbitstream.tcl", "w") as f:
                f.write(f"""
if {{[file exists {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc
}}

if {{[file exists {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc
}}

if {{[file exists {projdir}/{projname}/{projname}.runs/synth_1]==1}} {{
    reset_run synth_1
}}

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

launch_runs synth_1 -jobs {njobs}

wait_on_run synth_1
    """)
                if self.qspi and self.board == "cmoda7_15t":
                    f.write(f"""
file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1

file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new

close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]

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
                elif self.qspi and self.board == "cmoda7_35t":
                    f.write(f"""
file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1

file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new

close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]

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

                if self.detail_routing:
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
                        for j in range(1,self.N_inv,1):
                            f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j}]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j} }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
    """) 
                    f.write(f"""
endgroup

file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new

close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc w ]

add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc

set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc [current_fileset -constrset]

save_constraints -force

close_design

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0
    """) # Aqui termina detailed routing

                f.write(f"""
if {{[file exists {projdir}/{projname}/{projname}.runs/synth_1/__synthesis_is_complete__]==1}} {{
    reset_run synth_1
}}

launch_runs impl_1 -to_step write_bitstream -jobs {njobs}

wait_on_run impl_1
    """)

            with open("./partial_flows/launchsdk.tcl", "w") as f:
                f.write(f"""
file mkdir {projdir}/{projname}/{projname}.sdk

file copy -force {projdir}/{projname}/{projname}.runs/impl_1/design_1_wrapper.sysdef {projdir}/{projname}/{projname}.sdk/design_1_wrapper.hdf

launch_sdk -workspace {projdir}/{projname}/{projname}.sdk -hwspec {projdir}/{projname}/{projname}.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform
    """)

            with open("mkhwdplatform.tcl", "w") as f:
                f.write(f"""
source {projdir}/partial_flows/setupdesign.tcl

source {projdir}/partial_flows/genbitstream.tcl

source {projdir}/partial_flows/launchsdk.tcl
    """)


            ## vivado sources
            subprocess_run(["mkdir","./vivado_src"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/interfaz_pspl.v","./vivado_src/interfaz_pspl.cp.v"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/medidor_frec.v","./vivado_src/medidor_frec.cp.v"])
            if debug:
                subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/clock_divider.v","./vivado_src/clock_divider.cp.v"])

            with open("vivado_src/interfaz_pspl_config.vh", "w") as f:
                f.write(f"""{dw_ge_biw}
{biw_aligned_dw}
{biw_misaligned_dw}
{dw_ge_bow}
{bow_aligned_dw}
{bow_misaligned_dw}
            """)
            
            self.genRomatrix(out_name="vivado_src/romatrix.v", debug=debug)

            with open("vivado_src/top.v", "w") as f:
                if self.N_osc==1:
                    aux=""
                else:
                    aux=f".sel_ro(buffer_in[{self.N_bits_osc-1}:0]),"
                if self.tipo_lut == "lut1":
                    aux1=""
                else:
                    aux1=f".sel_pdl(buffer_in[{self.buffer_in_width-5-1}:{self.N_bits_osc}]),"

                f.write(f"""`timescale 1ns / 1ps


module TOP (
    input           clock,
    input[7:0]      ctrl_in,
    output[7:0]     ctrl_out,
    input[{self.data_width-1}:0] data_in,
    output[{self.data_width-1}:0]    data_out
);

    wire[{self.buffer_in_width-1}:0] buffer_in;
    wire[{self.buffer_out_width-1}:0] buffer_out;
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
        .DATA_WIDTH({self.data_width}),
        .BUFFER_IN_WIDTH({self.buffer_in_width}),
        .BUFFER_OUT_WIDTH({self.buffer_out_width})
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
        .OUT_WIDTH({self.buffer_out_width})
    ) medidor_frec (
        .clock(clock),
        .enable(enable_medidor),
        .clock_u(out_ro),
        .resol(buffer_in[{self.buffer_in_width-1}:{self.buffer_in_width-5}]),
        .lock(ack),
        .out(buffer_out)
    );

endmodule
    """)


            if self.pblock:
                PBLOCK_CORNERS=self.pblock.split()
                PBLOCK_CORNER_0=PBLOCK_CORNERS[0].split(',')
                PBLOCK_CORNER_1=PBLOCK_CORNERS[1].split(',')
                with open("./vivado_src/pblock.xdc", "w") as f:
                    f.write(f"""create_pblock pblock_TOP_0
add_cells_to_pblock [get_pblocks pblock_TOP_0] [get_cells -quiet [list design_1_i/TOP_0]]
resize_pblock [get_pblocks pblock_TOP_0] -add {{SLICE_X{PBLOCK_CORNER_0[0]}Y{PBLOCK_CORNER_0[1]}:SLICE_X{PBLOCK_CORNER_1[0]}Y{PBLOCK_CORNER_1[1]}}}
            """)


            ## sdk sources
            subprocess_run(["mkdir","./sdk_src"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/c-xilinx/sdk/interfaz_pcps-pspl.c","./sdk_src/interfaz_pcps-pspl.cp.c"])

            with open("./sdk_src/interfaz_pcps-pspl_config.h", "w") as f:
                if self.board == "cmoda7_15t" or self.board == "cmoda7_35t":
                    f.write("#include \"xuartlite.h\"\n\n")
                elif self.board == "zybo" or self.board == "pynqz2":
                    f.write("#include \"xuartps.h\"\n\n")

                f.write(f"""#define DATA_WIDTH          {self.data_width}
#define BUFFER_IN_WIDTH     {self.buffer_in_width}
#define BUFFER_OUT_WIDTH    {self.buffer_out_width}
#define OCTETO_IN_WIDTH     {octeto_in_width}
#define OCTETO_OUT_WIDTH    {octeto_out_width}
#define WORDS_IN_WIDTH      {words_in_width}
#define WORDS_OUT_WIDTH     {words_out_width}
    """)

            ## log info
            print(f"""
N_osc = {self.N_osc}
N_pdl = {2**self.N_bits_pdl}

(0) <---sel_ro({self.N_bits_osc})---><---sel_pdl({self.N_bits_pdl})---><---sel_resol(5)---> ({self.buffer_in_width-1})

fpga part: {self.fpga_part}

sdk source files: {projdir}/sdk_src/
    """)
            if self.qspi:
                print(f"q-spi part: {self.memory_part}")

    def medir(self, resol=16, osc=range(1), pdl=range(1), N_rep=1, f_ref=False, log=False, verbose=True, puerto='S1', baudrate=9600):
        """
         Esta función mide la frecuencia de una matriz de osciladores estándar 'ROMATRIX', una vez este ha sido implementado en FPGA.
         El resultado se devuelve como un objeto TENSOR.
         
         PARÁMETROS:
         -----------
         
            resol = 16
            
                log_2 del número de ciclos de referencia a completar para dar por terminada la medida.
            
            
            osc = range(1)
            
                Lista de osciladores a medir.
                
                
            pdl = range(1)
            
                Lista de PDL a medir.
                
                
            N_rep = 1
            
                Número d erepeticiones a medir.
                
                
            f_ref = False
            
                Frecuencia del reloj de referencia. Si se proporciona este valor, el resultado obtenido se devuelve
                en las mismas unidades en que se haya pasado este valor "f_ref".
                
                
            log = False
            
                Si se pasa "True" se escriben algunos datos a modo de log.
                
                
            verbose = True
            
                Si se pasa "True" se pinta una barra de progreso de la medida. Desactivar esta opción ("False") hace
                más cómodo utilizar esta función en un bucle.
                
                
            puerto = S1
            
                Esta opción especifica el puerto serie al que se conecta la FPGA. Actualmente está "hardcodeado" a 
                formato linux (ya que incluso en Windows lo estoy midiendo con WSL).
                
                
            baudrate = 9600
            
                Tasa de transferencia del protocolo serie UART PC<-->PS. Debe concordar con el programa compilador en
                PS.
        """
        
        osc_list=list(osc)
        pdl_list=list(pdl)
        N_osc = len(osc_list) # Número de osciladores a medir
        N_pdl = len(pdl_list) # Número de pdl a medir
        
        buffer_sel_ro=[]
        for i in osc_list:
            buffer_sel_ro.append(resizeArray(intToBitstr(i), self.N_bits_osc))
            
        buffer_sel_pdl=[]
        for i in pdl_list:
            buffer_sel_pdl.append(resizeArray(intToBitstr(i), self.N_bits_pdl))
            
        buffer_sel_resol=resizeArray(intToBitstr(resol), self.N_bits_resol)

        ## Info log
        if log:
            print(f"""
 INFO LOG:

 Resolución             = {resol}
 Número de osciladores  = {N_osc}
 Número de repeticiones = {N_rep}
 Número de PDL          = {N_pdl}
""")
            
        fpga = serial_Serial(port='/dev/tty'+puerto, baudrate=baudrate, bytesize=8)
        time_sleep(.1)
        
        if verbose:
            contador=0
            N_total = N_osc*N_rep*N_pdl
            pinta_progreso(0, N_total, barra=40)
        medidas=[]
        for rep in range(N_rep):
            for pdl in range(N_pdl):
                for osc in range(N_osc):
                    buffer_in = buffer_sel_ro[osc]+buffer_sel_pdl[pdl]+buffer_sel_resol
                    scan(fpga, buffer_in, self.buffer_in_width)
                    medida = bitstrToInt(calc(fpga, self.buffer_out_width))
                    if f_ref:
                        medida*=f_ref/2**resol
                    medidas.append(medida)
                if verbose:
                    contador+=N_osc
                    pinta_progreso(contador, N_total, barra=40)
        if verbose:
            pinta_progreso(N_total, N_total, barra=40)
        fpga.close()
        tensor_medidas = TENSOR(array=np_reshape(medidas, (N_rep,N_pdl,N_osc)), axis=['rep','pdl','osc'])
        
        return tensor_medidas
        
        
class GAROMATRIX:
    """
    Objeto que contiene una matriz de osciladores de anillo de Galois.
    """
    def __init__(self, N_inv=3, submatrices=OSC_SUBMATRIX()):
        """
        Esta función inicializa un objeto 'GAROMATRIX'; se llama automáticamente al crear un nuevo objeto de esta clase.
        
        PARÁMETROS:
        -----------
        
            N_inv 3
            
                Número de inversores de cada oscilador.
            
            
            submatrices OSC_SUBMATRIX(x0=2, y0=1, x1=10, y1=10, dx=2, dy=1, N_osc=10, directriz='y')
            
                Osciladores que forman la matriz. Se construye como una lista de objetos OSC_SUBMATRIX.
                Si solo pasamos un dominio de osciladores podemos pasar un objeto OSC_SUBMATRIX, en lugar
                de una lista.
        """
        self.submatrices=[]
        if type(submatrices) == type([]) or type(submatrices) == type(()):
            self.submatrices[:]=submatrices[:]
        else:
            self.submatrices.append(submatrices)
        self.osc_coord = genOscCoord(self.submatrices)
        self.N_inv = N_inv
        self.N_osc = len(self.osc_coord[0])
        
    def help(self):
        """
        Esta función es un atajo para "help('fpga.oscmatrix.GAROMATRIX')"
        """
        help('fpga.oscmatrix.GAROMATRIX')
        
    def save(self, file_name):
        """
        Esta función es un 'wrapper' para guardar objetos serializados con el módulo 'pickle'.
        """
        with open(file_name, "wb") as f:
            pickle_dump(self, f)
        
    def plotSubmatrices(self):
        """
        Esta función pinta una representación esquemática de las submatrices introducidas durante la creación de un
        objeto. Se trata de una herramienta de depuración, y la disposicón geométrica que muestra no tiene por qué 
        corresponderse con la que tendrán las matrices de osciladores dispuestas sobre la FPGA.
        """
        max_x = max(self.osc_coord[0])
        min_x = min(self.osc_coord[0])
        max_y = max(self.osc_coord[1])
        min_y = min(self.osc_coord[1])
        mapa = [[0 for j in range(min_y,max_y+1,1)] for i in range(min_x,max_x+1,1)]
        for i,x in enumerate(range(min_x,max_x,1)):
            for j,y in enumerate(range(min_y,max_y,1)):
                if f"{x},{y}" in self.osc_coord:
                    mapa[i][j]=1
        plt_imshow(mapa, interpolation='none', origin='lower')
        plt_show()

    def genGaromatrix(self, out_name="garomatrix.v"):
        """
        Esta función genera un diseño "out_name" en formato Verilog con la implementación de las submatrices de 
        osciladores introducidas durante la creación del objeto. El principal uso de esta función es dentro de la
        función "implement()".
        """
        with open(out_name, "w") as f:
            bel_ocupacion = ["BEL=\"A6LUT\"","BEL=\"B6LUT\"","BEL=\"C6LUT\"","BEL=\"D6LUT\""]
            
            pinmap_proc = []
            intaux = 0
            for i in range(len(self.pinmap)):
                if self.pinmap[i]==';':
                    saux = self.pinmap[intaux:i]
                    intaux+=i+1
                    if saux == "no":
                        pinmap_proc.append(" ")
                    else:
                        pinmap_proc.append(f", LOCK_PINS=\"{saux}\" ")
            for i in range(len(pinmap_proc),self.N_inv,1):
                pinmap_proc.append(pinmap_proc[-1])
                
            f.write(f"//N_osciladores de Galois: {self.N_osc}\n\n")
            
            f.write("module GAROMATRIX (\n")
            f.write("   input clock,\n")
            f.write("   input enable,\n")
            f.write("   input clock_s,\n")
            f.write(f"  input[{self.N_inv-2}:0] sel_poly,\n")
            
            if self.N_osc > 1:
                f.write(f"  input[{clog2(self.N_osc)-1}:0] sel_ro,\n")
                
            if self.tipo_lut == "lut4":
                if not self.minsel:
                    f.write(f"  input[{self.N_inv-1}:0] sel_pdl,\n")
                else:
                    f.write(f"  input[0:0] sel_pdl,\n")
            elif self.tipo_lut == "lut5":
                if not self.minsel:
                    f.write(f"  input[{2*self.N_inv-1}:0] sel_pdl,\n")
                else:
                    f.write(f"  input[1:0] sel_pdl,\n")
            elif self.tipo_lut == "lut6":
                if not self.minsel:
                    f.write(f"  input[{3*self.N_inv-1}:0] sel_pdl,\n")
                else:
                    f.write(f"  input[2:0] sel_pdl,\n")
                    
            f.write("   output out\n")
            f.write("   );\n\n")
            f.write(f"  wire[{self.N_osc-1}:0] out_ro;\n")
            f.write(f"  wire[{self.N_osc-1}:0] out_ro_sampled;\n")
            f.write("   (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *)\n")
            f.write(f"  wire[{self.N_inv-1}:0]\n")
            
            for i in range(1, self.N_osc, 1):
                f.write(f"      w_{i-1},\n")
            f.write(f"      w_{self.N_osc-1};\n")
            f.write("\n")

            if self.N_osc>1:
                f.write("   assign out = enable? out_ro_sampled[sel_ro] : clock;\n")
            else:
                f.write("   assign out = enable? out_ro_sampled[0] : clock;\n")
            f.write("\n\n")
            
            if self.tipo_lut == "lut3":
                for i in range(self.N_osc):
                    celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                    f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[0]}*) LUT3 #(8'h95) inv_{i}_0(.O(w_{i}[{0}]), .I0(w_{i}[{self.N_inv-1}]), .I1(1'b0), .I2(1'b0));\n")
                    
                    aux=1
                    ocupacion_celda=1
                    while True:
                        if ocupacion_celda==4:
                            if celda[0]%2 == 0:
                                celda[0]+=1
                            else:
                                celda[1]+=1
                            ocupacion_celda=0
                        if aux == self.N_inv:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) invout_{i}(.O(out_ro[{i}]), .I0(w_{i}[{self.N_inv-1}]));\n") # INV final
                            f.write(f"  (* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
                            
                            ocupacion_celda+=1
                            
                            break
                            
                        f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT3 #(8'h95) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{self.N_inv-1}]), .I2(sel_poly[{aux-1}]));\n")
                        
                        ocupacion_celda+=1
                        aux+=1
                    f.write("\n")

            elif self.tipo_lut == "lut4":
                for i in range(self.N_osc):
                    celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                    f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[0]}*) LUT4 #(16'h9595) inv_{i}_{0}(.O(w_{i}[{0}]), .I0(w_{i}[{self.N_inv-1}]), .I1(1'b0), .I2(1'b0), .I3(sel_pdl[0]));\n")
                    
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
                        if aux == self.N_inv:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) invout_{i}(.O(out_ro[{i}]), .I0(w_{i}[{self.N_inv-1}]));\n") # INV final
                            f.write(f"  (* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
                            
                            ocupacion_celda+=1
                            
                            break
                                
                        if not self.minsel:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h9595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{self.N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[{contador}]));\n")
                        else:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT4 #(16'h9595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{self.N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[0]));\n")
                                
                        contador+=1
                        ocupacion_celda+=1
                        aux+=1
                    f.write("\n")

            elif self.tipo_lut == "lut5":
                for i in range(self.N_osc):
                    celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                    f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[0]}*) LUT5 #(32'h95959595) inv_{i}_{0}(.O(w_{i}[{0}]), .I0(w_{i}[{self.N_inv-1}]), .I1(1'b0), .I2(1'b0), .I3(sel_pdl[0]), .I4(sel_pdl[1]));\n")
                    
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
                        if aux == self.N_inv:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) invout_{i}(.O(out_ro[{i}]), .I0(w_{i}[{self.N_inv-1}]));\n") # INV final
                            f.write(f"  (* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
                            
                            ocupacion_celda+=1
                            
                            break
                            
                        if not self.minsel:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h95959595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{self.N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[{contador}]), .I4(sel_pdl[{contador+1}]));\n")
                        else:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT5 #(32'h95959595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{self.N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[0]), .I4(sel_pdl[1]));\n")
                            
                        contador+=2
                        ocupacion_celda+=1
                        aux+=1
                    f.write("\n")

            elif self.tipo_lut == "lut6":
                for i in range(self.N_osc):
                    celda=[self.osc_coord[0][i],self.osc_coord[1][i]]
                    f.write(f"  (* {bel_ocupacion[0]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[0]}*) LUT6 #(64'h9595959595959595) inv_{i}_{0}(.O(w_{i}[{0}]), .I0(w_{i}[{self.N_inv-1}]), .I1(1'b0), .I2(1'b0), .I3(sel_pdl[0]), .I4(sel_pdl[1]), .I5(sel_pdl[2]));\n")
                    
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
                        if aux == self.N_inv:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) LUT1 #(2'b01) invout_{i}(.O(out_ro[{i}]), .I0(w_{i}[{self.N_inv-1}]));\n") # INV final
                            f.write(f"  (* BEL=\"DFF\", LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\" *) FDCE ff_{i}(.Q(out_ro_sampled[{i}]), .C(clock_s), .CE(1'b1), .CLR(1'b0), .D(out_ro[{i}]));\n\n")
                            
                            ocupacion_celda+=1
                            
                            break
                            
                        if not self.minsel:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h9595959595959595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{self.N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[{contador}]), .I4(sel_pdl[{contador+1}]), .I5(sel_pdl[{contador+2}]));\n")
                        else:
                            f.write(f"  (* {bel_ocupacion[ocupacion_celda]}, LOC=\"SLICE_X{celda[0]}Y{celda[1]}\", DONT_TOUCH=\"true\"{pinmap_proc[aux]}*) LUT6 #(64'h9595959595959595) inv_{i}_{aux}(.O(w_{i}[{aux}]), .I0(w_{i}[{aux-1}]), .I1(w_{i}[{self.N_inv-1}]), .I2(sel_poly[{aux-1}]), .I3(sel_pdl[0]), .I4(sel_pdl[1]), .I5(sel_pdl[2]));\n")
                            
                        contador+=3
                        ocupacion_celda+=1
                        aux+=1
                    f.write("\n")
                    
            f.write("endmodule\n")
        
    def implement(self, projname="project_garomatrix", projdir="./", njobs=4, linux=False, print_files=True, **kwargs):
        """
        Esta función copia/crea en el directorio 'projdir' todos los archivos necesarios para implementar una matriz
        de osciladores de anillo de Galois con medición del sesgo ("bias") y comunicación:

        pc (.py) <-> microprocesador (.c) <-> FSM en FPGA (.v),

        PARÁMETROS:
        -----------
            
        projname = 'project_romatrix'

            Nombre del proyecto de Vivado.

        projdir = './'

            Directorio donde se creará el proyecto de Vivado y las fuentes.
            
            
        board = 'pynqz2' | 'zybo' | 'cmoda7_15t' | 'cmoda7_35t'

            Placa de desarrollo utilizada en el proyecyo.
            
            
        njobs = 4

            Número de núcleos que utiilizará Vivado paralelamente para la síntesis/implementación.
            
            
        qspi = False

            Si esta opción se introduce con valor True, el flujo de diseño incluirá el guardado del bitstream en la memoria
            flash de la placa para que se auto-programe al encenderse.
            
            
        detailr = False

            Si esta opción se introduce con valor True el flujo .tcl incluirá el cableado de los inversores después de la
            síntesis. Esto aumenta las probabilidades de que la herramienta haga un cableado idéntico, pero es recomendable
            comprobarlo. (NOTA: no tengo garantías de que esta opción sea del todo compatible con -qspi).
            
            
        tipo_lut = 'lut3' | 'lut4' | 'lut5' | 'lut6'
        
            Esta opción especifica el tipo de LUT que utilizar (y con ello el número de posibles PDL).
            
            
        pinmap = 'no;'

            Esta opción permite cambiar la asignación de puertos de cada LUT que componen los anillos. Esta opción debe darse
            entre conmillas, y las asignaciones de cada inveror separadas por punto y coma ';'. por otro lado, cada
            asignación debe hacerse respetando el lenguaje XDC: 'I0:Ax,I1:Ay,I2:Az,...'. Si se dan menos asignaciones que
            inversores componen cada anillo, los inversores restantes se fijarán igual que el último inversor especificado. 
            Alternativamente, puede darse como asignación la palabra 'no', en cuyo caso el correspondiente inversor se dejará
            sin fijar (a elección del software de diseño). Recordar que el numero de entradas 'Ix' depende del tipo de LUT 
            utilizado. La nomenclatura de los pines de cada LUT es A1,A2,A3,A4,A5,A6. El puerto 'I0' corresponde al
            propagador de las oscilaciones. El resto de puertos ('I1' a 'I5') se utilizan o no en función del tipo de LUT 
            (ver opción "tipo_lut"). Algunos ejemplos son:

            pinmap = 'no;'
                Esta es la opción por defecto, y deja que el software elija qué pines utilizar para cada inversor.
                
            pinmap = 'I0:A5;I0:A6;I0:A4'
                Con esta opción fijamos el pin propagador de la oscilación para tres inversores. Este iseño corresponde 
                al ruteado más compacto en una fpga Zynq7000.
            
            pinmap = 'I0:A5;no;I0:A4'
                Parecido al anterior, pero ahora no fijamos el segundo inversor.
            
            pinmap = 'I0:A1,I1:A2,I2:A3,I3:A4,I5:A6'
                Fijamos todos los inversores igual, utilizando el puerto A1 para propagar la señal y el resto para la 
                selección de PDL (en una lut tipo LUT6).
            
            
        minsel = False

            Si esta opción está presente se realizará una selección mínima (únicamente 5 bits), que comparten todos
            los inversores de cada oscilador. (NOTA: por ahora solo esta implementada en tipo=lut6).


        pblock =  False

            Si esta opción está presente, el script añade una restricción PBLOCK para el módulo TOP (i.e., todos los
            elementos auxiliares de la matriz de osciladores se colocarán dentro de este espacio). Esta opción debe ir
            acompañada de dos puntos que representan respectivamente las esquinas inferior izda. y superior dcha. del
            rectángulo de restricción, separados por un espacio:

                'X0,Y0 X1,Y1'
                
            El diseñador es responsable de que la fpga utilizada disponga de recursos suficientes en el bloque descrito.
            Esta opción debe darse entre comillas.


        data_width | dw = 32
        
            Esta opción especifica la anchura del canal de datos PS<-->PL.
            

        buffer_out_width | bow = 32
        
            Esta opción especifica la anchura de la palabra de respuesta (i.e., de la medida).
            
            
        linux = False
        
            Marcar esta opción como True si la versión de Vivado que se va a utilizar está instalado en una máquina
            linux (de forma que los PATH se gestionen correctamente).
        """

        if 'board' in kwargs:
            self.board = kwargs['board']
        else:
            self.board = 'pynqz2'
        
        if 'qspi' in kwargs:
            self.qspi = kwargs['qspi']
        else:
            self.qspi = False
        
        if 'detail_routing' in kwargs:
            self.detail_routing = kwargs['detail_routing']
        elif 'detailr' in kwargs:
            self.detail_routing = kwargs['detailr']         
        else:
            self.detail_routing = False
        
        if 'tipo_lut' in kwargs:
            self.tipo_lut = kwargs['tipo_lut']
        else:
            self.tipo_lut = 'lut3'
        
        if 'pinmap' in kwargs:
            self.pinmap = kwargs['pinmap']
        else:
            self.pinmap = 'no;'
        
        if 'minsel' in kwargs:
            self.minsel = kwargs['minsel']
        else:
            self.minsel = False
        
        if 'pblock' in kwargs:
            self.pblock = kwargs['pblock']
        else:
            self.pblock = False
        
        if 'data_width' in kwargs:
            self.data_width = kwargs['data_width']
        elif 'dw' in kwargs:
            self.data_width = kwargs['dw']            
        else:
            self.data_width = 32
        
        if 'buffer_out_width' in kwargs:
            self.buffer_out_width = kwargs['buffer_out_width']
        elif 'bow' in kwargs:
            self.buffer_out_width = kwargs['bow']            
        else:
            self.buffer_out_width = 32
            
        self.N_bits_osc = clog2(self.N_osc)
        
        self.N_bits_poly = self.N_inv-1 # El primer inversor no se puede modificar (es una puerta NOT).
        
        self.N_bits_resol = 5  

        self.N_bits_fdiv = 5
        
        if self.board=='cmoda7_15t':
            self.fpga_part="xc7a15tcpg236-1"
            self.board_part="digilentinc.com:cmod_a7-15t:part0:1.1"
            self.memory_part="mx25l3233"
            self.clk_name="/clk_wiz_1/clk_out1 (100 MHz)"
        elif self.board=='cmoda7_35t':
            self.fpga_part="xc7a35tcpg236-1"
            self.board_part="digilentinc.com:cmod_a7-35t:part0:1.1"
            self.memory_part="mx25l3233"
            self.clk_name="/clk_wiz_1/clk_out1 (100 MHz)"
        elif self.board=='zybo':
            self.fpga_part="xc7z010clg400-1"
            self.board_part="digilentinc.com:zybo:part0:1.0"
            self.memory_part="?"
            self.clk_name = "/processing_system7_0/FCLK_CLK0 (100 MHz)"
        elif self.board=='pynqz2':
            self.fpga_part="xc7z020clg400-1"
            self.board_part="tul.com.tw:pynq-z2:part0:1.0"
            self.memory_part="?"
            self.clk_name = "/processing_system7_0/FCLK_CLK0 (100 MHz)"
        
        if self.minsel:
            if self.tipo_lut == "lut3":
                self.N_bits_pdl = 0
            elif self.tipo_lut == "lut4":
                self.N_bits_pdl = 1
            elif self.tipo_lut == "lut5":
                self.N_bits_pdl = 2
            elif self.tipo_lut == "lut6":
                self.N_bits_pdl = 3
            else:
                print("ERROR: 'tipo_lut' introducido incorrecto\n")
        else:
            if self.tipo_lut == "lut3":
                self.N_bits_pdl = 0
            elif self.tipo_lut == "lut4":
                self.N_bits_pdl = self.N_inv
            elif self.tipo_lut == "lut5":
                self.N_bits_pdl = 2*self.N_inv
            elif self.tipo_lut == "lut6":
                self.N_bits_pdl = 3*self.N_inv
            else:
                print("ERROR: 'tipo_lut' introducido incorrecto\n")
                
        self.buffer_in_width = self.N_bits_osc+self.N_bits_pdl+self.N_bits_poly+self.N_bits_resol+self.N_bits_fdiv
        
        if print_files:
            if self.buffer_in_width%8 == 0:
                octeto_in_width=self.buffer_in_width//8
            else:
                octeto_in_width=self.buffer_in_width//8+1

            if self.buffer_out_width%8 == 0:
                octeto_out_width=self.buffer_out_width//8
            else:
                octeto_out_width=self.buffer_out_width//8+1

            if self.buffer_in_width%self.data_width == 0:
                words_in_width=self.buffer_in_width//self.data_width
            else:
                words_in_width=self.buffer_in_width//self.data_width+1

            if self.buffer_out_width%self.data_width == 0:
                words_out_width=self.buffer_out_width//self.data_width
            else:
                words_out_width=self.buffer_out_width//self.data_width+1

            dw_ge_biw = ""
            biw_aligned_dw = ""
            biw_misaligned_dw = ""

            if self.data_width >= self.buffer_in_width:
                dw_ge_biw='`define DW_GE_BIW'
            elif self.buffer_in_width%self.data_width == 0:
                biw_aligned_dw='`define BIW_ALIGNED_DW'
            else:
                biw_misaligned_dw='`define BIW_MISALIGNED_DW'

            dw_ge_bow = ""
            bow_aligned_dw = ""
            bow_misaligned_dw = ""

            if self.data_width >= self.buffer_out_width:
                dw_ge_bow='`define DW_GE_BOW'
            elif self.buffer_out_width%self.data_width == 0:
                bow_aligned_dw='`define BOW_ALIGNED_DW'
            else:
                bow_misaligned_dw='`define BOW_MISALIGNED_DW'
                
            ## projdir (directorio actual)
            if not linux:
                projdir = subprocess_run(["wslpath","-w",os_environ['PWD']], capture_output=True, text=True).stdout.replace("\\","/").replace("\n","")
            else:
                projdir=os_environ['PWD']


            ## block design
            subprocess_run(["mkdir","./block_design"])
            if self.board == "cmoda7_15t":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])

            elif self.board == "cmoda7_35t":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])

            elif self.board == "zybo":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_zybo.tcl","./block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_zybo.tcl","./block_design/bd_design_1.tcl"])

            elif self.board == "pynqz2":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_pynqz2.tcl","./block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_pynqz2.tcl","./block_design/bd_design_1.tcl"])


            ## partial flows (tcl)
            subprocess_run(["mkdir","./partial_flows"])

            vivado_files=f"{projdir}/vivado_src/top.v {projdir}/vivado_src/interfaz_pspl.cp.v {projdir}/vivado_src/garomatrix.v {projdir}/vivado_src/medidor_bias.cp.v {projdir}/vivado_src/clock_divider.cp.v {projdir}/vivado_src/interfaz_pspl_config.vh"
            if self.pblock:
                vivado_files+=f" {projdir}/vivado_src/pblock.xdc"

            with open("./partial_flows/setupdesign.tcl", "w") as f:
                f.write(f"""
create_project {projname} {projdir}/{projname} -part {self.fpga_part}

set_property board_part {self.board_part} [current_project]

source {projdir}/block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

update_compile_order -fileset sources_1

add_files -norecurse {{ {vivado_files} }}

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

startgroup
set_property -dict [list CONFIG.C_GPIO_WIDTH {self.data_width} CONFIG.C_GPIO2_WIDTH {self.data_width}] [get_bd_cells axi_gpio_data]
endgroup

connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {{Clk {self.clk_name} }}  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files {projdir}/{projname}/{projname}.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse {projdir}/{projname}/{projname}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

set_property top design_1_wrapper [current_fileset]

update_compile_order -fileset sources_1

set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]
            """)

            with open("./partial_flows/genbitstream.tcl", "w") as f:
                f.write(f"""
if {{[file exists {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc
}}

if {{[file exists {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc
}}

if {{[file exists {projdir}/{projname}/{projname}.runs/synth_1]==1}} {{
    reset_run synth_1
}}

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

launch_runs synth_1 -jobs {njobs}

wait_on_run synth_1
            """)
                if self.qspi == 1 and self.board == "cmoda7_15t":
                    f.write(f"""
file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1

file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new

close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]

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
                elif self.qspi == 1 and self.board == "cmoda7_35t":
                    f.write(f"""
file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1

file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new

close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]

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

                if self.detail_routing:
                    f.write("""
open_run synth_1 -name synth_1

startgroup
            """)
                    for i in range(self.N_osc):
                        f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_0]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_0 }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_{i} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_{i} }}]
            """)
                        for j in range(1,self.N_inv,1):
                            f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j}]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j} }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
            """) 
                    f.write(f"""
endgroup

file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new

close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc w ]

add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc

set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/detail_routing.xdc [current_fileset -constrset]

save_constraints -force

close_design

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0
            """) # Aqui termina detailed routing

                f.write(f"""
if {{[file exists {projdir}/{projname}/{projname}.runs/synth_1/__synthesis_is_complete__]==1}} {{
    reset_run synth_1
}}
    
launch_runs impl_1 -to_step write_bitstream -jobs {njobs}

wait_on_run impl_1
            """)

            with open("./partial_flows/launchsdk.tcl", "w") as f:
                f.write(f"""
file mkdir {projdir}/{projname}/{projname}.sdk

file copy -force {projdir}/{projname}/{projname}.runs/impl_1/design_1_wrapper.sysdef {projdir}/{projname}/{projname}.sdk/design_1_wrapper.hdf

launch_sdk -workspace {projdir}/{projname}/{projname}.sdk -hwspec {projdir}/{projname}/{projname}.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform
            """)

            with open("mkhwdplatform.tcl", "w") as f:
                f.write(f"""
source {projdir}/partial_flows/setupdesign.tcl

source {projdir}/partial_flows/genbitstream.tcl

source {projdir}/partial_flows/launchsdk.tcl
            """)


            ## vivado sources
            subprocess_run(["mkdir","./vivado_src"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/interfaz_pspl.v","./vivado_src/interfaz_pspl.cp.v"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/medidor_bias.v","./vivado_src/medidor_bias.cp.v"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/clock_divider.v","./vivado_src/clock_divider.cp.v"])

            with open("vivado_src/interfaz_pspl_config.vh", "w") as f:
                f.write(f"""{dw_ge_biw}
{biw_aligned_dw}
{biw_misaligned_dw}
{dw_ge_bow}
{bow_aligned_dw}
{bow_misaligned_dw}
            """)

            self.genGaromatrix(out_name="vivado_src/garomatrix.v")

            with open("vivado_src/top.v", "w") as f:
                if self.N_osc==1:
                    aux=""
                else:
                    aux=f".sel_ro(buffer_in[{self.N_bits_osc-1}:0]),"
                if self.tipo_lut == "lut3":
                    aux1=""
                else:
                    aux1=f".sel_pdl(buffer_in[{self.N_bits_osc+self.N_bits_pdl-1}:{self.N_bits_osc}]),"
                    
                f.write(f"""`timescale 1ns / 1ps


module TOP (
    input           clock,
    input[7:0]      ctrl_in,
    output[7:0]     ctrl_out,
    input[{self.data_width-1}:0] data_in,
    output[{self.data_width-1}:0]    data_out
);

    wire[{self.buffer_in_width-1}:0] buffer_in;
    wire[{self.buffer_out_width-1}:0] buffer_out;
    wire sync;
    wire ack;
    wire out_ro;
    wire clock_s;
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
        .DATA_WIDTH({self.data_width}),
        .BUFFER_IN_WIDTH({self.buffer_in_width}),
        .BUFFER_OUT_WIDTH({self.buffer_out_width})
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
    
    CLOCK_DIVIDER clock_divider (
        .clock_in(clock),
        .fdiv(buffer_in[{self.buffer_in_width-1}:{self.buffer_in_width-5}]),
        .clock_out(clock_s)
    );
    
    GAROMATRIX garomatrix (
        .clock(clock),
        .enable(1'b1),
        .clock_s(clock_s),
        .sel_poly(buffer_in[{self.N_bits_osc+self.N_bits_pdl+self.N_bits_poly-1}:{self.N_bits_osc+self.N_bits_pdl}]),
        {aux}
        {aux1}
        .out(out_ro)
    );

    MEDIDOR_BIAS #(
        .OUT_WIDTH({self.buffer_out_width})
    ) medidor_bias (
        .clock(clock_s),
        .enable(enable_medidor),
        .muestra(out_ro),
        .resol(buffer_in[{self.N_bits_osc+self.N_bits_pdl+self.N_bits_poly+self.N_bits_resol-1}:{self.N_bits_osc+self.N_bits_pdl+self.N_bits_poly}]),
        .lock(ack),
        .out(buffer_out)
    );
    
endmodule
            """)

            if self.pblock:
                PBLOCK_CORNERS=self.pblock.split()
                PBLOCK_CORNER_0=PBLOCK_CORNERS[0].split(',')
                PBLOCK_CORNER_1=PBLOCK_CORNERS[1].split(',')
                with open("./vivado_src/pblock.xdc", "w") as f:
                    f.write(f"""create_pblock pblock_TOP_0
add_cells_to_pblock [get_pblocks pblock_TOP_0] [get_cells -quiet [list design_1_i/TOP_0]]
resize_pblock [get_pblocks pblock_TOP_0] -add {{SLICE_X{PBLOCK_CORNER_0[0]}Y{PBLOCK_CORNER_0[1]}:SLICE_X{PBLOCK_CORNER_1[0]}Y{PBLOCK_CORNER_1[1]}}}
                    """)

            ## sdk sources
            subprocess_run(["mkdir","./sdk_src"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/c-xilinx/sdk/interfaz_pcps-pspl.c","./sdk_src/interfaz_pcps-pspl.cp.c"])

            with open("./sdk_src/interfaz_pcps-pspl_config.h", "w") as f:
                if self.board == "cmoda7_15t" or self.board == "cmoda7_35t":
                    f.write("#include \"xuartlite.h\"\n\n")
                elif self.board == "zybo" or self.board == "pynqz2":
                    f.write("#include \"xuartps.h\"\n\n")

                f.write(f"""#define DATA_WIDTH          {self.data_width}
#define BUFFER_IN_WIDTH     {self.buffer_in_width}
#define BUFFER_OUT_WIDTH    {self.buffer_out_width}
#define OCTETO_IN_WIDTH     {octeto_in_width}
#define OCTETO_OUT_WIDTH    {octeto_out_width}
#define WORDS_IN_WIDTH      {words_in_width}
#define WORDS_OUT_WIDTH     {words_out_width}
            """)

            ## log info
            print(f"""
N_osc = {self.N_osc}
N_pdl = {2**self.N_bits_pdl}

(0) <---sel_ro({self.N_bits_osc})---><---sel_pdl({self.N_bits_pdl})---><---sel_poly({self.N_bits_poly})---><---sel_resol(5)---><---sel_fdiv(5)---> ({self.buffer_in_width-1})

fpga part: {self.fpga_part}

sdk source files: {projdir}/sdk_src/
            """)
            if self.qspi == 1:
                print(f"q-spi part: {self.memory_part}")
            
    def medir(self, resol=16, poly=0, fdiv=3, osc=range(1), pdl=range(1), N_rep=1, bias=False, log=False, verbose=True, puerto='S1', baudrate=9600):
        """
         Esta función mide la frecuencia de una matriz de osciladores estándar 'ROMATRIX', una vez este ha sido implementado en FPGA.
         El resultado se devuelve como un objeto TENSOR.
         
         PARÁMETROS:
         -----------
         
            resol = 16
            
                log_2 del número de ciclos de referencia a completar para dar por terminada la medida.
                
                
            poly = 0
            
                Esta variable indica el índice del polinomio a medir.
                
                
            fdiv = 3
            
                log_2 del factor de división para el reloj de muestreo.
            
            
            osc = range(1)
            
                Lista de osciladores a medir.
                
                
            pdl = range(1)
            
                Lista de PDL a medir.
                
                
            N_rep = 1
            
                Número d erepeticiones a medir.
                
                
            bias = False
            
                Si se pasa esta opción como "True" el resultado se dará en tanto por 1.
                
                
            log = False
            
                Si se pasa "True" se escriben algunos datos a modo de log.
                
                
            verbose = True
            
                Si se pasa "True" se pinta una barra de progreso de la medida. Desactivar esta opción ("False") hace
                más cómodo utilizar esta función en un bucle.
                
                
            puerto = S1
            
                Esta opción especifica el puerto serie al que se conecta la FPGA. Actualmente está "hardcodeado" a 
                formato linux (ya que incluso en Windows lo estoy midiendo con WSL).
                
                
            baudrate = 9600
            
                Tasa de transferencia del protocolo serie UART PC<-->PS. Debe concordar con el programa compilador en
                PS.
        """        
        def printUnicodeExp(numero):
            """
            Esta función coge un número y lo escribe como un exponente
            utilizando caracteres UNICODE.
            """
            super_unicode = ["\u2070","\u00B9","\u00B2","\u00B3","\u2074","\u2075","\u2076","\u2077","\u2078","\u2079"]
            result=""
            aux=[]
            while numero>0:
                aux.append(numero%10)
                numero//=10
            aux.reverse()
            result=""
            for i in aux:
                result+=super_unicode[i]
            return result

        def polyNomenclature(entrada):
            """
            Esta función toma un array 'poly' tal y como se utilizando
            en este script para representar un polinomio de Galois, y 
            lo escribe en forma de polinomio tal y como se escribe
            en el paper de Golic.
            """
            N=len(entrada)+1
            result="1"
            for i in range(1, N):
                if entrada[-i]:
                    if i==1:
                        result+="+x"
                    else:
                        result+=f"+x{printUnicodeExp(i)}"
            if N==1:
                result+="+x"
            else:
                result+=f"+x{printUnicodeExp(N)}"
            return result

        osc_list=list(osc)
        pdl_list=list(pdl)
        N_osc = len(osc_list) # Número de osciladores
        N_pdl = len(pdl_list) # Número de pdl_list
                      
        buffer_sel_ro=[]
        for i in osc_list:
            buffer_sel_ro.append(resizeArray(intToBitstr(i), self.N_bits_osc))
            
        buffer_sel_pdl=[]
        for i in pdl_list:
            buffer_sel_pdl.append(resizeArray(intToBitstr(i), self.N_bits_pdl))
            
        buffer_sel_resol=resizeArray(intToBitstr(resol), self.N_bits_resol)
        
        buffer_sel_poly=resizeArray(intToBitstr(poly), self.N_bits_poly)
        
        buffer_sel_fdiv=resizeArray(intToBitstr(fdiv), self.N_bits_fdiv)

        ## Info log
        if log:
            print(f"""
INFO LOG:

Resolución             = {resol}
Polinomio              = {poly} --> {polyNomenclature(buffer_sel_poly)}
Factor divisor clk.    = {2**fdiv}
Número de osciladores  = {N_osc}
Número de repeticiones = {N_rep}
Número de pdl_list     = {N_pdl}
""")
            
        fpga = serial_Serial(port='/dev/tty'+puerto, baudrate=baudrate, bytesize=8)
        time_sleep(.1)
        
        if verbose:
            contador=0
            N_total = N_osc*N_rep*N_pdl
            pinta_progreso(0, N_total, barra=40)
        medidas=[]
        for rep in range(N_rep):
            for pdl in range(N_pdl):
                for osc in range(N_osc):
                    buffer_in = buffer_sel_ro[osc]+buffer_sel_pdl[pdl]+buffer_sel_poly+buffer_sel_resol+buffer_sel_fdiv
                    scan(fpga, buffer_in, self.buffer_in_width)
                    medida = bitstrToInt(calc(fpga, self.buffer_out_width))
                    if bias:
                        medida/=2**resol
                    medidas.append(medida)
                if verbose:
                    contador+=N_osc
                    pinta_progreso(contador, N_total, barra=40)
        if verbose:
            pinta_progreso(N_total, N_total, barra=40)
        fpga.close()
        tensor_medidas = TENSOR(array=np_reshape(medidas, (N_rep,N_pdl,N_osc)), axis=['rep','pdl','osc'])
        
        return tensor_medidas
