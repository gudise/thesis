#!/usr/bin/python3.8

import os
import sys
from subprocess import run # esta función permite llamar a sub-rutinas de shell (e.g. programas en C), y pasar/recibir datos.
from fpga.romatrix import *

argv = sys.argv
argc = len(argv)

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
PINMAP="no;"
MINSEL=0
NINV=3
POSMAP="1,1;10,10;2,1;10;y"
NOSC=0
PBLOCK="no"
DATA_WIDTH=32
BUFFER_IN_WIDTH=0
BUFFER_OUT_WIDTH=32
WINDOWS_STYLE=1
CLK_NAME="/processing_system7_0/FCLK_CLK0 (100 MHz)"
DW_GE_BIW=""
DW_GE_BOW=""
BIW_ALIGNED_DW=""
BOW_ALIGNED_DW=""
BIW_MISALIGNED_DW=""
BOW_MISALIGNED_DW=""
DEBUG=0

for i, opt in enumerate(argv): # "i" es el indice que recorre la lista de opciones, "opt" es la opcion propiamente dicha
	if opt == "-help":
		with open(f"{os.environ['REPO_fpga']}/python/scripts/help/interfaz_pcpspl.help", "r") as f:
			print(f.read())
		exit()
			
	if opt == "-projname":
		PROJNAME=argv[i+1]
		
	if opt == "-board":
		if argv[i+1]=="cmoda7_15t":
			BRD="cmoda7_15t"
			PARTNUMBER="xc7a15tcpg236-1"
			BOARDPART="digilentinc.com:cmod_a7-15t:part0:1.1"
			MEMPART="mx25l3233"
			CLK_NAME="/clk_wiz_1/clk_out1 (100 MHz)"
		
		elif argv[i+1]=="cmoda7_35t":
			BRD="cmoda7_35t"
			PARTNUMBER="xc7a35tcpg236-1"
			BOARDPART="digilentinc.com:cmod_a7-35t:part0:1.1"
			MEMPART="mx25l3233"
			CLK_NAME="/clk_wiz_1/clk_out1 (100 MHz)"
		
		elif argv[i+1]=="zybo":
			BRD="zybo"
			PARTNUMBER="xc7z010clg400-1"
			BOARDPART="digilentinc.com:zybo:part0:1.0"
			MEMPART="?"
		
		elif argv[i+1]=="pynqz2":
			BRD="pynqz2"
			PARTNUMBER="xc7z020clg400-1"
			BOARDPART="tul.com.tw:pynq-z2:part0:1.0"
			MEMPART="?"
		
	if opt == "-njobs":
		NJOBS=int(argv[i+1])
		
	if opt == "-qspi":
		QSPI=1
		
	if opt == "-detailr":
		DETAILR=1
		
	if opt == "-tipo":
		TIPO=argv[i+1]
		
	if opt == "-pinmap":
		PINMAP=argv[i+1]
		
	if opt == "-minsel":
		MINSEL=1
		
	if opt == "-Ninv":
		NINV=int(argv[i+1])
		
	if opt == "-posmap":
		POSMAP=argv[i+1]
		
	if opt == "-pblock":
		PBLOCK=argv[i+1]
		
	if opt == "-dw":
		DATA_WIDTH=int(argv[i+1])
		
	if opt == "-biw":
		BUFFER_IN_WIDTH=int(argv[i+1])
		
	if opt == "-bow":
		BUFFER_OUT_WIDTH=int(argv[i+1])
		
	if opt == "-linux":
		WINDOWS_STYLE=0
		
	if opt == "-debug":
		DEBUG=1
		
		
POSMAP = genOscLocations(POSMAP)
NOSC = len(POSMAP.split())
NBITSOSC = clog2(NOSC)

if MINSEL:
	if TIPO == "lut1":
		NBITSPDL = 0
	elif TIPO == "lut2":
		NBITSPDL = 1
	elif TIPO == "lut3":
		NBITSPDL = 2
	elif TIPO == "lut4":
		NBITSPDL = 3
	elif TIPO == "lut5":
		NBITSPDL = 4
	elif TIPO == "lut6":
		NBITSPDL = 5
else:
	if TIPO == "lut1":
		NBITSPDL = 0
	elif TIPO == "lut2":
		NBITSPDL = NINV
	elif TIPO == "lut3":
		NBITSPDL = 2*NINV
	elif TIPO == "lut4":
		NBITSPDL = 3*NINV
	elif TIPO == "lut5":
		NBITSPDL = 4*NINV
	elif TIPO == "lut6":
		NBITSPDL = 5*NINV
		
