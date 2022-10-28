create_project project_1 C:/wdir/project_1 -part xc7a15tcpg236-1

set_property board_part digilentinc.com:cmod_a7-15t:part0:1.1 [current_project]

source C:/wdir/.block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

update_compile_order -fileset sources_1

add_files -norecurse C:/wdir/.vivado_src/top.v

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins axi_gpio_ctrl/gpio2_io_o] [get_bd_pins TOP_0/ctrl_in]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_1/clk_out1 (100 MHz)" }  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

save_bd_design

validate_bd_design

save_bd_design

make_wrapper -files [get_files C:/wdir/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse C:/wdir/project_1/project_1.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

update_compile_order -fileset sources_1

file mkdir C:/wdir/project_1/project_1.srcs/constrs_1

file mkdir C:/wdir/project_1/project_1.srcs/constrs_1/new

close [ open C:/wdir/project_1/project_1.srcs/constrs_1/new/bitstreamconfig.xdc w ]

add_files -fileset constrs_1 C:/wdir/project_1/project_1.srcs/constrs_1/new/bitstreamconfig.xdc

set_property target_constrs_file C:/wdir/project_1/project_1.srcs/constrs_1/new/bitstreamconfig.xdc [current_fileset -constrset]

set_property used_in_synthesis false [get_files  C:/wdir/project_1/project_1.srcs/constrs_1/new/bitstreamconfig.xdc]

update_compile_order -fileset sources_1

launch_runs synth_1 -jobs 4

wait_on_run synth_1

open_run synth_1 -name synth_1

startgroup

set_property BITSTREAM.CONFIG.CONFIGRATE 6 [get_designs synth_1]

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [get_designs synth_1]

set_property config_mode SPIx4 [current_design]

endgroup

save_constraints

close_design

launch_runs impl_1 -to_step write_bitstream -jobs 4

wait_on_run impl_1

update_compile_order -fileset sources_1

file mkdir C:/wdir/project_1/project_1.sdk

write_hwdef -force  -file C:/wdir/project_1/project_1.sdk/design_1_wrapper.hdf

launch_sdk -workspace C:/wdir/project_1/project_1.sdk -hwspec C:/wdir/project_1/project_1.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform
