PROJNAME="project_template_interfaz"
PROJDIR="./"
BRD="pynqz2"
BOARDPART="tul.com.tw:pynq-z2:part0:1.0"
PARTNUMBER="xc7z020clg400-1"
NJOBS=4
QSPI=0
MEMPART=""
DATA_WIDTH=32
USBPORT="ttyS4"
BUFFER_IN_WIDTH=32
BUFFER_OUT_WIDTH=32
WINDOWS_STYLE=1
PY_INTERACTIVO=0
CLK_NAME="/processing_system7_0/FCLK_CLK0 (100 MHz)"

i=2
for opcion
do
	if test "$opcion" == "-help"
		then
			printf "
Este script copia en el directorio actual los tres ficheros necesarios para implementar un sistema de comunicacion pc (.py) <-> microprocesador (.c) <-> FSM en FPGA (.v), ademas de un script .tcl generado por Vivado para reconstruir el blockDesign que contiene los modulos GPIO necesarios para la comunicacion. Al incluir el nombre de la placa, el script copia los ficheros correctos para implementar el diseno (en ausencia de nombre se considera por defecto 'zybo'.\n.
	Opciones:
		-help
		-projname [\"project_template_interfaz\"]
		-board [pynqz2 | zybo | cmoda7_15t | cmoda7_35t]
		-njobs [4]
		-qspi
		-dw [32], data_width (tamano del bus gpio)
		-biw [32], buffer_in_width (bits significativos de la palabra de entrada data_in)
		-bow [32], buffer_out_width (bits significativos de la palabra de salida data_out)
		-py_interactivo, esta opción permite cambiar el programa python de muestra (por defecto 'testbench').
		-linux, utilizar esta opcion si Vivado esta instalado en un SO linux. De otro modo el formato de los PATH sera el de Windows y los script tcl fallaran
		
"
			exit
			
		elif test "$opcion" == "-projname"
		then
			PROJNAME="${!i}"
			
		elif test "$opcion" == "-board"
		then
			if test "${!i}" == "cmoda7_15t"
			then
				BRD="cmoda7_15t"
				PARTNUMBER="xc7a15tcpg236-1"
				BOARDPART="digilentinc.com:cmod_a7-15t:part0:1.1"
				MEMPART="mx25l3233"
				CLK_NAME="/clk_wiz_1/clk_out1 (100 MHz)"
				
			elif test "${!i}" == "cmoda7_35t"
			then
				BRD="cmoda7_35t"
				PARTNUMBER="xc7a35tcpg236-1"
				BOARDPART="digilentinc.com:cmod_a7-35t:part0:1.1"
				MEMPART="mx25l3233"
				CLK_NAME="/clk_wiz_1/clk_out1 (100 MHz)"
					
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
			
		elif test "$opcion" == "-usbport"
		then
			USBPORT="${!i}"
			
		elif test "$opcion" == "-biw"
		then
			BUFFER_IN_WIDTH="${!i}"
			
		elif test "$opcion" == "-bow"
		then
			BUFFER_OUT_WIDTH="${!i}"
			
		elif test "$opcion" == "-py_interactivo"
		then
			PY_INTERACTIVO=1
			
		elif test "$opcion" == "-linux"
		then
			WINDOWS_STYLE=0
	fi
	i=$((i+1))
done

if test $((BUFFER_IN_WIDTH%8)) -eq 0
then
	((OCTETO_IN_WIDTH=BUFFER_IN_WIDTH/8))
else
	((OCTETO_IN_WIDTH=BUFFER_IN_WIDTH/8+1))
fi

if test $((BUFFER_OUT_WIDTH%8)) -eq 0
then
	((OCTETO_OUT_WIDTH=BUFFER_OUT_WIDTH/8))
else
	((OCTETO_OUT_WIDTH=BUFFER_OUT_WIDTH/8+1))
fi

if test $((BUFFER_IN_WIDTH%DATA_WIDTH)) -eq 0
then
	((WORDS_IN_WIDTH=BUFFER_IN_WIDTH/DATA_WIDTH))
else
	((WORDS_IN_WIDTH=BUFFER_IN_WIDTH/DATA_WIDTH+1))
fi

if test $((BUFFER_OUT_WIDTH%DATA_WIDTH)) -eq 0
then
	((WORDS_OUT_WIDTH=BUFFER_OUT_WIDTH/DATA_WIDTH))
else
	((WORDS_OUT_WIDTH=BUFFER_OUT_WIDTH/DATA_WIDTH+1))
fi

if test $DATA_WIDTH -ge $BUFFER_IN_WIDTH
then
	DW_GT_BIW='`define DW_GT_BIW'
elif test $((BUFFER_IN_WIDTH%DATA_WIDTH)) -eq 0
then
	BIW_ALIGNED_DW='`define BIW_ALIGNED_DW'
else
	BIW_MISALIGNED_DW='`define BIW_MISALIGNED_DW'
fi

if test $DATA_WIDTH -ge $BUFFER_OUT_WIDTH
then
	DW_GT_BOW='`define DW_GT_BOW'
elif test $((BUFFER_OUT_WIDTH%DATA_WIDTH)) -eq 0
then
	BOW_ALIGNED_DW='`define BOW_ALIGNED_DW'
else
	BOW_MISALIGNED_DW='`define BOW_MISALIGNED_DW'
fi

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
if test "$BRD" == "cmoda7_15t"
then
	if test $QSPI == 1
	then
		cp "$REPO_fpga/tcl/bd_interfaz_qspi_cmoda7_15t.tcl" ./block_design/bd_design_1.tcl
	else
		cp "$REPO_fpga/tcl/bd_interfaz_cmoda7_15t.tcl" ./block_design/bd_design_1.tcl
	fi
	
elif test "$BRD" == "cmoda7_35t"
then
	if test $QSPI == 1
	then
		cp "$REPO_fpga/tcl/bd_interfaz_qspi_cmoda7_35t.tcl" ./block_design/bd_design_1.tcl
	else
		cp "$REPO_fpga/tcl/bd_interfaz_cmoda7_35t.tcl" ./block_design/bd_design_1.tcl
	fi
	
elif test "$BRD" == "zybo"
then
	if test $QSPI == 1
	then
		cp "$REPO_fpga/tcl/bd_interfaz_qspi_zybo.tcl" ./block_design/bd_design_1.tcl
	else
		cp "$REPO_fpga/tcl/bd_interfaz_zybo.tcl" ./block_design/bd_design_1.tcl
	fi
	
elif test "$BRD" == "pynqz2"
then
	if test $QSPI == 1
	then
		cp "$REPO_fpga/tcl/bd_interfaz_qspi_pynqz2.tcl" ./block_design/bd_design_1.tcl
	else
		cp "$REPO_fpga/tcl/bd_interfaz_pynqz2.tcl" ./block_design/bd_design_1.tcl
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

add_files -norecurse {${PROJDIR}/vivado_src/top.v ${PROJDIR}/vivado_src/interfaz_pspl.cp.v ${PROJDIR}/vivado_src/contador.cp.v ${PROJDIR}/vivado_src/interfaz_pspl_define.vh}

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

startgroup
set_property -dict [list CONFIG.C_GPIO_WIDTH {$DATA_WIDTH} CONFIG.C_GPIO2_WIDTH {$DATA_WIDTH}] [get_bd_cells axi_gpio_data]
endgroup

connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk ${CLK_NAME} }  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

set_property top design_1_wrapper [current_fileset]

update_compile_order -fileset sources_1

set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]

