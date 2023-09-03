### NOTA: la idea es que este script sea generado por las funciones 'implement', para que así las variables 'projname' y 'projdir' tengan los valores correctos. Para que 'app_name' tenga también el valor correcto, debe encontrarse un script que se pueda ejecutar por xsdk y que cree la aplicación software, incluyendo la importación automática de las fuentes de SDK.


# Si el directorio 'projdir' contiene un proyecto de vivado 'projname', y en este se ha exportado el hardware y creado un proyecto SDK con una aplicación
# software de nombre 'app_name', y si hay una placa conectada, entonces este script lanzado por la aplicación 'xsdk' programa la FPGA con el bitstream y el
# micro del SoC con la aplicación software implementada. El comando xsdk para lanzar el script es: xsdk -source [este script]. Por ahora ha sido probado 
# con éxito en la placa pynq-z2, en linux.

## Variables
set projname "ropuf-pdl"
set projdir "/home/gds/Escritorio/WDIR/borrar/ropuf-pdl"
set app_name "my_app"

## Script
connect
fpga -file $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/design_1_wrapper.bit
source $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/ps7_init.tcl
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0
dow $projdir/$projname/$projname.sdk/$app_name/Debug/$app_name.elf
con
disconnect
