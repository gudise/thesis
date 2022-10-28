#include <main.h>


int main(int N_opcion, char** opcion)
{
	int i, j, N_dim=2, N_repeticiones=3, preservar=0;
	char input[256]={"input.mtz"}, output[256]={".temp"};
	MATRIZ* entrada;
	FILE* punte;
	
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa convierte una matriz (tomada como una trayectoria) en un fichero de texto y lo anima con gnuplot \n----------------------------------\nOpciones:\n\n");
            printf("-in [input.mtz],  nombre del fichero de entrada\n\n");
            printf("-out [nombre_de_salida], esta variable es opcional: si esta presente el programa escribira el fichero de salida para que gnuplot lo pueda leer\n\n");
            printf("-ndim [2], numero de dimensiones de la trayectoria: 1, 2, 3\n\n");
			printf("-nrep [3], numero de repeticiones en bucle de la animacion\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
            sprintf(input, "%s", opcion[i+1]);
		
        if( strcmp(opcion[i], "-out")==0 )
		{
            sprintf(output, "%s", opcion[i+1]);
			preservar=1;
		}
        
        if( strcmp(opcion[i], "-ndim")==0 )
            sscanf(opcion[i+1], "%d", &N_dim);
			
        if( strcmp(opcion[i], "-nrep")==0 )
            sscanf(opcion[i+1], "%d", &N_repeticiones);
    }
	
	entrada = leerMatriz(input);
	
	if(entrada->N_columnas%N_dim!=0)
	{
		printf("\nERROR: el numero de columnas de la entrada (%ld) es incompatible con la dimension asignada (%d)\n", entrada->N_columnas, N_dim);
		exit(0);
	}
		
	punte = fopen(output, "w");
	for(i=0; i<entrada->N_filas; i++)
	{
		if(N_dim==1)
		{
			for(j=0; j<entrada->N_columnas; j++)
				fprintf(punte, "%lf\t0\n", entrada->elemento[i][j]);
		}
		else if(N_dim==2)
		{
			for(j=0; j<entrada->N_columnas/2; j+=2)
				fprintf(punte, "%lf\t%lf\n", entrada->elemento[i][j], entrada->elemento[i][j+1]);
		}
		else
		{
			for(j=0; j<entrada->N_columnas/3; j+=3)
				fprintf(punte, "%lf\t%lf\t%lf\n", entrada->elemento[i][j], entrada->elemento[i][j+1], entrada->elemento[i][j+3]);
		}
		
		if(i<entrada->N_filas-1)
			fprintf(punte, "\n\n");
	}
	fclose(punte);
	
	animacionGnuplot(output, entrada->N_filas, N_dim, N_repeticiones);
	
	freeMatriz(entrada);
	
	if(!preservar)
		remove(output);
    
    return 0;
}
