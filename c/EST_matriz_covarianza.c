#include "include/main.h"
#include "include/matematicas.h"

int main(int N_opcion, char** opcion)
{
    int i,j,plot=0,N_lineas;
    char input[256]={"input.mtz"},output[256]={"output.mtz"};
	MATRIZ* entrada, *covar;
    FILE *punte;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nCalculo de la matriz de covarianza (la entrada sera una trayectoria) \n----------------------------------\nOpciones:\n\n");
            printf("-in [input.mtz]  fichero con la matriz de entrada (ver 'doc/my_doc')\n\n");
            printf("-out [output.mtz]  nombre del fichero de salida\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
            sprintf(input, "%s", opcion[i+1]);
        
        if( strcmp(opcion[i], "-out")==0 )
            sprintf(output, "%s", opcion[i+1]);
    }
    
	
	entrada = leerMatriz(input);

	covar = matrizCovarianza(entrada);
        
	punte = fopen(output, "w"); 
	pintarMatriz(covar, punte);
	fclose(punte);
	
	freeMatriz(entrada);
	freeMatriz(covar);
	
    
    return 0;
}
