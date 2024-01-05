class Lut1:
    """Modelo de FPGA-LUT de 1 entrada.
    
    :param name: Nombre de la instancia LUT.
    :type name: string
    
    :param init: Valor numérico que representa la función realizada por la LUT. La relación entrada/salida se asigna de la forma:
        I0 | Out
        ----------
        0  | init[0]
        1  | init[1]
        
    donde init[i] es el i-ésimo bit del número 'init' en representación binaria LSB (i.e., init[0] es el bit menos significativo -"más a la izda."- de init).
    :type init: string
    
    :param loc: Posición de la celda a la que pertenece la LUT en la FPGA. El formato de esta opción es un string formado por dos números enteros (coordenadas X, Y) separados por una coma.
    :type loc: string
    
    :param w_out: Nombre del cable que contiene la señal de salida de la LUT.
    :type w_out: string
    
    :param w_in: Nombre del cable que contiene la señal de entrada a la LUT. Se trata de una string con el nombres de la entrada correspondientes al pin I0.
    :type w_in: string
    
    :param bel: Restricción BEL que indica qué LUT concreta es ocupada dentro de la celda. Puede ser 'A', 'B', 'C' o 'D'.
    :type bel: char
    
    :param pin: Restricción de los pines lógicos ("I") a los pines físicos ("A") de la LUT. El formato es el mismo que el de la restricción LOCK_PINS de XDC, i.e., un string de elementos 'I<puerto lógico>:A<puerto físico>' separados por comas, donde el puertos lógico es 'I0', y los puertos físicos varían de 'A1' a 'A6'.
    :type pin: string
    """
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """Constructor"""
        self.name = name
        self.init = init
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        self.w_out = w_out
        self.w_in = w_in
        if bel=='':
            self.bel = ''
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para implementar la LUT inicializada.
        """
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT1 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in}));"
        
        
class Lut2:
    """Modelo de FPGA-LUT de 2 entradas.
    """
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """
        Inicialización del objeto 'Lut'.

        Parámetros:
        ---------
            name : <string>
                Nombre de la instancia LUT.
                
            init : <string>
                Valor numérico que representa la función realizada por la LUT. 
                La relación entrada/salida se asigna de la forma:
                    I1 I0 Out
                    0  0  init[0]
                    0  1  init[1]
                    1  0  init[2]
                    1  1  init[3]
                                        
                donde init[i] es el i-ésimo bit del número 'init' en
                representación binaria LSB (i.e., init[0] es el bit menos 
                significativo -"más a la izda."- de init).
                
            loc : <string>
                Posición de la celda a la que pertenece la LUT en la FPGA. El 
                formato de esta opción es un string formado por dos números 
                enteros (coordenadas X, Y) separados por una coma.
                
            w_out : <string>
                Nombre del cable que contiene la señal de salida de la LUT. 
                
            w_in : <lista de string>
                Nombres de los cables que contienen las señales de entrada a la 
                LUT. Se trata de una lista con los nombres de las entrdas 
                correspondientes a cada uno de los pines. Estos nombres se 
                asignan de forma correlativa: el primero corresponde a 'I0', el 
                segundo a 'I1'. Pueden darse menos de 2 nombres, pero los
                puertos que falten se fijaran a 1'b0.
                
            bel : <caracter>
                Restricción BEL que indica qué LUT concreta es ocupada dentro de 
                la celda. Puede ser 'A', 'B', 'C' o 'D'.
                
            pin : <string> 
                Restricción de los pines lógicos ("I") a los pines físicos ("A")
                de la LUT. El formato es el mismo que el de la restricción 
                LOCK_PINS de XDC, i.e., un string de elementos 
                'I<puerto lógico>:A<puerto físico>' separados por comas, donde 
                los puertos lógicos varían de 'I0' a 'I1', y los puertos físicos
                de 'A1' a 'A6'.
        """
        self.name = name
        self.init = init
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        self.w_out = w_out
        self.w_in = w_in[:]
        while len(self.w_in)<2:
            self.w_in.append("1'b0")
        if bel=='':
            self.bel = ''
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """
        Esta función devuelve un 'string' que contiene el código en lenguaje 
        Verilog necesario para implementar la LUT inicializada.
        """
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT2 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in[0]}), .I1({self.w_in[1]}));"        


class Lut3:
    """
    Modelo de FPGA-LUT de 3 entradas.
    """
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """
        Inicialización del objeto 'Lut'.

        Parámetros:
        ---------
            name : <string>
                Nombre de la instancia LUT.
                
            init : <string>
                Valor numérico que representa la función realizada por la LUT. 
                La relación entrada/salida se asigna de la forma:
                    I2 I1 I0 Out
                    0  0  0  init[0]
                    0  0  1  init[1]
                    0  1  0  init[2]
                    
                    
                    
                    1  1  1  init[7]
                    
                donde init[i] es el i-ésimo bit del número 'init' en
                representación binaria LSB (i.e., init[0] es el bit menos 
                significativo -"más a la izda."- de init).
                
            loc : <string>
                Posición de la celda a la que pertenece la LUT en la FPGA. El 
                formato de esta opción es un string formado por dos números 
                enteros (coordenadas X, Y) separados por una coma.
                
            w_out : <string>
                Nombre del cable que contiene la señal de salida de la LUT. 
                
            w_in : <lista de string>
                Nombres de los cables que contienen las señales de entrada a la 
                LUT. Se trata de una lista con los nombres de las entrdas 
                correspondientes a cada uno de los pines. Estos nombres se 
                asignan de forma correlativa: el primero corresponde a 'I0', el 
                segundo a 'I1', etc. Pueden darse menos de 3 nombres, pero los
                puertos que falten se fijaran a 1'b0.
                
            bel : <caracter>
                Restricción BEL que indica qué LUT concreta es ocupada dentro de 
                la celda. Puede ser 'A', 'B', 'C' o 'D'.
                
            pin : <string> 
                Restricción de los pines lógicos ("I") a los pines físicos ("A")
                de la LUT. El formato es el mismo que el de la restricción 
                LOCK_PINS de XDC, i.e., un string de elementos 
                'I<puerto lógico>:A<puerto físico>' separados por comas, donde 
                los puertos lógicos varían de 'I0' a 'I2', y los puertos físicos
                de 'A1' a 'A6'.
        """
        self.name = name
        self.init = init
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        self.w_out = w_out
        self.w_in = w_in[:]
        while len(self.w_in)<3:
            self.w_in.append("1'b0")
        if bel=='':
            self.bel = ''
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """
        Esta función devuelve un 'string' que contiene el código en lenguaje 
        Verilog necesario para implementar la LUT inicializada.
        """
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT3 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in[0]}), .I1({self.w_in[1]}), .I2({self.w_in[2]}));"
        
        
class Lut4:
    """
    Modelo de FPGA-LUT de 4 entradas.
    """
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """
        Inicialización del objeto 'Lut'.

        Parámetros:
        ---------
            name : <string>
                Nombre de la instancia LUT.
                
            init : <string>
                Valor numérico que representa la función realizada por la LUT. 
                La relación entrada/salida se asigna de la forma:
                    I3 I2 I1 I0 Out
                    0  0  0  0  init[0]
                    0  0  0  1  init[1]
                    0  0  1  0  init[2]
                    
                    
                    
                    1  1  1  1  init[15]
                    
                donde init[i] es el i-ésimo bit del número 'init' en
                representación binaria LSB (i.e., init[0] es el bit menos 
                significativo -"más a la izda."- de init).
                
            loc : <string>
                Posición de la celda a la que pertenece la LUT en la FPGA. El 
                formato de esta opción es un string formado por dos números 
                enteros (coordenadas X, Y) separados por una coma.
                
            w_out : <string>
                Nombre del cable que contiene la señal de salida de la LUT. 
                
            w_in : <lista de string>
                Nombres de los cables que contienen las señales de entrada a la 
                LUT. Se trata de una lista con los nombres de las entrdas 
                correspondientes a cada uno de los pines. Estos nombres se 
                asignan de forma correlativa: el primero corresponde a 'I0', el 
                segundo a 'I1', etc. Pueden darse menos de 6 nombres, pero los
                puertos que falten se fijaran a 1'b0.
                
            bel : <caracter>
                Restricción BEL que indica qué LUT concreta es ocupada dentro de 
                la celda. Puede ser 'A', 'B', 'C' o 'D'.
                
            pin : <string> 
                Restricción de los pines lógicos ("I") a los pines físicos ("A")
                de la LUT. El formato es el mismo que el de la restricción 
                LOCK_PINS de XDC, i.e., un string de elementos 
                'I<puerto lógico>:A<puerto físico>' separados por comas, donde 
                los puertos lógicos varían de 'I0' a 'I3', y los puertos físicos
                de 'A1' a 'A6'.
        """
        self.name = name
        self.init = init
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        self.w_out = w_out
        self.w_in = w_in[:]
        while len(self.w_in)<4:
            self.w_in.append("1'b0")
        if bel=='':
            self.bel = ''
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """
        Esta función devuelve un 'string' que contiene el código en lenguaje 
        Verilog necesario para implementar la LUT inicializada.
        """
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT4 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in[0]}), .I1({self.w_in[1]}), .I2({self.w_in[2]}), .I3({self.w_in[3]}));"        


class Lut6:
    """
    Modelo de FPGA-LUT de 6 entradas.
    """
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """
        Inicialización del objeto 'Lut'.

        Parámetros:
        ---------
            name : <string>
                Nombre de la instancia LUT.
                
            init : <string>
                Valor numérico que representa la función realizada por la LUT. 
                La relación entrada/salida se asigna de la forma:
                    I5 I4 I3 I2 I1 I0 Out
                    0  0  0  0  0  0  init[0]
                    0  0  0  0  0  1  init[1]
                    0  0  0  0  1  0  init[2]
                    .
                    .
                    .
                    1  1  1  1  1  1  init[63]
                    
                donde init[i] es el i-ésimo bit del número 'init' en
                representación binaria LSB (i.e., init[0] es el bit menos 
                significativo -"más a la izda."- de init).
                
            loc : <string>
                Posición de la celda a la que pertenece la LUT en la FPGA. El 
                formato de esta opción es un string formado por dos números 
                enteros (coordenadas X, Y) separados por una coma.
                
            w_out : <string>
                Nombre del cable que contiene la señal de salida de la LUT. 
                
            w_in : <lista de string>
                Nombres de los cables que contienen las señales de entrada a la 
                LUT. Se trata de una lista con los nombres de las entrdas 
                correspondientes a cada uno de los pines. Estos nombres se 
                asignan de forma correlativa: el primero corresponde a 'I0', el 
                segundo a 'I1', etc. Pueden darse menos de 6 nombres, pero los
                puertos que falten se fijaran a 1'b0.
                
            bel : <caracter>
                Restricción BEL que indica qué LUT concreta es ocupada dentro de 
                la celda. Puede ser 'A', 'B', 'C' o 'D'.
                
            pin : <string> 
                Restricción de los pines lógicos ("I") a los pines físicos ("A")
                de la LUT. El formato es el mismo que el de la restricción 
                LOCK_PINS de XDC, i.e., un string de elementos 
                'I<puerto lógico>:A<puerto físico>' separados por comas, donde 
                los puertos lógicos varían de 'I0' a 'I5', y los puertos físicos
                de 'A1' a 'A6'.
        """
        self.name = name
        self.init = init
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        self.w_out = w_out
        self.w_in = w_in[:]
        while len(self.w_in)<6:
            self.w_in.append("1'b0")
        if bel=='':
            self.bel = ''
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """
        Esta función devuelve un 'string' que contiene el código en lenguaje 
        Verilog necesario para implementar la LUT inicializada.
        """
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT6 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in[0]}), .I1({self.w_in[1]}), .I2({self.w_in[2]}), .I3({self.w_in[3]}), .I4({self.w_in[4]}), .I5({self.w_in[5]}));"
        
        
class FlipFlop:
    """
    Modelo de FPGA-FF.
    """
    def __init__(self, name, loc, w_out, w_clock, w_in, bel=''):
        """
        Inicialización del objeto 'FlipFlop'.
        
        Parámetros:
        -----------
            name : <string>
                Nombre de la instancia FF.
                
            loc : <string>
                Posición de la celda a la que pertenece el FF en la FPGA. El 
                formato de esta opción es un string formado por dos números 
                enteros (coordenadas X, Y) separados por una coma.
                
            w_out : <string>
                Nombre del cable que contiene la señal de salida del FF.
                
            w_clock : <string>
                Nombre del cable que lleva la señal de reloj al FF.                
                
            w_in : <string>
                Nombre del cable que contiene la señal de entrada al FF.
                
            bel : <caracter>
                Restrición BEL que indica qué FF concreto es ocupado dentro de 
                la celda. Puede ser 'A', 'B', 'C' o 'D'.
        """
        self.name = name
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        self.w_out = w_out
        self.w_clock = w_clock
        self.w_in = w_in
        if bel=='':
            self.bel = ''
        else:
            self.bel = f"BEL=\"{bel}FF\", "
            
    def impl(self):
        """
        Esta función devuelve un 'string' que contiene el código en lenguaje
        Verilog necesario para implementar el FF inicializado.
        """
        return f"(* {self.bel}{self.loc}, DONT_TOUCH=\"true\" *) FDCE {self.name}(.Q({self.w_out}), .C({self.w_clock}), .CE(1'b1), .CLR(1'b0), .D({self.w_in}));"
        
        
        
        
import os
aux = os.path.join(os.path.dirname(__file__),'tcl/program_fpga.tcl')
def prueba_lectura():
    with open(aux, 'r') as archivo:
        contenido = archivo.read()
        print(contenido)