if BUFFER_IN_WIDTH==0:
	BUFFER_IN_WIDTH = NBITSOSC+NBITSPDL+5

if BUFFER_IN_WIDTH%8 == 0:
	OCTETO_IN_WIDTH=BUFFER_IN_WIDTH//8
else:
	OCTETO_IN_WIDTH=BUFFER_IN_WIDTH//8+1

if BUFFER_OUT_WIDTH%8 == 0:
	OCTETO_OUT_WIDTH=BUFFER_OUT_WIDTH//8
else:
	OCTETO_OUT_WIDTH=BUFFER_OUT_WIDTH//8+1

if BUFFER_IN_WIDTH%DATA_WIDTH == 0:
	WORDS_IN_WIDTH=BUFFER_IN_WIDTH//DATA_WIDTH
else:
	WORDS_IN_WIDTH=BUFFER_IN_WIDTH//DATA_WIDTH+1

if BUFFER_OUT_WIDTH%DATA_WIDTH == 0:
	WORDS_OUT_WIDTH=BUFFER_OUT_WIDTH//DATA_WIDTH
else:
	WORDS_OUT_WIDTH=BUFFER_OUT_WIDTH//DATA_WIDTH+1

if DATA_WIDTH >= BUFFER_IN_WIDTH:
	DW_GE_BIW='`define DW_GE_BIW'
elif BUFFER_IN_WIDTH%DATA_WIDTH == 0:
	BIW_ALIGNED_DW='`define BIW_ALIGNED_DW'
else:
	BIW_MISALIGNED_DW='`define BIW_MISALIGNED_DW'

if DATA_WIDTH >= BUFFER_OUT_WIDTH:
	DW_GE_BOW='`define DW_GE_BOW'
elif BUFFER_OUT_WIDTH%DATA_WIDTH == 0:
	BOW_ALIGNED_DW='`define BOW_ALIGNED_DW'
else:
	BOW_MISALIGNED_DW='`define BOW_MISALIGNED_DW'
	
	
## projdir (directorio actual)
if WINDOWS_STYLE == 1:
	PROJDIR = run(["wslpath","-w",os.environ['PWD']], capture_output=True, text=True).stdout.replace("\\","/").replace("\n","")
else:
	PROJDIR=os.environ['PWD']
	
	
## block design
run(["mkdir","./block_design"])
if BRD == "cmoda7_15t":
	if QSPI == 1:
		run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])
	else:
		run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])
	
elif BRD == "cmoda7_35t":
	if QSPI == 1:
		run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])
	else:
		run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])
	
elif BRD == "zybo":
	if QSPI == 1:
		run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_zybo.tcl","./block_design/bd_design_1.tcl"])
	else:
		run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_zybo.tcl","./block_design/bd_design_1.tcl"])
	
elif BRD == "pynqz2":
	if QSPI == 1:
		run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_pynqz2.tcl","./block_design/bd_design_1.tcl"])
	else:
		run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_pynqz2.tcl","./block_design/bd_design_1.tcl"])
		
		
## partial flows (tcl)
run(["mkdir","./partial_flows"])

VIVADO_FILES=f"{PROJDIR}/vivado_src/top.v {PROJDIR}/vivado_src/interfaz_pspl.cp.v {PROJDIR}/vivado_src/romatrix.v {PROJDIR}/vivado_src/medidor_frec.cp.v {PROJDIR}/vivado_src/interfaz_pspl_config.vh"
if DEBUG:
	VIVADO_FILES+=f" {PROJDIR}/vivado_src/clock_divider.cp.v"
if PBLOCK != "no":
	VIVADO_FILES+=f" {PROJDIR}/vivado_src/pblock.xdc"

with open("./partial_flows/setupdesign.tcl", "w") as f:
	f.write(f"""
create_project {PROJNAME} {PROJDIR}/{PROJNAME} -part {PARTNUMBER}

set_property board_part {BOARDPART} [current_project]

source {PROJDIR}/block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

update_compile_order -fileset sources_1

add_files -norecurse {{ {VIVADO_FILES} }}

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

startgroup
set_property -dict [list CONFIG.C_GPIO_WIDTH {DATA_WIDTH} CONFIG.C_GPIO2_WIDTH {DATA_WIDTH}] [get_bd_cells axi_gpio_data]
endgroup

connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {{Clk {CLK_NAME} }}  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

set_property top design_1_wrapper [current_fileset]

update_compile_order -fileset sources_1

set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]

""")

