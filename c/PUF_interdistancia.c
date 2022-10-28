#include "main.h"
#include "digital.h"
#include "matematicas.h"

int main(int N_opcion, char **opcion)
{
    char input[256]={"input.pex"}, output[256]={"output.txt"}, ajuste_name[300], ajuste=0, verbose=1, realdata=0, normalizacion=1;
    int i, N_iteraciones=100;
    double caja=1.0, media=0, desviacion=0, p_binomial;
    PUFEXP* experimento;
    MATRIZ* set_interdist;
    DISTRI* hist_interdist, *ajuste_binomial;
    
    
    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\ninterdistancia:\n----------------------------------\nEste programa calcula la distribucion de interdistancias en el experimento 'input.pex', y devuelve el histograma normalizado de dicha distribucion asi como su ajuste a una binomial. El nombre del ajuste incluira el valor de los parametros de la binomial 'p' y 'n' (= N_bits).--------------------------------------------------------:\n\n");
            printf("-in [input.pex], nombre del fichero de entrada (formato putty)\n\n");
            printf("-out [output.txt], nombre del fichero de salida (histograma de 0 a N_items).\n\n");
            printf("-realdata, si esta opcion esta presente el programa entiende que el archivo '.pex' proporcionado esta en formato real (y no binario), de forma que calculara la distancia euclidea de cada entrada en lugar de la distancia Hamming.\n\n");
			printf("-ajuste [N_iteraciones], si esta opcion esta presente el programa realizara el ajuste a una binomial del histograma 'output.mtz'. Opcionalmente puede anadirse un numero [N_iteraciones]para especificar el numero de iteraciones realizadas en el ajuste. El formato de salida sera de hecho '.mtz', y el nombre sera 'output.mtz.fit' (se anade '.fit' al final de la extension para dejar claro que es un ajuste del mismo archivo).\n\n");
            printf("-caja [1.0], tamano de la caja para el histograma. Por defecto se utilizan tantas cajas como items se leen de 'input.pex', i.e., caja=1.0\n\n");
            printf("-nv, si esta opcion esta presente el programa se ejecutara 'no verbose': la informacion de salida se dara como un unico numero, bien el promedio de la intradistancia, o bien el valor 'p' de ajuste binomial si se ha dado la opcion 'ajuste' (en ambos casos en tanto por ciento)\n\n");
			printf("-normalt, si esta opcion esta presente el programa calcula el histograma de distancias hamming normalizado en altura en lugar del area. Esto solo es para que se vea bonito, pero no tiene utilidad (el histograma debe estar normalizado por area)\n\n");
            
            exit(0);
        }
         if(strcmp(opcion[i], "-in")==0)
            sscanf(opcion[i+1], "%s", input);
         
         if(strcmp(opcion[i], "-out")==0)
            sscanf(opcion[i+1], "%s", output);
        
        if(strcmp(opcion[i], "-ajuste")==0)
        {
            ajuste=1;
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-') 
                    sscanf(opcion[i+1], "%d", &N_iteraciones);
            }
        }
        
        if(strcmp(opcion[i], "-caja")==0)
            sscanf(opcion[i+1], "%lf", &caja);
			
		if(strcmp(opcion[i], "-nv")==0)
			verbose=0;
			
		if(strcmp(opcion[i], "-realdata")==0)
			realdata=1;
        
		if(strcmp(opcion[i], "-normalt")==0)
			normalizacion=2;
    }

    experimento = leerPufexp(input);

    set_interdist = interdistanciaSet(experimento, realdata);

    hist_interdist = construyeHistograma(set_interdist, caja, -1, -1, normalizacion, 1, 0);
    pintarDistribucion(hist_interdist, output);
    
    media=0;
    for(i=0; i<set_interdist->N_filas; i++)
        media+=set_interdist->elemento[i][0];
    media/=set_interdist->N_filas;

    desviacion = 0;
    for(i=0; i<set_interdist->N_filas; i++)
        desviacion+=(set_interdist->elemento[i][0]-media)*(set_interdist->elemento[i][0]-media);
    desviacion/=(set_interdist->N_filas-1);

    if(verbose)
		printf("\nInter-distancia media = %lg ( %lg %% )\nDesviacion estandar = %lg ( %lg %% )\n", media, media/experimento->tamano_respuesta*100, sqrt(desviacion/set_interdist->N_filas),sqrt(desviacion/set_interdist->N_filas)/experimento->tamano_respuesta*100);
	else if(!ajuste)
		printf("%lg", media/experimento->tamano_respuesta*100);
    
	if(ajuste)
    {
        ajuste_binomial = ajustarBinomial(hist_interdist, experimento->tamano_respuesta, N_iteraciones);
        sscanf(ajuste_binomial->string, "%lf", &p_binomial);
        sprintf(ajuste_name, "%s.fit", output);
        pintarDistribucion(ajuste_binomial, ajuste_name);
        
        if(verbose)
			printf("\np_bonimial = %lg (%lg %%)\nn_binomial = %d\n\n", p_binomial, p_binomial*100, experimento->tamano_respuesta);
		else
			printf("%lg", p_binomial*100);
        
        freeDistribucion(ajuste_binomial);
    }
    
    freePufexp(experimento);
    freeMatriz(set_interdist);
    freeDistribucion(hist_interdist);
        
    return 0;
}
