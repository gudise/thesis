class Lut:
    """
    Modelo de FPGA-LUT.
    """
    def __init__(self, name, tipo, init, loc, w_out, w_in, bel='', pin=''):
        """
        Opciones:
        ---------
            name = '<>'
                Nombre de la instancia LUT.
                
            tipo = 'LUT1' | 'LUT2'| 'LUT3'| 'LUT4'| 'LUT5'| 'LUT6'
                Tipo de la LUT.
                
            init
                Valor numérico que representa la función realizada por la LUT. La relación
                entrada/salida se asigna de la forma:
                    I5 I4 I3 I2 I1 I0 Out
                    0  0  0  0  0  0  init[0]
                    0  0  0  0  0  1  init[1]
                    0  0  0  0  1  0  init[2]
                    .
                    .
                    .
                    1  1  1  1  1  1  init[63]
                    
                donde init[i] es el i-ésimo bit del número 'init' en representación
                binaria LSB (i.e., init[0] es el bit menos significativo -"más a la izda."- de init).
                
            loc = '<x>,<y>'
                Posición de la celda a la que pertenece la LUT en la FPGA. 
                
            bel = 'A' | 'B' | 'C' | 'D'
                Restrición BEL que indica qué LUT concreta es ocupada dentro de la celda. Puede ser
                'A', 'B', 'C' o 'D'.
                
            pin = 'I<>:A<>, I<>:A<>, ...'
                Mapeo de los pines lógicos ("I") a los pines físicos ("A") de la LUT. El formato es 
                el mismo que el de la restricción LOCK_PINS de XDC, i.e., un string de elementos
                'I<puerto lógico>:A<puerto físico>' separados por comas, donde los puertos lógicos
                varían de 'I0' a 'I5', y los puertos físicos de 'A1' a 'A6'. Notar que la numeración
                de los puertos lógicos debe ser compatible con el tipo de LUT: una luto tipo 'LUT1'
                solo tiene un puerto lógico ('I0'), 'lut2' tiene dos puertos lógicos ('I0', 'I1'), etc.
                
            w_out = '<>'
                Nombre del cable que contiene la señal de salida de la LUT. 
                
            w_in = '<>' | ['<>', '<>', ...]
                Nombres de los cables que contienen las señales de entrada a la LUT. Se trata de una
                lista con los nombres de las entrdas correspondientes a cad auno de los pines. Estos
                nombres se asignan de forma correlativa: el primero corresponde a 'I0', el segundo a
                'I1', etc. por lo tanto, el número de elementos de esta lista está determinada por el
                tipo de LUT. Alternativamente, si tipo=LUT1, puede darse solo el nombre (sin necesidad
                de pasarlo como lista).
        """
        self.name = name
        self.tipo = tipo
        self.init = init
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        self.w_out = w_out
        if type(w_in) == type([]):
            self.w_in = w_in[:]
        else:
            self.w_in = [w_in]
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
        Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para
        implementar la LUT inicializada.
        """
        aux = ', '.join([f".I{i}({self.w_in[i]})" for i in range(len(self.w_in))])
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) {self.tipo} #({self.init}) {self.name}(.O({self.w_out}), {aux});"
        
        
class FlipFlop:
    """
    Modelo de FPGA-FF.
    """
    def __init__(self, name, loc, w_out, w_clock, w_in, bel=''):
        """
        Opciones:
        ---------
            name = '<>'
                Nombre de la instancia LUT.
                
            loc = '<x>,<y>'
                Posición de la celda a la que pertenece la LUT en la FPGA. 
                
            bel = 'A' | 'B' | 'C' | 'D'
                Restrición BEL que indica qué LUT concreta es ocupada dentro de la celda. Puede ser
                'A', 'B', 'C' o 'D'.
                
            w_out = '<>'
                Nombre del cable que contiene la señal de salida de la LUT.
                
            w_clock = '<>'
                Nombre del cable que lleva la señal de reloj al flip-flop.
                
            w_in = '<>'
                Nombres de los cables que contienen las señales de entrada a la LUT. Se trata de una
                lista con los nombres de las entrdas correspondientes a cad auno de los pines. Estos
                nombres se asignan de forma correlativa: el primero corresponde a 'I0', el segundo a
                'I1', etc. por lo tanto, el número de elementos de esta lista está determinada por el
                tipo de LUT. Alternativamente, si tipo=LUT1, puede darse solo el nombre (sin necesidad
                de pasarlo como lista).
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
        Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para
        implementar la LUT inicializada.
        """
        return f"(* {self.bel}{self.loc}, DONT_TOUCH=\"true\" *) FDCE {self.name}(.Q({self.w_out}), .C({self.w_clock}), .CE(1'b1), .CLR(1'b0), .D({self.w_in}));"

