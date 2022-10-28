## Concatenacion de bitstream + ELF files 
add_files -norecurse {C:/wdir/project_1/project_1.sdk/boot/Debug/boot.elf C:/wdir/project_1/project_1.sdk/app/Debug/app.elf}

set_property used_in_simulation 0 [get_files C:/wdir/project_1/project_1.sdk/boot/Debug/boot.elf]

set_property used_in_simulation 0 [get_files C:/wdir/project_1/project_1.sdk/app/Debug/app.elf]

set_property SCOPED_TO_REF design_1 [get_files -all -of_objects [get_fileset sources_1] {C:/wdir/project_1/project_1.sdk/app/Debug/app.elf}]

set_property SCOPED_TO_CELLS { microblaze_0 } [get_files -all -of_objects [get_fileset sources_1] {C:/wdir/project_1/project_1.sdk/app/Debug/app.elf}]

reset_run -prev_step impl_1

launch_runs impl_1 -to_step write_bitstream -jobs 4

wait_on_run impl_1

write_cfgmem  -format bin -size 4 -interface SPIx4 -loadbit {up 0x00000000 "C:/wdir/project_1/project_1.runs/impl_1/design_1_wrapper.bit" } -file "C:/wdir/project_1/bitstream.bin"

open_hw

connect_hw_server

open_hw_target

set_property PROGRAM.FILE {C:/wdir/project_1/project_1.runs/impl_1/design_1_wrapper.bit} [get_hw_devices xc7a15t_0]

current_hw_device [get_hw_devices xc7a15t_0]

refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7a15t_0] 0]

create_hw_cfgmem -hw_device [lindex [get_hw_devices xc7a15t_0] 0] [lindex [get_cfgmem_parts {mx25l3233f-spi-x1_x2_x4}] 0]

set_property PROGRAM.BLANK_CHECK  0 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.ERASE  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.CFG_PROGRAM  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.VERIFY  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.CHECKSUM  0 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

refresh_hw_device [lindex [get_hw_devices xc7a15t_0] 0]

set_property PROGRAM.ADDRESS_RANGE  {use_file} [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.FILES [list "C:/wdir/project_1/bitstream.bin" ] [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.PRM_FILE {} [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.UNUSED_PIN_TERMINATION {pull-none} [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.BLANK_CHECK  0 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.ERASE  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.CFG_PROGRAM  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.VERIFY  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

set_property PROGRAM.CHECKSUM  0 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

startgroup

create_hw_bitstream -hw_device [lindex [get_hw_devices xc7a15t_0] 0] [get_property PROGRAM.HW_CFGMEM_BITFILE [ lindex [get_hw_devices xc7a15t_0] 0]]; program_hw_devices [lindex [get_hw_devices xc7a15t_0] 0]; refresh_hw_device [lindex [get_hw_devices xc7a15t_0] 0];

program_hw_cfgmem -hw_cfgmem [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a15t_0] 0]]

endgroup
