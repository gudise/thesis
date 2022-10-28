set number_of_jobs 4

create_project project_1 C:/wdir/project_1 -part xc7z010clg400-1

set_property board_part digilentinc.com:zybo:part0:1.0 [current_project]

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

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/processing_system7_0/FCLK_CLK0 (100 MHz)" }  [get_bd_pins TOP_0/clock]

regenerate_bd_layout

validate_bd_design

save_bd_design

make_wrapper -files [get_files C:/wdir/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd] -top

add_files -norecurse C:/wdir/project_1/project_1.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v

update_compile_order -fileset sources_1

update_compile_order -fileset sources_1

update_compile_order -fileset sources_1

launch_runs impl_1 -to_step write_bitstream -jobs $number_of_jobs

wait_on_run impl_1

update_compile_order -fileset sources_1

file mkdir C:/wdir/project_1/project_1.sdk

file copy -force C:/wdir/project_1/project_1.runs/impl_1/design_1_wrapper.sysdef C:/wdir/project_1/project_1.sdk/design_1_wrapper.hdf

launch_sdk -workspace C:/wdir/project_1/project_1.sdk -hwspec C:/wdir/project_1/project_1.sdk/design_1_wrapper.hdf
## Aqui termina la generacion de hwd_platform