" > ./partial_flows/setupdesign.tcl

printf "
if {[file exists ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/detail_routing.xdc]==1} {
    export_ip_user_files -of_objects  [get_files ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/detail_routing.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/detail_routing.xdc
}

if {[file exists ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc]==1} {
    export_ip_user_files -of_objects  [get_files ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc
}

if {[file exists ${PROJDIR}/${PROJNAME}/${PROJNAME}.runs/synth_1]==1} {
	reset_run synth_1
}

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

launch_runs synth_1 -jobs $NJOBS

wait_on_run synth_1

" > ./partial_flows/genbitstream.tcl

if test $QSPI -eq 1 && test $BRD = "cmoda7_15t"
then
printf "
file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1

file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new

close [ open ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc]

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

open_run synth_1 -name synth_1

startgroup

set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]

set_property config_mode SPIx4 [current_design]

endgroup

save_constraints

close_design

" >> ./partial_flows/genbitstream.tcl
fi

if test $QSPI -eq 1 && test $BRD = "cmoda7_35t"
then
printf "
file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1

file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new

close [ open ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc]

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

open_run synth_1 -name synth_1

startgroup

set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]

set_property config_mode SPIx4 [current_design]

endgroup

save_constraints

close_design

" >> ./partial_flows/genbitstream.tcl
fi

