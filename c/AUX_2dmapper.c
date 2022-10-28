#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


double promedio(double* data, int N);


int main(int N_opcion, char** opcion)
{
    int     i, j, aux, config, N_filas=0, Nx=0, Ny=0;
    char    input[256]={"input.txt"}, output[256]={"output.txt"}, string[512];
    double* data, X, Y, XOS_i=1, YOS_i=1, XOS_incr=1, YOS_incr=1, XOS_max=48, YOS_max=98;
    FILE    *punte;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa toma una tabla como entrada, y la convierte en un mapa bidimensional. El programa lee unicamente la primera columna como 'x' y la segunda como 'y'. Si hay mas columnas en la tabla son ignoradas\n--------------------------------------------------------------\nopciones:\n\n");
            printf("-in [input.txt], \n\n");
            printf("-out [output.txt]\n\n");
            printf("-XOSi [1], posicion inicial de la coordenada 'X'\n\n");
            printf("-YOSi [1], posicion inicial de la coordenada 'Y'\n\n");
            printf("-XOSincr [1], incremento de la coordenada 'X' en cada iteracion, i.e. distancia entre dos columnas consecutivas\n\n");
            printf("-YOSincr [1], incremento de la coordenada 'Y' en cada iteracion, i.e. distancia entre dos filas consecutivas\n\n");
            printf("-XOSmax [48], valor maximo de la coordenada 'X'\n\n");
            printf("-YOSmax [98], valor maximo de la coordenada 'Y'\n\n");
            printf("-config [0], sentido de crecimiento de la matriz: '0' llena primero las filas y pasa a una nueva columna cuando la fila esta completa; '1' llena primero las columnas y pasa a una nueva fila cuando la columna esta completa\n\n");

            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
            sprintf(input, "%s", opcion[i+1]);

        if(strcmp(opcion[i], "-out")==0)
            sprintf(output, "%s", opcion[i+1]);
        
        if(strcmp(opcion[i], "-XOSi")==0)
            sscanf(opcion[i+1], "%lf", &XOS_i);
            
        if(strcmp(opcion[i], "-YOSi")==0)
            sscanf(opcion[i+1], "%lf", &YOS_i);
            
        if(strcmp(opcion[i], "-XOSincr")==0)
            sscanf(opcion[i+1], "%lf", &XOS_incr);
            
        if(strcmp(opcion[i], "-YOSincr")==0)
            sscanf(opcion[i+1], "%lf", &YOS_incr);
            
        if(strcmp(opcion[i], "-XOSmax")==0)
            sscanf(opcion[i+1], "%lf", &XOS_max);
            
        if(strcmp(opcion[i], "-YOSmax")==0)
            sscanf(opcion[i+1], "%lf", &YOS_max);
        
        if(strcmp(opcion[i], "-config")==0)
            sscanf(opcion[i+1], "%d", &config);
    }
    
    sprintf(string, "%s", input);
    punte = fopen(string, "r");
    
    while(fgetc(punte)!='\n'){} //salto de linea
    while(1)
    {
        while(fgetc(punte)!='\n'){} //salto de linea
        
        N_filas++;
        
        if(fgetc(punte)==EOF) break;
    }
    
    fclose(punte);
    
    data = (double*)malloc(sizeof(double)*N_filas);
    
    sprintf(string, "%s", input);
    punte = fopen(string, "r");
    
    while(fgetc(punte)!='\n'){} //salto de la primera linea
    
    for(i=0; i<N_filas; i++)
    {
        fscanf(punte, "%d %lf", &aux, &(data[i]));
        while(fgetc(punte)!='\n'){}//con esto evitamos leer el resto de columnas
    }
    
    fclose(punte);
    
    sprintf(string, "%s", output);
    punte = fopen(string, "w");
    
    X=XOS_i;
    Y=YOS_i;
    for(i=0; i<N_filas; i++)
    {
        fprintf(punte, "%lf\t%lf\t%lf\n", X, Y, data[i]);
        
        if(!config)
        {
            X+=XOS_incr;

            if(X>XOS_max)
            {
                X=XOS_i;
                Y+=YOS_incr;
                Ny++;
            }
        }
        else
        {
            Y+=YOS_incr;

            if(Y>YOS_max)
            {
                Y=YOS_i;
                X+=XOS_incr;
                Nx++;
            }
        }
    }
    if(!config) Nx = N_filas/Ny;
    else Ny = N_filas/Nx;
    
    fclose(punte);
    
    sprintf(string, "gplot_%s", output);
    punte = fopen(string, "w");
    
    fprintf(punte, "set dgrid3d %d,%d,1\n", Nx, Ny);
    fprintf(punte, "set pm3d map\n");
    fprintf(punte, "splot '%s'", output);
    fclose(punte);
    

    return 0;
}

