#include "include/main.h"
#include "include/digital.h"
#include "include/matematicas.h"

int main(int N_opcion, char** opcion)
{
    int i, j, k, N_instancias=0;
    char **files;
    FILE* punte, *pipe;
    MATRIZ** putty;
    
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa debe correrse en un directorio que contenga varios archivos .log, cada uno de los cuales ha de contener una serie de N_claves repeticiones. Este programa combina todos los ficheros en un único 'putty.log' formateado para un experimento de N_instancias instancias y N_claves repeticiones\n----------------------------------\n\n");
            
            exit(0);
        }
    }
    
    pipe = popen("ls *.log > temp\n", "w");
    pclose(pipe);
    
    punte = fopen("temp", "r");
    while(1)
    {
        if(fgetc(punte)=='\n') N_instancias++;
        if(feof(punte)) break;
    }
    fclose(punte);
    
    files = (char**)malloc(sizeof(char*)*N_instancias);
    for(i=0; i<N_instancias; i++)
        files[i] = (char*)malloc(sizeof(char)*256);
    punte = fopen("temp", "r");
    for(i=0; i<N_instancias; i++)
        fscanf(punte, "%s", files[i]);
    fclose(punte);
    
    putty = (MATRIZ**)malloc(sizeof(MATRIZ*)*N_instancias);
    for(i=0; i<N_instancias; i++)
        putty[i] = hacerDePuttyUnaMatriz(files[i]); 

    punte = fopen("putty.log", "w");
    fprintf(punte, "--putty generado por \"combinarInstancias\"--\n\nN_claves = %ld\n",N_instancias*putty[0]->N_filas); 
    for(i=0; i<N_instancias; i++)
    {
        for(j=0; j<putty[i]->N_filas; j++)
        {
            for(k=0; k<putty[i]->N_columnas; k++)
                fprintf(punte, "%d ", (int)(putty[i]->elemento[j][k]));
            fprintf(punte, "\n");
        }
    }
    fprintf(punte, "-fin-");
    fclose(punte);
    
    pipe = popen("rm temp\n", "w");
    pclose(pipe);
    
    return 0;
}






