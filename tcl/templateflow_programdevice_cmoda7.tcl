## Concatenacion de bitstream + ELF files 
add_files -norecurse C:/wdir/project_1/project_1.sdk/app/Debug/app.elf

set_property used_in_simulation 0 [get_files C:/wdir/project_1/project_1.sdk/app/Debug/app.elf]

set_property SCOPED_TO_REF design_1 [get_files -all -of_objects [get_fileset sources_1] {C:/wdir/project_1/project_1.sdk/app/Debug/app.elf}]

set_property SCOPED_TO_CELLS { microblaze_0 } [get_files -all -of_objects [get_fileset sources_1] {C:/wdir/project_1/project_1.sdk/app/Debug/app.elf}]

reset_run -prev_step impl_1

launch_runs impl_1 -to_step write_bitstream -jobs 4

wait_on_run impl_1

open_hw

connect_hw_server

open_hw_target

set_property PROGRAM.FILE {C:/wdir/project_1/project_1.runs/impl_1/design_1_wrapper.bit} [get_hw_devices xc7a15t_0]

current_hw_device [get_hw_devices xc7a15t_0]

refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7a15t_0] 0]

set_property PROBES.FILE {} [get_hw_devices xc7a15t_0]

set_property FULL_PROBES.FILE {} [get_hw_devices xc7a15t_0]

set_property PROGRAM.FILE {C:/wdir/project_1/project_1.runs/impl_1/design_1_wrapper.bit} [get_hw_devices xc7a15t_0]

program_hw_devices [get_hw_devices xc7a15t_0]

refresh_hw_device [lindex [get_hw_devices xc7a15t_0] 0]






