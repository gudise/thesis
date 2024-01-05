## Este script lee un fichero 'SDK.log' generado por XSDK, y produce un fichero 'SDK.tcl' con todos los comandos ejecutados. Notar que el nivel de LO en la aplicación SDK debe estar ajustado a 'Trace'.

cmds = []
with open('SDK.log','r') as f:
    for line in f:
        aux = line.split(' ')
        for iword,word in enumerate(aux):
            if word == 'XSCT':
                if aux[iword+1] == 'Command:':
                    cmds.append(line)

with open('SDK.tcl', 'w') as f:
    for cmd in cmds:

        f.write(cmd[cmd.find("[")+1:cmd.rfind("]")])
        f.write("\n\n")
