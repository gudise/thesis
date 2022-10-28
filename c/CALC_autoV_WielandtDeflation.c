
#include "include/main.h"
#include "include/matematicas.h"

int main(int N_opcion, char** opcion)
{
    int i,j,plot=0,N_lineas;
    
    char input[256]={"input.mtz"},output[256]={"output.txt"},string[256],letra;
    
    double tolerancia = 1e-7;
    
    MATRIZ *entrada,**autovector;
    
    FILE *punte;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nCalculo de autovectores y autovalores de una matriz utilizando el metodo de deflacion de Wielandt\n----------------------------------\nOpciones:\n\n");
            printf("-input [input.mtz]  fichero con la matriz de entrada. Formato: Matriz cuadrada, indices crecientes de izda. a dcha. y de arriba abajo\n\n");
            printf("-output [output.txt]  nombre del fichero de salida\n\n");
            printf("-tolerancia [1e-7], error permitido en el autovalor para truncar la aproximacion\n\n");
            printf("-plot\tsi esta opcion esta presente el programa solo calculara los autovalores, y los escribira en formato de texto listo para ser ploteado\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-input")==0 )
            sprintf(input, "%s", opcion[i+1]);
        
        if( strcmp(opcion[i], "-output")==0 )
            sprintf(output, "%s", opcion[i+1]);
        
        if( strcmp(opcion[i], "-tolerancia")==0 )
            sscanf(opcion[i+1], "%lf", &tolerancia);
        
        if( strcmp(opcion[i], "-plot")==0 )
            plot = 1;
    }
    
    entrada = leerMatriz(input);
    
    autovector = autovectorWielandDeflation(entrada, tolerancia); 
    
    sprintf(string, "%s", output);
    punte = fopen(string, "w");
    
    fprintf(punte, "[matriz]\n\n");
    for(i=0; i<N_lineas; i++)
    {
        for(j=0; j<N_lineas; j++)
            fprintf(punte, "%.2lf\t", entrada->elemento[i][j]);
        
        fprintf(punte, "\n");
    }
    
    fprintf(punte, "\n[autovalores]\n\n");
    for(i=0; i<N_lineas; i++)
        fprintf(punte, "%d)\t%lf\n", i, moduloVector(autovector[i]));
    
    fprintf(punte, "\n[autovectores]\n\n");
    for(i=0; i<N_lineas; i++)
    {
        fprintf(punte, "%d) ", i);
        for(j=0; j<N_lineas; j++)
            fprintf(punte, "\t%lf", autovector[i]->elemento[j][0]);
        fprintf(punte, "\n");
    }
        
    fclose(punte);
    
    return 0;
}
