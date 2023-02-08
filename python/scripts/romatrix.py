#!/usr/bin/python3.8

import os
import sys
from subprocess import run # esta función permite llamar a sub-rutinas de shell (e.g. programas en C), y pasar/recibir datos.
from fpga.oscmatrix import *

argv = sys.argv
argc = len(argv)

proj_name           = "project_romatrix"
proj_dir            = "./"
board               = "pynqz2"
board_part          = "tul.com.tw:pynq-z2:part0:1.0"
fpga_part           = "xc7z020clg400-1"
clk_name            = "/processing_system7_0/FCLK_CLK0 (100 MHz)"
memory_part         = ""
n_jobs              = 4
qspi                = False
detail_routing      = False
tipo_lut            = "lut1"
pinmap              = "no;"
minsel              = False
N_inv               = 3
posmap              = "1,1;10,10;2,1;10;y"
pblock              = "no"
data_width          = 32
buffer_out_width    = 32
wstyle              = True
debug               = False

for i, opt in enumerate(argv): # "i" es el indice que recorre la lista de opciones, "opt" es la opcion propiamente dicha
    if opt == "-help":
        with open(f"{os.environ['REPO_fpga']}/python/scripts/help/romatrix.help", "r") as f:
            print(f.read())
        exit()
            
    if opt == "-projname":
        proj_name=argv[i+1]
        
    if opt == "-board":
        if argv[i+1]=="cmoda7_15t":
            board="cmoda7_15t"
            fpga_part="xc7a15tcpg236-1"
            board_part="digilentinc.com:cmod_a7-15t:part0:1.1"
            memory_part="mx25l3233"
            clk_name="/clk_wiz_1/clk_out1 (100 MHz)"
        
        elif argv[i+1]=="cmoda7_35t":
            board="cmoda7_35t"
            fpga_part="xc7a35tcpg236-1"
            board_part="digilentinc.com:cmod_a7-35t:part0:1.1"
            memory_part="mx25l3233"
            clk_name="/clk_wiz_1/clk_out1 (100 MHz)"
        
        elif argv[i+1]=="zybo":
            board="zybo"
            fpga_part="xc7z010clg400-1"
            board_part="digilentinc.com:zybo:part0:1.0"
            memory_part="?"
        
        elif argv[i+1]=="pynqz2":
            board="pynqz2"
            fpga_part="xc7z020clg400-1"
            board_part="tul.com.tw:pynq-z2:part0:1.0"
            memory_part="?"
        
    if opt == "-njobs":
        n_jobs=int(argv[i+1])
        
    if opt == "-qspi":
        qspi=True
        
    if opt == "-detailr":
        detail_routing=True
        
    if opt == "-tipo":
        tipo_lut=argv[i+1]
        
    if opt == "-pinmap":
        pinmap=argv[i+1]
        
    if opt == "-minsel":
        minsel=True
        
    if opt == "-ninv":
        N_inv=int(argv[i+1])
        
    if opt == "-posmap":
        posmap=argv[i+1]
        
    if opt == "-pblock":
        pblock=argv[i+1]
        
    if opt == "-data_width" or opt == "-dw":
        data_width=int(argv[i+1])
        
    if opt == "-buffer_out_width" or opt == "-bow":
        buffer_out_width=int(argv[i+1])
        
    if opt == "-linux":
        wstyle=False
        
    if opt == "-debug":
        debug=True
        
        
posmap = genOscLocations(posmap)
N_osc = len(posmap.split())
N_bits_osc = clog2(N_osc)

if minsel:
    if tipo_lut == "lut1":
        N_bits_pdl = 0
    elif tipo_lut == "lut2":
        N_bits_pdl = 1
    elif tipo_lut == "lut3":
        N_bits_pdl = 2
    elif tipo_lut == "lut4":
        N_bits_pdl = 3
    elif tipo_lut == "lut5":
        N_bits_pdl = 4
    elif tipo_lut == "lut6":
        N_bits_pdl = 5
