set number_of_jobs 4

create_project project_1 C:/wdir/project_1 -part xc7z020clg400-1

set_property board_part tul.com.tw:pynq-z2:part0:1.0 [current_project]

source C:/wdir/.block_design/bd_design_1.tcl

update_compile_order -fileset sources_1

regenerate_bd_layout

update_compile_order -fileset sources_1

add_files -norecurse {C:/wdir/.vivado_src/slow_ro.v C:/wdir/.vivado_src/fast_ro.v C:/wdir/.vivado_src/medium_ro.v C:/wdir/.vivado_src/top.v C:/wdir/.vivado_src/multiplex_4_1.v}

update_compile_order -fileset sources_1

create_bd_cell -type module -reference TOP TOP_0

connect_bd_net [get_bd_pins TOP_0/data_out] [get_bd_pins axi_gpio_data/gpio_io_i]

connect_bd_net [get_bd_pins axi_gpio_ctrl/gpio2_io_o] [get_bd_pins TOP_0/ctrl_in]

connect_bd_net [get_bd_pins TOP_0/ctrl_out] [get_bd_pins axi_gpio_ctrl/gpio_io_i]

startgroup
make_bd_pins_external  [get_bd_pins TOP_0/osciloscopio]
endgroup

startgroup
make_bd_pins_external  [get_bd_pins TOP_0/selector]
endgroup

set_property name osciloscopio [get_bd_ports osciloscopio_0]

set_property name selector [get_bd_ports selector_0]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/processing_system7_0/FCLK_CLK0 (100 MHz)" }  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files C:/wdir/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse C:/wdir/project_1/project_1.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

set_property top design_1_wrapper [current_fileset]

update_compile_order -fileset sources_1

launch_runs synth_1 -jobs $number_of_jobs

wait_on_run synth_1

open_run synth_1 -name synth_1

place_ports osciloscopio Y18

place_ports {selector[0]} M20

place_ports {selector[1]} M19

set_property IOSTANDARD LVCMOS33 [get_ports [list osciloscopio]]

set_property IOSTANDARD LVCMOS33 [get_ports [list {selector[0]} {selector[1]}]]

file mkdir C:/wdir/project_1/project_1.srcs/constrs_1/new

close [ open C:/wdir/project_1/project_1.srcs/constrs_1/new/constraints.xdc w ]

add_files -fileset constrs_1 C:/wdir/project_1/project_1.srcs/constrs_1/new/constraints.xdc

set_property target_constrs_file C:/wdir/project_1/project_1.srcs/constrs_1/new/constraints.xdc [current_fileset -constrset]

save_constraints -force

close_design

reset_run synth_1

launch_runs impl_1 -to_step write_bitstream -jobs $number_of_jobs

wait_on_run impl_1

update_compile_order -fileset sources_1

file mkdir C:/wdir/project_1/project_1.sdk

write_hwdef -force  -file C:/wdir/project_1/project_1.sdk/design_1_wrapper.hdf

launch_sdk -workspace C:/wdir/project_1/project_1.sdk -hwspec C:/wdir/project_1/project_1.sdk/design_1_wrapper.hdf

## Aqui termina la generacion de hwd_platform
