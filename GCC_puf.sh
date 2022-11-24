#script para facilitar la compilacion utilizando las bibliotecas 'gsl', 'gmp' y 'mpfr' y la API 'openMP'. El script esta disenado para que las librerias esten instaladas bajo los directorios $HOME/gsl, $HOME/gmp y $HOME/mpfr


if test "$1" = "-help"
then
    echo "GCC a b"
    echo "donde:"
    echo "  a = fuente"
    echo "  b = output"
    echo "Se incluye informacion para debuggear (usando gdb o valgrind)"
	echo ""
    
    exit
fi

gcc "${1}" -o "${2}" -g -lm -lgsl -lgslcblas -lgmp -lmpfr -fopenmp -I"$REPO_puf/c/include" # utilizar esta línea si las bibliotecas gsl, gmp y mpfr se han instalado usando el gestor de paquetes 'apt'
#gcc "${1}" -o "${2}" -g -lm -lgsl -lgslcblas -lgmp -lmpfr -fopenmp -I"$REPO_puf/c/include" -I"$GSL_DIR/include" -I"$GMP_DIR/include" -I"$MPFR_DIR/include" -L"$GSL_DIR/lib" -L"$GMP_DIR/lib" -L"$MPFR_DIR/lib" # usar esta línea si las bibliotecas gsl, gmp y mpfr se han compilado desde la fuente.
