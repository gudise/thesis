class BarraProgreso:
    """
    Est aclase instancia un objeto "BarraProgreso".
    """
    def __init__(self, total, long_barra=40, caracter='#'):
        """
        Inicializa un objeto "BarraProgreso".
        
        Parámetros:
        -----------
            total: <int>
                Número de iteraciones que hacen el 100% de la barra.
                
            long_barra: <int>
                Número de caracteres que componen la barra.
                
            caracter: <char>
                Caracter que se utiliza para decorar la barra.
        """
        self.total = total
        self.long_barra = long_barra
        self.caracter = caracter
        self.indice = 0
                    
        print(f" |{''.join(['.' for i in range(self.long_barra)])}| {0.00:.2f} %", end='\r')
        
    def __call__(self, incr=1):
        """
        Actualiza la barra; en función de las variables 'total'
        y 'long_barra', esta actualización puede reflejarse o
        no en el dibujo.
        
        Parámetros:
        -----------
            incr: <int>
                Este valor indica en cuantas unidades debe incrementarse el contador
                de iteraciones 'indice'. Por defecto es "1", pero puede ocurrir que
                se utilice una barra de progreso en un bucle anidado y no se quiera
                actualizar con el bucle más rápido.
        """
        self.indice+=incr
        
        perone = self.indice/self.total
        n_puntos = int(self.long_barra*perone)
        
        if n_puntos==self.long_barra:
            print(f" |{''.join([self.caracter for i in range(self.long_barra)])}| {100.00:.2f} %")
        else:
            print(f" |{''.join([self.caracter for i in range(n_puntos)])+''.join(['.' for i in range(self.long_barra-n_puntos)])}| {100*perone:.2f} %", end='\r')