"""
Este módulo contiene una serie de clases y funciones para implementar y
medir una matriz de osciladores de anillo en FPGA, tanto estándar como
de Galois.
"""

from os                     import  environ as os_environ
from subprocess             import  run     as subprocess_run
from pickle                 import  dump    as pickle_dump,\
                                    load    as pickle_load
from serial                 import  Serial  as serial_Serial
from time                   import  sleep   as time_sleep
from numpy                  import  reshape as np_reshape,\
                                    log2    as np_log2
from myaux                  import  *
from myfpga                 import  *
from myfpga.interfaz_pcps   import  *
from mytensor               import  *
        
        
def clog2(N):
    """
    Numero de bits necesarios para especificar 'N' estados.
    """
    if N<=0:
        return 1
    else:
        if np_log2(N)>int(np_log2(N)):
            return int(np_log2(N))+1
        else:
            return int(np_log2(N))
            
            
def load(file_name):
    """
    'Wrapper' para la función 'load' del módulo
    'pickle', que permite cargar un objeto guardado serializado de
    cualquier clase.
    """
    with open(file_name, "rb") as f:
        result = pickle_load(f)
    return result


class StdRing():
    """
    Lista de elementos (LUT) que constituyen un oscilador de anillo
    estándar junto con la información necesaria para su implementación
    en FPGA utilzando el software 'Vivado'.
    
    Un anillo 'StdRing' consta de N_inv+1 elementos:
        . Puerta AND.
        . N_inv inversores.
    """
    def __init__(self, name, N_inv, loc, bel='', pin='', modo='min'):
        """
        Inicialización del objeto 'StdRing'.
        
        Parámetros:
        -----------
            name : <string>
                Nombre del anillo. En la práctica este precederá a los nombres
                de cada instancia LUT que forme parte del anillo, e.g. si
                name='a', los nombres de todas las instancias LUT que forman
                parte del anillo serán:
                    'a_AND' # AND inicial.
                    'a_0'   # Primer inversor.
                    'a_1'   # Segundor inversor.
                      .
                      .
                      .
                    'a_N-1' # "N-1"-ésimo inversor.
                    
            N_inv : <int>
                Número de inversores que forman el anillo.
                    
            loc : <string>
                Parámetro 'loc' de la primera LUT del anillo (ver clase 'Lut'). En 
                un anillo estándar esta LUT siempre corresponde a la puerta AND
                inicial. Solo se da la posición de la primera LUT porque los anillos
                siempre se construyen ocupando todos los 'bel' de una celda antes de
                pasar a la celda inmediatamente superior (de modo que, dadas las 
                coordenadas del primer elemento, las demás están predeterminadas). 
                La "celda inmediatamente superior" es X+1 si X es par, Y+1 si X es 
                impar.
                
            bel : <caracter o lista de caracteres> 
                Lista de parámetros 'bel' para cada LUT del anillo (ver clase 'Lut').
                Cada elemento de la lista se aplica en orden correlativo al elemento
                del anillo (la primera restricción se aplica al AND inicial, la
                segunda al primer inversor, etc). Pueden darse menos restricciones 
                que inversores forman el anillo, pero entonces quedarán LUT sin 
                fijar. Notar además que el valor de esta restricción es excluyente 
                entre LUT que forman parte de la misma celda: cada celda tiene 
                cuatro posibles posiciones A, B, C y D, y dos LUT no pueden ocupar 
                el mismo espacio. Esta función no avisa de esta violación: el 
                usuario es responsable de que los valores 'bel' introducidos sean 
                todos distintos entre sí en grupos de cuatro. En otro caso, el 
                diseño fallará. Además de una lista de caracteres, esta opción 
                admite un único caracter, pero entonces solo se restringirá la AND 
                inicial. Ver opción 'bel' de la clase 'Lut'. Notar que el número de 
                elementos total de un anillo "StdRing" es igual a N_inv+1, siendo el
                primero siempre una LUT2 (AND).
                
            pin : <string o lista de string> 
                Lista de parámetros 'pin' de cada LUT que forma parte del anillo 
                (ver clase 'Lut'). Cada elemento de la lista se aplica en orden 
                correlativo al elemento del anillo (la primera restricción se aplica
                al AND inicial, la segunda al primer inversor, etc). Pueden darse 
                menos restricciones que inversores forman el anillo, pero entonces 
                quedarán LUT sin mapear. Notar que el mapeo debe ser coherente con 
                el tipo de LUT que se está fijando, y en particular el AND inicial 
                siempre es de tipo LUT2 (i.e., solo se pueden fijar los pines 'I0' y
                'I1'). Notar que el número de elementos total de un anillo "StdRing"
                es igual a N_inv+1, siendo el primero siempre una LUT2 (AND).
                
            modo : <string>
                Esta opción determina el modo de implementación de las LUT del
                anillo:
                    'min'
                        Minimalista, se utilizan modelos LUT1 para los inversores y
                        LUT2 el enable AND.
                    
                    'config'
                        Se utilizan modelos LUT6 para los inversores, permitiendo
                        utilizar 5 puertos para configurar el anillo mediante PDL.
        """
        self.name = name
        self.N_inv = N_inv
        self.loc = [loc]
        x = int(self.loc[-1].replace(' ','').split(',')[0])
        y = int(self.loc[-1].replace(' ','').split(',')[1])
        for i in range(N_inv):
            if (i+1)%4==0:  # N. luts por celda.
                if x%2==0:
                    x+=1
                else:
                    x = int(self.loc[-1].replace(' ','').split(',')[0])
                    y+=1
            self.loc.append(f"{x},{y}")
            
        self.bel = ['' for i in range(N_inv+1)]
        if type(bel) == type([]):
            for i in range(len(bel)):
                self.bel[i] = bel[i]
        else:
            self.bel[0] = bel
            
        self.pin = ['' for i in range(N_inv+1)]
        if type(pin) == type([]):
            for i in range(len(pin)):
                self.pin[i] = pin[i]
        else:
            self.pin[0] = pin
            
        if modo=='min':
            self.elements = [Lut2(f"AND_{name}", "4'h8", self.loc[0], f"w_{name}[0]", [f"enable_romatrix[{name}]", f"out_romatrix[{name}]"], self.bel[0], self.pin[0])]
            for i in range(N_inv):
                w_in = f"w_{name}[{i}]"
                if i==N_inv-1:
                    w_out = f"out_romatrix[{name}]"
                else:
                    w_out = f"w_{name}[{i+1}]"
                self.elements.append(Lut1(f"inv_{name}_{i}", "2'h1", self.loc[i+1], w_out, w_in, self.bel[i+1], self.pin[i+1]))
        
        elif modo=='config':
            self.elements = [Lut2(f"AND_{name}", "4'h8", self.loc[0], f"w_{name}[0]", [f"enable_romatrix[{name}]", f"out_romatrix[{name}]"], self.bel[0], self.pin[0])]
            for i in range(N_inv):
                w_in = [f"w_{name}[{i}]", f"sel_pdl[0]", f"sel_pdl[1]", f"sel_pdl[2]", f"sel_pdl[3]", f"sel_pdl[4]"]
                if i==N_inv-1:
                    w_out = f"out_romatrix[{name}]"
                else:
                    w_out = f"w_{name}[{i+1}]"
                self.elements.append(Lut6(f"inv_{name}_{i}", "64'h5555555555555555", self.loc[i+1], w_out, w_in, self.bel[i+1], self.pin[i+1]))
            
    def help(self):
        """
        Ayuda de la clase 'StdRing'.
        """
        help('myfpga.ring_osc.StdRing')
            
            
