#include "include/main.h"
#include "include/digital.h"
#include "include/matematicas.h"

int main(int N_opcion, char **opcion)
{
    double p_binomial, media, desviacion;
    long i, N_claves, N_bits, ncajas, niter=100;
    long N_retos=1, N_rep=1, N_inst=1;
    char in_name[256]={"putty.log"}, outname[256]={"inter-distancia"}, string[512], nobin=0;
    EXP* experimento;
    MATRIZ* interdistancia;
    DISTRI* histograma, *binomial;
    FILE*   punte;
    
    N_bits = extraerNbitsPutty("putty.log");
    ncajas = N_bits;
    
    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\ninterdistancia:\n----------------------------------\nEste programa calcula la distribucion de interdistancias medidos en 'putty.log', y devuelve el histograma normalizado de dicha distribucion asi como su ajuste a una binomial. El nombre del ajuste incluira el valor de los parametros de la binomial 'p' y 'n' (= N_bits). Este programa siempre debe ejcutarse en compañia de un archivo 'putty.log' valido, formateado de acuerdo con las especificaciones dadas en la memoria del DOCTORADO para un experimento sobre PUFs.\n\nOpciones:\n\n");
            printf("-in [putty.log], nombre del fichero de entrada (formato putty)\n\n");
            printf("-out [inter-distancia], nombre que se utilizara para nombrar el histograma de interdistancias ([outname].txt) y el ajuste binomial ([outname]_ajuste(p,n).txt)\n\n");
            printf("-ncajas [N_bits], numero de cajas para el histograma. Por defecto se utilizan tantas cajas como bits se leen del fichero 'putty.log'\n\n");
            printf("-niter [100], numero de iteraciones para el ajuste por minimos cuadrados de la binomial. El nivel de precision del ajuste sera 1/niter\n\n");
            printf("-nobin, si esta opcion esta presente NO se realizara el ajuste a la binomial. Unicamente se mostraran el promedio y varianza de la distribucion\n\n");
            printf("-Nretos [1], numero de retos de que consta la entrada 'putty'\n\n");
            printf("-Ninst [N_claves], numero de instancias de que consta la entrada 'putty'\n\n");
            printf("-Nrep [1], numero de repeticiones de que consta la entrada 'putty'\n\n");
            
            exit(0);
        }
        
        if(strcmp(opcion[i], "-in")==0)
            sprintf(in_name, "%s", opcion[i+1]);
        
        if(strcmp(opcion[i], "-out")==0)
            sprintf(outname, "%s", opcion[i+1]);
        
        if(strcmp(opcion[i], "-ncajas")==0)
            sscanf(opcion[i+1], "%ld", &ncajas);
        
        if(strcmp(opcion[i], "-niter")==0)
            sscanf(opcion[i+1], "%ld", &niter);
        
        if(strcmp(opcion[i], "-nobin")==0)
            nobin=1;
        
        if( strcmp(opcion[i], "-Nretos")==0 )
            sscanf(opcion[i+1], "%ld", &N_retos);
        
        if( strcmp(opcion[i], "-Ninst")==0 )
            sscanf(opcion[i+1], "%ld", &N_inst);
        
        if( strcmp(opcion[i], "-Nrep")==0 )
            sscanf(opcion[i+1], "%ld", &N_rep);
        
    }
    
    if(N_bits==-1)
    {
        printf("\nERROR: este programa debe ejecutarse en un directorio que aloje un fichero 'putty.log' valido\n");
        exit(0);
    }
    
    leerParametrosPutty(&N_claves, &N_bits, in_name);
    if(N_retos==1 && N_rep==1)
        N_inst = N_claves;
    
    experimento = mallocExp(N_retos, N_inst, N_rep, N_bits); 
    
    leerPutty(experimento, in_name);
    
    interdistancia = interdistanciaSet(experimento); 
    
    histograma = construyeHistograma(interdistancia, 1.0, -1, -1, 1, 0);
    
    media = promedio(histograma);
    desviacion = sqrt(varianza(histograma));
   
    if(nobin==0)
    {
        binomial = ajustarBinomial(histograma, N_bits, niter);
        sscanf(binomial->string, "%lf", &p_binomial);
    }
    
    sprintf(string, "%s.txt", outname);
    pintarDistribucion(histograma, string);
    
    if(nobin==0)
    {
        sprintf(string, "%s_ajuste(p=%.3lf,n=%ld).txt", outname, p_binomial, N_bits);
        pintarDistribucion(binomial, string);
    }
    
    freeExp(experimento);
    freeMatriz(interdistancia);
    freeDistribucion(histograma);
    if(nobin==0) free(binomial);
    
    if(nobin==0)
        printf("\nAjuste binomial:\n\tn = %ld\n\tp = %lf ( = %.1lf%%)\n\tpromedio = %lf\n\nPromedio distribucion = %lf ( = %.1lf%%)\nDesviacion distribucion = %lf\n\n", N_bits, p_binomial, p_binomial*100, p_binomial*N_bits, media, media*100.0/N_bits, desviacion);
    else
        printf("\nPromedio distribucion = %lf ( = %.1lf%%)\nDesviacion distribucion = %lf\n\n", interdistanciaPromedio(experimento), media*100.0/N_bits, desviacion);
    
    return 0;
}
