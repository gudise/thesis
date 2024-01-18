class Lut1: 
    """
    Modelo de FPGA-LUT de 1 entrada.

    Parameters
    ----------
    name : str
        Nombre de la instancia LUT.
    
    init : str
        Valor numérico que representa la función realizada por la LUT. La relación entrada/salida se asigna de la forma:
        
        +----+-----------+
        | I0 |     Out   |
        +====+===========+
        | 0  |   init[0] |
        +----+-----------+
        | 1  |   init[1] |
        +----+-----------+
        
        donde init[i] es el i-ésimo bit del número 'init' en representación binaria LSB (i.e., init[0] es el bit menos significativo -"más a la izquierda"- de init).
    
    loc : str
        Posición de la celda a la que pertenece la LUT en la FPGA. El formato de esta opción es un string formado por dos números enteros (coordenadas X, Y) separados por una coma.
    
    w_out : str
        Nombre del cable que contiene la señal de salida de la LUT.
    
    w_in : str
        Nombre del cable que contiene la señal de entrada a la LUT. Se trata de una string con el nombre de la entrada correspondiente al pin I0.
    
    bel : {'A', 'B', 'C', 'D'}
        Restricción BEL que indica qué LUT concreta es ocupada dentro de la celda.
    
    pin : str
        Restricción de los pines lógicos ("I") a los pines físicos ("A") de la LUT. El formato es el mismo que el de la restricción LOCK_PINS de XDC, i.e., un string de elementos 'I<puerto lógico>:A<puerto físico>' separados por comas, donde el puerto lógico es 'I0', y los puertos físicos varían de 'A1' a 'A6'.
    """  
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """Constructor"""
        self.name = name
        """Nombre de la instancia LUT."""
        self.init = init
        """función realizada por la LUT."""
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        """Posición de la LUT en la FPGA."""
        self.w_out = w_out
        """Señal de salida."""
        self.w_in = w_in
        """Señal de entrada."""
        if bel=='':
            self.bel = ''
            """Restricciones BEL."""
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
            """Restricciones de pines."""
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para implementar la LUT inicializada.

        Returns
        -------
        str
            Cadena que representa el código Verilog para la implementación de la LUT inicializada, con los parámetros actuales de la instancia.
        """        
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT1 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in}));"
        
        
class Lut2:
    """
    Modelo de FPGA-LUT de 2 entradas.

    Parameters
    ----------
    name : str
        Nombre de la instancia LUT.
        
    init : str
        Valor numérico que representa la función realizada por la LUT. La relación entrada/salida se asigna de la forma:
        
        +----+----+--------+
        | I1 | I0 |  Out   |
        +====+====+========+
        |  0 |  0 | init[0]|
        +----+----+--------+
        |  0 |  1 | init[1]|
        +----+----+--------+
        |  1 |  0 | init[2]|
        +----+----+--------+
        |  1 |  1 | init[3]|
        +----+----+--------+
        
        donde init[i] es el i-ésimo bit del número 'init' en representación binaria LSB (i.e., init[0] es el bit menos significativo -"más a la izquierda"- de init).
    
    loc : str
        Posición de la celda a la que pertenece la LUT en la FPGA. El formato de esta opción es un string formado por dos números enteros (coordenadas X, Y) separados por una coma.
    
    w_out : str
        Nombre del cable que contiene la señal de salida de la LUT.
    
    w_in : str
        Nombre del cable que contiene la señal de entrada a la LUT. Se trata de una string con el nombre de la entrada correspondiente al pin I0.
    
    bel : {'A', 'B', 'C', 'D'}
        Restricción BEL que indica qué LUT concreta es ocupada dentro de la celda.
    
    pin : str
        Restricción de los pines lógicos ("I") a los pines físicos ("A") de la LUT. El formato es el mismo que el de la restricción LOCK_PINS de XDC, i.e., un string de elementos 'I<puerto lógico>:A<puerto físico>' separados por comas, donde el puerto lógico es 'I0', y los puertos físicos varían de 'A1' a 'A6'.        
    """
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """Constructor"""
        self.name = name
        """Nombre de la instancia LUT."""
        self.init = init
        """función realizada por la LUT."""
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        """Posición de la LUT en la FPGA."""
        self.w_out = w_out
        """Señal de salida."""
        self.w_in = w_in[:]
        """Lista de señales de entrada."""
        while len(self.w_in)<2:
            self.w_in.append("1'b0")
        if bel=='':
            self.bel = ''
            """Restricciones BEL."""
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
            """Restricciones de pines."""
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para implementar la LUT inicializada.

        Returns
        -------
        str
            Cadena que representa el código Verilog para la implementación de la LUT inicializada, con los parámetros actuales de la instancia.
        """ 
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT2 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in[0]}), .I1({self.w_in[1]}));"        


