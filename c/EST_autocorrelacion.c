#include <main.h>
#include <digital.h>
#include <matematicas.h>


int main(int N_opcion, char** opcion)
{
    long i, j, k;
    char input[1024]={"input.mtz"}, output[1024]={"output.mtz"};
    double *vaux, *promedio, *varianza;
    FILE *punte;
    MATRIZ* data, *autocorrelacion;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa calcula la autocorrelacion (sin normalizar) de una serie temporal de un vector aleatorio. El formato de entrada sera una matriz '.mtz', en la que cada columna de la matriz contiene las entradas del vector, y cada fila contiene el vector en un instante diferente. El programa devuelve la matriz de autocorrelacion en formato '.mtz', que contiene en el elemento i,j el promedio del producto de las componentes i,j. \n----------------------------------------------------------------------\n\n");
            printf("-in [input.mtz], \n\n");
            printf("-out [output.mtz]\n\n");

            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
            sprintf(input, "%s", opcion[i+1]);

        if(strcmp(opcion[i], "-out")==0)
            sprintf(output, "%s", opcion[i+1]);
    }
    
    data = leerMatriz(input);
    vaux = malloc(sizeof(double)*data->N_filas);

    promedio=malloc(sizeof(double)*data->N_columnas);
    varianza=malloc(sizeof(double)*data->N_columnas);
    for(j=0; j<data->N_columnas; j++)
    {
        for(i=0; i<data->N_filas; i++)
            vaux[i]=data->elemento[i][j];
        
        promedio[j] = calcular_promedio_varianza(vaux, data->N_filas, varianza+j);
    }
	
    autocorrelacion=mallocMatriz(data->N_columnas, data->N_columnas);
    for(j=0; j<data->N_columnas; j++)
    {
        for(k=0;k<data->N_columnas; k++)
        {
            for(i=0; i<data->N_filas; i++)
                vaux[i] = data->elemento[i][j]*data->elemento[i][k];
            
            autocorrelacion->elemento[j][k] = calcular_promedio(vaux, data->N_filas);
            autocorrelacion->elemento[j][k] = (autocorrelacion->elemento[j][k]-promedio[j]*promedio[k])/sqrt(varianza[j]*varianza[k]);
        }
    }
    
    punte=fopen(output, "w");
    pintarMatriz(autocorrelacion, punte);
    fclose(punte);
    
    free(vaux);
    free(promedio);
    free(varianza);
    freeMatriz(autocorrelacion);

    return 0;
}
