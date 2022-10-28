if test "$1" = "-help"
then
	printf "pinout [zybo|pynq]\nEste programa copia en el directorio actual el fichero de pines maestro de la placa correspondiente\n\n"
	exit
fi

if test "$1" = "zybo"
then
    cp "$COMPUTACION/../recursos_consulta/diseno electronico/FPGA/Zybo/pinout_zybo.xdc" ./
    exit
fi

if test "$1" = "pynq"
then
    cp "$COMPUTACION/../recursos_consulta/diseno electronico/FPGA/PYNQ-Z2/pinout_pynqz2.xdc" ./
    exit
fi
