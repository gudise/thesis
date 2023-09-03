### NOTA: Cuando funcione, este script se lanzará después del script 'mkhwdplatform' de Vivado, y generará una aplicación con las fuentes SDK correctas. La idea es lanzar después el script SDK.tcl para que todo TODO el proceso de programación se pueda automatizar.

set projdir "/home/gds/Escritorio/WDIR/borrar/borrar"
set projname "ropuf-pdl"
set app_name "my_app"


# hsi::utils::get_supported_os -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf

::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf

# ::hsi::utils::get_all_app_details -json
# 
# ::hsi::utils::is_app_supported_on_hw_sw  -hw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf -os standalone -processor ps7_cortexa9_0 -app hello_world -sw {} -arch {}
# 
# ::hsi::utils::is_app_supported_on_hw_sw  -hw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf -os standalone -processor ps7_cortexa9_0 -app hello_world -sw {} -arch {}

# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::get_all_app_details -json
# 
# ::hsi::utils::is_app_supported_on_hw_sw  -hw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf -os standalone -processor ps7_cortexa9_0 -app empty_application -sw {} -arch {}

::hsi::utils::write_sw_mss -hw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf -sw ${app_name}_bsp -dir $projdir/$projname/$projname.sdk/${app_name}_bsp -processor ps7_cortexa9_0 -os standalone -app empty_application

#  ::hsi::utils::set_current_hw_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss 
# 
# ::hsi::utils::get_hw_sw_details -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss

::hsi::utils::generate_bsp_sources $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsp/system.mss $projdir/$projname/$projname.sdk/${app_name}_bsp

# ::hsi::utils::get_libs_from_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::get_sw_libs -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::get_connected_periphs $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf ps7_cortexa9_0
# 
# ::hsi::utils::get_drivers_for_sw -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# hsi::utils::get_drivers_for_hw -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::get_libs_from_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::get_sw_libs -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss

#::hsi::utils::closesw $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss

#  ::hsi::utils::set_current_hw_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss 
# 
# ::hsi::utils::get_hw_sw_details -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::openhw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf
# 
# ::hsi::utils::opensw $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss

::hsi::utils::generate_app_template -hw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf -sw $projdir/$projname/$projname.sdk/${app_name}_bsp/system.mss -app empty_application -processor ps7_cortexa9_0 -os standalone -dir $projdir/$projname/$projname.sdk/$app_name/src

#  ::hsi::utils::set_current_hw_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss 
# 
# ::hsi::utils::get_hw_sw_details -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::get_drivers_for_sw -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
#  ::hsi::utils::set_current_hw_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss 
# 
# ::hsi::utils::get_hw_sw_details -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::get_libs_from_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::closesw $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
#  ::hsi::utils::set_current_hw_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss 
# 
# ::hsi::utils::get_hw_sw_details -json $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::get_libs_from_sw $projdir/$projname/$projname.sdk/design_1_wrapper_hw_platform_0/system.hdf $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss
# 
# ::hsi::utils::closesw $projdir/$projname/$projname.sdk/${app_name}_bsb/system.mss













#importsources -name $app_name -path /home/gds/Escritorio/WDIR/borrar/ropuf-pdl/sdk_src