printf "
if {[file exists ${PROJDIR}/${PROJNAME}/${PROJNAME}.runs/synth_1/__synthesis_is_complete__]==1} {
	reset_run synth_1
}
	
launch_runs impl_1 -to_step write_bitstream -jobs $NJOBS

wait_on_run impl_1
" >> ./partial_flows/genbitstream.tcl

printf "
file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.sdk

file copy -force ${PROJDIR}/${PROJNAME}/${PROJNAME}.runs/impl_1/design_1_wrapper.sysdef ${PROJDIR}/${PROJNAME}/${PROJNAME}.sdk/design_1_wrapper.hdf

launch_sdk -workspace ${PROJDIR}/${PROJNAME}/${PROJNAME}.sdk -hwspec ${PROJDIR}/${PROJNAME}/${PROJNAME}.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform

" > ./partial_flows/launchsdk.tcl

printf "
source ${PROJDIR}/partial_flows/setupdesign.tcl

source ${PROJDIR}/partial_flows/genbitstream.tcl

source ${PROJDIR}/partial_flows/launchsdk.tcl

" > mkhwdplatform.tcl


## python script
if test $PY_INTERACTIVO -eq 1
then
printf "import serial
import time
from fpga.interfazpcps import *

buffer_in_width = $BUFFER_IN_WIDTH
buffer_out_width = $BUFFER_OUT_WIDTH

