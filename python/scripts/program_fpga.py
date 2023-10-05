import os
from subprocess import run

projdir = os.path.dirname(os.path.abspath(__file__)).replace("\\","/")
sdkdir = f"{projdir}/project_vivado/project_vivado.sdk"
appname = "app"

if os.name=='nt': # Estamos en Windows
    run(["C:/Xilinx/SDK/2019.1/bin/xsdk.bat","-batch","-source",f"{projdir}/flow/program/program_fpga.cp.tcl",sdkdir,appname])

elif os.name=='posix': # Estamos en Unix
    run(["/tools/Xilinx/SDK/2019.1/bin/xsdk","-batch","-source",f"{projdir}/flow/program/program_fpga.cp.tcl",sdkdir,appname])