else:
    if tipo_lut == "lut1":
        N_bits_pdl = 0
    elif tipo_lut == "lut2":
        N_bits_pdl = N_inv
    elif tipo_lut == "lut3":
        N_bits_pdl = 2*N_inv
    elif tipo_lut == "lut4":
        N_bits_pdl = 3*N_inv
    elif tipo_lut == "lut5":
        N_bits_pdl = 4*N_inv
    elif tipo_lut == "lut6":
        N_bits_pdl = 5*N_inv
        
buffer_in_width = N_bits_osc+N_bits_pdl+5

if buffer_in_width%8 == 0:
    octeto_in_width=buffer_in_width//8
else:
    octeto_in_width=buffer_in_width//8+1

if buffer_out_width%8 == 0:
    octeto_out_width=buffer_out_width//8
else:
    octeto_out_width=buffer_out_width//8+1

if buffer_in_width%data_width == 0:
    words_in_width=buffer_in_width//data_width
else:
    words_in_width=buffer_in_width//data_width+1

if buffer_out_width%data_width == 0:
    words_out_width=buffer_out_width//data_width
else:
    words_out_width=buffer_out_width//data_width+1

dw_ge_biw = ""
biw_aligned_dw = ""
biw_misaligned_dw = ""

if data_width >= buffer_in_width:
    dw_ge_biw='`define DW_GE_BIW'
elif buffer_in_width%data_width == 0:
    biw_aligned_dw='`define BIW_ALIGNED_DW'
else:
    biw_misaligned_dw='`define BIW_MISALIGNED_DW'

dw_ge_bow = ""
bow_aligned_dw = ""
bow_misaligned_dw = ""

if data_width >= buffer_out_width:
    dw_ge_bow='`define DW_GE_BOW'
elif buffer_out_width%data_width == 0:
    bow_aligned_dw='`define BOW_ALIGNED_DW'
else:
    bow_misaligned_dw='`define BOW_MISALIGNED_DW'
    
    
## proj_dir (directorio actual)
if wstyle:
    proj_dir = run(["wslpath","-w",os.environ['PWD']], capture_output=True, text=True).stdout.replace("\\","/").replace("\n","")
else:
    proj_dir=os.environ['PWD']
    
    
## block design
run(["mkdir","./block_design"])
if board == "cmoda7_15t":
    if qspi:
        run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])
    else:
        run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_15t.tcl","./block_design/bd_design_1.tcl"])
    
elif board == "cmoda7_35t":
    if qspi:
        run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])
    else:
        run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_cmoda7_35t.tcl","./block_design/bd_design_1.tcl"])
    
elif board == "zybo":
    if qspi:
        run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_zybo.tcl","./block_design/bd_design_1.tcl"])
    else:
        run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_zybo.tcl","./block_design/bd_design_1.tcl"])
    
elif board == "pynqz2":
    if qspi:
        run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_qspi_pynqz2.tcl","./block_design/bd_design_1.tcl"])
    else:
        run(["cp",f"{os.environ['REPO_fpga']}/tcl/bd_interfaz_pynqz2.tcl","./block_design/bd_design_1.tcl"])
        
        
## partial flows (tcl)
run(["mkdir","./partial_flows"])

vivado_files=f"{proj_dir}/vivado_src/top.v {proj_dir}/vivado_src/interfaz_pspl.cp.v {proj_dir}/vivado_src/romatrix.v {proj_dir}/vivado_src/medidor_frec.cp.v {proj_dir}/vivado_src/interfaz_pspl_config.vh"
if debug:
    vivado_files+=f" {proj_dir}/vivado_src/clock_divider.cp.v"
if pblock != "no":
    vivado_files+=f" {proj_dir}/vivado_src/pblock.xdc"

