#include "main.h"
#include "matematicas.h"
#include "redneuronal.h"


int main(int N_opcion, char** opcion)
{
    long i, j, contador;
    long N_capas=2, *neuronas_por_capa, lote=1, iteraciones=1, trainset=0;
    char guardar=0, cargar=0, entren_name[256]={"training.crp"}, test_name[256]={"test.crp"}, file_name_g[256]={"red.ffn"}, file_name_c[256]={"red.ffn"}, verbose=1, outputAct='s';
    double aciertos, coste, velocidad=1.0, reg_factor=0.0, tolerancia=-1, init_media=0, init_sigma=-1;
    fActivacion_punte *fActivacion_por_capa;
	void (*gradiente_coste)(REDNEU*, gsl_matrix*, gsl_matrix*)=gradiente_costeCuadratico;
	double (*funcion_coste)(REDNEU*, gsl_matrix*, gsl_matrix*)=costeCuadratico;
    REDNEU* red;
    ENTREN  *entrenamiento, *subset, *test;
    
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nPrograma para disenar, entrenar y evaluar una red neuronal artificial de tipo 'feed forward'\n----------------------------------\n----------------------------------\n");
			
			printf("\nOpciones I/O:\n-----------------");
            printf("\n\t-training [\"training.crp\"], esta opcion indica al programa el archivo 'training.crp' para utilizar como conjunto de entrenamiento. El nombre del archivo puede ser (opcionalmente) indicado a continuacion de la opcion, incluida la extension. Sin embargo el programa leera el fichero en formato .crp, de forma que se recomienda mantener esa extension.\n");
            printf("\n\t-test [\"test.crp\"], esta opcion indica al programa el archivo 'test.crp' para utilizar como conjunto de test. El nombre del archivo puede ser (opcionalmente) indicado a continuacion de la opcion, incluida la extension. Sin embargo el programa leera el fichero en formato .crp, de forma que se recomienda mantener esa extension. Notar que este archivo puede ser el mismo que '-training'\n");
            printf("\n\t-trainset [0], esta opcion permite determinar el numero de items utilizado para entrenar, tomados aleatoriomente del conjunto de entrenamiento de MNIST. Si vale '0' se utilizaran todos los items del grupo de entrenamiento. Notar que el programa todavia es sensible a la opcion '-lote'\n");
            printf("\n\t-guardar [\"red\"], si esta opcion esta presenta el programa guardara la RN en formato '.ffn' (ver carpeta '/doc/my_doc') Debe especificarse el nombre del archivo deseado\n");
            printf("\n\t-cargar [\"red.ffn\"], si esta opcion esta presenta el programa cargara la RN guardada en el archivo 'red.ffn', en formato rna. Si esta opcion esta presente el programa ignorara las opciones relativas a la topologia de la red (-hidden)\n");
			
			printf("\nTopologia de la red:\n-----------------");
			printf("\n\t-hidden [], esta opcion especifica el numero de neuronas por cada capa oculta (por defecto no hay capas ocultas). El numero de capas ocultas en si es deducido a partir de cuantos numeros se proporcionan con esta opcion\n");
            printf("\n\t-hiddenAct {[sigm], [tanh], [relu], [swish]}, esta opcion especifica el tipo de funcion de activacion para las neuronas de cada capa oculta. El numero de capas ocultas sigue deduciendose de la opcion '-hidden'. Para especificar la funcion de activacion se escribe el nombre de la funcion correspondiente a cada capa separada por un espacio. Al contrario que la opcion '-hidden', esta opcion SI admite especificar menos funciones que capas ocultas tenga la red: en este caso a las capas no especificadas se les asignara la ultima funcion de activacion especificada. Actualmente las funciones disponibles son:\n\t\tsigm --> funcion sigmoide\n\t\ttanh --> funcion tangente hiperbolica\n\t\tswish --> funcion x*sigmoide\n\t\trelu --> funcion 'rectified linear unit'.\n\n\t\t e.g:(en una red de 4 capas ocultas)\n\n\t\t-hiddenAct sigm sigm relu sigm --> la tercera capa oculta sera una funcion relu, las demas seran sigmoides\n\t\t-hiddenAct sigm sigm relu --> las dos primeras capas seran sigmoides, y las dos ultimas seran relu\n\t\t-hiddenAct relu --> las 4 capas ocultas seran relu\n");
            printf("\n\t-outputAct {[sigm], [tanh], [relu], [swish]}, funcion de activacion para la capa de salida. Se recomiendo evitar la funcion 'relu' para la capa de salida\n");
			
			printf("\nHiper-parametros:\n-----------------");
            printf("\n\t-reg [0.0], factor de regularizacion: este valor fuerza a la matriz de pesos a mantener valores pequenos, dificultando la sobreparametrizacion\n");
            printf("\n\t-lote [1], esta opcion especifica el tamano de cada lote de entrenamiento\n");
            printf("\n\t-velocidad [1.0], velocidad de aprendizaje, i.e., el factor multiplicativo del gradiente usado para corregir los parametros de la red\n");
			
            printf("\n-check {[clasificar], [ajustar (tolerancia)]}, esta opcion permite cambiar la manera en la que se tratan las salidas de la red a efectos de chequear los resultados con el conjunto de test. Con la opcion 'clasificar' (por defecto) la red extrae como respuesta la neurona de salida cuyo valor de activacion es mayor. Con la opcion 'ajustar' se utiliza el vector de salida de la red al completo, y la respuesta de la red se considera positiva cuando todas las neuronas de salida se distancien menos que 'tolerancia' de la respuesta de test\n");
			printf("\n-fcoste {[sqr], [log]}, esta opcion permite cambiar la funcion de coste utilizada. Por defecto se utiliza la funcion de coste cuadratico (sqr). Las opciones disponibles son:\n\t\tsqr --> coste cuadratico\n\t\tlog --> coste 'cross entropy'\n");
			printf("\n-iteraciones [1], numero de iteraciones que se repite cada sesion de entrenamiento\n");
			printf("\n-init {[auto], [(media) (sigma)]}, si esta opcion esta presente se especificara el metodo de inicializacion de la red: 'auto' inicializa los sesgos con una distribucion normal (0,1), y los pesos con una distribucion (0, 1/sqrt([neuronas de entrada a la capa]). Si en lugar se especifican dos numeros reales (>0) estos se utilizaran respectivamente como los valores de 'media' y 'sigma' para ajustar la distribucion (normal) inicial de sesgos y pesos\n");
            printf("\n-nv, si esta opcion esta presente el programa escribira toda la informacion separada unicamente por saltos de linea\n\n");

            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-training")==0 )
            sscanf(opcion[i+1], "%s", entren_name);
            
        if( strcmp(opcion[i], "-test")==0 )
            sscanf(opcion[i+1], "%s", test_name);
        
        if( strcmp(opcion[i], "-trainset")==0 )
            sscanf(opcion[i+1], "%ld", &trainset);
        
        if( strcmp(opcion[i], "-hidden")==0 )
        {
            j=i+1;
            for(j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0]=='-')
                    break;
                else
                    N_capas++;
            }
            neuronas_por_capa=(long*)malloc(sizeof(long)*N_capas);
            for(j=1; j<N_capas-1; j++)
                sscanf(opcion[i+j], "%ld", &(neuronas_por_capa[j]));
            
            fActivacion_por_capa = (fActivacion_punte*)malloc(sizeof(fActivacion_punte)*N_capas);
            for(j=0; j<N_capas; j++)
                fActivacion_por_capa[j]=activacionSigmoide;
        }
        
        if( strcmp(opcion[i], "-hiddenAct")==0 )
        {
            contador=1;
            j=i+1;
            for(j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0]=='-')
                    break;
                else
                {
                    if(strcmp(opcion[j], "sigm")==0)
                        fActivacion_por_capa[contador]=activacionSigmoide;
                    if(strcmp(opcion[j], "swish")==0)
                        fActivacion_por_capa[contador]=activacionSwish;
                    else if(strcmp(opcion[j], "tanh")==0)
                        fActivacion_por_capa[contador]=activacionTanh;
                    else if(strcmp(opcion[j], "relu")==0)
                        fActivacion_por_capa[contador]=activacionRELU;
                }
                contador++;
            }
            if(contador<N_capas-1)
            {
                for(j=contador; j<N_capas-1; j++)
                    fActivacion_por_capa[j]=fActivacion_por_capa[contador-1];
            }
        }
        
        if( strcmp(opcion[i], "-outputAct")==0 )
        {
            if(strcmp(opcion[i+1], "sigm")==0)
                outputAct='s';
            else if(strcmp(opcion[i+1], "swish")==0)
                outputAct='w';
            else if(strcmp(opcion[i+1], "tanh")==0)
                outputAct='t';
            else if(strcmp(opcion[i+1], "relu")==0)
                outputAct='r';
        }
        
        if( strcmp(opcion[i], "-reg")==0 )
            sscanf(opcion[i+1], "%lf", &reg_factor);
        
        if( strcmp(opcion[i], "-lote")==0 )
            sscanf(opcion[i+1], "%ld", &lote);
        
        if( strcmp(opcion[i], "-iteraciones")==0 )
            sscanf(opcion[i+1], "%ld", &iteraciones);
        
        if( strcmp(opcion[i], "-velocidad")==0 )
            sscanf(opcion[i+1], "%lf", &velocidad);
        
        if( strcmp(opcion[i], "-check")==0 )
		{
			if(strcmp(opcion[i+1], "clasificar")==0)
				tolerancia=-1;
			else if(strcmp(opcion[i+1], "ajustar")==0) 
				sscanf(opcion[i+2], "%lf", &tolerancia);
		}
        
        if( strcmp(opcion[i], "-guardar")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-') sprintf(file_name_g, "%s.ffn" ,opcion[i+1]);
            }
            guardar=1;
        }
        
        if( strcmp(opcion[i], "-cargar")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-') sscanf(opcion[i+1], "%s", file_name_c);
            }
            cargar=1;
        }
        
        if( strcmp(opcion[i], "-nv")==0 )
            verbose=0;
		
		if( strcmp(opcion[i], "-fcoste")==0 )
		{
			if(strcmp(opcion[i+1], "log")==0)
			{
				gradiente_coste=gradiente_costeCrossEntropy;
				funcion_coste=costeCrossEntropy;
			}
		}
		
        if( strcmp(opcion[i], "-init")==0 )
        {
			if(strcmp(opcion[i+1], "auto")==0)
				init_sigma=-1.0;
			else
			{
				sscanf(opcion[i+1], "%lf", &init_media);
				sscanf(opcion[i+2], "%lf", &init_sigma);
			}

        }
    }
    if(N_capas==2)
    {
        neuronas_por_capa=(long*)malloc(sizeof(long)*N_capas);
        
        fActivacion_por_capa = (fActivacion_punte*)malloc(sizeof(fActivacion_punte)*N_capas);
    }

    
    

    entrenamiento = cargarEntren(entren_name);
    
    if(trainset==0)
        trainset=entrenamiento->N;
    
    subset = mallocEntren(trainset, entrenamiento->tamano_entrada, entrenamiento->tamano_salida);
    
    gsl_ran_shuffle(idum, entrenamiento->shuffling, entrenamiento->N, sizeof(long));
    
    for(i=0; i<trainset; i++)
    {
        gsl_matrix_memcpy(subset->entrada[i], entrenamiento->entrada[entrenamiento->shuffling[i]]);
        gsl_matrix_memcpy(subset->salida[i], entrenamiento->salida[entrenamiento->shuffling[i]]);
    }
    
    freeEntren(entrenamiento);
    
    test = cargarEntren(test_name);
    
    
    if(cargar)
        red = cargarRedneu(file_name_c);
    else
    {
        neuronas_por_capa[0]=subset->tamano_entrada;
        neuronas_por_capa[N_capas-1]=subset->tamano_salida;
        
        fActivacion_por_capa[0]=NULL; //la 'activacion' de la entrada es la identidad por definicion

        if(outputAct=='s')
            fActivacion_por_capa[N_capas-1]=activacionSigmoide;
        else if(outputAct=='w')
            fActivacion_por_capa[N_capas-1]=activacionSwish;
        else if(outputAct=='t')
            fActivacion_por_capa[N_capas-1]=activacionTanh;
        else if(outputAct=='r')
            fActivacion_por_capa[N_capas-1]=activacionRELU;

        red = mallocRedneu(N_capas, neuronas_por_capa, fActivacion_por_capa);

        inicializarRed(red, init_media, init_sigma);
    }
    
    if(verbose) printf("epoca\tcoste\taciertos(%%)\n");
    for(i=0; i<iteraciones; i++)
    {
        gsl_ran_shuffle(idum, subset->shuffling, subset->N, sizeof(long));
       
        for(j=0; j<subset->N/lote; j++) //el entrenamiento se realiza por lotes de tamano 'lote'
            algoritmoAprendizaje_gradientDescent(red, subset, j*lote, (j+1)*lote-1, velocidad, reg_factor, gradiente_coste);
        
        aciertos=0;
        coste=0;
        for(j=0; j<test->N; j++)
        {
            coste+= funcion_coste(red, test->entrada[j], test->salida[j]);
    
            if(checkRespuestaRed(red, test->entrada[j], test->salida[j], tolerancia))
                aciertos+=1.0;

        }
        coste/=test->N;
        aciertos/=test->N;
        
        if(verbose)
            printf("%ld\t%.3lf\t%.3lf\n",i, coste, aciertos*100);
        else
            printf("%ld\t%lf\t%lf\n",i, coste, aciertos*100);
    }
    
    if(guardar) guardarRedneu(red, file_name_g);
    
    
    free(neuronas_por_capa);
    free(fActivacion_por_capa);
    freeEntren(subset);
    freeEntren(test);
    freeRedneu(red);
    
    return 0;
}
