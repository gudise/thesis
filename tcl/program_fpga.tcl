## Este scrip programa un SoC Zynq-7000 con un bitstream, y programa el micro con una aplicación software. Está diseñado para ser utilizado junto con la función 'design' del módulo 'ring_osc'. Requiere un directorio 'sdk' tal y como es creado por Vivado, con un proyecto hardware y una aplicación software. Se dan por hecho que los nombres de los proyectos y ficheros son los dados por defecto.

## Deben pasarse OBLIGATORIAMENTE dos parámetros a este script: el primero ha de ser el directorio del proyecto SDK; el segundo, el nombre de la aplicación.

set sdkdir [lindex $argv 0]

set appname [lindex $argv 1]


connect

targets 2

rst

fpga ${sdkdir}/design_1_wrapper_hw_platform_0/design_1_wrapper.bit

loadhw ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf

source ${sdkdir}/design_1_wrapper_hw_platform_0/ps7_init.tcl

ps7_init
ps7_post_config

dow ${sdkdir}/${appname}/Debug/${appname}.elf

con

disconnect