with open("./partial_flows/genbitstream.tcl", "w") as f:
	f.write(f"""
if {{[file exists {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/detail_routing.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/detail_routing.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/detail_routing.xdc
}}

if {{[file exists {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc
}}

if {{[file exists {PROJDIR}/{PROJNAME}/{PROJNAME}.runs/synth_1]==1}} {{
	reset_run synth_1
}}

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

launch_runs synth_1 -jobs {NJOBS}

wait_on_run synth_1

""")
	if QSPI == 1 and BRD == "cmoda7_15t":
		f.write(f"""
file mkdir {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1

file mkdir {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new

close [ open {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc]

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
""")
	elif QSPI == 1 and BRD == "cmoda7_35t":
		f.write(f"""
file mkdir {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1

file mkdir {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new

close [ open {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/bitstreamconfig.xdc]

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
""")

	if DETAILR:
		f.write("""
open_run synth_1 -name synth_1

startgroup
""")
		for i in range(NOSC):
			f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_0]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_0 }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_{i} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_{i} }}]
""")
			for j in range(1,NINV,1):
				f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j}]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j} }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
""") 
		f.write(f"""
endgroup

file mkdir {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new

close [ open {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/detail_routing.xdc w ]

add_files -fileset constrs_1 {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/detail_routing.xdc

set_property target_constrs_file {PROJDIR}/{PROJNAME}/{PROJNAME}.srcs/constrs_1/new/detail_routing.xdc [current_fileset -constrset]

save_constraints -force

close_design

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0
""") # Aqui termina detailed routing

	f.write(f"""
if {{[file exists {PROJDIR}/{PROJNAME}/{PROJNAME}.runs/synth_1/__synthesis_is_complete__]==1}} {{
	reset_run synth_1
}}
	
launch_runs impl_1 -to_step write_bitstream -jobs {NJOBS}

wait_on_run impl_1
""")

with open("./partial_flows/launchsdk.tcl", "w") as f:
	f.write(f"""
file mkdir {PROJDIR}/{PROJNAME}/{PROJNAME}.sdk

file copy -force {PROJDIR}/{PROJNAME}/{PROJNAME}.runs/impl_1/design_1_wrapper.sysdef {PROJDIR}/{PROJNAME}/{PROJNAME}.sdk/design_1_wrapper.hdf

launch_sdk -workspace {PROJDIR}/{PROJNAME}/{PROJNAME}.sdk -hwspec {PROJDIR}/{PROJNAME}/{PROJNAME}.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform

""")

with open("mkhwdplatform.tcl", "w") as f:
	f.write(f"""
source {PROJDIR}/partial_flows/setupdesign.tcl

source {PROJDIR}/partial_flows/genbitstream.tcl

source {PROJDIR}/partial_flows/launchsdk.tcl

""")


## vivado sources
run(["mkdir","./vivado_src"])
run(["cp",f"{os.environ['REPO_fpga']}/verilog/interfaz_pspl.v","./vivado_src/interfaz_pspl.cp.v"])
run(["cp",f"{os.environ['REPO_fpga']}/verilog/medidor_frec.v","./vivado_src/medidor_frec.cp.v"])
if DEBUG:
	run(["cp",f"{os.environ['REPO_fpga']}/verilog/clock_divider.v","./vivado_src/clock_divider.cp.v"])

with open("vivado_src/interfaz_pspl_config.vh", "w") as f:
	f.write(f"""{DW_GE_BIW}
{BIW_ALIGNED_DW}
{BIW_MISALIGNED_DW}
{DW_GE_BOW}
{BOW_ALIGNED_DW}
{BOW_MISALIGNED_DW}
""")

genRomatrix(out_name="vivado_src/romatrix.v", N_inv=NINV, tipo=TIPO, pinmap_opt=PINMAP, minsel=MINSEL, posmap=POSMAP, debug=DEBUG)

