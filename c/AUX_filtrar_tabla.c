#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int main(int N_opcion, char** opcion)
{
    long i, j, Nfilas, Ncolumnas, Nfilas_filtrado, contador, colfil=1, abscisas=2, ordenadas=3;
    char in_name[256]={"tabla.txt"}, out_name[256]={"filtrado.txt"}, abscisas_name[256]={"x"}, ordenadas_name[256]={"y"}, basura[256], encabezado=1, plot=0;
    double **data, **data_filtrado, valfil=0.0, tolfil=0.5;
    FILE* punte, *pipe;
    
    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nPrograma para filtrar datos de una tabla. Ojo! La numeracion de las columnas es la misma que en gnuplot (empieza en '1', NO en '0'): \n--------------------------\n\n");
            printf("-in [\"tabla.txt\"], nombre del fichero de entrada\n\n");
            printf("-out [\"filtrado.txt\"], nombre del fichero de salida\n\n");
            printf("-noenc, utilizar este flag si la tabla original NO tiene encabezado\n\n");
            printf("-colfil [1], columna que se utilizara para filtrar los datos\n\n");
            printf("-valfil [0.0], valor de filtrado: la tabla de salida contendra las filas cuyo valor de 'colfil' coincida con 'valfil' +- 'tolfil' (tolerancia)\n\n");
            printf("-tolfil [0.5], tolerancia para el filtrado \n\n");
            printf("-x [2], columna que se utilizara como 'x' en la tabla filtrada\n\n");
            printf("-y [3], columna que se utilizara como 'y' en la tabla filtrada\n\n");
            printf("-plot, utilizar esta opcion para pintar (gnuplot) la tabla de salida\n\n");
            
            exit(0);
        }
        
        if(strcmp(opcion[i], "-in")==0)
            sscanf(opcion[i+1], "%s", in_name);
        
        if(strcmp(opcion[i], "-out")==0)
            sscanf(opcion[i+1], "%s", out_name);
        
        if(strcmp(opcion[i], "-noenc")==0)
            encabezado=0;
        
        if(strcmp(opcion[i], "-colfil")==0)
            sscanf(opcion[i+1], "%ld", &colfil);
        
        if(strcmp(opcion[i], "-valfil")==0)
            sscanf(opcion[i+1], "%lf", &valfil);
        
        if(strcmp(opcion[i], "-tolfil")==0)
            sscanf(opcion[i+1], "%lf", &tolfil);
        
        if(strcmp(opcion[i], "-x")==0)
            sscanf(opcion[i+1], "%ld", &abscisas);
        
        if(strcmp(opcion[i], "-y")==0)
            sscanf(opcion[i+1], "%ld", &ordenadas);
        
        if(strcmp(opcion[i], "-plot")==0)
            plot=1;
    }
    
    
    punte=fopen(in_name, "r");
    Ncolumnas=1;
    for(i=0; i>=0; i++)
    {
        fscanf(punte, "%s", basura);
        
        if(fgetc(punte)!='\n')
            Ncolumnas++;
        else
            break;
        
        if(encabezado)
        {
            if(i==abscisas-1)
                sscanf(basura, "%s", abscisas_name);
            if(i==ordenadas-1)
                sscanf(basura, "%s", ordenadas_name);
        }
    }
    fclose(punte);

    punte=fopen(in_name, "r");
    if(encabezado) Nfilas=0;
    else Nfilas=1;
    for(i=0; i<1; i+=0)
    {
        while(fgetc(punte)!='\n')
        {}
            
        if(fgetc(punte)<33)
            break;
        else
            Nfilas++;
    }
    fclose(punte);
        
    data = (double**)malloc(sizeof(double*)*Nfilas);
    for(i=0; i<Nfilas; i++)
        data[i]=(double*)malloc(sizeof(double)*Ncolumnas);
    
    punte=fopen(in_name, "r");
    if(encabezado)
    {
        while(fgetc(punte)!='\n')
        {}
    }
    for(i=0; i<Nfilas; i++)
    {
        for(j=0; j<Ncolumnas; j++)
            fscanf(punte, "%lf", &data[i][j]);
    }
    fclose(punte); //Aqui llenamos el array 'data' con todos los datos del fichero
    
    Nfilas_filtrado=0;
    for(i=0; i<Nfilas; i++)
    {
        if(fabs(data[i][colfil-1]-valfil)<tolfil)
            Nfilas_filtrado++;
    }
    data_filtrado = (double**)malloc(sizeof(double*)*Nfilas_filtrado);
    for(i=0; i<Nfilas_filtrado; i++)
        data_filtrado[i]=(double*)malloc(sizeof(double)*2);
    
    contador=0;
    for(i=0; i<Nfilas; i++)
    {
        if(fabs(data[i][colfil-1]-valfil)<tolfil)
        {
            data_filtrado[contador][0]=data[i][abscisas-1];
            data_filtrado[contador][1]=data[i][ordenadas-1];
            contador++;
        }
    }//Aqui ya estan guardados los datos filtrados
    
    
    punte=fopen(out_name, "w");
        
    fprintf(punte, "%s\t%s\n", abscisas_name, ordenadas_name);
    for(i=0; i<Nfilas_filtrado; i++)
        fprintf(punte, "%lf\t%lf\n", data_filtrado[i][0], data_filtrado[i][1]);
        
    fclose(punte);
    
    
    if(plot)
    {
        punte=fopen(".temp", "w");
        fprintf(punte, "plot '%s' u 1:2 w lp", out_name);
        fclose(punte);
        
        pipe=popen("gnuplot .temp -p\n", "w");
        pclose(pipe);
    
        remove(".temp");
    }
    
    for(i=0; i<Nfilas; i++)
        free(data[i]);
    free(data);
    
    for(i=0; i<Nfilas_filtrado; i++)
        free(data_filtrado[i]);
    free(data_filtrado);

    
    return 0;
}
