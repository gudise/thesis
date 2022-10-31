if test "$1" = "-help"
then
	echo "Compila todos los programas fuente de la carpeta c en /exe"
	exit
fi

cd "$REPOS_WRAPPER/exe"
if ! test -e "$REPO_puf_NAME"
then
	mkdir puf
fi

cd "$REPO_puf"

GCC_puf.sh c/analizar_puf.c	"$REPOS_WRAPPER/exe/$REPO_puf_NAME/analizar_puf.x"
GCC_puf.sh c/gen_topol.c	"$REPOS_WRAPPER/exe/$REPO_puf_NAME/gen_topol.x"

echo ""
echo "-- Compilación terminada --"
echo ""
