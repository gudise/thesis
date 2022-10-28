#include "include/main.h"
#include "include/matematicas.h"

int main(int N_opcion, char** opcion)
{
    int i,j;
    char input[256]={"input.mtz"},output[256]={"output.mtz"}, modo=0, filtrar=0;
    double sigma=5.0, *vector_auxiliar, promedio, varianza;
	MATRIZ* entrada, *salida;
    FILE *punte;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa calcula el promedio de las filas o las columnas de una matriz (.mtz) de entrada. Dada una entrada de dimension MxN, la salida se dara en forma de una matriz que sera:\n\t2xN, donde cada columna de la primera fila contendra el promedio de las M-filas y cada columna de la segunda fila contendra las desviaciones tipicas de la media (i.e., desviacion tipica/sqrt(M)) (en caso de realizar promedio por filas)\n\t Mx2, donde cada fila de la primera columna contendra el promedio de las N-columnas y cada fila de la segunda columna contendra las desviaciones tipicas de la media (i.e., desviacion tipica/sqrt(N)) (en caso de realizar promedio por columnas)\n----------------------------------\nOpciones:\n\n");
            printf("-in [input.mtz]  fichero con la matriz de entrada (ver 'doc/my_doc')\n\n");
            printf("-out [output.mtz]  nombre del fichero de salida\n\n");
            printf("-columnas, si esta opcion esta presente el programa realizara el promedio de las columnas en lugar de las filas (que es la opcion por defecto)\n\n");
            printf("-filtrar [5], si esta opcion esta presenta el promedio se calculara excluyendo los puntos que se alejen mas de [x] sigmas del promedio. por defecto x=5\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
            sprintf(input, "%s", opcion[i+1]);
        
        if( strcmp(opcion[i], "-out")==0 )
            sprintf(output, "%s", opcion[i+1]);
        
        if( strcmp(opcion[i], "-columnas")==0 )
            modo=1;
        
        if( strcmp(opcion[i], "-filtrar")==0 )
        {
            filtrar=1;
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
                {
                    sscanf(opcion[i+1], "%lf", &sigma);
                }
            }
        }
    }
    
	
	entrada = leerMatriz(input);
    
    if(modo==0)
    {
        salida = mallocMatriz(2, entrada->N_columnas);
        vaciarMatriz(salida);
        
        vector_auxiliar = malloc(sizeof(double)*entrada->N_filas);
        for(j=0; j<entrada->N_columnas; j++)
        {
            for(i=0; i<entrada->N_filas; i++)
                vector_auxiliar[i] = entrada->elemento[i][j];
            
            if(filtrar)
                promedio = calcular_promedio_varianza_filtrando(vector_auxiliar, entrada->N_filas, sigma, &varianza);
            else
                promedio = calcular_promedio_varianza(vector_auxiliar, entrada->N_filas, &varianza);
            
            salida->elemento[0][j]=promedio;
            salida->elemento[1][j]=sqrt(varianza);
        }
    }
    
    else
    {
        salida = mallocMatriz(entrada->N_filas, 2);
        vaciarMatriz(salida);
        
        vector_auxiliar = malloc(sizeof(double)*entrada->N_columnas);
        for(i=0; i<entrada->N_filas; i++)
        {
            for(j=0; j<entrada->N_columnas; j++)
                vector_auxiliar[j] = entrada->elemento[i][j];
            
            if(filtrar)
                promedio = calcular_promedio_varianza_filtrando(vector_auxiliar, entrada->N_columnas, sigma, &varianza);
            else
                promedio = calcular_promedio_varianza(vector_auxiliar, entrada->N_columnas, &varianza);
            
            salida->elemento[i][0]=promedio;
            salida->elemento[i][1]=sqrt(varianza);
        }
    }

	punte = fopen(output, "w"); 
	pintarMatriz(salida, punte);
	fclose(punte);
	
	freeMatriz(entrada);
	freeMatriz(salida);
    free(vector_auxiliar);
	
    return 0;
}
