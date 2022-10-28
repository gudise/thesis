#include "include/main.h"
#include "include/matematicas.h"

int main(int N_opcion, char** opcion)
{
    int i,j,plot=0,N_lineas;
    char input[256]={"input.mtz"}, output[256]={"output.mtz"}, flagin=0, flagout=0;
	gsl_matrix* entrada, *salida;
    FILE *punte;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa calcula la matriz traspuesta de la entrada 'input.mtz' \n----------------------------------\nOpciones:\n\n");
            printf("-in [input.mtz], si esta opcion esta presente el programa leera la entrada de un fichero. En caso contrario leera la entrada de stdin\n\n");
			printf("-out [output.mtz], si esta opcion esta presente el programa volcara la salida en un fichero. En caso contrario volcara la salida en stdout\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
        {
            flagin=1;
            if(i+1 < N_opcion)
            {
                if(opcion[i+1][0]!='-')
                    sprintf(input, "%s", opcion[i+1]);
            }
        }
        
        if( strcmp(opcion[i], "-out")==0 )
        {
            flagout=1;
            if(i+1 < N_opcion)
            {
                if(opcion[i+1][0]!='-')
                    sprintf(output, "%s", opcion[i+1]);
            }
        }
    }
    
    if(flagin)
        punte = fopen(input, "r");
    else
        punte = stdin;
    entrada = GSL_leerMatriz(punte);
    if(flagin)
        fclose(punte);
    
    salida = gsl_matrix_alloc(entrada->size2, entrada->size1);
	
	gsl_matrix_transpose_memcpy(salida, entrada);
        
    if(flagout)
        punte = fopen(output, "w");
    else
        punte = stdout; 
    GSL_pintarMatriz(salida, punte);
    if(flagout)
        fclose(punte);
	
	gsl_matrix_free(entrada);
    gsl_matrix_free(salida);
	
    return 0;
}