with open("vivado_src/top.v", "w") as f:
	if NOSC==1:
		aux=""
	else:
		aux=f".sel_ro(buffer_in[{NBITSOSC-1}:0]),"
	if TIPO == "lut1":
		aux1=""
	else:
		aux1=f".sel_pdl(buffer_in[{BUFFER_IN_WIDTH-5-1}:{NBITSOSC}]),"
		
	f.write(f"""`timescale 1ns / 1ps


module TOP (
	input			clock,
	input[7:0]		ctrl_in,
	output[7:0]		ctrl_out,
	input[{DATA_WIDTH-1}:0]	data_in,
	output[{DATA_WIDTH-1}:0]	data_out
);

	wire[{BUFFER_IN_WIDTH-1}:0] buffer_in;
	wire[{BUFFER_OUT_WIDTH-1}:0] buffer_out;
	wire sync;
	wire ack;
	wire out_ro;
	reg enable_medidor=0;
	
	always @(posedge clock) begin
		case ({{sync,ack}})
			2'b10:
				enable_medidor <= 1;
				
			default:
				enable_medidor <= 0;
		endcase
	end

	INTERFAZ_PSPL #(
		.DATA_WIDTH({DATA_WIDTH}),
		.BUFFER_IN_WIDTH({BUFFER_IN_WIDTH}),
		.BUFFER_OUT_WIDTH({BUFFER_OUT_WIDTH})
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
	
	ROMATRIX romatrix (
		.clock(clock),
		.enable(1'b1),
		{aux}
		{aux1}
		.out(out_ro)
	);

	MEDIDOR_FREC #(
		.OUT_WIDTH({BUFFER_OUT_WIDTH})
	) medidor_frec (
		.clock(clock),
		.enable(enable_medidor),
		.clock_u(out_ro),
		.resol(buffer_in[{BUFFER_IN_WIDTH-1}:{BUFFER_IN_WIDTH-5}]),
		.lock(ack),
		.out(buffer_out)
	);
	
endmodule
""")


if PBLOCK!="no":
	PBLOCK_CORNERS=PBLOCK.split()
	PBLOCK_CORNER_0=PBLOCK_CORNERS[0].split(',')
	PBLOCK_CORNER_1=PBLOCK_CORNERS[1].split(',')
	with open("./vivado_src/pblock.xdc", "w") as f:
		f.write(f"""create_pblock pblock_TOP_0
add_cells_to_pblock [get_pblocks pblock_TOP_0] [get_cells -quiet [list design_1_i/TOP_0]]
resize_pblock [get_pblocks pblock_TOP_0] -add {{SLICE_X{PBLOCK_CORNER_0[0]}Y{PBLOCK_CORNER_0[1]}:SLICE_X{PBLOCK_CORNER_1[0]}Y{PBLOCK_CORNER_1[1]}}}
		""")


## sdk sources
run(["mkdir","./sdk_src"])
run(["cp",f"{os.environ['REPO_fpga']}/c-xilinx/sdk/interfaz_pcps-pspl.c","./sdk_src/interfaz_pcps-pspl.cp.c"])

with open("./sdk_src/interfaz_pcps-pspl_config.h", "w") as f:
	if BRD == "cmoda7_15t" or BRD == "cmoda7_35t":
		f.write("#include \"xuartlite.h\"\n\n")
	elif BRD == "zybo" or BRD == "pynqz2":
		f.write("#include \"xuartps.h\"\n\n")

	f.write(f"""#define DATA_WIDTH			{DATA_WIDTH}
#define BUFFER_IN_WIDTH		{BUFFER_IN_WIDTH}
#define BUFFER_OUT_WIDTH	{BUFFER_OUT_WIDTH}
#define OCTETO_IN_WIDTH		{OCTETO_IN_WIDTH}
#define OCTETO_OUT_WIDTH	{OCTETO_OUT_WIDTH}
#define WORDS_IN_WIDTH		{WORDS_IN_WIDTH}
#define WORDS_OUT_WIDTH		{WORDS_OUT_WIDTH}
""")


## log info
print(f"""
 N_osc = {NOSC}
 N_pdl = {2**NBITSPDL}
 
 Trama del selector(ts): {NBITSOSC} {NBITSPDL} 5
 sel_ro_width = {NBITSOSC}
 sel_pdl_width = {NBITSPDL}
 dw  = {DATA_WIDTH}
 biw = {BUFFER_IN_WIDTH}
 bow = {BUFFER_OUT_WIDTH}
 
 fpga part: {PARTNUMBER}
 
 sdk source files: {PROJDIR}/sdk_src/
 
""")
if QSPI == 1:
	print(f"q-spi part: {MEMPART}")
