
#include "include/main.h"
#include "include/matematicas.h"
#include "include/digital.h"

int main(int N_opcion, char** opcion)
{
	long i, j, mpfr_prec=53, N_filas=0, N_columnas=0;
    int type = 0, entrop=0, minentrop=0, N_data=0, resolucion=6, col=1;
    char in_name[256]={"input.mtz"}, out_name[256]={"output.txt"}, normalizar=0, verbose=1, gsl_sort=0;
    MPFR_DISTRI* histograma;
    MPFR_MATRIZ* data;
    mpfr_t caja, fcaja, xmin, xmax;
    FILE* punte;
	
	for(i=0; i<N_opcion; i++)
	{
		if( strcmp(opcion[i], "*")==0 )
			sscanf(opcion[i+1], "%ld", &mpfr_prec);
	}
    mpfra_set_default_prec(mpfr_prec);
    mpfra_init_set_ui(caja, 0);
	mpfra_init_set_ui(fcaja, 0);
	mpfra_init_set_ui(xmin, 0);
	mpfra_init_set_ui(xmax, 0);

    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nPrograma para calcular el histograma de una distribucion de valores. El formato de entrada es una matriz de una sola columna\n----------------------------------\n\n");
			printf("* [mpfr_precision], flag especial para indicar el numero d ebits de precioson utilizado por la biblioteca 'mpfr'. Por defecto se utilizan 53 bits\n\n");
            printf("-in [input.mtz],  nombre del fichero de entrada\n\n");
			printf("-col [1], esta opcion permite cambiar la coumna del fichero de entrada que se utilizara para construir el histograma (por defecto se utiliza la primera columna)\n\n");
            printf("-out [output.txt],  nombre del fichero de salida. El nombre especial 'no' evitara que se pinte el histograma\n\n");
            printf("-caja [x | f x], con esta opcion se introduce el tamano de la caja. Si la opcion no esta presente, el tamano se calcula automaticamente utilizando la regla de Friedmann-Diaconis. Si se escribe el caracter 'f' delante de un numero (real), entonces dicho numero real se utilizara como factor multiplicativo sobre el tamano decaja calculado automaticamente mediante la regla de Friedmann-Diaconis. Notar que en el metodo 'sspace' la caja se ignora\n\n");
			printf("-xmin [-1], esta opcion permite fijar el extremo inferior del histograma. Si se introduce un valor negativo (por defecto) el extremo se calcula automaticamente a partir de los datos\n\n");
			printf("-xmax [-1], esta opcion permite fijar el extremo superior del histograma. Si se introduce un valor negativo (por defecto) el extremo se calcula automaticamente a partir de los datos\n\n");
            printf("-resolucion [6], esta cantidad es el numero de cifras decimales de la distribucion de entrada. Para valores 'float' este valor es '6'. Si la distribucion proviene de una conversion binaria->decimal, la resolucion sera '0'. Esta opcion solo es relevante para el metodo 'sspace', y se ignora en el metodo 'histograma'\n\n");
			printf("-type [histograma], {histograma, sspace},  tecnica utilizada para aproximar la distribucion: histograma, sampling space. El tipo 'sspace' ignora el tamano de caja, y solo debe utilizarse sobre espacios compactos\n\n");
            printf("-norm {1|2}, si esta opcion esta presente el histograma se normalizara por el area. Opcionalmente puede indicarse el tipo de normalizacion: '1' normaliza el histograma por el area, '2' normaliza el histograma por el maximo valor\n\n");
			printf("-entropia, si esta opcion esta presente el programa devolvera la entropia de la distribucion.\n\n");
            printf("-minentropia, si esta opcion esta presente el programa devolvera la minentropia de la distribucion.\n\n");
			printf("-nv, si esta opcion esta presente, el programa solo escribira las cantidades elegidas. Esta opcion esta pensada para utilizar el programa como parte de un script\n\n");
			printf("-gsl_sort, si esta opcion esta presente se utilizaran las funciones de la biblioteca GSL para ordenar arreglos. En caso contrario se utilizara el algoritmo 'quickSort'. El metodo GSL es mucho mas rapido pero consume mas memoria\n\n");           

            exit(0);
        }

        if( strcmp(opcion[i], "-in")==0 )
            sscanf(opcion[i+1], "%s", in_name);

        if( strcmp(opcion[i], "-out")==0 )
            sscanf(opcion[i+1], "%s", out_name);

        if( strcmp(opcion[i], "-caja")==0 )
		{
			if(strcmp(opcion[i+1], "f")==0)
				mpfra_set_str(fcaja, opcion[i+2], 10);
			else
				mpfra_set_str(caja, opcion[i+1], 10);
		}
			
        if( strcmp(opcion[i], "-col")==0 )
            sscanf(opcion[i+1], "%d", &col);
			
        if( strcmp(opcion[i], "-xmin")==0 )
			mpfra_set_str(xmin, opcion[i+1], 10);
			
        if( strcmp(opcion[i], "-xmax")==0 )
			mpfra_set_str(xmax, opcion[i+1], 10);
			
        if( strcmp(opcion[i], "-resolucion")==0 )
            sscanf(opcion[i+1], "%d", &resolucion);

        if( strcmp(opcion[i], "-type")==0 )
		{
			if(strcmp(opcion[i+1], "sspace")==0) type=1;		
		}
        
        if( strcmp(opcion[i], "-norm")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
                {
                    if(strcmp(opcion[i+1], "1")==0)
                        normalizar=1;
                    else if(strcmp(opcion[i+1], "2")==0)
                        normalizar=2;
                }
                else
                    normalizar=1;
            }
            else 
                normalizar=1;
        }
        
        if( strcmp(opcion[i], "-entropia")==0 )
            entrop=1;
        
        if( strcmp(opcion[i], "-minentropia")==0 )
            minentrop=1;
		
		if(strcmp(opcion[i], "-nv")==0)
			verbose=0;
			
		if( strcmp(opcion[i], "gsl_sort") ==0)
			gsl_sort=1;
    }
    
    data = MPFR_leerMatriz(in_name);

    if(gsl_sort==0)
		MPFR_ordenarData(data);
	else
		MPFR_GSL_ordenarData(data);
    
    if(mpfr_zero_p(caja) && type==0)
	{
        MPFR_FreedmanDiaconis(&caja, data, 0);
		if(!mpfr_zero_p(fcaja))
			mpfra_mul(caja, caja, fcaja);
	}

	if(type==0) 
        histograma = MPFR_construyeHistograma(data, caja, xmin, xmax, normalizar, 0, verbose);
	else 
        histograma = MPFR_construyeHistograma_Sspace(data, resolucion, normalizar, 0);

    if(entrop)
	{    
        if(verbose) printf("Entropia = %lg\n", MPFR_calculaEntropiaPlugin(histograma));
		else printf("%lg",MPFR_calculaEntropiaPlugin(histograma));
	}
	
    if(minentrop)
	{
		if(verbose) printf("Min-entropia = %lf\n", MPFR_calculaMinEntropiaPlugin(histograma));
		else printf("%lf", MPFR_calculaMinEntropiaPlugin(histograma));
	}
    
    if(strcmp(out_name, "no")!=0) 
		MPFR_pintarDistribucion(histograma, out_name);

    MPFR_freeMatriz(data);
    MPFR_freeDistribucion(histograma);

    return 0;
}
