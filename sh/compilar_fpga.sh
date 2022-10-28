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

GCC_fpga.sh c/calc_nbits.c				"$REPOS_WRAPPER/exe/$REPO_fpga_NAME/calc_nbits.x"
GCC_fpga.sh c/hwd_genRO.c				"$REPOS_WRAPPER/exe/$REPO_fpga_NAME/hwd_genRO.x"
GCC_fpga.sh c/hwd_genConfigurableRO.c	"$REPOS_WRAPPER/exe/$REPO_fpga_NAME/hwd_genConfigurableRO.x"



echo "-- Compilación terminada --"
echo ""
