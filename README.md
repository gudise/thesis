Programas y scripts para mi tesis doctoral.


# Paquete 'myutils'
Este paquete contiene funciones auxiliares para diversos entornos.

El módulo 'stats' contiene código relacionado con el modelado estadístico, en especial 'notebooks' de Jupyter que pueden interpretarse como "documentación" interactiva. La razón de crear un repositorio y no una carpeta de 'notebooks' es facilitar el crear módulos de python reutilizables, así como un proyecto GIT.

El módulo 'tensor' contiene una reinterpretación del objeto 'nparray', diseñado para poder acceder a cada eje
a través de un diccionario con nombres significativos.

El módulo 'bool' contiene álgebra en el cuerpo binario GF(2).


# Paquete 'myfpga'
Los 'cable drivers' no se instalan automáticamente en Linux. Deben instalarse a mano corriendo el siguiente script de Xilinx ubicado en:
	[carpeta de instalación de Xilinx]/Vivado/2019.1/data/xicom/cable_drivers/lin64/install_script/install_drivers/
	
Desde esta carpeta ejecutamos ./install_drivers
	
Además, para utilizar el USB-serial desde una terminal debemos añadir nuestro usuario al grupo 'dialout' del SO (en este grupo están los usuarios con permiso de escritura/lectura sobre los puertos tty). Para ello hacemos:

	sudo adduser $USER dialout
	
En caso de que este grupo no exista (podemos verificarlo con el comando 'groups'), debemos crearlo haciendo:
	
	newgrp dialout

Para poder lanzar Vivado desde la terminal sin más que escribir el comando 'vivado', debe añadirse al PATH el directorio:
	[carpeta de instalación de Xilinx]/Vivado/2019.1/bin

La versión para Linux a veces falla con un error inesperado. Cambiando la opción synthesis/resource_sharing a 'off' puede ayudar (con el comando tcl:
		set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]

La versión de Vivado HLS para Windows falla al exportar el código RTL (en forma de IP) a menos que se cambie la hora del PC previa a 2020 (surrealista...). El parche "y2k22_patch-1.2" soluciona este problema; para instalarlo debe descomprimirse en la carpeta de instalación de Xilinx (por defecto, C\Xilinx) y seguir las instrucciones que encontramos en la carpeta descomprimida.


# Paquete 'mypuf'
Funciones para llevar a cabo el análisis de un experimento PUF.


