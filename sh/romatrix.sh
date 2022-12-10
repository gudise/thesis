PROJNAME="project_romatrix"
PROJDIR="./"
BRD="pynqz2"
BOARDPART="tul.com.tw:pynq-z2:part0:1.0"
PARTNUMBER="xc7z020clg400-1"
NJOBS=4
QSPI=0
MEMPART=""
DETAILR=0
TIPO="lut1"
DIRECTRIZ="y"
PINMAP="no"
MINSEL=0
NINV=3
NOSC=2
POSMAP="0,0"
XOSincr=1
XOSmax=20
YOSincr=1
YOSmax=20
PBLOCK="no"
RESOLUCION=1000000
DATA_WIDTH=32
BUFFER_IN_WIDTH=0
BUFFER_OUT_WIDTH=32
WINDOWS_STYLE=1

i=2
for opcion
do
	if test "$opcion" = "-help"
	then
		tabs 4; cat "$REPO_fpga/sh/help/romatrix.help"
		exit
		
	elif test "$opcion" = "-projname"
	then
		PROJNAME="${!i}"
		
	elif test "$opcion" = "-board"
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

	elif test "$opcion" = "-njobs"
	then
		NJOBS=${!i}
		
	elif test "$opcion" = "-qspi"
	then
		QSPI=1
		
	elif test "$opcion" = "-detailr"
	then
		DETAILR=1
		
	elif test "$opcion" = "-tipo"
	then
		TIPO="${!i}"
		
	elif test "$opcion" = "-directriz"
	then
		DIRECTRIZ="${!i}"
		
	elif test "$opcion" = "-pinmap"
	then
		PINMAP="${!i}"
		
	elif test "$opcion" = "-minsel"
	then
		MINSEL=1
		
	elif test "$opcion" = "-Ninv"
	then
		NINV=${!i}
		
	elif test "$opcion" = "-Nosc"
	then
		NOSC=${!i}
		
	elif test "$opcion" = "-posmap"
	then
		POSMAP=${!i}
		
	elif test "$opcion" = "-Xincr"
	then
		XOSincr=${!i}
		
	elif test "$opcion" = "-Xmax"
	then
		XOSmax=${!i}
		
	elif test "$opcion" = "-Yincr"
	then
		YOSincr=${!i}
		
	elif test "$opcion" = "-Ymax"
	then
		YOSmax=${!i}
		
	elif test "$opcion" = "-pblock"
	then
		PBLOCK=${!i}
		
	elif test "$opcion" = "-resolucion"
	then
		RESOLUCION=${!i}
		
	elif test "$opcion" = "-dw"
	then
		DATA_WIDTH="${!i}"
		
	elif test "$opcion" = "-biw"
	then
		BUFFER_IN_WIDTH="${!i}"
		
	elif test "$opcion" = "-bow"
	then
		BUFFER_OUT_WIDTH="${!i}"
		
	elif test "$opcion" = "-linux"
	then
		WINDOWS_STYLE=0
	fi
	i=$((i+1))
done

if test $BUFFER_IN_WIDTH = 0
then
	aux=`calc_nbits.x $NOSC`
	if test $MINSEL = 0
	then
		if test $TIPO = "lut1"
		then
			BUFFER_IN_WIDTH=$aux
		elif test "$TIPO" = "lut2"
		then
			((BUFFER_IN_WIDTH=$aux+$NINV))
		elif test "$TIPO" = "lut3"
		then
			((BUFFER_IN_WIDTH=$aux+2*$NINV))
		elif test $TIPO = "lut3mr"
		then
			((BUFFER_IN_WIDTH=$aux+$NINV))
		elif test $TIPO = "lut4"
		then
			((BUFFER_IN_WIDTH=$aux+3*$NINV))
		elif test $TIPO = "lut5"
		then
			((BUFFER_IN_WIDTH=$aux+4*$NINV))
		elif test $TIPO = "lut6"
		then
			((BUFFER_IN_WIDTH=$aux+5*$NINV))
		elif test $TIPO = "lut6mr"
		then
			((BUFFER_IN_WIDTH=$aux+2*$NINV))
		elif test $TIPO = "lut6_2"
		then
			((BUFFER_IN_WIDTH=$aux+5*$NINV))
		fi
	else
		if test $TIPO = "lut1"
		then
			BUFFER_IN_WIDTH=$aux
		elif test "$TIPO" = "lut2"
		then
			((BUFFER_IN_WIDTH=$aux+1))
		elif test "$TIPO" = "lut3"
		then
			((BUFFER_IN_WIDTH=$aux+2))
		elif test $TIPO = "lut3mr"
		then
			((BUFFER_IN_WIDTH=$aux))
		elif test $TIPO = "lut4"
		then
			((BUFFER_IN_WIDTH=$aux+3))
		elif test $TIPO = "lut5"
		then
			((BUFFER_IN_WIDTH=$aux+4))
		elif test $TIPO = "lut6"
		then
			((BUFFER_IN_WIDTH=$aux+5))
		elif test $TIPO = "lut6mr"
		then
			((BUFFER_IN_WIDTH=$aux+2))
		elif test $TIPO = "lut6_2"
		then
			((BUFFER_IN_WIDTH=$aux+5))
		fi
	fi
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

