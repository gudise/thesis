## Este scrip programa un SoC Zynq-7000 con un bitstream, y programa el micro con una aplicación software. Está diseñado para ser utilizado junto con la función 'design' del módulo 'ring_osc'. Requiere un directorio 'sdk' tal y como es creado por Vivado, con un proyecto hardware y una aplicación software. Se dan por hecho que los nombres de los proyectos y ficheros son los dados por defecto.

## Deben pasarse OBLIGATORIAMENTE dos parámetros a este script: el primero ha de ser el directorio del proyecto SDK; el segundo, el nombre de la aplicación.

set sdkdir [lindex $argv 0]

set appname [lindex $argv 1]


::hsi::utils::get_hw_files_on_hw ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf bit

::hsi::utils::get_addr_tag_info -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf 

::hsi::utils::get_all_periphs -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf

::hsi::utils::get_design_properties -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf

::hsi::utils::get_bram_blocks ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf 

connect -url tcp:127.0.0.1:3121

jtag targets -filter {level == 0}

jtag targets -filter {level == 0}

jtag targets -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==1}

targets -set -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==0} -index 1

fpga -file ${sdkdir}/design_1_wrapper_hw_platform_0/design_1_wrapper.bit

::hsi::utils::get_bram_blocks ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf 

 ::hsi::utils::set_current_hw_sw ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ${sdkdir}/app_bsp/system.mss 

::hsi::utils::get_hw_sw_details -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ${sdkdir}/app_bsp/system.mss

::hsi::utils::closesw ${sdkdir}/app_bsp/system.mss

::hsi::utils::get_ps_config_params -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf

::hsi::utils::get_all_register_data -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0

::hsi::utils::get_addr_ranges -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0 C_DEBUG_ENABLED

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0 C_DEBUG_EVENT_COUNTERS

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0 C_DEBUG_LATENCY_COUNTERS

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0 C_DEBUG_COUNTER_WIDTH

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0 C_FREQ

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0 C_CPU_CLK_FREQ_HZ

::hsi::utils::get_cpu_nr -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf

::hsi::utils::get_all_register_data -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_1

::hsi::utils::get_addr_ranges -json ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_1

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_1 C_DEBUG_ENABLED

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_1 C_DEBUG_EVENT_COUNTERS

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_1 C_DEBUG_LATENCY_COUNTERS

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_1 C_DEBUG_COUNTER_WIDTH

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_1 C_FREQ

::hsi::utils::get_hw_param_value ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_1 C_CPU_CLK_FREQ_HZ

::hsi::utils::get_connected_periphs ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0

connect -url tcp:127.0.0.1:3121

jtag targets -filter {level == 0}

jtag targets -filter {level == 0}

jtag targets -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==1}

targets -set -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==0} -index 1

fpga -state

connect -url tcp:127.0.0.1:3121

jtag targets -filter {level == 0}

version -server

version

jtag targets -filter {level == 0}

jtag targets -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==1}

jtag targets -filter {level == 0}

jtag targets -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==1}

jtag targets -filter {level == 0}

jtag targets -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==1}

jtag targets -filter {level == 0}

jtag targets -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==1}

jtag targets -filter {level == 0}

jtag targets -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==1}

jtag targets -filter {level == 0}

jtag targets -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==1}

jtag targets -set -filter {name =~ "Xilinx TUL 1234-tulA" && level == 0}

jtag frequency

source ${sdkdir}/design_1_wrapper_hw_platform_0/ps7_init.tcl

targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0

loadhw -hw ${sdkdir}/design_1_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]

configparams force-mem-access 1

targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0

stop

ps7_init

ps7_post_config

targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0

rst -processor

targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0

dow ${sdkdir}/${appname}/Debug/${appname}.elf

configparams force-mem-access 0

targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0

con

disconnect tcfchan#1

