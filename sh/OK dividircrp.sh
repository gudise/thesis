if test "$1" = "-help"
then
	echo "Este script toma cualquier fichero .crp presente en la carpeta y lo divide en dos ficheros .crpx (retos) y .crpy (respuestas). Se utiliza los nombres 'crpx' y 'crpy' por coherencia, pero de hecho estos ficheros son matrices .mtz"
	exit
fi

IFS=$'\n'
for i in `ls *.crp`
do
	namex=`echo "${i}x"`
	rm $namex
	
	namey=`echo "${i}y"`
	rm $namey
	
	n=0
	nx=0
	ny=0
	for line in `fscanf $i`
	do
		((n=n+1))

		if (($n > 3))
		then
			if (($nx == $ny))
			then
				printf "%s\n\n" "$line" >> $namex
				
				if ((nx == 0))
				then
					ncolx=`echo "$line" | wc -w`
				fi
				
				((nx = nx+1))

			else
				printf "%s\n\n" "$line" >> $namey
				
				if ((ny == 0))
				then
					ncoly=`echo "$line" | wc -w`
				fi
				
				((ny = ny+1))
			fi		
		fi
	done
	
	echo -e "# [N_items = $nx]\n# [tamano_reto = $ncolx] \n\n`cat $namex`" > $namex
	echo -e "# [N_items = $ny]\n# [tamano_respuesta = $ncoly] \n\n`cat $namey`" > $namey	
done