with open("./partial_flows/setupdesign.tcl", "w") as f:
    f.write(f"""
create_project {proj_name} {proj_dir}/{proj_name} -part {fpga_part}

set_property board_part {board_part} [current_project]

source {proj_dir}/block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

update_compile_order -fileset sources_1

add_files -norecurse {{ {vivado_files} }}

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

startgroup
set_property -dict [list CONFIG.C_GPIO_WIDTH {data_width} CONFIG.C_GPIO2_WIDTH {data_width}] [get_bd_cells axi_gpio_data]
endgroup

connect_bd_net [get_bd_pins TOP_0/data_in] [get_bd_pins axi_gpio_data/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins TOP_0/ctrl_in] [get_bd_pins axi_gpio_ctrl/gpio2_io_o]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {{Clk {clk_name} }}  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files {proj_dir}/{proj_name}/{proj_name}.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse {proj_dir}/{proj_name}/{proj_name}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

set_property top design_1_wrapper [current_fileset]

update_compile_order -fileset sources_1

set_property STEPS.SYNTH_DESIGN.ARGS.RESOURCE_SHARING off [get_runs synth_1]

""")

with open("./partial_flows/genbitstream.tcl", "w") as f:
    f.write(f"""
if {{[file exists {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc
}}

if {{[file exists {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc]==1}} {{
    export_ip_user_files -of_objects  [get_files {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc] -no_script -reset -force -quiet
    remove_files  -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc
}}

if {{[file exists {proj_dir}/{proj_name}/{proj_name}.runs/synth_1]==1}} {{
    reset_run synth_1
}}

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0

launch_runs synth_1 -jobs {n_jobs}

wait_on_run synth_1

""")
    if qspi and board == "cmoda7_15t":
        f.write(f"""
file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1

file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new

close [ open {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc]

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
    elif qspi and board == "cmoda7_35t":
        f.write(f"""
file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1

file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new

close [ open {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/bitstreamconfig.xdc]

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

    if detail_routing:
        f.write("""
open_run synth_1 -name synth_1

startgroup
""")
        for i in range(N_osc):
            f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_0]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_0 }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_{i} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_0 design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/AND_{i} }}]
""")
            for j in range(1,N_inv,1):
                f.write(f"""
route_design -nets [get_nets design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j}]
set_property is_route_fixed 1 [get_nets {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/w_{i}_{j} }}]
set_property is_bel_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
set_property is_loc_fixed 1 [get_cells {{design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_{j} design_1_i/TOP_0/inst/romatrix_interfaz_pl_frontend/romatrix/inv_{i}_${j-1} }}]
""") 
        f.write(f"""
endgroup

