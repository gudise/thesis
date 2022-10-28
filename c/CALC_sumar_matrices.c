#include "include/main.h"
#include "include/matematicas.h"

int main(int N_opcion, char** opcion)
{
    int i,j,plot=0,N_lineas;
    char input1[256]={"input1.mtz"}, input2[256]={"input2.mtz"}, output[256]={"output.mtz"}, flagin1=0, flagin2=0, flagout=0;
	gsl_matrix* entrada1, *entrada2;
    FILE *punte;

    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nMultiplicacion de matrices (input1 * input2) (implementado por la biblioteca gsl). Pueden pasarse los dos sumandos usando un pipe en bash, pero deben separarse por un simbolo '\n': por ejemplo, si la matriz 'a' esta guardada en la variable $a, y 'b' en $b, pueden sumarse haciendo: echo $a \\n $b | calc_sumarMatrices\n----------------------------------\nOpciones:\n\n");
            printf("-in1 [input1.mtz], si esta opcion esta presente el programa leera la entrada de un fichero. En caso contrario leera la entrada de stdin\n\n");
			printf("-in2 [input2.mtz]si esta opcion esta presente el programa leera la entrada de un fichero. En caso contrario leera la entrada de stdin\n\n");            
			printf("-out [output.mtz], si esta opcion esta presente el programa volcara la salida en un fichero. En caso contrario volcara la salida en stdout\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in1")==0 )
        {
            flagin1=1;
            if(i+1 < N_opcion)
            {
                if(opcion[i+1][0]!='-')
                    sprintf(input1, "%s", opcion[i+1]);
            }
        }
			
        if( strcmp(opcion[i], "-in2")==0 )
        {
            flagin2=1;
            if(i+1 < N_opcion)
            {
                if(opcion[i+1][0]!='-')
                    sprintf(input2, "%s", opcion[i+1]);
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

    if(flagin1)
        punte = fopen(input1, "r");
    else
        punte = stdin;

    entrada1 = GSL_leerMatriz(punte);
    if(flagin1)
        fclose(punte);
    
    if(flagin2)
        punte = fopen(input2, "r");
    else
        punte = stdin;
    entrada2 = GSL_leerMatriz(punte);
    if(flagin2)
        fclose(punte);

    gsl_matrix_add(entrada1, entrada2);
        
    if(flagout)
        punte = fopen(output, "w");
    else
        punte = stdout; 
    GSL_pintarMatriz(entrada1, punte);
    if(flagout)
        fclose(punte);

    gsl_matrix_free(entrada1);
    gsl_matrix_free(entrada2);

    return 0;
}
