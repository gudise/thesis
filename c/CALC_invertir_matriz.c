#include "main.h"
#include "matematicas.h"

int main(int N_opcion, char** opcion)
{
    char in_name[256]={"input.mtz"},out_name[256]={"output.mtz"}, flagin=0, flagout=0;
	gsl_matrix* entrada;
    FILE *punte;
    
    //lectura de opciones
    for(int i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nCalculo de la matriz inversa (implementado por la biblioteca gsl) \n----------------------------------\nOpciones:\n\n");
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
                    sprintf(in_name, "%s", opcion[i+1]);
            }
        }
        
        if( strcmp(opcion[i], "-out")==0 )
        {
            flagout=1;
            if(i+1 < N_opcion)
            {
                if(opcion[i+1][0]!='-')
                    sprintf(out_name, "%s", opcion[i+1]);
            }
        }
    }
    
    if(flagin==0)
    {
        punte = stdin;
        entrada = GSL_leerMatriz(punte);
    }
    else
    {
        punte = fopen(in_name, "r");
        entrada = GSL_leerMatriz(punte);
        fclose(punte);
    }

    GSL_invertirMatriz(entrada);
        
    if(flagout)
    {
        punte = fopen(out_name, "w");
        GSL_pintarMatriz(entrada, punte);
        fclose(punte);
    }
    else
    {
        punte = stdout; 
        GSL_pintarMatriz(entrada, punte);
    }
	
	gsl_matrix_free(entrada);
    
    return 0;
}