class Lut3:
    """
    Modelo de FPGA-LUT de 3 entradas.

    Parameters
    ----------
    name : str
        Nombre de la instancia LUT.
    
    init : str
        Valor numérico que representa la función realizada por la LUT. La relación entrada/salida se asigna de la forma:
        
        +----+----+----+--------+
        | I2 | I1 | I0 |  Out   |
        +====+====+====+========+
        |  0 |  0 |  0 | init[0]|
        +----+----+----+--------+
        |  0 |  0 |  1 | init[1]|
        +----+----+----+--------+
        |  0 |  1 |  0 | init[2]|
        +----+----+----+--------+
        |  . |  . |  . |   .    |
        +----+----+----+--------+
        |  1 |  1 |  1 | init[7]|
        +----+----+----+--------+
        
        donde init[i] es el i-ésimo bit del número 'init' en representación binaria LSB (i.e., init[0] es el bit menos significativo -"más a la izquierda"- de init).
    
    loc : str
        Posición de la celda a la que pertenece la LUT en la FPGA. El formato de esta opción es un string formado por dos números enteros (coordenadas X, Y) separados por una coma.
    
    w_out : str
        Nombre del cable que contiene la señal de salida de la LUT.
    
    w_in : str
        Nombre del cable que contiene la señal de entrada a la LUT. Se trata de una string con el nombre de la entrada correspondiente al pin I0.
    
    bel : {'A', 'B', 'C', 'D'}
        Restricción BEL que indica qué LUT concreta es ocupada dentro de la celda.
    
    pin : str
        Restricción de los pines lógicos ("I") a los pines físicos ("A") de la LUT. El formato es el mismo que el de la restricción LOCK_PINS de XDC, i.e., un string de elementos 'I<puerto lógico>:A<puerto físico>' separados por comas, donde el puerto lógico es 'I0', y los puertos físicos varían de 'A1' a 'A6'.
    """
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """Constructor"""
        self.name = name
        """Nombre de la instancia LUT."""
        self.init = init
        """función realizada por la LUT."""
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        """Posición de la LUT en la FPGA."""
        self.w_out = w_out
        """Señal de salida."""
        self.w_in = w_in[:]
        """Lista de señales de entrada."""
        while len(self.w_in)<3:
            self.w_in.append("1'b0")
        if bel=='':
            self.bel = ''
            """Restricciones BEL."""
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
            """Restricciones de pines."""
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para implementar la LUT inicializada.

        Returns
        -------
        str
            Cadena que representa el código Verilog para la implementación de la LUT inicializada, con los parámetros actuales de la instancia.
        """ 
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT3 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in[0]}), .I1({self.w_in[1]}), .I2({self.w_in[2]}));"
        
        
class Lut4:
    """
    Modelo de FPGA-LUT de 4 entradas.

    Parameters
    ----------
    name : str
        Nombre de la instancia LUT.
        
    init : str
        Valor numérico que representa la función realizada por la LUT. La relación entrada/salida se asigna de la forma:
        
        +----+----+----+----+--------+
        | I3 | I2 | I1 | I0 |  Out   |
        +====+====+====+====+========+
        |  0 |  0 |  0 |  0 | init[0]|
        +----+----+----+----+--------+
        |  0 |  0 |  0 |  1 | init[1]|
        +----+----+----+----+--------+
        |  0 |  0 |  1 |  0 | init[2]|
        +----+----+----+----+--------+
        |  . |  . |  . |  . |   .    |
        +----+----+----+----+--------+
        |  1 |  1 |  1 |  1 |init[15]|
        +----+----+----+----+--------+
        
        donde init[i] es el i-ésimo bit del número 'init' en representación binaria LSB (i.e., init[0] es el bit menos significativo -"más a la izquierda"- de init).
    
    loc : str
        Posición de la celda a la que pertenece la LUT en la FPGA. El formato de esta opción es un string formado por dos números enteros (coordenadas X, Y) separados por una coma.
    
    w_out : str
        Nombre del cable que contiene la señal de salida de la LUT.
    
    w_in : str
        Nombre del cable que contiene la señal de entrada a la LUT. Se trata de una string con el nombre de la entrada correspondiente al pin I0.
    
    bel : {'A', 'B', 'C', 'D'}
        Restricción BEL que indica qué LUT concreta es ocupada dentro de la celda.
    
    pin : str
        Restricción de los pines lógicos ("I") a los pines físicos ("A") de la LUT. El formato es el mismo que el de la restricción LOCK_PINS de XDC, i.e., un string de elementos 'I<puerto lógico>:A<puerto físico>' separados por comas, donde el puerto lógico es 'I0', y los puertos físicos varían de 'A1' a 'A6'.
    """    
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """Constructor"""
        self.name = name
        """Nombre de la instancia LUT."""
        self.init = init
        """función realizada por la LUT."""
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        """Posición de la LUT en la FPGA."""
        self.w_out = w_out
        """Señal de salida."""
        self.w_in = w_in[:]
        """Lista de señales de entrada."""
        while len(self.w_in)<4:
            self.w_in.append("1'b0")
        if bel=='':
            self.bel = ''
            """Restricciones BEL."""
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
            """Restricciones de pines."""
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para implementar la LUT inicializada.

        Returns
        -------
        str
            Cadena que representa el código Verilog para la implementación de la LUT inicializada, con los parámetros actuales de la instancia.
        """ 
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT4 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in[0]}), .I1({self.w_in[1]}), .I2({self.w_in[2]}), .I3({self.w_in[3]}));"        


