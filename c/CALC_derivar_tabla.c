#include "main.h"
#include "matematicas.h"
#include "digital.h"


int main(int N_opcion, char** opcion)
{
	int i, j, N_filas=0, N_columnas=0, *wheredatastarts;
	char in_name[256]={"input.txt"}, out_name[256]={"output.txt"};
    TABLA* tabla_in, *tabla_new;
	
	for(i=0; i<N_opcion; i++)
	{
		if(strcmp(opcion[i], "-help")==0)
		{
			printf("Este programa calcula la derivada numérica del fichero de entrada. Por defecto el programa considera que la primera fila es de títulos y la primera columna la coordenada 'x' respecto de la que se derivan las demás columnas\n----------------\n\n");
			printf("-in [input.txt], nombre del fichero de entrada\n\n");
			printf("-out [output.txt], nombre del fichero de salida\n\n");			
			
			exit(0);
		}
		
		if(strcmp(opcion[i], "-in")==0)
			sscanf(opcion[i+1], "%s", in_name);
			
		if(strcmp(opcion[i], "-out")==0)
			sscanf(opcion[i+1], "%s", out_name);
		
	}
	
	tabla_in = leerTabla(in_name);

    tabla_new = mallocTabla(tabla_in->N_filas-1, tabla_in->N_columnas);

    for(i=0; i<tabla_new->N_columnas; i++)
        sscanf(tabla_in->encabezado[i], "%s", tabla_new->encabezado[i]);
    
    for(i=0; i<tabla_new->N_filas-1; i++)
    {
        tabla_new->data[i][0]=tabla_in->data[i][0];
        for(j=1; j<tabla_new->N_columnas; j++)
            tabla_new->data[i][j] = (tabla_in->data[i+1][j]-tabla_in->data[i][j])/(tabla_in->data[i+1][0]-tabla_in->data[i][0]);
    }
    
    pintarTabla(tabla_new, out_name);

    freeTabla(tabla_in);
    freeTabla(tabla_new);
	
	return 0;
}