VIVADO_FILES="${PROJDIR}/vivado_src/top.v ${PROJDIR}/vivado_src/romatrix_interfaz_pl_frontend.v ${PROJDIR}/vivado_src/romatrix.v ${PROJDIR}/vivado_src/include/interfaz_pl_backend.cp.vh ${PROJDIR}/vivado_src/include/interfaz_pl_define.cp.vh"
if test "$PBLOCK" != "no"
then
	VIVADO_FILES="$VIVADO_FILES ${PROJDIR}/vivado_src/pblock.xdc"
fi

printf "
create_project ${PROJNAME} ${PROJDIR}/${PROJNAME} -part $PARTNUMBER

set_property board_part $BOARDPART [current_project]

source ${PROJDIR}/block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

add_files -norecurse {$VIVADO_FILES}

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

if test $DETAILR -eq 1
then
printf "
open_run synth_1 -name synth_1

startgroup
" >> ./partial_flows/genbitstream.tcl

for((i=0; i<$NOSC; i=i+1))
do
	printf "
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_${i}_0]
set_property is_route_fixed 1 [get_nets {design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_${i}_0 }]
set_property is_bel_fixed 1 [get_cells {design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_${i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_${i} }]
set_property is_loc_fixed 1 [get_cells {design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_${i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_${i} }]
" >> ./partial_flows/genbitstream.tcl

	for((j=1; j<$NINV; j=j+1))
	do
		((aux = $j-1))
		printf "
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_${i}_${j}]
set_property is_route_fixed 1 [get_nets {design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_${i}_${j} }]
set_property is_bel_fixed 1 [get_cells {design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_${i}_${j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_${i}_${aux} }]
set_property is_loc_fixed 1 [get_cells {design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_${i}_${j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_${i}_${aux} }]
" >> ./partial_flows/genbitstream.tcl
	done
done

printf "
endgroup

file mkdir ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new

close [ open ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/detail_routing.xdc w ]

add_files -fileset constrs_1 ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/detail_routing.xdc

set_property target_constrs_file ${PROJDIR}/${PROJNAME}/${PROJNAME}.srcs/constrs_1/new/detail_routing.xdc [current_fileset -constrset]

save_constraints -force

close_design

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0
" >> ./partial_flows/genbitstream.tcl
fi # Aqui termina detailed routing

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


## log info
aux=`calc_nbits.x $NOSC`
printf "
 sel_ro_width = $aux
 
 sel_pdl_width = $(($BUFFER_IN_WIDTH-$aux))
"

## vivado sources
mkdir vivado_src
mkdir vivado_src/include
cp "$REPO_fpga/verilog/include/interfaz_pl_backend.vh" ./vivado_src/include/interfaz_pl_backend.cp.vh
cp "$REPO_fpga/verilog/include/interfaz_pl_define.vh" ./vivado_src/include/interfaz_pl_define.cp.vh

gen_romatrix.x -out "romatrix" -directriz $DIRECTRIZ -Ninv $NINV -Nosc $NOSC -posmap $POSMAP -XOSincr $XOSincr -XOSmax $XOSmax -YOSincr $YOSincr -YOSmax $YOSmax -tipo $TIPO -pinmap $PINMAP -minsel $MINSEL -resolucion $RESOLUCION
		
mv romatrix.v ./vivado_src/

((aux=$DATA_WIDTH-1))
printf "\`timescale 1ns / 1ps


module TOP (
	input		clock,
	input[7:0]	ctrl_in,
	output[7:0]	ctrl_out,
	input[$aux:0]	data_in,
	output[$aux:0]	data_out
);

	ROMATRIX_INTERFAZ_PL_FRONTEND #(
		.DATA_WIDTH($DATA_WIDTH),
		.BUFFER_IN_WIDTH($BUFFER_IN_WIDTH),
		.BUFFER_OUT_WIDTH($BUFFER_OUT_WIDTH)
	) romatrix_interfaz_pl_frontend (
		.clock(clock),
		.ctrl_in(ctrl_in),
		.ctrl_out(ctrl_out),
		.data_in(data_in),
		.data_out(data_out)
	);
	
endmodule
" > vivado_src/top.v

if test $NOSC -eq 1
then
printf "\`include \"interfaz_pl_define.cp.vh\"


module ROMATRIX_INTERFAZ_PL_FRONTEND #(
	//parametros
	parameter   DATA_WIDTH = 32,
	parameter   BUFFER_IN_WIDTH = 16,
	parameter   BUFFER_OUT_WIDTH = 16
	) (
	//I/O estandar
	input						  clock,
	input[7:0]					  ctrl_in,
	output reg[7:0]				  ctrl_out,
	input[DATA_WIDTH-1:0]		 data_in,
	output reg[DATA_WIDTH-1:0]	data_out
	);

	//buffer de entrada/salida
	reg[BUFFER_IN_WIDTH-1:0] buffer_in;
	reg[BUFFER_OUT_WIDTH-1:0] buffer_out;

	//variables internas estandar
	reg[3:0] state=\`IDLE;
	reg[1:0] busy_frontend=\`LOW;
	reg[1:0] busy_backend=\`LOW;
	reg[7:0] ctrl_in_reg;
	reg[DATA_WIDTH-1:0] data_in_reg;
	reg[7:0] contador_std;
	reg calc_end;
	
	//variables internas custom
	reg[31:0] contador=0;
	reg[31:0] contador_ro;
	reg enable_ro=0;
	wire out_ro;
	
	//registro de input
	always @(posedge clock) begin
		ctrl_in_reg <= ctrl_in;
		
		data_in_reg <= data_in;
	end
" > vivado_src/romatrix_interfaz_pl_frontend.v
if test $TIPO = "lut1"
then
printf "
	//asignaciones combinacionales/submodulos
	ROMATRIX romatrix (
		.clock(clock),
		.enable(enable_ro),
		.out(out_ro)
	);
" >> vivado_src/romatrix_interfaz_pl_frontend.v
else
printf "
	//asignaciones combinacionales/submodulos
	ROMATRIX romatrix (
		.clock(clock),
		.sel(buffer_in),
		.enable(enable_ro),
		.out(out_ro)
	);
" >> vivado_src/romatrix_interfaz_pl_frontend.v
fi
printf "
	always @(posedge out_ro) begin
		if(enable_ro==0)
			contador_ro<=0;
		else
			contador_ro <= contador_ro+1;
	end	
	 
	//definicion de estados
	always @(posedge clock) begin
		busy_frontend[1] <= busy_frontend[0];		
		case (state)
			\`IDLE: begin
				//condicion de estabilidad de 'IDLE'
				if(calc_end==0 && contador==0 && contador_ro==0) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//definicion de 'IDLE'
				calc_end <= 0;
				contador<=0;
				enable_ro <= 0;
			end
			
			\`RST: begin
				//condicion de estabilidad de 'RST'
				if(calc_end==0 && contador==0 && contador_ro==0) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//definicion de 'RST'
				calc_end <= 0;
				contador<=0;
				enable_ro <= 0;
			end
			
			\`CALC: begin
				//condicion de estabilidad de 'CALC'
				if(calc_end) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//definicion de 'CALC'
				if(!calc_end) begin
					if(contador==0)
						enable_ro <= 1;
						
					if(contador<=1000000)
						contador <= contador+1;
					else begin
						calc_end <= 1;
						buffer_out <= contador_ro;
					end
				end
			end
		endcase
	end

	\`include \"interfaz_pl_backend.cp.vh\"
	
endmodule
" >> vivado_src/romatrix_interfaz_pl_frontend.v

else
printf "\`include \"interfaz_pl_define.cp.vh\"


module ROMATRIX_INTERFAZ_PL_FRONTEND #(
	//parametros
	parameter   DATA_WIDTH = 32,
	parameter   BUFFER_IN_WIDTH = 16,
	parameter   BUFFER_OUT_WIDTH = 16
	) (
	//I/O estandar
	input						  clock,
	input[7:0]					  ctrl_in,
	output reg[7:0]				  ctrl_out,
	input[DATA_WIDTH-1:0]		 data_in,
	output reg[DATA_WIDTH-1:0]	data_out
	);
	
	//buffer de entrada/salida
	reg[BUFFER_IN_WIDTH-1:0] buffer_in;
	reg[BUFFER_OUT_WIDTH-1:0] buffer_out;
	
	//variables internas estandar
	reg[3:0] state=\`IDLE;
	reg[1:0] busy_frontend=\`LOW;
	reg[1:0] busy_backend=\`LOW;
	reg[7:0] ctrl_in_reg;
	reg[DATA_WIDTH-1:0] data_in_reg;
	reg[7:0] contador_std;
	reg calc_end;
	
	//variables internas custom
	reg[31:0] contador=0;
	reg[31:0] contador_ro;
	reg enable_ro=0;
	wire out_ro;
	
	//registro de input
	always @(posedge clock) begin
		ctrl_in_reg <= ctrl_in;
		
		data_in_reg <= data_in;
	end
" > vivado_src/romatrix_interfaz_pl_frontend.v
if test $TIPO = "lut1"
then
printf "
	//asignaciones combinacionales/submodulos
	ROMATRIX romatrix (
		.clock(clock),
		.sel_ro(buffer_in),
		.enable(enable_ro),
		.out(out_ro)
	);
" >> vivado_src/romatrix_interfaz_pl_frontend.v
else
aux=`calc_nbits.x $NOSC`
((aux1=$aux-1))
((aux2=$BUFFER_IN_WIDTH-1))
printf "
	//asignaciones combinacionales/submodulos
	ROMATRIX romatrix (
		.clock(clock),
		.sel_ro(buffer_in[$aux1 : 0]),
		.sel(buffer_in[$aux2 : $aux]),
		.enable(enable_ro),
		.out(out_ro)
	);
" >> vivado_src/romatrix_interfaz_pl_frontend.v
fi
printf "
	always @(posedge out_ro) begin
		if(enable_ro==0)
			contador_ro<=0;
		else
			contador_ro <= contador_ro+1;
	end	
	 
	//definicion de estados
	always @(posedge clock) begin
		busy_frontend[1] <= busy_frontend[0];		
		case (state)
			\`IDLE: begin
				//condicion de estabilidad de 'IDLE'
				if(calc_end==0 && contador==0 && contador_ro==0) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//definicion de 'IDLE'
				calc_end <= 0;
				contador<=0;
				enable_ro <= 0;
			end
			
			\`RST: begin
				//condicion de estabilidad de 'RST'
				if(calc_end==0 && contador==0 && contador_ro==0) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//definicion de 'RST'
				calc_end <= 0;
				contador<=0;
				enable_ro <= 0;
			end
			
			\`CALC: begin
				//condicion de estabilidad de 'CALC'
				if(calc_end) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//definicion de 'CALC'
				if(!calc_end) begin
					if(contador==0)
						enable_ro <= 1;
						
					if(contador<=1000000)
						contador <= contador+1;
					else begin
						calc_end <= 1;
						buffer_out <= contador_ro;
					end
				end
			end
		endcase
	end

	\`include \"interfaz_pl_backend.cp.vh\"
	
endmodule
" >> vivado_src/romatrix_interfaz_pl_frontend.v
fi

if test "$PBLOCK" != "no"
then
	IFS=' ' read -ra PBLOCK_CORNERS <<< $PBLOCK
	IFS=',' read -ra PBLOCK_CORNER_0 <<< ${PBLOCK_CORNERS[0]}
	IFS=',' read -ra PBLOCK_CORNER_1 <<< ${PBLOCK_CORNERS[1]}

printf "create_pblock pblock_TOP_0
add_cells_to_pblock [get_pblocks pblock_TOP_0] [get_cells -quiet [list design_1_i/TOP_0]]
resize_pblock [get_pblocks pblock_TOP_0] -add {SLICE_X${PBLOCK_CORNER_0[0]}Y${PBLOCK_CORNER_0[1]}:SLICE_X${PBLOCK_CORNER_1[0]}Y${PBLOCK_CORNER_1[1]}}
" >> vivado_src/pblock.xdc
fi


## sdk sources
mkdir sdk_src
cp "$REPO_fpga/c-xilinx/sdk/interfaz_ps_backend.c" ./interfaz_ps_backend.cp.c

printf "
#define DATA_WIDTH			%d
#define BUFFER_IN_WIDTH		%d
#define BUFFER_OUT_WIDTH	%d
" $DATA_WIDTH $BUFFER_IN_WIDTH $BUFFER_OUT_WIDTH > ./sdk_src/interfaz_ps_define.h

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

echo $xuart | cat - interfaz_ps_backend.cp.c > temp && mv temp interfaz_ps_backend.cp.c
mv interfaz_ps_backend.cp.c ./sdk_src/interfaz_ps_backend.cp.c


## echo log
echo ""
echo " fpga part: ${PARTNUMBER}"
echo ""
echo " sdk source files: ${PROJDIR}/sdk_src/"
echo ""

if test $QSPI -eq 1
then
	echo "q-spi part: ${MEMPART}"
	
	echo ""
fi