class Lut6:
    """
    Modelo de FPGA-LUT de 6 entradas.

    Parameters
    ----------
    name : str
        Nombre de la instancia LUT.
        
    init : str
        Valor numérico que representa la función realizada por la LUT. La relación entrada/salida se asigna de la forma:
        
        +----+----+----+----+----+----+--------+
        | I5 | I4 | I3 | I2 | I1 | I0 |  Out   |
        +====+====+====+====+====+====+========+
        |  0 |  0 |  0 |  0 |  0 |  0 | init[0]|
        +----+----+----+----+----+----+--------+
        |  0 |  0 |  0 |  0 |  0 |  1 | init[1]|
        +----+----+----+----+----+----+--------+
        |  0 |  0 |  0 |  0 |  1 |  0 | init[2]|
        +----+----+----+----+----+----+--------+
        |  . |  . |  . |  . |  . |  . |   .    |
        +----+----+----+----+----+----+--------+
        |  1 |  1 |  1 |  1 |  1 |  1 |init[63]|
        +----+----+----+----+----+----+--------+
        
        donde init[i] es el i-ésimo bit del número 'init' en representación binaria LSB (i.e., init[0] es el bit menos significativo -"más a la izquierda"- de init).
    
    loc : str
        Posición de la celda a la que pertenece la LUT en la FPGA. El formato de esta opción es un string formado por dos números enteros (coordenadas X, Y) separados por una coma.
    
    w_out : str
        Nombre del cable que contiene la señal de salida de la LUT.
    
    w_in : str
        Nombre del cable que contiene la señal de entrada a la LUT. Se trata de una string con el nombre de la entrada correspondiente al pin I0.
    
    bel : {'A', 'B', 'C', 'D'}
        Restricción BEL que indica qué LUT concreta es ocupada dentro de la celda.
    
    pin : str
        Restricción de los pines lógicos ("I") a los pines físicos ("A") de la LUT. El formato es el mismo que el de la restricción LOCK_PINS de XDC, i.e., un string de elementos 'I<puerto lógico>:A<puerto físico>' separados por comas, donde el puerto lógico es 'I0', y los puertos físicos varían de 'A1' a 'A6'.
    """
    def __init__(self, name, init, loc, w_out, w_in, bel='', pin=''):
        """Constructor"""
        self.name = name
        """Nombre de la instancia LUT."""
        self.init = init
        """función realizada por la LUT."""
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        """Posición de la LUT en la FPGA."""
        self.w_out = w_out
        """Señal de salida."""
        self.w_in = w_in[:]
        """Lista de señales de entrada."""
        while len(self.w_in)<6:
            self.w_in.append("1'b0")
        if bel=='':
            self.bel = ''
            """Restricciones BEL."""
        else:
            self.bel = f"BEL=\"{bel}6LUT\", "
        if pin=='':
            self.pin = ''
            """Restricciones de pines."""
        else:
            self.pin = f", LOCK_PINS=\"{pin}\""
            
    def impl(self):
        """Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para implementar la LUT inicializada.

        Returns
        -------
        str
            Cadena que representa el código Verilog para la implementación de la LUT inicializada, con los parámetros actuales de la instancia.
        """ 
        return f"(* {self.bel}{self.loc}{self.pin}, DONT_TOUCH=\"true\" *) LUT6 #({self.init}) {self.name}(.O({self.w_out}), .I0({self.w_in[0]}), .I1({self.w_in[1]}), .I2({self.w_in[2]}), .I3({self.w_in[3]}), .I4({self.w_in[4]}), .I5({self.w_in[5]}));"
        
        
