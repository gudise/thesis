if test "$1" = "-help"
then
	echo "Compila todos los programas fuente de la carpeta c en /exe"
	exit
fi

cd "$REPOS_WRAPPER/exe"
if ! test -e "$REPO_fpga_NAME"
then
	mkdir fpga
fi

cd "$REPO_fpga"

GCC_fpga.sh c/calc_fft.c						../exe/calc_fft
GCC_fpga.sh c/calc_nbits.c						../exe/calc_nbits

GCC_fpga.sh c/hwd_genROmatriz.c						../exe/hwd_genROmatriz
GCC_fpga.sh c/hwd_genRO.c							../exe/hwd_genRO
GCC_fpga.sh c/hwd_genConfigurableRO.c				../exe/hwd_genConfigurableRO



echo "-- Compilación terminada --"
echo ""
