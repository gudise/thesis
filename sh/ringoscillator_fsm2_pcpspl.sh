PROJNAME="project_ringoscillatorfsm2"
PROJDIR="./"
NJOBS=4
PARTNUMBER=""
BRD="pynqz2"
BOARDPART=""
MEMPART=""
QSPI=0
DATA_WIDTH=32
BUFFER_IN_WIDTH=8
BUFFER_OUT_WIDTH=32
WINDOWS_STYLE=1

i=2
for opcion
do
	if test "$opcion" == "-help"
		then
			printf "
Este script copia en el directorio actual los tres ficheros necesarios para implementar un sistema de comunicacion pc (.py) <-> microprocesador (.c) <-> FSM en FPGA (.v), ademas de un script .tcl generado por Vivado para reconstruir el blockDesign que contiene los modulos GPIO necesarios para la comunicacion. Al incluir el nombre de la placa, el script copia los ficheros correctos para implementar el diseno (en ausencia de nombre se considera por defecto 'zybo'.\n.
	Opciones:
		-help
		-projname [\"project_ringoscillatorfsm2\"]
		-board [zybo | pynqz2 | cmoda7]
		-njobs [4]
		-qspi
		-dw [32], data_width (tamano del bus gpio)
		-biw [8], buffer_in_width (bits significativos de la palabra de entrada data_in)
		-bow [32], buffer_out_width (bits significativos de la palabra de salida data_out)
		-linux, utilizar esta opcion si Vivado esta instalado en un SO linux. De otro modo el formato de los PATH sera el de Windows y los script tcl fallaran
		
"
			exit
			
		elif test "$opcion" == "-projname"
		then
			PROJNAME="${!i}"
			
		elif test "$opcion" == "-board"
		then
			if test "${!i}" == "cmoda7"
			then
				BRD="cmoda7"
				PARTNUMBER="xc7a15tcpg236-1"
				BOARDPART="digilentinc.com:cmod_a7-15t:part0:1.1"
				MEMPART="mx25l3233"
					
			elif test "${!i}" == "zybo"
			then
				BRD="zybo"
				PARTNUMBER="xc7z010clg400-1"
				BOARDPART="digilentinc.com:zybo:part0:1.0"
				MEMPART="?"
					
			elif test "${!i}" == "pynqz2"
			then
				BRD="pynqz2"
				PARTNUMBER="xc7z020clg400-1"
				BOARDPART="tul.com.tw:pynq-z2:part0:1.0"
				MEMPART="?"

		fi
	
		elif test "$opcion" == "-njobs"
		then
			NJOBS=${!i}
			
		elif test "$opcion" == "-qspi"
		then
			QSPI=1
			
		elif test "$opcion" == "-dw"
		then
			DATA_WIDTH="${!i}"
			
		elif test "$opcion" == "-biw"
		then
			BUFFER_IN_WIDTH="${!i}"
			
		elif test "$opcion" == "-bow"
		then
			BUFFER_OUT_WIDTH="${!i}"
			
		elif test "$opcion" == "-linux"
		then
			WINDOWS_STYLE=0
	fi
	i=$((i+1))
done


## projdir (directorio actual)
if test $WINDOWS_STYLE = 1
then
	aux=`wslpath -w "${PWD}"` #convierte el directorio donde invocamos el script a formato Windows
	PROJDIR=${aux//["\\"]/"/"}
else
	PROJDIR="${PWD}"
fi


## block design
mkdir ./block_design
if test "$BRD" == "cmoda7"
then
	if test $QSPI == 1
	then
		cp "${COMPUTACION}/tcl/bd_fsm2_pcmbpspl_qspi_cmoda7.tcl" ./block_design/bd_design_1.tcl
	else
		cp "${COMPUTACION}/tcl/bd_fsm2_pcmbpspl_cmoda7.tcl" ./block_design/bd_design_1.tcl
	fi
	
elif test "$BRD" == "zybo"
then
	if test $QSPI == 1
	then
		cp "${COMPUTACION}/tcl/bd_fsm2_pczynqpspl_qspi_zynq.tcl" ./block_design/bd_design_1.tcl
	else
		cp "${COMPUTACION}/tcl/bd_fsm2_pczynqpspl_zynq.tcl" ./block_design/bd_design_1.tcl
	fi
	
elif test "$BRD" == "pynqz2"
then
	if test $QSPI == 1
	then
		cp "${COMPUTACION}/tcl/bd_fsm2_pczynqpspl_qspi_zynq.tcl" ./block_design/bd_design_1.tcl
	else
		cp "${COMPUTACION}/tcl/bd_fsm2_pczynqpspl_zynq.tcl" ./block_design/bd_design_1.tcl
	fi
	
fi

## partial flows (tcl)
mkdir ./partial_flows
printf "
create_project ${PROJNAME} ${PROJDIR}/${PROJNAME} -part $PARTNUMBER

set_property board_part $BOARDPART [current_project]

source ${PROJDIR}/block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

update_compile_order -fileset sources_1

add_files -norecurse {${PROJDIR}/vivado_src/top.v ${PROJDIR}/vivado_src/fsm2_pl_0.v ${PROJDIR}/vivado_src/multiplex_4_1.v ${PROJDIR}/vivado_src/slow_ro.v ${PROJDIR}/vivado_src/medium_ro.v ${PROJDIR}/vivado_src/fast_ro.v ${PROJDIR}/vivado_src/include/fsm2_pl_stateloop.local.vh ${PROJDIR}/vivado_src/include/def_ctes.local.vh}

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

startgroup
set_property -dict [list CONFIG.C_GPIO_WIDTH {$DATA_WIDTH} CONFIG.C_GPIO2_WIDTH {$DATA_WIDTH}] [get_bd_cells axi_gpio_data]
endgroup

connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk \"/processing_system7_0/FCLK_CLK0 (100 MHz)\" }  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

set_property top design_1_wrapper [current_fileset]

update_compile_order -fileset sources_1

update_compile_order -fileset sources_1

" > ./partial_flows/mkhwdplatform.tcl

if test $QSPI == 1 && "$BRD" == "cmoda7"
then
printf "
file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1

file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new

close [ open ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc]

update_compile_order -fileset sources_1

launch_runs synth_1 -jobs $NJOBS

wait_on_run synth_1

open_run synth_1 -name synth_1

startgroup

set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]

set_property config_mode SPIx4 [current_design]

endgroup

save_constraints

close_design

" >> ./partial_flows/mkhwdplatform.tcl
fi

printf "
launch_runs impl_1 -to_step write_bitstream -jobs $NJOBS

wait_on_run impl_1

update_compile_order -fileset sources_1

file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.sdk

file copy -force ${PROJDIR}/${PROJNAME}/${PROJNAME}.runs/impl_1/design_1_wrapper.sysdef ${PROJDIR}/${PROJNAME}/${PROJNAME}.sdk/design_1_wrapper.hdf

launch_sdk -workspace ${PROJDIR}/${PROJNAME}/${PROJNAME}.sdk -hwspec ${PROJDIR}/${PROJNAME}/${PROJNAME}.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform

	" >> ./partial_flows/mkhwdplatform.tcl


## python script
cp "${COMPUTACION}/python/my_module.py" ./my_module_local.py

printf "
import sys
import serial
import time
import numpy as np
import my_module_local as mm

buffer_in_width = $BUFFER_IN_WIDTH
buffer_out_width = $BUFFER_OUT_WIDTH
numero_vueltas = 10 # 1-64 x 100000


fpga = serial.Serial(port='/dev/ttyUSB1', baudrate=9600, bytesize=8)
time.sleep(.1)

for j in range(4):
	buffer_numero_vueltas = mm.int_to_buffer(numero_vueltas)
	buffer_selector = mm.resize_array(mm.int_to_buffer(j),2)
	buffer_in = buffer_selector + buffer_numero_vueltas

	octeto_in = mm.buffer_to_octeto(buffer_in, buffer_in_width)

	mm.fsmuart2_scan_cycle(fpga, buffer_in, buffer_in_width)

	medidas=[]
	for i in range(10):
		medidas.append(mm.buffer_to_int(mm.fsmuart2_calc_cycle(fpga, buffer_out_width))/(numero_vueltas*1000))

	media = np.average(medidas)

	desviacion = np.std(medidas)
	
	if j == 0:
		print(\"Clock\t\", round(media,4), \"\t\t+/-\", round(desviacion, 4), \"\tMHz\")
	elif j == 1:
		print(\"Slow\t\", round(media,4), \"\t+/-\", round(desviacion, 4), \"\tMHz\")
	elif j == 2:
		print(\"Medium\t\", round(media,4), \"\t+/-\", round(desviacion, 4), \"\tMHz\")
	elif j == 3:
		print(\"Fast\t\", round(media,4), \"\t+/-\", round(desviacion, 4), \"\tMHz\")

fpga.close()
" > ringoscillator.py


## vivado sources
mkdir vivado_src
mkdir vivado_src/include
cp "${COMPUTACION}/src/verilog/ringoscillator_fsm2_pl.v" ./vivado_src/fsm2_pl_0.v
cp "${COMPUTACION}/src/verilog/multiplex_4_1.v" ./vivado_src/multiplex_4_1.v
cp "${COMPUTACION}/src/verilog/include/fsm2_pl_stateloop.vh" ./vivado_src/include/fsm2_pl_stateloop.local.vh
cp "${COMPUTACION}/src/verilog/include/def_ctes.vh" ./vivado_src/include/def_ctes.local.vh
hwd_genRO -XOSi 5 -YOSi 1 -Ninv 25 -out fast_ro
hwd_genRO -XOSi 9 -YOSi 1 -Ninv 51 -out medium_ro
hwd_genRO -XOSi 13 -YOSi 1 -Ninv 101 -out slow_ro
mv fast_ro.v medium_ro.v slow_ro.v vivado_src/ 

((aux=$DATA_WIDTH-1))
printf "
\`timescale 1ns / 1ps


module TOP (
	input		clock,
	input[7:0]	ctrl_in,
	output[7:0]	ctrl_out,
	input[$aux:0]	data_in,
	output[$aux:0]	data_out
);

	RO_FSM2_PL #(
		.DATA_WIDTH($DATA_WIDTH),
		.BUFFER_IN_WIDTH($BUFFER_IN_WIDTH),
		.BUFFER_OUT_WIDTH($BUFFER_OUT_WIDTH)
	) fsm2_pl_0 (
		.clock(clock),
		.ctrl_in(ctrl_in),
		.ctrl_out(ctrl_out),
		.data_in(data_in),
		.data_out(data_out)
	);
	
endmodule
" > vivado_src/top.v


## sdk sources
mkdir sdk_src
cp "${COMPUTACION}/src/xilinx_c/include/comm2.h" ./sdk_src/comm2.local.h
cp "${COMPUTACION}/src/xilinx_c/template_fsmuart2.c" ./ringoscillator.c

printf "
#define DATA_WIDTH			%d
#define BUFFER_IN_WIDTH		%d
#define BUFFER_OUT_WIDTH	%d
" $DATA_WIDTH $BUFFER_IN_WIDTH $BUFFER_OUT_WIDTH > ./sdk_src/comm2param.local.h

if test "$brd" == "cmoda7"
then
	cp "${COMPUTACION}/../recursos/FPGA/CMOD-A7-15T/pinout_cmoda7.xdc" ./vivado_src/puertos.xdc

	xuart="#include \"xuartlite.h\""
	
elif test "$brd" == "zybo"
then
	cp "${COMPUTACION}/../recursos/FPGA/Zybo/pinout_zybo.xdc" ./vivado_src/puertos.xdc

	xuart="#include \"xuartps.h\""
	
elif test "$brd" == "pynqz2"
then
	cp "${COMPUTACION}/../recursos/FPGA/PYNQ-Z2/pinout_pynqz2.xdc" ./vivado_src/puertos.xdc
	
	xuart="#include \"xuartps.h\""

fi

echo $xuart | cat - ringoscillator.c > temp && mv temp ringoscillator.c
mv ringoscillator.c ./sdk_src/ringoscillator.c


## echo log
echo ""

echo "fpga part: ${PARTNUMBER}"

echo ""

echo "sdk source files: ${PROJDIR}/sdk_src/"

echo ""

if test $QSPI == 1
then
	echo "q-spi part: ${MEMPART}"
	
	echo ""
fi