file mkdir {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new

close [ open {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc w ]

add_files -fileset constrs_1 {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc

set_property target_constrs_file {proj_dir}/{proj_name}/{proj_name}.srcs/constrs_1/new/detail_routing.xdc [current_fileset -constrset]

save_constraints -force

close_design

update_compile_order -fileset sources_1

update_module_reference design_1_TOP_0_0
""") # Aqui termina detailed routing

    f.write(f"""
if {{[file exists {proj_dir}/{proj_name}/{proj_name}.runs/synth_1/__synthesis_is_complete__]==1}} {{
    reset_run synth_1
}}
    
launch_runs impl_1 -to_step write_bitstream -jobs {n_jobs}

wait_on_run impl_1
""")

with open("./partial_flows/launchsdk.tcl", "w") as f:
    f.write(f"""
file mkdir {proj_dir}/{proj_name}/{proj_name}.sdk

file copy -force {proj_dir}/{proj_name}/{proj_name}.runs/impl_1/design_1_wrapper.sysdef {proj_dir}/{proj_name}/{proj_name}.sdk/design_1_wrapper.hdf

launch_sdk -workspace {proj_dir}/{proj_name}/{proj_name}.sdk -hwspec {proj_dir}/{proj_name}/{proj_name}.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform

""")

with open("mkhwdplatform.tcl", "w") as f:
    f.write(f"""
source {proj_dir}/partial_flows/setupdesign.tcl

source {proj_dir}/partial_flows/genbitstream.tcl

source {proj_dir}/partial_flows/launchsdk.tcl

""")


## vivado sources
run(["mkdir","./vivado_src"])
run(["cp",f"{os.environ['REPO_fpga']}/verilog/interfaz_pspl.v","./vivado_src/interfaz_pspl.cp.v"])
run(["cp",f"{os.environ['REPO_fpga']}/verilog/medidor_frec.v","./vivado_src/medidor_frec.cp.v"])
if debug:
    run(["cp",f"{os.environ['REPO_fpga']}/verilog/clock_divider.v","./vivado_src/clock_divider.cp.v"])

with open("vivado_src/interfaz_pspl_config.vh", "w") as f:
    f.write(f"""{dw_ge_biw}
{biw_aligned_dw}
{biw_misaligned_dw}
{dw_ge_bow}
{bow_aligned_dw}
{bow_misaligned_dw}
""")

genRomatrix(out_name="vivado_src/romatrix.v", N_inv=N_inv, tipo=tipo_lut, pinmap_opt=pinmap, minsel=minsel, posmap=posmap, debug=debug)

with open("vivado_src/top.v", "w") as f:
    if N_osc==1:
        aux=""
    else:
        aux=f".sel_ro(buffer_in[{N_bits_osc-1}:0]),"
    if tipo_lut == "lut1":
        aux1=""
    else:
        aux1=f".sel_pdl(buffer_in[{buffer_in_width-5-1}:{N_bits_osc}]),"
        
    f.write(f"""`timescale 1ns / 1ps


module TOP (
    input           clock,
    input[7:0]      ctrl_in,
    output[7:0]     ctrl_out,
    input[{data_width-1}:0] data_in,
    output[{data_width-1}:0]    data_out
);

    wire[{buffer_in_width-1}:0] buffer_in;
    wire[{buffer_out_width-1}:0] buffer_out;
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
        .DATA_WIDTH({data_width}),
        .BUFFER_IN_WIDTH({buffer_in_width}),
        .BUFFER_OUT_WIDTH({buffer_out_width})
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
        .OUT_WIDTH({buffer_out_width})
    ) medidor_frec (
        .clock(clock),
        .enable(enable_medidor),
        .clock_u(out_ro),
        .resol(buffer_in[{buffer_in_width-1}:{buffer_in_width-5}]),
        .lock(ack),
        .out(buffer_out)
    );
    
endmodule
""")


if pblock!="no":
    PBLOCK_CORNERS=pblock.split()
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
    if board == "cmoda7_15t" or board == "cmoda7_35t":
        f.write("#include \"xuartlite.h\"\n\n")
    elif board == "zybo" or board == "pynqz2":
        f.write("#include \"xuartps.h\"\n\n")

    f.write(f"""#define DATA_WIDTH          {data_width}
#define BUFFER_IN_WIDTH     {buffer_in_width}
#define BUFFER_OUT_WIDTH    {buffer_out_width}
#define OCTETO_IN_WIDTH     {octeto_in_width}
#define OCTETO_OUT_WIDTH    {octeto_out_width}
#define WORDS_IN_WIDTH      {words_in_width}
#define WORDS_OUT_WIDTH     {words_out_width}
""")

## config file
with open("medir_romatrix.config","w") as f:
    f.write(f""" sel_ro_width = {N_bits_osc}
 sel_pdl_width = {N_bits_pdl}
 sel_resol_width = {5}
 buffer_out_width = {buffer_out_width}
""")


## log info
print(f"""
 N_osc = {N_osc}
 N_pdl = {2**N_bits_pdl}
 
 Trama del selector(ts): {N_bits_osc} {N_bits_pdl} 5
 
    (0) <---sel_ro({N_bits_osc})---><---sel_pdl({N_bits_pdl})---><---sel_resol(5)---> ({buffer_in_width-1})
    
    
 sel_ro_width = {N_bits_osc}
 sel_pdl_width = {N_bits_pdl}
 dw  = {data_width}
 biw = {buffer_in_width}
 bow = {buffer_out_width}
 
 fpga part: {fpga_part}
 
 sdk source files: {proj_dir}/sdk_src/
 
""")
if qspi:
    print(f"q-spi part: {memory_part}")