class FlipFlop:
    """
    Modelo de FPGA-FF.

    Parameters
    ----------
    name : str
        Nombre de la instancia FF.
    
    loc : str
        Posición de la celda a la que pertenece el FF en la FPGA. El formato de esta opción es un string formado por dos números enteros (coordenadas X, Y) separados por una coma.
    
    w_out : str
        Nombre del cable que contiene la señal de salida del FF.
    
    w_clock : str
        Nombre del cable que lleva la señal de reloj al FF.
    
    w_in : str
        Nombre del cable que contiene la señal de entrada al FF.
    
    bel : {'A', 'B', 'C', 'D'}
        Restrición BEL que indica qué FF concreto es ocupado dentro de la celda.
    
    pin : str
        Restricción de los pines lógicos ("I") a los pines físicos ("A") de la LUT. El formato es el mismo que el de la restricción LOCK_PINS de XDC, i.e., un string de elementos 'I<puerto lógico>:A<puerto físico>' separados por comas, donde el puerto lógico es 'I0', y los puertos físicos varían de 'A1' a 'A6'.
    """    
    def __init__(self, name, loc, w_out, w_clock, w_in, bel=''):
        """Constructor"""
        self.name = name
        """Nombre de la instancia FF."""
        self.loc = f"LOC=\"SLICE_X{loc.replace(' ','').split(',')[0]}Y{loc.replace(' ','').split(',')[1]}\""
        """Posición del FF en la FPGA."""
        self.w_out = w_out
        """Señal de salida."""
        self.w_clock = w_clock
        """Señal de reloj."""
        self.w_in = w_in
        """Señal de entrada."""
        if bel=='':
            self.bel = ''
            """Restricciones BEL."""
        else:
            self.bel = f"BEL=\"{bel}FF\", "
            
    def impl(self):
        """
        Esta función devuelve un 'string' que contiene el código en lenguaje Verilog necesario para implementar el FF inicializada.

        Returns
        -------
        str
            Cadena que representa el código Verilog para la implementación del FF inicializado, con los parámetros actuales de la instancia.
        """ 
        return f"(* {self.bel}{self.loc}, DONT_TOUCH=\"true\" *) FDCE {self.name}(.Q({self.w_out}), .C({self.w_clock}), .CE(1'b1), .CLR(1'b0), .D({self.w_in}));"
