#include "include/main.h"
#include "include/matematicas.h"

int main(int N_opcion, char** opcion)
{
    int i, j, nfilas=2, ncolumnas=2;
    char out_name[256]={"random.mtz"}, flagout=0;
	double min=0, max=1;
    MATRIZ* entrada;
    FILE *punte;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEsta funcion pinta una matriz aleatoria \n----------------------------------\nOpciones:\n\n");
            printf("-out [random.mtz],  si esta opcion esta presente el programa volcara la salida en un fichero. En caso contrario volcara la salida en stdout\n\n");
            printf("-nfil [2], numero de filas\n\n");
            printf("-ncol [2], numero de columnas\n\n");
            printf("-min [0.0]\n\n");
            printf("-max [1.0]\n\n");
            
            exit(0);
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
        
        if( strcmp(opcion[i], "-nfil")==0 )
            sscanf(opcion[i+1], "%d", &nfilas);
        
        if( strcmp(opcion[i], "-ncol")==0 )
            sscanf(opcion[i+1], "%d", &ncolumnas);
        
        if( strcmp(opcion[i], "-min")==0 )
            sscanf(opcion[i+1], "%lf", &min);
        
        if( strcmp(opcion[i], "-max")==0 )
            sscanf(opcion[i+1], "%lf", &max);
    }
    
    entrada = mallocMatriz(nfilas, ncolumnas);
    
    for(i=0; i<nfilas; i++)
    {
        for(j=0; j<ncolumnas; j++)
            entrada->elemento[i][j] = (max-min)*ran1()+min;
    }
    
    if(flagout)
    {
        punte = fopen(out_name, "w");
        pintarMatriz(entrada, punte);
        fclose(punte);
    }
    else
    {
        punte = stdout; 
        pintarMatriz(entrada, punte);
    }
    
    
    freeMatriz(entrada);
    
    return 0;
}