class GaloisRing():
    """
    Lista de elementos (LUT) que constituyen un oscilador de anillo
    de Galois junto con la información necesaria para su implementación
    en FPGA utilzando el software 'Vivado'.
    
    Un anillo 'GaloisRing' consta de N_inv+2 elementos:
        . Inversor inicial.
        . N_inv-1 elementos inversores/XNOR.
        . Inversor de salida.
        . flip-flop de muestreo.
    """
    def __init__(self, name, N_inv, loc, bel='', pin='', modo='min'):
        """
        Inicialización del objeto 'GaloisRing'.
        
        Parámetros:
        -----------
            name : <cadena de caracteres>
                Nombre del anillo. En la práctica este precederá a los nombres
                de cada instancia LUT que forme parte del anillo, e.g. si
                name='a', los nombres de todas las instancias LUT que forman
                parte del anillo serán:
                    'a_AND' # AND inicial.
                    'a_0'   # Primer inversor.
                    'a_1'   # Segundor inversor.
                      .
                      .
                      .
                    'a_N-1' # "N-1"-ésimo inversor.
                    
            N_inv : <entero>
                Número de inversores que forman el anillo.
                    
            loc : <string>
                Parámetro 'loc' de la primera LUT del anillo (ver clase 'Lut').
                Solo se da la posición de la primera LUT porque los anillos siempre 
                se construyen ocupando todos los 'bel' de una celda antes de pasar a
                la celda inmediatamente superior (de modo que, dadas las coordenadas
                del primer elemento, las demás están predeterminadas). La "celda 
                inmediatamente superior" es X+1 si X es par, Y+1 si X es impar.
                
            bel : <caracter o lista de caracteres> 
                Lista de parámetros 'bel' para cada elemento del anillo (ver clase 
                'Lut'). Cada elemento de la lista se aplica en orden correlativo al
                elemento del anillo (la primera restricción se aplica al primer 
                elemento, la segunda al segundo, etc). Pueden darse menos 
                restricciones que inversores forman el anillo, pero entonces 
                quedarán LUT sin fijar. Notar además que el valor de esta 
                restricción es excluyente entre LUT que forman parte de la misma 
                celda: cada celda tiene cuatro posibles posiciones A, B, C y D, y 
                dos LUT no pueden ocupar el mismo espacio. Esta función no avisa de
                esta violación: el usuario es responsable de que los valores 'bel' 
                introducidos sean todos distintos entre sí en grupos de cuatro. En 
                otro caso, el diseño fallará. Además de una lista de caracteres, 
                esta opción admite un único caracter, pero entonces solo se 
                restringirá la primera LUT. Notar que el número de  elementos total
                de un anillo  "GaloisRing" es igual a N_inv+2, siendo el penúltimo 
                siempre una LUT1 (inversor), y el último un flip-flop.
                
            pin : <cadena de caracteres o lista de cadenas> 
                Lista de parámetros 'pin' de cada LUT que forma parte del anillo 
                (ver clase 'Lut'). Cada elemento de la lista se aplica en orden
                correlativo al elemento del anillo (la primera restricción se aplica
                al primer elemento, la segunda al segundo, etc). Pueden darse menos
                restricciones que inversores forman el anillo, pero entonces 
                quedarán LUT sin mapear. Notar que el mapeo debe ser coherente con 
                el tipo de LUT que se está fijando, y en particular los inversores 
                inicial y final siempre son de tipo LUT1 (i.e., solo se puede fijar
                el pin 'I0'). Notar que, aunque el número de elementos en un objeto 
                'GaloisRing' sea N_inv+2, a efectos de la opción "pin" el último se 
                ignora (esta restringe solo los pines de las LUT, no del flip-flop).
            
            modo : <string>
                Esta opción determina el modo de implementación de las LUT del
                anillo:
                    'min'
                        Minimalista, se utilizan modelos LUT1 para los inversores y
                        LUT2 el enable AND.
                    
                    'config'
                        Se utilizan modelos LUT6 para los inversores, permitiendo
                        utilizar 5 puertos para configurar el anillo mediante PDL.
        """
        self.name = name
        self.N_inv = N_inv
        self.loc = [loc]
        x = int(self.loc[-1].replace(' ','').split(',')[0])
        y = int(self.loc[-1].replace(' ','').split(',')[1])
        for i in range(N_inv):
            if (i+1)%4==0:  # N. luts por celda.
                if x%2==0:
                    x+=1
                else:
                    x = int(self.loc[-1].replace(' ','').split(',')[0])
                    y+=1
            self.loc.append(f"{x},{y}")
            
        self.bel = ['' for i in range(N_inv+2)]
        if type(bel) == type([]):
            for i in range(len(bel)):
                self.bel[i] = bel[i]
        else:
            self.bel[0] = bel
            
        self.pin = ['' for i in range(N_inv+1)]
        if type(pin) == type([]):
            for i in range(len(pin)):
                self.pin[i] = pin[i]
        else:
            self.pin[0] = pin
            
        if modo=='min':
            self.elements = []
            for i in range(N_inv):
                w_out = f"w_{name}[{i}]"
                
                if i==0:
                    w_in = f"w_{name}[{N_inv-1}]"
                    self.elements.append(Lut1(f"inv_{name}_{i}", "2'h1", self.loc[i], w_out, w_in, self.bel[i], self.pin[i]))
                else:
                    w_in = [f"w_{name}[{i-1}]", f"w_{name}[{N_inv-1}]", f"sel_poly[{i-1}]"]
                    self.elements.append(Lut3(f"inv_{name}_{i}", "8'h95", self.loc[i], w_out, w_in, self.bel[i], self.pin[i]))
        
        elif modo=='config':
            self.elements = []
            for i in range(N_inv):
                w_out = f"w_{name}[{i}]"
                
                if i==0:
                    w_in = [f"w_{name}[{N_inv-1}]", f"sel_pdl[{0}]", f"sel_pdl[{1}]", f"sel_pdl[{2}]"]
                    self.elements.append(Lut4(f"inv_{name}_{i}", "16'h5555", self.loc[i], w_out, w_in, self.bel[i], self.pin[i]))
                else:
                    w_in = [f"w_{name}[{i-1}]", f"w_{name}[{N_inv-1}]", f"sel_poly[{i-1}]", f"sel_pdl[{0}]", f"sel_pdl[{1}]", f"sel_pdl[{2}]"]
                    self.elements.append(Lut6(f"inv_{name}_{i}", "64'h9595959595959595", self.loc[i], w_out, w_in, self.bel[i], self.pin[i]))            
                    
        self.elements.append(Lut1(f"invout_{name}", "2'h1", self.loc[N_inv], f"out_ro[{name}]", w_out, self.bel[N_inv], self.pin[N_inv]))    
        self.elements.append(FlipFlop(f"ff_{name}", self.loc[N_inv], f"out_sampled[{name}]", 'clock_s', f"out_ro[{name}]", self.bel[N_inv+1]))
        
    def help(self):
        """
        Ayuda de la clase 'GaloisRing'.
        """
        help('myfpga.ring_osc.GaloisRing')
        
        
class Dominio:
    """
    Este objeto contiene las localizaciones de un conjunto de 
    osciladores dispuestos atendiendo a diversos parámetros geométricos 
    (ver función '__init__').
    """
    def __init__(self, N_osc=10, x0=0, x1=99, dx=1, y0=0, y1=99, dy=1, directriz='y'):
        """
        Inicialización del objeto 'Dominio'.
        
        Parámetros:
        -----------
            N_osc : <int>
                Número de osciladores del dominio.
                
            x0 : <int>
                Coordenada X del primer oscilador de la matriz.
             
            x1 : <int>
                Coordenada X máxima de la matriz (no se sobrepasará).
                
            dx : <int>
                Incremento de la coordenada X. Notar que habitualmente en las FPGA 
                de Xilinx se reservan las coordenadas X par/impar para distintos
                tipos de celda ('0' y '1').
                
            y0 : <int>
                Coordenada Y del primer oscilador de la matriz.
                
            y1 : <int>
                Coordenada Y máxima de la matriz (no se sobrepasará).
                
            dy : <int>
                Incremento de la coordenada Y.
                
            directriz : <caracter>
                Dirección de crecimiento de la matriz (hasta llegar a la coordenada 
                máxima). Puede ser 'y' o 'x'.
        """
        self.N_osc = N_osc
        self.directriz = directriz
        self.x0 = x0
        self.y0 = y0
        self.x1 = x1
        self.y1 = y1
        self.dx = dx
        self.dy = dy
                
        self.osc_coord = []
        x = self.x0
        y = self.y0
        for i in range(self.N_osc):
            if x > self.x1:
                if y==self.y1 or y+self.dy > self.y1:
                    break
                else:
                    x=self.x0
                    y+=self.dy
                    
            if y > self.y1:
                if x==self.x1 or x+self.dx > self.x1:
                    break
                else:
                    x+=self.dx
                    y=self.y0
            self.osc_coord.append(f"{x},{y}")
            if self.directriz == 'y':
                y+=self.dy
            else:
                x+=self.dx
                
    def help(self):
        """
        Ayuda de la clase 'Dominio'.
        """
        help('myfpga.ring_osc.Dominio')
        
        
