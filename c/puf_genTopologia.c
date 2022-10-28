#include <main.h>
#include <digital.h>

int main(int N_opcion, char** opcion)
{
    char output[1024]={"output.cmt"}, special=0;
    int i, j, N_osciladores=8, N_modulos=1, N_bits=0;
    FILE *punte;
    CMTOPOL* topologia;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa genera una topologia aleatoriamente\n----------------------------------------------------------------------\n\n");
            printf(" !, flag especial para indicar que se cree un fichero '.indexec' al terminar el programa, con el indice de ejecucion del programa. Esto permite que un lanzamiento sucesivo de este programa utilice cada vez una semilla diferente para el TRNG\n\n");
            printf("-out [output.cmt], nombre del archivo de salida\n\n");
            printf("-Nosc [8], numero de osciladores de la topologia\n\n");
            printf("-Nbits [0], numero de bits que contendra la toologia. Si este valor es >0, la opcion '-Nmod' se ignora (y el numero de modulos no estara acotado). Por otro lado, si esta opcion es <= 0, se utilizara la cantidad '-Nmod' para calcular el numero de modulos de la topologia\n\n");
            printf("-Nmod [1], numero de modulos (por defecto se tomara 1)\n\n");
            
            return 0;
        }
        
        if(strcmp(opcion[i], "!")==0)
            special=1;
        
        if(strcmp(opcion[i], "-out")==0)
            sscanf(opcion[i+1], "%s", output);
        
        if(strcmp(opcion[i], "-Nosc")==0)
            sscanf(opcion[i+1], "%d", &N_osciladores);
        
        if(strcmp(opcion[i], "-Nbits")==0)
            sscanf(opcion[i+1], "%d", &N_bits);
        
        if(strcmp(opcion[i], "-Nmod")==0)
            sscanf(opcion[i+1], "%d", &N_modulos);
    }
    if(special)
    {        
        punte = fopen(".indexec", "w");
        fprintf(punte, "%d\n", indexec);
        fclose(punte);
    }
    
    topologia = generarTopologia(N_osciladores, N_bits, N_modulos);

    punte = fopen(output, "w");
    fprintf(punte, "#[N_celdas] %d\n\n#[matriz]\n", N_osciladores);
    
    for(i=N_osciladores-1; i>=0; i--)
    {
        for(j=0; j<N_osciladores; j++)
        {
            if(i==j)
                fprintf(punte, "x ");
            
            else if(topologia->matriz[i][j]<0)
                fprintf(punte, "* ");
            
            else fprintf(punte, "o ");
        }
        fprintf(punte, "\n");
    }
    fprintf(punte, "\n#[fin]\n");
    
    fclose(punte);
    
    freeCmtopol(topologia);

    
    return 0;
}
