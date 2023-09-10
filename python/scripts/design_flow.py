import os
from subprocess import run


projdir = os.path.dirname(__file__).replace("\\","/")

if os.name=='nt': # Estamos en Windows
    run(["C:/Xilinx/Vivado/2019.1/bin/vivado.bat","-mode","batch","-source",f"{projdir}/design_flow/vivado_flow.tcl"])
    run(["C:/Xilinx/SDK/2019.1/bin/xsdk.bat","-batch","-source",f"{projdir}/design_flow/sdk_flow.tcl"])

elif os.name=='posix': # Estamos en Unix
    run(["/tools/Xilinx/Vivado/2019.1/bin/vivado","-mode","batch","-source",f"{projdir}/design_flow/vivado_flow.tcl"])
    run(["/tools/Xilinx/SDK/2019.1/bin/xsdk","-batch","-source",f"{projdir}/design_flow/sdk_flow.tcl"])