class StdMatrix:
    """Objeto que contiene una matriz de osciladores de anillo estándar."""
    def __init__(self, N_inv=3, dominios=Dominio(), bel='', pin='', modo='min'):
        """
        Inicialización del objeto 'StdMatrix'.
        
        Parámetros:
        ---------
            N_inv : <int>
                Número de inversores de cada oscilador.

            dominios : <objeto 'Dominio' o lista de estos>
                Osciladores que forman la matriz. Se construye como una lista de 
                objetos 'Dominio'. Si solo pasamos un dominio de osciladores podemos
                pasar un objeto 'Dominio', en lugar de una lista.
                
            bel : <caracter o lista de caracteres>
                Dado que todos los anillos de la matriz son idénticos 
                por diseño, esta opción es la misma que la aplicada para
                un solo oscilador (ver 'bel' en 'StdRing').
                    
            pin : <cadena de caracteres o lista de cadenas>
                Dado que todos los anillos de la matriz son idénticos 
                por diseño, esta opción es la misma que la aplicada para
                un solo oscilador (ver 'pin' en 'StdRing').
                
            modo : <string>
                Esta opción determina el modo de implementación de las LUT del
                anillo:
                    'min'
                        Minimalista, se utilizan modelos LUT1 para los inversores y
                        LUT2 el enable AND.
                    
                    'config'
                        Se utilizan modelos LUT6 para los inversores, permitiendo
                        utilizar 5 puertos para configurar el anillo mediante PDL.
        
        """
        self.dominios=[]
        if type(dominios) == type([]) or type(dominios) == type(()):
            self.dominios[:]=dominios[:]
        else:
            self.dominios.append(dominios)
            
        self.N_inv = N_inv
            
        self.bel = ['' for i in range(N_inv+1)]
        if type(bel)==type([]):
            for i in range(len(bel)):
                self.bel[i] = bel[i]
        else:
            self.bel[0] = bel

        self.pin = ['' for i in range(N_inv+1)]
        if type(pin)==type([]):
            for i in range(len(pin)):
                self.pin[i] = pin[i]
        else:
            self.pin[0] = pin

        self.modo = modo
            
        self.osc_list = []
        self.N_osc=0
        for dominio in self.dominios:
            for osc_coord in dominio.osc_coord:
                self.osc_list.append(StdRing(f"{self.N_osc}", self.N_inv, osc_coord, self.bel, self.pin, self.modo))
                self.N_osc+=1
                
        self.N_bits_osc = clog2(self.N_osc)
        self.N_bits_resol = 5
        if self.modo=='min':
            self.N_bits_pdl = 0
        elif modo=='config':
            self.N_bits_pdl = 5
                
    def help(self):
        """
        Ayuda de la clase 'StdMatrix'.
        """
        help('myfpga.ring_osc.StdMatrix')
        
    def save(self, file_name):
        """
        'Wrapper' para guardar objetos serializados con el módulo 'pickle'.
        """
        with open(file_name, "wb") as f:
            pickle_dump(self, f)
        
    def gen_romatrix(self, out_name="romatrix.v", debug=False):
        """
        Genera un diseño 'out_name' en formato Verilog con la implementación de 
        los dominios introducidos durante la inicialización del objeto. El 
        principal uso de esta función es dentro de la función 'implement()'.

        Parámetros:
        -----------
            out_name : <string> (opcional)
                Nombre del fichero de salida.
                
            debug : <bool> (opcional)
                Flag que indica si se debe geenrar un diseño de depuración en 
                lugar de una verdadera matriz de osciladores de anillo. En el 
                diseño de depuración se substituye cada anillo por un divisor de
                reloj de frecuencia conocida, lo que permite depurar la interfaz
                de medida.
        """
        with open(out_name, "w") as f:
            f.write(f"//N_osciladores: {self.N_osc}\n\n")
            
            f.write("module ROMATRIX (\n")
            f.write(f"    input[{self.N_osc-1}:0] enable_romatrix,\n")
            if self.modo=='config':
                f.write(f"    input[4:0] sel_pdl,\n")
            f.write("    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *)\n")
            f.write(f"    output[{self.N_osc-1}:0] out_romatrix\n")
            f.write("    );\n\n")
            
            for i in range(self.N_osc):
                f.write(f"    wire[{self.N_inv-1}:0] w_{i};\n")
            f.write("\n")
            
            if not debug:
                for osc in self.osc_list:
                    for lut in osc.elements:
                        f.write(f"    {lut.impl()}\n")
                    f.write("\n")
                f.write("endmodule\n")
            
            else:
                for i in range(self.N_osc):
                    f.write(f"    CLOCK_DIVIDER cd_{i} (\n")
                    f.write("        .clock_in(clock),\n")
                    f.write(f"        .fdiv({i}),\n")
                    f.write(f"        .clock_out(ring{i}_out)\n")
                    f.write("    );\n")
                    f.write("\n")
                f.write("endmodule\n")
                
    def implement(self, projname='project_romatrix', projdir='.', njobs=4, linux=False, 
                  debug=False, files=True, board='pynqz2', qspi=False, routing=False, 
                  pblock=False, data_width=32, buffer_out_width=32, f_clock=100):
        """
        Copia en el directorio 'projdir' todos los archivos necesarios para 
        implementar una matriz de osciladores de anillo con medición de la 
        frecuencia y comunicación pc <-> microprocesador <-> FPGA.

        Parámetros:
        ---------
            projname : <string>
                Nombre del proyecto de Vivado.
            
            projdir : <string>
                Directorio donde se creará el proyecto de Vivado y las fuentes (por
                defecto el directorio de trabajo actual).
            
            njobs : <int>
                Número de núcleos que utiilizará Vivado paralelamente para la 
                síntesis/implementación.

            linux : <bool>
                Marcar esta opción como True si la versión de Vivado que se va a
                utilizar está instalado en una máquina linux (de forma que los 
                PATH se gestionen correctamente).

            debug : <bool>
                Si "True", se implementará una matriz de divisores de reloj de 
                frecuencia conocida, lo que permite depurar el diseño al conocer
                qué resultados deben salir.
                
            files : <bool>
                 Si "True", pinta los archivos necesarios para implementar la 
                 matriz en FPGA. Esta opción se puede desactivar (False) cuando 
                 queremos configurar un objeto tipo 'Romatrix' pero no vamos a 
                 implementarla físicamente (por ejemplo porque ya lo hemos hecho
                 y solo queremos medir, o vamos a simularla sin realizarla).
                
            board : <string>
                Placa de desarrollo utilizada en el proyecyo. Las opciones 
                soportadas son: 'pynqz2', 'zybo', 'cmoda7_15t' o 'cmoda7_35t'.
                
            qspi : <bool>
                Si "True", el flujo de diseño incluirá el guardado del bitstream
                en la memoria flash de la placa para que se auto-programe al 
                encenderse.
                
            routing : <bool>
                Si "True", el flujo de diseño incluirá el cableado de los 
                inversores después de la síntesis. Esto aumenta las 
                probabilidades de que la herramienta haga un cableado idéntico,
                pero es recomendable comprobarlo. (NOTA: no tengo garantías de 
                que esta opción sea del todo compatible con -qspi).
                
            pblock : <string>
                Se añade una restricción PBLOCK para el módulo TOP (i.e., todos los
                elementos auxiliares de la matriz de osciladores se colocarán 
                dentro de este espacio). El formato de esta opción es un string que
                consta de dos puntos separados por un espacio, donde cada punto 
                está dado por un par de coordenadas X,Y separadas por una coma; 
                estos  representan respectivamente las esquinas inferior izda. y 
                superior dcha. del rectángulo de restricción: 'X0,Y0 X1,Y1'. El 
                diseñador es responsable de que la FPGA utilizada disponga de 
                recursos suficientes en el bloque descrito.

            data_width(dw) : <int>
                Esta opción especifica la anchura del canal de datos PS<-->PL.

            buffer_out_width(bow) : <int> 
                Esta opción especifica la anchura de la palabra de respuesta 
                (i.e., de la medida).
                
            f_clock : <int>
                Frecuencia del reloj del diseño (en MHz).
        """
        self.board = board
        self.qspi = qspi
        self.routing = routing
        self.pblock = pblock
        self.data_width = data_width
        self.buffer_out_width = buffer_out_width
        self.f_clock = f_clock
            
        if self.board=='cmoda7_15t':
            self.fpga_part="xc7a15tcpg236-1"
            self.board_part="digilentinc.com:cmod_a7-15t:part0:1.1"
            self.memory_part="mx25l3233"
            self.clk_name=f"/clk_wiz_1/clk_out1 ({self.f_clock} MHz)"
        elif self.board=='cmoda7_35t':
            self.fpga_part="xc7a35tcpg236-1"
            self.board_part="digilentinc.com:cmod_a7-35t:part0:1.1"
            self.memory_part="mx25l3233"
            self.clk_name=f"/clk_wiz_1/clk_out1 ({self.f_clock} MHz)"
        elif self.board=='zybo':
            self.fpga_part="xc7z010clg400-1"
            self.board_part="digilentinc.com:zybo:part0:1.0"
            self.memory_part="?"
            self.clk_name =f"/processing_system7_0/FCLK_CLK0 ({self.f_clock} MHz)"
        elif self.board=='pynqz2':
            self.fpga_part="xc7z020clg400-1"
            self.board_part="tul.com.tw:pynq-z2:part0:1.0"
            self.memory_part="?"
            self.clk_name =f"/processing_system7_0/FCLK_CLK0 ({self.f_clock} MHz)"
            
        self.buffer_in_width = self.N_bits_osc+self.N_bits_pdl+self.N_bits_resol
        
        if files:
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
                
            ## projdir
            wdir = projdir
            if projdir=='.':
                projdir = os_environ['PWD']
            if not linux:
                projdir = subprocess_run(["wslpath","-w",projdir], capture_output=True, text=True).stdout.replace("\\","/").replace("\n","")
                
            ## block design
            subprocess_run(["mkdir",f"{wdir}/block_design"])
            if self.board == "cmoda7_15t":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_15t.tcl",f"{wdir}/block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_15t.tcl",f"{wdir}/block_design/bd_design_1.tcl"])

            elif self.board == "cmoda7_35t":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_35t.tcl",f"{wdir}/block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_35t.tcl",f"{wdir}/block_design/bd_design_1.tcl"])

            elif self.board == "zybo":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_zybo.tcl",f"{wdir}/block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_zybo.tcl",f"{wdir}/block_design/bd_design_1.tcl"])

            elif self.board == "pynqz2":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_pynqz2.tcl",f"{wdir}/block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_pynqz2.tcl",f"{wdir}/block_design/bd_design_1.tcl"])

            ## partial flows (tcl)
            subprocess_run(["mkdir",f"{wdir}/partial_flows"])

            vivado_files=f"{projdir}/vivado_src/top.v {projdir}/vivado_src/interfaz_pspl.cp.v {projdir}/vivado_src/romatrix.v {projdir}/vivado_src/medidor_frec.cp.v {projdir}/vivado_src/interfaz_romatrix.cp.v {projdir}/vivado_src/interfaz_pspl_config.vh"
            if debug:
                vivado_files+=f" {projdir}/vivado_src/clock_divider.cp.v"
            if self.pblock:
                vivado_files+=f" {projdir}/vivado_src/pblock.xdc"

            with open(f"{wdir}/partial_flows/setupdesign.tcl", "w") as f:
                f.write(f"create_project {projname} {projdir}/{projname} -part {self.fpga_part}\n")
                f.write(f"set_property board_part {self.board_part} [current_project]\n")
                f.write(f"source {projdir}/block_design/bd_design_1.tcl\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"regenerate_bd_layout\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"add_files -norecurse {{ {vivado_files} }}\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"create_bd_cell -type module -reference TOP TOP_0\n")
                f.write(f"set_property -dict [list CONFIG.C_GPIO_WIDTH {self.data_width} CONFIG.C_GPIO2_WIDTH {self.data_width}] [get_bd_cells axi_gpio_data]\n")
                f.write(f"connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]\n")
                f.write(f"connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]\n")
                f.write(f"connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]\n")
                f.write(f"connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]\n")
                f.write(f"apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {{Clk {self.clk_name} }}  [get_bd_pins TOP_0/clock]\n")
                f.write(f"regenerate_bd_layout\n")
                f.write(f"make_wrapper -files [get_files {projdir}/{projname}/{projname}.srcs/sources_1/bd/design_1/design_1.bd] -top\n")
                f.write(f"add_files -norecurse {projdir}/{projname}/{projname}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"set_property top design_1_wrapper [current_fileset]\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]\n")
                if self.f_clock!=100:
                    if self.board=="zybo" or self.board=="pynqz2":
                        f.write(f"set_property -dict [list CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ {{{self.f_clock}}}] [get_bd_cells processing_system7_0]\n")
                f.write(f"validate_bd_design\n")
                f.write(f"save_bd_design\n")
                
            with open(f"{wdir}/partial_flows/genbitstream.tcl", "w") as f:
                f.write(f"if {{[file exists {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc]==1}} {{\n")
                f.write(f"    export_ip_user_files -of_objects  [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc] -no_script -reset -force -quiet\n")
                f.write(f"    remove_files  -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc\n")
                f.write(f"}}\n")
                f.write("\n")
                f.write(f"if {{[file exists {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]==1}} {{\n")
                f.write(f"    export_ip_user_files -of_objects  [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc] -no_script -reset -force -quiet\n")
                f.write(f"    remove_files  -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc\n")
                f.write(f"}}\n")
                f.write("\n")
                f.write(f"if {{[file exists {projdir}/{projname}/{projname}.runs/synth_1]==1}} {{\n")
                f.write(f"    reset_run synth_1\n")
                f.write(f"}}\n")
                f.write("\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"update_module_reference design_1_TOP_0_0\n")
                f.write(f"launch_runs synth_1 -jobs {njobs}\n")
                f.write(f"wait_on_run synth_1\n")
                
                if self.qspi and self.board == "cmoda7_15t":
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1\n")
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new\n")
                    f.write(f"close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc w ]\n")
                    f.write(f"add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc\n")
                    f.write(f"set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]\n")
                    f.write(f"set_property used_in_synthesis false [get_files  {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]\n")
                    f.write(f"update_compile_order -fileset sources_1\n")
                    f.write(f"update_module_reference design_1_TOP_0_0\n")
                    f.write(f"open_run synth_1 -name synth_1\n")
                    f.write(f"set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]\n")
                    f.write(f"set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]\n")
                    f.write(f"set_property config_mode SPIx4 [current_design]\n")
                    f.write(f"save_constraints\n")
                    f.write(f"close_design\n")

                elif self.qspi and self.board == "cmoda7_35t":
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1\n")
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new\n")
                    f.write(f"close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc w ]\n")
                    f.write(f"add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc\n")
                    f.write(f"set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]\n")
                    f.write(f"set_property used_in_synthesis false [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]\n")
                    f.write(f"update_compile_order -fileset sources_1\n")
                    f.write(f"update_module_reference design_1_TOP_0_0\n")
                    f.write(f"open_run synth_1 -name synth_1\n")
                    f.write(f"set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]\n")
                    f.write(f"set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]\n")
                    f.write(f"set_property config_mode SPIx4 [current_design]\n")
                    f.write(f"save_constraints\n")
                    f.write(f"close_design\n\n")

                if self.routing:
                    f.write(f"open_run synth_1 -name synth_1\n\n")

                    for i in range(self.N_osc):
                        for j in range(self.N_inv):
                            f.write(f"route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix/ring{i}_w{j}]\n")
                            f.write(f"set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix/ring{i}_w{j} }}]\n\n")

                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new\n")
                    f.write(f"close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc w ]\n")
                    f.write(f"add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc\n")
                    f.write(f"set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc [current_fileset -constrset]\n")
                    f.write(f"save_constraints -force\n")
                    f.write(f"close_design\n")
                    f.write(f"update_compile_order -fileset sources_1\n")
                    f.write(f"update_module_reference design_1_TOP_0_0\n") # Aqui termina detailed routing

                f.write(f"if {{[file exists {projdir}/{projname}/{projname}.runs/synth_1/__synthesis_is_complete__]==1}} {{\n")
                f.write(f"    reset_run synth_1\n")
                f.write(f"}}\n")
                f.write(f"launch_runs impl_1 -to_step write_bitstream -jobs {njobs}\n")
                f.write(f"wait_on_run impl_1\n")

            with open(f"{wdir}/partial_flows/launchsdk.tcl", "w") as f:
                f.write(f"file mkdir {projdir}/{projname}/{projname}.sdk\n")
                f.write(f"file copy -force {projdir}/{projname}/{projname}.runs/impl_1/design_1_wrapper.sysdef {projdir}/{projname}/{projname}.sdk/design_1_wrapper.hdf\n")
                f.write(f"launch_sdk -workspace {projdir}/{projname}/{projname}.sdk -hwspec {projdir}/{projname}/{projname}.sdk/design_1_wrapper.hdf\n")
                f.write(f"## Aqui termina la generacion de hwd_platform\n")

            with open(f"{wdir}/mkhwdplatform.tcl", "w") as f:
                f.write(f"source {projdir}/partial_flows/setupdesign.tcl\n")
                f.write(f"source {projdir}/partial_flows/genbitstream.tcl\n")
                f.write(f"source {projdir}/partial_flows/launchsdk.tcl\n")

            ## vivado sources
            subprocess_run(["mkdir",f"{wdir}/vivado_src"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/interfaz_pspl.v",f"{wdir}/vivado_src/interfaz_pspl.cp.v"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/medidor_frec.v",f"{wdir}/vivado_src/medidor_frec.cp.v"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/interfaz_romatrix.v",f"{wdir}/vivado_src/interfaz_romatrix.cp.v"])
            if debug:
                subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/clock_divider.v",f"{wdir}/vivado_src/clock_divider.cp.v"])

            with open(f"{wdir}/vivado_src/interfaz_pspl_config.vh", "w") as f:
                f.write(f"{dw_ge_biw}\n")
                f.write(f"{biw_aligned_dw}\n")
                f.write(f"{biw_misaligned_dw}\n")
                f.write(f"{dw_ge_bow}\n")
                f.write(f"{bow_aligned_dw}\n")
                f.write(f"{bow_misaligned_dw}\n")
            
            self.gen_romatrix(out_name=f"{wdir}/vivado_src/romatrix.v", debug=debug)

            with open(f"{wdir}/vivado_src/top.v", "w") as f:
                f.write(f"module TOP (\n")
                f.write(f"    input           clock,\n")
                f.write(f"    input[7:0]      ctrl_in,\n")
                f.write(f"    output[7:0]     ctrl_out,\n")
                f.write(f"    input[{self.data_width-1}:0]    data_in,\n")
                f.write(f"    output[{self.data_width-1}:0]    data_out\n")
                f.write(f");\n\n")
                
                f.write(f"    wire[{self.buffer_in_width-1}:0] buffer_in;\n")
                f.write(f"    wire[{self.buffer_out_width-1}:0] buffer_out;\n")
                f.write(f"    wire sync;\n")
                f.write(f"    wire ack;\n")
                f.write(f"    wire[{self.N_osc-1}:0] enable_romatrix;\n")
                f.write(f"    wire[{self.N_osc-1}:0] out_romatrix;\n")
                f.write(f"    wire out_ro;\n")
                f.write(f"    reg enable_medidor=0;\n\n")
                
                f.write(f"    always @(posedge clock) begin\n")
                f.write(f"        case ({{sync,ack}})\n")
                f.write(f"            2'b10:\n")
                f.write(f"                enable_medidor <= 1;\n")
                f.write(f"            default:\n")
                f.write(f"                enable_medidor <= 0;\n")
                f.write(f"        endcase\n")
                f.write(f"    end\n\n")
                
                f.write(f"    INTERFAZ_PSPL #(\n")
                f.write(f"        .DATA_WIDTH({self.data_width}),\n")
                f.write(f"        .BUFFER_IN_WIDTH({self.buffer_in_width}),\n")
                f.write(f"        .BUFFER_OUT_WIDTH({self.buffer_out_width})\n")
                f.write(f"    ) interfaz_pspl (\n")
                f.write(f"        .clock(clock),\n")
                f.write(f"        .ctrl_in(ctrl_in),\n")
                f.write(f"        .ctrl_out(ctrl_out),\n")
                f.write(f"        .data_in(data_in),\n")
                f.write(f"        .data_out(data_out),\n")
                f.write(f"        .sync(sync),\n")
                f.write(f"        .ack(ack),\n")
                f.write(f"        .buffer_in(buffer_in),\n")
                f.write(f"        .buffer_out(buffer_out)\n")
                f.write(f"    );\n\n")
                
                f.write(f"    (* DONT_TOUCH=\"true\" *)\n")
                f.write(f"    INTERFAZ_ROMATRIX #(\n")
                f.write(f"        .N_OSC({self.N_osc})\n")
                f.write(f"    ) interfaz_romatrix (\n")
                f.write(f"        .clock(clock),\n")
                f.write(f"        .enable(enable_medidor),\n")
                f.write(f"        .sel_ro(buffer_in[{self.N_bits_osc-1}:0]),\n")
                f.write(f"        .out_romatrix(out_romatrix),\n")
                f.write(f"        .enable_romatrix(enable_romatrix),\n")
                f.write(f"        .out(out_ro)\n")
                f.write(f"    );\n\n")
                
                f.write(f"    (* DONT_TOUCH=\"true\" *)\n")
                f.write(f"    ROMATRIX romatrix (\n")
                f.write(f"        .enable_romatrix(enable_romatrix),\n")
                if self.modo=='config':
                    f.write(f"        .sel_pdl(buffer_in[{self.buffer_in_width-5-1}:{self.N_bits_osc}]),\n")
                f.write(f"        .out_romatrix(out_romatrix)\n")
                f.write(f"    );\n\n")
                
                f.write(f"    (* DONT_TOUCH=\"true\" *)\n")
                f.write(f"    MEDIDOR_FREC #(\n")
                f.write(f"        .OUT_WIDTH({self.buffer_out_width})\n")
                f.write(f"    ) medidor_frec (\n")
                f.write(f"        .clock(clock),\n")
                f.write(f"        .enable(enable_medidor),\n")
                f.write(f"        .clock_u(out_ro),\n")
                f.write(f"        .resol(buffer_in[{self.buffer_in_width-1}:{self.buffer_in_width-5}]),\n")
                f.write(f"        .lock(ack),\n")
                f.write(f"        .out(buffer_out)\n")
                f.write(f"    );\n\n")
                
                f.write(f"endmodule\n")

            if self.pblock:
                PBLOCK_CORNERS=self.pblock.split()
                PBLOCK_CORNER_0=PBLOCK_CORNERS[0].split(',')
                PBLOCK_CORNER_1=PBLOCK_CORNERS[1].split(',')
                with open(f"{wdir}/vivado_src/pblock.xdc", "w") as f:
                    f.write(f"create_pblock pblock_TOP_0\n")
                    f.write(f"add_cells_to_pblock [get_pblocks pblock_TOP_0] [get_cells -quiet [list design_1_i/TOP_0/inst/interfaz_pspl design_1_i/TOP_0/inst/interfaz_romatrix design_1_i/TOP_0/inst/medidor_frec]]\n")
                    f.write(f"resize_pblock [get_pblocks pblock_TOP_0] -add {{SLICE_X{PBLOCK_CORNER_0[0]}Y{PBLOCK_CORNER_0[1]}:SLICE_X{PBLOCK_CORNER_1[0]}Y{PBLOCK_CORNER_1[1]}}}\n")
                    
            ## sdk sources
            subprocess_run(["mkdir",f"{wdir}/sdk_src"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/c-xilinx/sdk/interfaz_pcps-pspl.c",f"{wdir}/sdk_src/interfaz_pcps-pspl.cp.c"])

            with open(f"{wdir}/sdk_src/interfaz_pcps-pspl_config.h", "w") as f:
                if self.board == "cmoda7_15t" or self.board == "cmoda7_35t":
                    f.write("#include \"xuartlite.h\"\n\n")
                elif self.board == "zybo" or self.board == "pynqz2":
                    f.write("#include \"xuartps.h\"\n\n")
                    
                f.write(f"#define DATA_WIDTH          {self.data_width}\n")
                f.write(f"#define BUFFER_IN_WIDTH     {self.buffer_in_width}\n")
                f.write(f"#define BUFFER_OUT_WIDTH    {self.buffer_out_width}\n")
                f.write(f"#define OCTETO_IN_WIDTH     {octeto_in_width}\n")
                f.write(f"#define OCTETO_OUT_WIDTH    {octeto_out_width}\n")
                f.write(f"#define WORDS_IN_WIDTH      {words_in_width}\n")
                f.write(f"#define WORDS_OUT_WIDTH     {words_out_width}\n")
                
            ## log info
            print(f"N_osc       = {self.N_osc}")
            print(f"N_bits_osc  = {self.N_bits_osc}\n")
            print(f"N_pdl       = {2**self.N_bits_pdl}")
            print(f"N_bits_pdl  = {self.N_bits_pdl}\n")
            print(f"N_bits_resol= {self.N_bits_resol}\n")
            print(f"data_width      = {self.data_width}")
            print(f"buffer_in_width = {self.buffer_in_width}")
            print(f"buffer_out_width= {self.buffer_out_width}\n")
            print("Trama de datos:")
            print(f"    (0) <---sel_ro({self.N_bits_osc})---><---sel_pdl({self.N_bits_pdl})---><---sel_resol({self.N_bits_resol})---> ({self.buffer_in_width-1})\n")
            print(f"fpga part: {self.fpga_part}\n")
            print(f"sdk source files: {projdir}/sdk_src/")
            if self.qspi:
                print(f"q-spi part: {self.memory_part}")

    def medir(self, puerto='S1', osc=[0], pdl=[0], N_rep=1, resol=17, f_ref=False, 
              log=False, verbose=True, baudrate=9600):
        """
        Esta función mide la frecuencia de una matriz de osciladores estándar 
        'StdMatrix', una vez esta ha sido implementado en FPGA. El resultado 
        se devuelve como un objeto 'Tensor'.

        Parámetros:
        -----------
            puerto : <string>
                Esta opción especifica el puerto serie al que se conecta la 
                FPGA. Actualmente está "hardcodeado" a formato linux (ya que 
                incluso en Windows lo estoy midiendo con WSL).
                
            osc : <int o lista de int>
                Lista de osciladores a medir.
                
            pdl : <int o lista de int>
                Lista de PDL a medir.
                
            N_rep : <int>
                Número de repeticiones a medir.
                
            resol : <int>
                log_2 del número de ciclos de referencia a completar para dar 
                por terminada la medida (por defecto 17, i.e., 2^17 = 131072 
                ciclos).
                
            f_ref = <real>
                Frecuencia del reloj de referencia. Si se proporciona este 
                valor, el resultado obtenido se devuelve en las mismas unidades
                en que se haya pasado este valor "f_ref".
                
            log : <bool>
                Si se pasa "True" se escriben algunos datos a modo de log.
                
            verbose : <bool>
                Si se pasa "True" se pinta una barra de progreso de la medida. 
                Desactivar esta opción ("False") hace más cómodo utilizar esta 
                función en un bucle.
                
            baudrate : <int>
                Tasa de transferencia del protocolo serie UART PC<-->PS. Debe 
                concordar con el programa compilador en PS.
        """
        if type(osc)==type([]):
            osc_list = osc[:]
        else:
            osc_list = [osc]
            
        if type(pdl)==type([]):
            pdl_list = pdl[:]
        else:
            pdl_list = [pdl]
            
        N_osc = len(osc_list) # Número de osciladores a medir
        N_pdl = len(pdl_list) # Número de pdl a medir
        
        buffer_sel_ro=[]
        for i in osc_list:
            buffer_sel_ro.append(resize_array(int_to_bitstr(i), self.N_bits_osc))
            
        buffer_sel_pdl=[]
        for i in pdl_list:
            buffer_sel_pdl.append(resize_array(int_to_bitstr(i), self.N_bits_pdl))
            
        buffer_sel_resol=resize_array(int_to_bitstr(resol), self.N_bits_resol)

        ## Info log
        if log:
             print(f"INFO LOG:\n\n")
            
             print(f"Resolución             = {resol}\n")
             print(f"Número de osciladores  = {N_osc}\n")
             print(f"Número de repeticiones = {N_rep}\n")
             print(f"Número de PDL          = {N_pdl}\n")
            
        fpga = serial_Serial(port='/dev/tty'+puerto, baudrate=baudrate, bytesize=8)
        time_sleep(.1)
        
        if verbose:
            pinta_progreso = BarraProgreso(N_osc*N_rep*N_pdl)
            
        medidas=[]
        for rep in range(N_rep):
            for pdl in range(N_pdl):
                for osc in range(N_osc):
                    buffer_in = buffer_sel_ro[osc]+buffer_sel_pdl[pdl]+buffer_sel_resol
                    scan(fpga, buffer_in, self.buffer_in_width)
                    medida = bitstr_to_int(calc(fpga, self.buffer_out_width))
                    if f_ref:
                        medida*=f_ref/2**resol
                    medidas.append(medida)
                if verbose:
                    pinta_progreso(N_osc)
        fpga.close()
        
        tensor_medidas = Tensor(array=np_reshape(medidas, (N_rep,N_pdl,N_osc)), axis=['rep','pdl','osc'])
        
        return tensor_medidas
        
        
class GaloisMatrix:
    """
    Objeto que contiene una matriz de osciladores de anillo de Galois.
    """
    def __init__(self, N_inv=3, dominios=Dominio(), bel='', pin='', modo='min', trng=0):
        """    
        Inicialización del objeto 'StdMatrix'.

        Parámetros:
        ---------
            N_inv : <int>
                Número de inversores de cada oscilador.

            dominios : <objeto 'Dominio' o lista de estos>
                Osciladores que forman la matriz. Se construye como una lista de 
                objetos 'Dominio'. Si solo pasamos un dominio de osciladores podemos
                pasar un objeto 'Dominio', en lugar de una lista.
                
            bel : <caracter o lista de caracteres> (opcional)
                Dado que todos los anillos de la matriz son idénticos 
                por diseño, esta opción es la misma que la aplicada para
                un solo oscilador (ver 'bel' en 'GaloisRing').
                    
            pin : <cadena de caracteres o lista de cadenas> (opcional)
                Dado que todos los anillos de la matriz son idénticos 
                por diseño, esta opción es la misma que la aplicada para
                un solo oscilador (ver 'pin' en 'GaloisRing').
                
            modo : <string>
                Esta opción determina el modo de implementación de las LUT del
                anillo:
                    'min'
                        Minimalista, se utilizan modelos LUT1 para los inversores y
                        LUT2 el enable AND.
                    
                    'config'
                        Se utilizan modelos LUT6 para los inversores, permitiendo
                        utilizar 5 puertos para configurar el anillo mediante PDL.
                
            trng: <int positivo>
                Si esta variable se indica con un número positivo, el diseño incluirá un módulo para guardar los bits aleatorios
                generados por cada oscilador, y los enviará junto con el dato de sesgo durante el proceso de medida. El número 
                de bits empleados para el sesgo sigue siendo indicado por la variable 'buffer_out_width' de la función
                'implement' como siempre; el número de esta variable será el número de bits aleatorios almacenados. En caso de 
                utilizar esta opción, la función 'medir' devolverá dos números: el primero será el sesgo, y el segundo la 
                secuencia de bits aleatorios dados como una string de ceros y unos.
        """
        self.dominios=[]
        if type(dominios) == type([]) or type(dominios) == type(()):
            self.dominios[:]=dominios[:]
        else:
            self.dominios.append(dominios)
            
        self.N_inv = N_inv
            
        self.bel = ['' for i in range(N_inv+2)]
        if type(bel)==type([]):
            for i in range(len(bel)):
                self.bel[i] = bel[i]
        else:
            self.bel[0] = bel
        
        self.pin = ['' for i in range(N_inv+1)]
        if type(pin)==type([]):
            for i in range(len(pin)):
                self.pin[i] = pin[i]
        else:
            self.pin[0] = pin

        self.modo = modo
        self.trng = trng
        self.osc_list = []
        self.N_osc=0
        for dominio in self.dominios:
            for osc_coord in dominio.osc_coord:
                self.osc_list.append(GaloisRing(f"{self.N_osc}", self.N_inv, osc_coord, self.bel, self.pin, self.modo))
                self.N_osc+=1
                
        self.N_bits_osc = clog2(self.N_osc)
        self.N_bits_poly = self.N_inv-1 # El primer inversor no se puede modificar (es una puerta NOT).
        self.N_bits_resol = 5  
        self.N_bits_fdiv = 5
        
        if self.modo=='min':
            self.N_bits_pdl = 0
        elif self.modo=='config':
            self.N_bits_pdl = 3
        
    def help(self):
        """ 
        Ayuda de la clase 'GaloisMatrix'.
        """
        help('myfpga.ring_osc.GaloisMatrix')
        
    def save(self, file_name):
        """
        'Wrapper' para guardar objetos serializados con el módulo 'pickle'.
        """
        with open(file_name, "wb") as f:
            pickle_dump(self, f)
        
    def gen_garomatrix(self, out_name="garomatrix.v"):
        """
        Genera un diseño 'out_name' en formato Verilog con la implementación de 
        los dominios introducidos durante la inicialización del objeto. El 
        principal uso de esta función es dentro de la función 'implement()'.

        Parámetros:
        -----------
            out_name : <string> (opcional)
                Nombre del fichero de salida.
        """
        with open(out_name, "w") as f:
            f.write(f"//N_osciladores de Galois: {self.N_osc}\n\n")
            
            f.write("module GAROMATRIX (\n")
            f.write("    input clock,\n")
            f.write("    input clock_s,\n")
            f.write(f"    input[{self.N_inv-2}:0] sel_poly,\n")
            
            if self.N_osc > 1:
                f.write(f"    input[{clog2(self.N_osc)-1}:0] sel_ro,\n")
            if self.modo=='config':
                f.write(f"    input[2:0] sel_pdl,\n")     
            f.write("    output out\n")
            f.write("    );\n\n")
            
            f.write(f"    wire[{self.N_osc-1}:0] out_ro;\n")
            f.write(f"    wire[{self.N_osc-1}:0] out_sampled;\n\n")
            
            for i in range(self.N_osc):
                f.write(f"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[{self.N_inv-1}:0] w_{i};\n")
            f.write("\n")
            if self.N_osc > 1:
                f.write("    assign out = out_sampled[sel_ro];\n\n")
            else:
                f.write("    assign out = out_sampled;\n\n")
            
            for osc in self.osc_list:
                for lut in osc.elements:
                    f.write(f"    {lut.impl()}\n")
                f.write("\n")
            f.write("endmodule\n")
            
    def implement(self, projname="project_garomatrix", projdir='.', njobs=4, 
                  linux=False, files=True, board='pynqz2', qspi=False, routing=False, 
                  pblock=False, data_width=32, buffer_out_width=32):
        """
        Copia en el directorio 'projdir' todos los archivos necesarios para 
        implementar una matriz de osciladores de anillo de Galois con medición 
        del sesgo ("bias") y comunicación pc <-> microprocesador <-> FPGA.

        Parámetros:
        ---------
            projname : <string>
                Nombre del proyecto de Vivado.
            
            projdir : <string>
                Directorio donde se creará el proyecto de Vivado y las fuentes (por
                defecto el directorio de trabajo actual).
            
            njobs : <int>
                Número de núcleos que utiilizará Vivado paralelamente para la 
                síntesis/implementación.

            linux : <bool>
                Marcar esta opción como True si la versión de Vivado que se va a
                utilizar está instalado en una máquina linux (de forma que los 
                PATH se gestionen correctamente).

            debug : <bool>
                Si "True", se implementará una matriz de divisores de reloj de 
                frecuencia conocida, lo que permite depurar el diseño al conocer
                qué resultados deben salir.
                
            files : <bool>
                    Si "True", pinta los archivos necesarios para implementar la 
                    matriz en FPGA. Esta opción se puede desactivar (False) cuando 
                    queremos configurar un objeto tipo 'Romatrix' pero no vamos a 
                    implementarla físicamente (por ejemplo porque ya lo hemos hecho
                    y solo queremos medir, o vamos a simularla sin realizarla).
                    
            board : <string> 
                Placa de desarrollo utilizada en el proyecyo. Las opciones 
                soportadas son: 'pynqz2', 'zybo', 'cmoda7_15t' o 'cmoda7_35t'.
                
            qspi : <bool>
                Si "True", el flujo de diseño incluirá el guardado del bitstream
                en la memoria flash de la placa para que se auto-programe al 
                encenderse.
                
            routing : <bool> 
                Si "True", el flujo de diseño incluirá el cableado de los 
                inversores después de la síntesis. Esto aumenta las 
                probabilidades de que la herramienta haga un cableado idéntico,
                pero es recomendable comprobarlo. (NOTA: no tengo garantías de 
                que esta opción sea del todo compatible con -qspi).
                
            pblock : <string>
                Se añade una restricción PBLOCK para el módulo TOP (i.e., todos los
                elementos auxiliares de la matriz de osciladores se colocarán 
                dentro de este espacio). El formato de esta opción es un string que
                consta de dos puntos separados por un espacio, donde cada punto 
                está dado por un par de coordenadas X,Y separadas por una coma; 
                estos  representan respectivamente las esquinas inferior izda. y 
                superior dcha. del rectángulo de restricción: 'X0,Y0 X1,Y1'. El 
                diseñador es responsable de que la FPGA utilizada disponga de 
                recursos suficientes en el bloque descrito.

            data_width : <int>
                Esta opción especifica la anchura del canal de datos PS<-->PL.

            buffer_out_width : <int>
                Esta opción especifica la anchura de la palabra de respuesta 
                (i.e., de la medida).
        """
        self.board = board
        self.qspi = qspi
        self.routing = routing
        self.pblock = pblock
        self.data_width = data_width
        self.buffer_out_width = buffer_out_width + self.trng
        
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
            
        self.buffer_in_width = self.N_bits_osc+self.N_bits_pdl+self.N_bits_poly+self.N_bits_resol+self.N_bits_fdiv
        
        if files:
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
                
            ## projdir
            wdir = projdir
            if projdir=='.':
                projdir = os_environ['PWD']
            if not linux:
                projdir = subprocess_run(["wslpath","-w",projdir], capture_output=True, text=True).stdout.replace("\\","/").replace("\n","")


            ## block design
            subprocess_run(["mkdir",f"{wdir}/block_design"])
            if self.board == "cmoda7_15t":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_15t.tcl",f"{wdir}/block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_15t.tcl",f"{wdir}/block_design/bd_design_1.tcl"])

            elif self.board == "cmoda7_35t":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_35t.tcl",f"{wdir}/block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_35t.tcl",f"{wdir}/block_design/bd_design_1.tcl"])

            elif self.board == "zybo":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_zybo.tcl",f"{wdir}/block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_zybo.tcl",f"{wdir}/block_design/bd_design_1.tcl"])

            elif self.board == "pynqz2":
                if self.qspi:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_qspi_pynqz2.tcl",f"{wdir}/block_design/bd_design_1.tcl"])
                else:
                    subprocess_run(["cp",f"{os_environ['REPO_fpga']}/tcl/bd_interfaz_pynqz2.tcl",f"{wdir}/block_design/bd_design_1.tcl"])

            ## partial flows (tcl)
            subprocess_run(["mkdir",f"{wdir}/partial_flows"])

            vivado_files=f"{projdir}/vivado_src/top.v {projdir}/vivado_src/interfaz_pspl.cp.v {projdir}/vivado_src/garomatrix.v {projdir}/vivado_src/medidor_bias.cp.v {projdir}/vivado_src/clock_divider.cp.v {projdir}/vivado_src/interfaz_pspl_config.vh"
            if self.trng:
                vivado_files+=f" {projdir}/vivado_src/bit_pool.cp.v"
            if self.pblock:
                vivado_files+=f" {projdir}/vivado_src/pblock.xdc"

            with open(f"{wdir}/partial_flows/setupdesign.tcl", "w") as f:
                f.write(f"create_project {projname} {projdir}/{projname} -part {self.fpga_part}\n")
                f.write(f"set_property board_part {self.board_part} [current_project]\n")
                f.write(f"source {projdir}/block_design/bd_design_1.tcl\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"regenerate_bd_layout\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"add_files -norecurse {{ {vivado_files} }}\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"create_bd_cell -type module -reference TOP TOP_0\n")
                f.write(f"set_property -dict [list CONFIG.C_GPIO_WIDTH {self.data_width} CONFIG.C_GPIO2_WIDTH {self.data_width}] [get_bd_cells axi_gpio_data]\n")
                f.write(f"connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]\n")
                f.write(f"connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]\n")
                f.write(f"connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]\n")
                f.write(f"connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]\n")
                f.write(f"apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {{Clk {self.clk_name} }}  [get_bd_pins TOP_0/clock]\n")
                f.write(f"regenerate_bd_layout\n")
                f.write(f"validate_bd_design\n")
                f.write(f"save_bd_design\n")
                f.write(f"make_wrapper -files [get_files {projdir}/{projname}/{projname}.srcs/sources_1/bd/design_1/design_1.bd] -top\n")
                f.write(f"add_files -norecurse {projdir}/{projname}/{projname}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"set_property top design_1_wrapper [current_fileset]\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]\n")
                
            with open(f"{wdir}/partial_flows/genbitstream.tcl", "w") as f:
                f.write(f"if {{[file exists {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc]==1}} {{\n")
                f.write(f"    export_ip_user_files -of_objects  [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc] -no_script -reset -force -quiet\n")
                f.write(f"    remove_files  -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc\n")
                f.write(f"}}\n")
                f.write(f"if {{[file exists {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]==1}} {{\n")
                f.write(f"    export_ip_user_files -of_objects  [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc] -no_script -reset -force -quiet\n")
                f.write(f"    remove_files  -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc\n")
                f.write(f"}}\n")
                f.write(f"if {{[file exists {projdir}/{projname}/{projname}.runs/synth_1]==1}} {{\n")
                f.write(f"    reset_run synth_1\n")
                f.write(f"}}\n")
                f.write(f"update_compile_order -fileset sources_1\n")
                f.write(f"update_module_reference design_1_TOP_0_0\n")
                f.write(f"launch_runs synth_1 -jobs {njobs}\n")
                f.write(f"wait_on_run synth_1\n")

                if self.qspi == 1 and self.board == "cmoda7_15t":
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1\n")
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new\n")
                    f.write(f"close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc w ]\n")
                    f.write(f"add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc\n")
                    f.write(f"set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]\n")
                    f.write(f"set_property used_in_synthesis false [get_files  {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]\n")
                    f.write(f"update_compile_order -fileset sources_1\n")
                    f.write(f"update_module_reference design_1_TOP_0_0\n")
                    f.write(f"open_run synth_1 -name synth_1\n")
                    f.write(f"set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]\n")
                    f.write(f"set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]\n")
                    f.write(f"set_property config_mode SPIx4 [current_design]\n")
                    f.write(f"save_constraints\n")
                    f.write(f"close_design\n")
                    
                elif self.qspi == 1 and self.board == "cmoda7_35t":
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1\n")
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new\n")
                    f.write(f"close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc w ]\n")
                    f.write(f"add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc\n")
                    f.write(f"set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]\n")
                    f.write(f"set_property used_in_synthesis false [get_files {projdir}/{projname}/{projname}.srcs/constrs_1/new/bitstreamconfig.xdc]\n")
                    f.write(f"update_compile_order -fileset sources_1\n")
                    f.write(f"update_module_reference design_1_TOP_0_0\n")
                    f.write(f"open_run synth_1 -name synth_1\n")
                    f.write(f"set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]\n")
                    f.write(f"set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]\n")
                    f.write(f"set_property config_mode SPIx4 [current_design]\n")
                    f.write(f"save_constraints\n")
                    f.write(f"close_design\n")
                    
                if self.routing:
                    f.write(f"open_run synth_1 -name synth_1\n\n")
                    
                    for i in range(self.N_osc):
                        for j in range(self.N_inv):
                            f.write(f"route_design -nets [get_nets design_1_i/TOP_0/inst/garomatrix/ring{i}_w{j}]\n")
                            f.write(f"set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/garomatrix/ring{i}_w{j} }}]\n\n")
                            
                        f.write(f"route_design -nets [get_nets design_1_i/TOP_0/inst/garomatrix/ring{i}_out]\n")
                        f.write(f"set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/garomatrix/ring{i}_out }}]\n\n")
                        
                    f.write(f"file mkdir {projdir}/{projname}/{projname}.srcs/constrs_1/new\n")
                    f.write(f"close [ open {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc w ]\n")
                    f.write(f"add_files -fileset constrs_1 {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc\n")
                    f.write(f"set_property target_constrs_file {projdir}/{projname}/{projname}.srcs/constrs_1/new/routing.xdc [current_fileset -constrset]\n")
                    f.write(f"save_constraints -force\n")
                    f.write(f"close_design\n")
                    f.write(f"update_compile_order -fileset sources_1\n")
                    f.write(f"update_module_reference design_1_TOP_0_0\n")# Aqui termina detailed routing
                    
                f.write(f"if {{[file exists {projdir}/{projname}/{projname}.runs/synth_1/__synthesis_is_complete__]==1}} {{\n")
                f.write(f"    reset_run synth_1\n")
                f.write(f"}}\n")                    
                f.write(f"launch_runs impl_1 -to_step write_bitstream -jobs {njobs}\n")
                f.write(f"wait_on_run impl_1\n")
                
            with open(f"{wdir}/partial_flows/launchsdk.tcl", "w") as f:
                f.write(f"file mkdir {projdir}/{projname}/{projname}.sdk\n")
                f.write(f"file copy -force {projdir}/{projname}/{projname}.runs/impl_1/design_1_wrapper.sysdef {projdir}/{projname}/{projname}.sdk/design_1_wrapper.hdf\n")
                f.write(f"launch_sdk -workspace {projdir}/{projname}/{projname}.sdk -hwspec {projdir}/{projname}/{projname}.sdk/design_1_wrapper.hdf\n")

            with open(f"{wdir}/mkhwdplatform.tcl", "w") as f:
                f.write(f"source {projdir}/partial_flows/setupdesign.tcl\n")
                f.write(f"source {projdir}/partial_flows/genbitstream.tcl\n")
                f.write(f"source {projdir}/partial_flows/launchsdk.tcl\n")

            ## vivado sources
            subprocess_run(["mkdir",f"{wdir}/vivado_src"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/interfaz_pspl.v",f"{wdir}/vivado_src/interfaz_pspl.cp.v"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/medidor_bias.v",f"{wdir}/vivado_src/medidor_bias.cp.v"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/clock_divider.v",f"{wdir}/vivado_src/clock_divider.cp.v"])
            if self.trng:
                subprocess_run(["cp",f"{os_environ['REPO_fpga']}/verilog/bit_pool.v",f"{wdir}/vivado_src/bit_pool.cp.v"])

            with open(f"{wdir}/vivado_src/interfaz_pspl_config.vh", "w") as f:
                f.write(f"{dw_ge_biw}\n")
                f.write(f"{biw_aligned_dw}\n")
                f.write(f"{biw_misaligned_dw}\n")
                f.write(f"{dw_ge_bow}\n")
                f.write(f"{bow_aligned_dw}\n")
                f.write(f"{bow_misaligned_dw}\n")

            self.gen_garomatrix(out_name=f"{wdir}/vivado_src/garomatrix.v")

            with open(f"{wdir}/vivado_src/top.v", "w") as f:
                if self.N_osc==1:
                    aux=""
                else:
                    aux=f".sel_ro(buffer_in[{self.N_bits_osc-1}:0]),"
                
                if self.modo=='config':
                    aux1=f".sel_pdl(buffer_in[{self.N_bits_osc+self.N_bits_pdl-1}:{self.N_bits_osc}]),"
                else:
                    aux1=""
                    
                f.write(f"module TOP (\n")
                f.write(f"    input           clock,\n")
                f.write(f"    input[7:0]      ctrl_in,\n")
                f.write(f"    output[7:0]     ctrl_out,\n")
                f.write(f"    input[{self.data_width-1}:0] data_in,\n")
                f.write(f"    output[{self.data_width-1}:0]    data_out\n")
                f.write(f");\n\n")
                
                f.write(f"    wire[{self.buffer_in_width-1}:0] buffer_in;\n")
                f.write(f"    wire[{self.buffer_out_width-1}:0] buffer_out;\n")
                f.write(f"    wire sync;\n")
                f.write(f"    wire ack;\n")
                f.write(f"    wire out_ro;\n")
                f.write(f"    wire clock_s;\n")
                f.write(f"    reg enable_medidor=0;\n")
                f.write(f"    wire lock;\n")
                if self.trng:
                    f.write(f"    wire full;\n\n")
                    
                    f.write(f"    assign ack = lock && full;\n\n")
                else:
                    f.write(f"\n    assign ack = lock;\n\n")
                
                f.write(f"    always @(posedge clock) begin\n")
                f.write(f"        case ({{sync,ack}})\n")
                f.write(f"            2'b10:\n")
                f.write(f"                enable_medidor <= 1;\n")
                f.write(f"                \n")
                f.write(f"            default:\n")
                f.write(f"                enable_medidor <= 0;\n")
                f.write(f"        endcase\n")
                f.write(f"    end\n\n")
                
                f.write(f"    INTERFAZ_PSPL #(\n")
                f.write(f"        .DATA_WIDTH({self.data_width}),\n")
                f.write(f"        .BUFFER_IN_WIDTH({self.buffer_in_width}),\n")
                f.write(f"        .BUFFER_OUT_WIDTH({self.buffer_out_width})\n")
                f.write(f"    ) interfaz_pspl (\n")
                f.write(f"        .clock(clock),\n")
                f.write(f"        .ctrl_in(ctrl_in),\n")
                f.write(f"        .ctrl_out(ctrl_out),\n")
                f.write(f"        .data_in(data_in),\n")
                f.write(f"        .data_out(data_out),\n")
                f.write(f"        .sync(sync),\n")
                f.write(f"        .ack(ack),\n")
                f.write(f"        .buffer_in(buffer_in),\n")
                f.write(f"        .buffer_out(buffer_out)\n")
                f.write(f"    );\n\n")
                
                f.write(f"    CLOCK_DIVIDER clock_divider (\n")
                f.write(f"        .clock_in(clock),\n")
                f.write(f"        .fdiv(buffer_in[{self.buffer_in_width-1}:{self.buffer_in_width-5}]),\n")
                f.write(f"        .clock_out(clock_s)\n")
                f.write(f"    );\n\n")
                
                f.write(f"    GAROMATRIX garomatrix (\n")
                f.write(f"        .clock(clock),\n")
                f.write(f"        .clock_s(clock_s),\n")
                f.write(f"        .sel_poly(buffer_in[{self.N_bits_osc+self.N_bits_pdl+self.N_bits_poly-1}:{self.N_bits_osc+self.N_bits_pdl}]),\n")
                f.write(f"        {aux}\n")
                f.write(f"        {aux1}\n")
                f.write(f"        .out(out_ro)\n")
                f.write(f"    );\n\n")
                
                f.write(f"    MEDIDOR_BIAS #(\n")
                f.write(f"        .OUT_WIDTH({self.buffer_out_width-self.trng})\n")
                f.write(f"    ) medidor_bias (\n")
                f.write(f"        .clock(clock_s),\n")
                f.write(f"        .enable(enable_medidor),\n")
                f.write(f"        .muestra(out_ro),\n")
                f.write(f"        .resol(buffer_in[{self.N_bits_osc+self.N_bits_pdl+self.N_bits_poly+self.N_bits_resol-1}:{self.N_bits_osc+self.N_bits_pdl+self.N_bits_poly}]),\n")
                f.write(f"        .lock(lock),\n")
                f.write(f"        .out(buffer_out[{self.buffer_out_width-self.trng-1}:0])\n")
                f.write(f"    );\n\n")
                
                if self.trng:
                    f.write(f"    BIT_POOL #(\n")
                    f.write(f"        .POOL_WIDTH({self.trng})\n")
                    f.write(f"    ) bit_pool (\n")
                    f.write(f"        .clock(clock_s),\n")
                    f.write(f"        .enable(enable_medidor),\n")
                    f.write(f"        .muestra(out_ro),\n")
                    f.write(f"        .full(full),\n")
                    f.write(f"        .out(buffer_out[{self.buffer_out_width-1}:{self.buffer_out_width-self.trng}])\n")
                    f.write(f"    );\n\n")                
                
                f.write(f"endmodule\n")
                
            if self.pblock:
                PBLOCK_CORNERS=self.pblock.split()
                PBLOCK_CORNER_0=PBLOCK_CORNERS[0].split(',')
                PBLOCK_CORNER_1=PBLOCK_CORNERS[1].split(',')
                with open(f"{wdir}/vivado_src/pblock.xdc", "w") as f:
                    f.write(f"create_pblock pblock_TOP_0\n")
                    f.write(f"add_cells_to_pblock [get_pblocks pblock_TOP_0] [get_cells -quiet [list design_1_i/TOP_0]]\n")
                    f.write(f"resize_pblock [get_pblocks pblock_TOP_0] -add {{SLICE_X{PBLOCK_CORNER_0[0]}Y{PBLOCK_CORNER_0[1]}:SLICE_X{PBLOCK_CORNER_1[0]}Y{PBLOCK_CORNER_1[1]}}}\n")

            ## sdk sources
            subprocess_run(["mkdir",f"{wdir}/sdk_src"])
            subprocess_run(["cp",f"{os_environ['REPO_fpga']}/c-xilinx/sdk/interfaz_pcps-pspl.c",f"{wdir}/sdk_src/interfaz_pcps-pspl.cp.c"])

            with open(f"{wdir}/sdk_src/interfaz_pcps-pspl_config.h", "w") as f:
                if self.board == "cmoda7_15t" or self.board == "cmoda7_35t":
                    f.write("#include \"xuartlite.h\"\n\n")
                elif self.board == "zybo" or self.board == "pynqz2":
                    f.write("#include \"xuartps.h\"\n\n")

                f.write(f"#define DATA_WIDTH          {self.data_width}\n")
                f.write(f"#define BUFFER_IN_WIDTH     {self.buffer_in_width}\n")
                f.write(f"#define BUFFER_OUT_WIDTH    {self.buffer_out_width}\n")
                f.write(f"#define OCTETO_IN_WIDTH     {octeto_in_width}\n")
                f.write(f"#define OCTETO_OUT_WIDTH    {octeto_out_width}\n")
                f.write(f"#define WORDS_IN_WIDTH      {words_in_width}\n")
                f.write(f"#define WORDS_OUT_WIDTH     {words_out_width}\n")

            ## log info
            print(f"N_osc       = {self.N_osc}")
            print(f"N_bits_osc  = {self.N_bits_osc}\n")
            print(f"N_pdl       = {2**self.N_bits_pdl}")
            print(f"N_bits_pdl  = {self.N_bits_pdl}\n")
            print(f"N_bits_poly = {self.N_bits_poly}\n")
            print(f"N_bits_resol= {self.N_bits_resol}\n")
            print(f"N_bits_fdiv = {self.N_bits_fdiv}\n")
            print(f"data_width      = {self.data_width}")
            print(f"buffer_in_width = {self.buffer_in_width}")
            print(f"buffer_out_width= {self.buffer_out_width}\n")
            print("Trama de datos:")
            print(f"    (0) <---sel_ro({self.N_bits_osc})---><---sel_pdl({self.N_bits_pdl})---><---sel_poly({self.N_bits_poly})---><---sel_resol({self.N_bits_resol})---><---sel_fdiv({self.N_bits_fdiv})---> ({self.buffer_in_width-1})\n\n")
            
            print(f"fpga part: {self.fpga_part}\n")
            
            print(f"sdk source files: {projdir}/sdk_src/")
            
            if self.qspi == 1:
                print(f"q-spi part: {self.memory_part}")
            
    def medir(self, puerto='S1', osc=[0], pdl=[0], N_rep=1, resol=17, poly=0, fdiv=3, 
              bias=False, log=False, verbose=True, baudrate=9600):
        """
        Esta función mide la frecuencia de una matriz de osciladores de Galois 
        'GaloisMatrix', una vez esta ha sido implementado en FPGA. El resultado
        se devuelve como un objeto 'Tensor'.
        
        Parámetros:
        ---------
            puerto : <string>
                Esta opción especifica el puerto serie al que se conecta la 
                FPGA. Actualmente está "hardcodeado" a formato linux (ya que 
                incluso en Windows lo estoy midiendo con WSL).
                
            osc : <int o lista de int>
                Lista de osciladores a medir.
                
            pdl : <int o lista de int>
                Lista de PDL a medir.
                
            N_rep : <int>
                Número de repeticiones a medir.
                
            resol : <int>
                log_2 del número de ciclos de referencia a completar para dar 
                por terminada la medida (por defecto 17, i.e., 2^17 = 131072 
                ciclos).
                
            poly : <int>
                Esta variable indica el índice del polinomio a medir.
                
            fdiv : <int>
                log_2 del factor de división para el reloj de muestreo.
                
            bias : <bool>
                Si se pasa esta opción como "True" el resultado se dará en 
                tanto por 1.
                
            log : <bool>
                Si se pasa "True" se escriben algunos datos a modo de log.
                
            verbose : <bool>
                Si se pasa "True" se pinta una barra de progreso de la medida. 
                Desactivar esta opción ("False") hace más cómodo utilizar esta 
                función en un bucle.
                
            baudrate : <int>
                Tasa de transferencia del protocolo serie UART PC<-->PS. Debe 
                concordar con el programa compilador en PS.
        """        
        def print_unicode_exp(numero):
            """
            Esta función toma un número y lo escribe como un exponente utilizando 
            caracteres UNICODE.
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

        def poly_nomenclature(entrada):
            """
            Esta función toma un array 'poly' tal y como se utilizando en este 
            script para representar un polinomio de Galois, y lo escribe en forma 
            de polinomio tal y como se escribe en el paper de Golic.
            """
            N=len(entrada)+1
            result="1"
            for i in range(1, N):
                if entrada[-i]:
                    if i==1:
                        result+="+x"
                    else:
                        result+=f"+x{print_unicode_exp(i)}"
            if N==1:
                result+="+x"
            else:
                result+=f"+x{print_unicode_exp(N)}"
            return result

        if type(osc)==type([]):
            osc_list = osc[:]
        else:
            osc_list = [osc]
            
        if type(pdl)==type([]):
            pdl_list = pdl[:]
        else:
            pdl_list = [pdl]
                
        N_osc = len(osc_list) # Número de osciladores
        N_pdl = len(pdl_list) # Número de pdl_list
        
        buffer_sel_ro=[]
        for i in osc_list:
            buffer_sel_ro.append(resize_array(int_to_bitstr(i), self.N_bits_osc))
            
        buffer_sel_pdl=[]
        for i in pdl_list:
            buffer_sel_pdl.append(resize_array(int_to_bitstr(i), self.N_bits_pdl))
            
        buffer_sel_resol=resize_array(int_to_bitstr(resol), self.N_bits_resol)
        
        buffer_sel_poly=resize_array(int_to_bitstr(poly), self.N_bits_poly)
        
        buffer_sel_fdiv=resize_array(int_to_bitstr(fdiv), self.N_bits_fdiv)

        ## Info log
        if log:
            print(f"INFO LOG:\n\n")
            
            print(f"Resolución             = {resol}\n")
            print(f"Polinomio              = {poly} --> {poly_nomenclature(buffer_sel_poly)}\n")
            print(f"Factor divisor clk.    = {2**fdiv}\n")
            print(f"Número de osciladores  = {N_osc}\n")
            print(f"Número de repeticiones = {N_rep}\n")
            print(f"Número de pdl_list     = {N_pdl}\n")
            
        fpga = serial_Serial(port='/dev/tty'+puerto, baudrate=baudrate, bytesize=8)
        time_sleep(.1)
        
        if verbose:
            pinta_progreso = BarraProgreso(N_osc*N_rep*N_pdl)
            
        medidas_sesgo=[]
        if self.trng:
            medidas_trng=[[[] for j in range(N_pdl)] for i in range(N_rep)]
            
        for rep in range(N_rep):
            for pdl in range(N_pdl):
                for osc in range(N_osc):
                    buffer_in = buffer_sel_ro[osc]+buffer_sel_pdl[pdl]+buffer_sel_poly+buffer_sel_resol+buffer_sel_fdiv
                    scan(fpga, buffer_in, self.buffer_in_width)
                    medida = calc(fpga, self.buffer_out_width)
                    if self.trng:
                        medida_sesgo = bitstr_to_int(medida[:self.buffer_out_width-self.trng])
                        medida_trng = medida[self.buffer_out_width-self.trng:]
                    else:
                        medida_sesgo = bitstr_to_int(medida)
                        
                    if bias:
                        medida_sesgo/=2**resol
                        
                    medidas_sesgo.append(medida_sesgo)
                    if self.trng:
                        medidas_trng[rep][pdl].append(medida_trng)
                        
                if verbose:
                    pinta_progreso(N_osc)
        fpga.close()
        
        tensor_medidas = Tensor(array=np_reshape(medidas_sesgo, (N_rep,N_pdl,N_osc)), axis=['rep','pdl','osc'])
        
        if self.trng:
            return tensor_medidas,medidas_trng
        
        return tensor_medidas