fpga = serial.Serial(port=\"/dev/${USBPORT}\", baudrate=9600, bytesize=8)
time.sleep(.1)

## valor de entrada
print(\"\\\nvalor introducido:\\\t\", end='')
entrada = int(input())

## enviamos el valor a la FPGA (convertido en un 'bit string')
scan(fpga, intToBitstr(entrada), buffer_in_width)

## leemos el valor devuelto (convertido en un 'int')
resultado = bitstrToInt(calc(fpga, buffer_out_width))

print(f\"valor devuelto:\\\t\\\t{resultado}\\\n\")

fpga.close()
" > editame_interfaz.py
else
printf "import serial
import time
import random
from tqdm import tqdm
from fpga import pinta_progreso
from fpga.interfazpcps import *

buffer_in_width = $BUFFER_IN_WIDTH
buffer_out_width = $BUFFER_OUT_WIDTH

random.seed(time.time())
valores_test = [random.randint(0, 2**buffer_in_width-1) for i in range(1000)]
valores_check = [(i+1)%%2**buffer_out_width for i in valores_test] # Puede que esto este mal.

fpga = serial.Serial(port=\"/dev/${USBPORT}\", baudrate=9600, bytesize=8)
time.sleep(.1)

success = 0
fail = 0
for i in tqdm(range(1000)):

	## enviamos el valor a la FPGA (convertido en un 'bit string')
	scan(fpga, intToBitstr(valores_test[i]), buffer_in_width)

	## leemos el valor devuelto (convertido en un 'int')
	resultado = bitstrToInt(calc(fpga, buffer_out_width))
	
	if resultado == valores_check[i]:
		success+=1
	else:
		fail+=1

fpga.close()

print(f\"\\\n Success: {success}\\\n Fail: {fail}\\\n\\\n\")
" > editame_interfaz.py
fi

## vivado sources
mkdir vivado_src
cp "$REPO_fpga/verilog/interfaz_pspl.v" ./vivado_src/interfaz_pspl.cp.v
cp "$REPO_fpga/verilog/contador.v" ./vivado_src/contador.cp.v

printf "$DW_GT_BIW
$BIW_ALIGNED_DW
$BIW_MISALIGNED_DW
$DW_GT_BOW
$BOW_ALIGNED_DW
$BOW_MISALIGNED_DW
" > vivado_src/interfaz_pspl_define.vh


printf "\`timescale 1ns / 1ps


module TOP (
	input			clock,
	input[7:0]		ctrl_in,
	output[7:0]		ctrl_out,
	input[$((DATA_WIDTH-1)):0]	data_in,
	output[$((DATA_WIDTH-1)):0]	data_out
);
	wire sync;
	wire ack;
	wire[$BUFFER_IN_WIDTH-1:0] buffer_in;
	wire[$BUFFER_OUT_WIDTH-1:0] buffer_out;
	
	INTERFAZ_PSPL #(
		.DATA_WIDTH($DATA_WIDTH),
		.BUFFER_IN_WIDTH($BUFFER_IN_WIDTH),
		.BUFFER_OUT_WIDTH($BUFFER_OUT_WIDTH)
	) interfaz_pspl (
		.clock(clock),
		.ctrl_in(ctrl_in),
		.ctrl_out(ctrl_out),
		.data_in(data_in),
		.data_out(data_out),
		.sync(sync),
		.ack(ack),
		.buffer_in(buffer_in),
		.buffer_out(buffer_out)
	);

	CONTADOR #(
		.IN_WIDTH($BUFFER_IN_WIDTH),
		.OUT_WIDTH($BUFFER_OUT_WIDTH)
	) contador (
		.clock(clock),
		.sync(sync),
		.ack(ack),
		.data_in(buffer_in),
		.data_out(buffer_out)
	);
	
endmodule
" > vivado_src/top.v


## sdk sources
mkdir sdk_src
cp "$REPO_fpga/c-xilinx/sdk/interfaz_pcps-pspl.c" ./interfaz_pcps-pspl.cp.c

printf "#define DATA_WIDTH			$DATA_WIDTH
#define BUFFER_IN_WIDTH		$BUFFER_IN_WIDTH
#define BUFFER_OUT_WIDTH	$BUFFER_OUT_WIDTH
#define OCTETO_IN_WIDTH		$OCTETO_IN_WIDTH
#define OCTETO_OUT_WIDTH	$OCTETO_OUT_WIDTH
#define WORDS_IN_WIDTH		$WORDS_IN_WIDTH
#define WORDS_OUT_WIDTH		$WORDS_OUT_WIDTH
" > ./sdk_src/interfaz_pcps-pspl_define.h

if test "$BRD" == "cmoda7_15t"
then
	xuart="#include \"xuartlite.h\""
	
elif test "$BRD" == "cmoda7_35t"
then
	xuart="#include \"xuartlite.h\""
	
elif test "$BRD" == "zybo"
then
	xuart="#include \"xuartps.h\""
	
elif test "$BRD" == "pynqz2"
then
	xuart="#include \"xuartps.h\""
fi

echo $xuart | cat - interfaz_pcps-pspl.cp.c > temp && mv temp interfaz_pcps-pspl.cp.c
mv interfaz_pcps-pspl.cp.c ./sdk_src/interfaz_pcps-pspl.cp.c


## log info
printf "
 dw	=	$DATA_WIDTH
 biw	=	$BUFFER_IN_WIDTH
 bow	=	$BUFFER_OUT_WIDTH
 oiw	=	$OCTETO_IN_WIDTH
 oow	=	$OCTETO_OUT_WIDTH
 wiw	=	$WORDS_IN_WIDTH
 wow	=	$WORDS_OUT_WIDTH
 
 fpga part: ${PARTNUMBER}
 
 sdk source files: ${PROJDIR}/sdk_src/
 
 ¡Si modificas las fuentes (cambias el nombre o añades archivos) recuerda editar
 'partial_flows/setupdesign.tcl' (comando 'add_files') para reflejar este cambio!
 
"

if test $QSPI -eq 1
then
	echo "q-spi part: ${MEMPART}"
	
	echo ""
fi
