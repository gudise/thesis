#include "main.h"
#include "hardware.h"



void main(int N_opcion, char** opcion)
{   
    int i;

	//lectura de opciones
	for(i=0; i<N_opcion; i++)
	{
		if( strcmp(opcion[i], "-help")==0 )
		{
			printf("\ncalc_nbits x , este programa devuelve el numero de bits necesarios para alojar 'x' estados (bien redondeados).\n\n");

			exit(0);
		}
	}
	
	int Nestados;
	
	sscanf(opcion[1], "%d", &Nestados);
	
	printf("%d", N_bits_MUX(Nestados));
    
    //return 0;
}

