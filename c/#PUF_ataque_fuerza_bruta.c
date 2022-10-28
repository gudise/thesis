
#include "main.h"
#include "matematicas.h"
#include "digital.h"


int main(int N_opcion, char** opcion)
{
	long i, j, N_numeros, mpfr_prec=53;
	PUFEXP* experimento;
	char input_name[256]={"input.pex"}, tune=0, tune_name[256]={"histograma.txt"}, verbose=1, gsl_sort=0;
    double *tiempo_ataque, caja;
	MPFR_MATRIZ *data;
    MPFR_DISTRI* histograma;
	mpfr_t* candidato, aux;
	
	for(i=0; i<N_opcion; i++)
	{
		if( strcmp(opcion[i], "*")==0 )
			sscanf(opcion[i+1], "%ld", &mpfr_prec);
	}
	mpfra_set_default_prec(mpfr_prec);
	mpfra_init_set_ui(aux, 0);

	//lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nPrograma para simular un ataque de fuerza bruta sobre una PUF. El programa lee un fichero en formato '.pex'\n----------------------------------\n\n");
			printf("* [mpfr_precision], flag especial para indicar el numero d ebits de precioson utilizado por la biblioteca 'mpfr'. Por defecto se utilizan 53 bits\n\n");
            printf("-in [input.pex], \n\n");
            printf("-tune [orden] € {orden, random, histograma.txt}, esta opcion permite introducir una distribucion de probabilidad, de forma que el programa prueba con candidatos en orden descedente de probabilidad. Es recomendable que el histograma haya sido construido siendo consecuente con los limites xmin y xmax, asi como el tamano de caja\n\n");
            printf("-nv, si esta opcion esta presente el programa solo pintara el resultado (no-verbose)\n\n");
			printf("-gsl_sort, si esta opcion esta presente se utilizaran las funciones de la biblioteca GSL para ordenar arreglos. En caso contrario se utilizara el algoritmo 'quickSort'. El metodo GSL es mucho mas rapido pero consume mas memoria\n\n");
            
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
            sscanf(opcion[i+1], "%s", input_name);
        
        if( strcmp(opcion[i], "-tune")==0 ) //tune=0: orden; tune=1: random; tune=2: distribucion
        {
            if( strcmp(opcion[i+1], "orden")==0 )
                tune=0;
            else if( strcmp(opcion[i+1], "random")==0 )
                tune=1;
            else
            {
                sscanf(opcion[i+1], "%s", tune_name);
                tune=2;
            }
        }
        
        if( strcmp(opcion[i], "-nv")==0 )
            verbose=0;
			
		if( strcmp(opcion[i], "gsl_sort") ==0)
			gsl_sort=1;
    }
	
    
	experimento = leerPufexp(input_name);
	data = MPFR_mallocMatriz(experimento->N_retos*experimento->N_instancias*experimento->N_repeticiones, experimento->N_items);
	for(i=0; i<data->N_filas;)
	
    tiempo_ataque = malloc(sizeof(double)*data->N_filas);
    for(i=0; i<data->N_filas; i++)
        tiempo_ataque[i]=0; 
    
    if(tune==2)
    {
        histograma = MPFR_leerDistribucion(tune_name, 1);
		
		caja= mpfra_get_d(histograma->ejex[1]) - mpfra_get_d(histograma->ejex[0]);
				
		N_numeros = (long)(mpfra_get_d(histograma->ejex[histograma->N-1]) - mpfra_get_d(histograma->ejex[0])+caja);
        
        if(gsl_sort==0)
			MPFR_ordenarData_distribucion(histograma);
		else
			MPFR_GSL_ordenarData_distribucion(histograma);
		
		candidato = malloc(sizeof(mpfr_t)*N_numeros);
		for(i=0; i<N_numeros; i++)
			mpfr_init(candidato[i]);
		
		for(i=0; i<histograma->N; i++)
		{
			mpfra_set(aux, histograma->ejex[histograma->N-1-i]); 
			for(j=0; j<(long)caja; j++)
			{printf("\nCONTROL0: %ld / %ld\n", i*((long)caja)+j, N_numeros);
				mpfra_set(candidato[i*((long)caja)+j], aux); printf("\nCONTROL1\n");
				mpfra_add_ui(aux, aux, 1);printf("\nCONTROL2\n");
			}
		}
		MPFR_freeDistribucion(histograma);
    }
    else 
	{
		N_numeros = (long)(pow(2, N_bits));
		candidato = malloc(sizeof(mpfr_t)*N_numeros);
		for(i=0; i<N_numeros; i++)
			mpfra_init_set_ui(candidato[i], i);
    }
    
	for(i=0; i<N_claves; i++)
	{
        if(tune==1)
            gsl_ran_shuffle(idum, candidato, (size_t)N_numeros, sizeof(mpfr_t));
        
		for(j=0; j<N_numeros; j++)
        {
            if(mpfr_cmp(candidato[j], data->elemento[i][0])==0)
                break;
            tiempo_ataque[i]+=1.0; 
        }
	}
	for(i=1; i<N_claves; i++)
		tiempo_ataque[0]+=tiempo_ataque[i];
	tiempo_ataque[0]/=N_claves;

	if(verbose)
        printf("Tiempo promedio de ataque: %lf\n", tiempo_ataque[0]);
    else
        printf("%lf", tiempo_ataque[0]);

	free(tiempo_ataque);

    return 0;
}
