
#include "include/main.h"
#include "include/matematicas.h"
#include "include/digital.h"

int main(int N_opcion, char** opcion)
{
    int i, j, N_data=0, resolucion=6, metodo=0;
	long N_filas=0, N_columnas=0, mpfr_prec=53;
    char input[256]={"input.mtz"}, minentrop=0, guesswork=0, verbose=1, gsl_sort=0;
    MPFR_MATRIZ* data;
    MPFR_DISTRI* histograma;
    double entropia;
    FILE* punte;
	
	for(i=0; i<N_opcion; i++)
	{
		if( strcmp(opcion[i], "*")==0 )
			sscanf(opcion[i+1], "%ld", &mpfr_prec);
	}
    mpfra_set_default_prec(mpfr_prec);
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nPrograma para calcular la entropia de una distribucion de valores. El formato de esta distribucion debe tener titulo (i.e. la primera linea del fichero se ignora)\n----------------------------------\n\n");
			printf("* [mpfr_precision], flag especial para indicar el numero d ebits de precioson utilizado por la biblioteca 'mpfr'. Por defecto se utilizan 53 bits\n\n");
            printf("-in [input.mtz],  nombre del fichero de entrada. El programa calcula la entropia de una distribucion de valores dada en una matriz (que se espera tenga solo una columna).\n\n");
            printf("-resolucion [6], esta cantidad es el numero de cifras decimales de la distribucion de entrada. Para valores 'float' este valor es '6'. Si la distribucion proviene de una conversion binaria->decimal, la resolucion sera '0'. Esta opcion solo es relevante para el metodo 'sspace', y se ignora en el metodo 'histograma'\n\n");
            printf("-minentropia, si esta opcion esta presente el programa devolvera la minentropia de la distribucion.\n\n");
            printf("-guesswork, [SIN TERMINAR] si esta opcion esta presente el programa devolvera el 'guesswork' de la distribucion\n\n");
			printf("-metodo [0], esta opcion permite seleccionar el metodo para calcular la entropia:\n\t0: correccion Grassberger, sensible a '-caja', ignora '-resolucion'. Este es el metodo recomendable para Nbits<13 \n\t1: spacing estimate, sensible a '-resolucion', ignora '-caja'. Este es el metodo recomendable para Nbits>20\n\n");
			printf("-nv, si esta opcion esta presente, el programa solo escribira las cantidades elegidas. Esta opcion esta pensada para utilizar el programa como parte de un script\n\n");
			printf("-gsl_sort, si esta opcion esta presente se utilizaran las funciones de la biblioteca GSL para ordenar arreglos. En caso contrario se utilizara el algoritmo 'quickSort'. El metodo GSL es mucho mas rapido pero consume mas memoria\n\n");

            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
            sscanf(opcion[i+1], "%s", input);
        
        if( strcmp(opcion[i], "-resolucion")==0 )
            sscanf(opcion[i+1], "%d", &resolucion);
        
        if( strcmp(opcion[i], "-minentropia")==0 )
            minentrop=1;
			
        if( strcmp(opcion[i], "-guesswork")==0 )
            guesswork=1;
        
        if( strcmp(opcion[i], "-metodo")==0 )
            sscanf(opcion[i+1], "%d", &metodo);
		
		if(strcmp(opcion[i], "-nv")==0)
			verbose=0;
			
		if( strcmp(opcion[i], "gsl_sort") ==0)
			gsl_sort=1;
    }
    
    data = MPFR_leerMatriz(input);
    
    if(gsl_sort==0)
		MPFR_ordenarData(data);
	else
		MPFR_GSL_ordenarData(data);
    
    if(guesswork==0)
	{    
		switch(metodo)
		{
			case 0:
				if(minentrop==0)
					entropia = MPFR_calculaEntropiaGrassberger(data, 0);
				else
					entropia = MPFR_calculaMinEntropiaGrassberger(data, 0);
				break;
            
			case 1:
				if(minentrop==0)
					entropia = MPFR_calculaEntropiaSpacingEstimate(data, resolucion, 0);
				else
					entropia = MPFR_calculaMinEntropiaSpacingEstimate(data, resolucion, 0);
				break;
		}
		
		if(verbose) printf("Entropia = %lf\n", entropia);
		else printf("%lf", entropia);
	}
	//else
	//{
		//histograma = MPFR_construyeHistograma(data, caja, 0, 0, 0, 0, verbose);
		//MPFR_calculaGuesswork(histograma, 1);
	//}
	
    
    MPFR_freeMatriz(data);
    
    return 0;
}
