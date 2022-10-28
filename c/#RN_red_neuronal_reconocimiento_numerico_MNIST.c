#include "main.h"
#include "matematicas.h"
#include "redneuronal.h"


int main(int N_opcion, char** opcion)
{
    long i, j, numero, etiqueta, aux;
    long N_capas=2, *neuronas_por_capa, lote=1, iteraciones=1, trainset=0;
    char    entren_labels_name[256]={"train-labels-idx1-ubyte"},
            entren_images_name[256]={"train-images-idx3-ubyte"},
            test_labels_name[256]={"t10k-labels-idx1-ubyte"},
            test_images_name[256]={"t10k-images-idx3-ubyte"},
            guardar=0, cargar=0, file_name_g[256]={"red"}, file_name_c[256]={"red.rna"}, verbose=1, debug=0, fcoste=0, guardar_trainset=0, guardar_test=0, file_name_trainset[256]={"trainset.crp"}, file_name_test[256]={"test.crp"};
    double aciertos, coste, velocidad=1.0, reg_factor=-1.0;
    REDNEU* red;
    ENTREN  *entrenamiento, *subset, *test;
    
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste es un programa de muestra que implementa, ejecuta y evalua una red neuronal artificial para clasificar numeros manuscritos, utilizando la muestra de numeros del MNIST (que debe proporcionarse al programa). La red constara al menos de dos capas de 784 y 10 neuronas respectivamente, necesarias para representar los pixeles de entrada y el vector de salida de la red, sin embargo el programa permite definir una cantidad arbitraria de capas ocultas\n\n----------------------------------\n\n");
            printf("-training [train-labels-idx1-ubyte train-images-idx3-ubyte], esta opcion especifica respectivamente los nombres de los ficheros de labels e imagenes de MNIST usadas como entrenamiento\n\n");
            printf("-test [t10k-labels-idx1-ubyte t10k-images-idx3-ubyte], esta opcion especifica respectivamente los nombres de los ficheros de labels e imagenes de MNIST usadas como test\n\n");
            printf("-trainset [0], esta opcion permite determinar el numero de items utilizado para entrenar, tomados aleatoriomente del conjunto de entrenamiento de MNIST. Si vale '0' se utilizaran todos los items del grupo de entrenamiento. Notar que el programa todavia es sensible a la opcion '-lote'\n\n");
            printf("-guardar_trainset [\"trainset\"], si esta opcion esta presente el programa guardara el conjunto de entrenamientos utilizados en formato .crp\n\n");
            printf("-guardar_test [\"test\"], si esta opcion esta presente el programa guardara el conjunto de test en formato .crp\n\n");
            printf("-hidden [], esta opcion especifica el numero de neuronas por cada capa oculta (por defecto no hay capas ocultas). El numero de capas ocultas en si es deducido a partir de cuantos numeros se proporcionan con esta opcion, e.g:\n\t -hidden 15 21 20 --> implementa una RN con 5 capas de 784, 15, 21, 20 y 10 neuronas\n\n");
            printf("-costelog, si esta opcion esta presente el programa utilizara una funcion de coste logaritmica (entropia cruzada) en lugar de la funcion cuadratica estandar\n\n");
            printf("-reg [0.0], factor de regularizacion: este valor fuerza a la matriz de pesos a mantener valores pequenos, dificultando la sobreparametrizacion\n\n");
            printf("-lote [1], esta opcion especifica el tamano de cada lote de entrenamiento.\n\n");
            printf("-iteraciones [1], numero de iteraciones que se repite cada sesion de entrenamiento \n\n");
            printf("-velocidad [1.0], velocidad de aprendizaje, i.e., el factor multiplicativo del gradiente usado para corregir los parametros de la red\n\n");
            printf("-guardar [\"red\"], si esta opcion esta presenta el programa guardara la RN en formato rna. Debe especificarse el nombre del archivo deseado\n\n");
            printf("-cargar [red.rna], si esta opcion esta presenta el programa cargara la RN guardada en el archivo 'red.rna', en formato rna. Si esta opcion esta presente el programa ignorara las opciones relativas a la topologia de la red (-hidden)\n\n");
            printf("-debug, si esta opcion esta presente el programa pintara cada numero fallado por la red durante el test\n\n");
            printf("-nv, si esta opcion esta presente el programa escribira toda la informacion separada unicamente por saltos de linea\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-training")==0 )
        {
            sscanf(opcion[i+1], "%s", entren_labels_name);
            sscanf(opcion[i+2], "%s", entren_images_name);
        }
            
        
        if( strcmp(opcion[i], "-test")==0 )
        {
            sscanf(opcion[i+1], "%s", test_labels_name);
            sscanf(opcion[i+2], "%s", test_images_name);
        }
        
        if( strcmp(opcion[i], "-trainset")==0 )
            sscanf(opcion[i+1], "%ld", &trainset);
        
        if( strcmp(opcion[i], "-guardar_trainset")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-') sprintf(file_name_trainset,"%s.crp", opcion[i+1]);
            }
            guardar_trainset=1;
        }
        
        if( strcmp(opcion[i], "-guardar_test")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-') sprintf(file_name_test, "%s.crp", opcion[i+1]);
            }
            guardar_test=1;
        }
        
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
            neuronas_por_capa[0]=784;
            neuronas_por_capa[N_capas-1]=10;
            
            for(j=1; j<N_capas-1; j++)
                sscanf(opcion[i+j], "%ld", &(neuronas_por_capa[j]));
        }
        
        if( strcmp(opcion[i], "-costelog")==0 )
            fcoste=1;
        
        if( strcmp(opcion[i], "-reg")==0 )
            sscanf(opcion[i+1], "%lf", &reg_factor);
        
        if( strcmp(opcion[i], "-lote")==0 )
            sscanf(opcion[i+1], "%ld", &lote);
        
        if( strcmp(opcion[i], "-iteraciones")==0 )
            sscanf(opcion[i+1], "%ld", &iteraciones);
        
        if( strcmp(opcion[i], "-velocidad")==0 )
            sscanf(opcion[i+1], "%lf", &velocidad);
        
        if( strcmp(opcion[i], "-guardar")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-') sprintf(file_name_g, "%s.rna", opcion[i+1]);
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
        
        if( strcmp(opcion[i], "-debug")==0 )
            debug=1;
        
        if( strcmp(opcion[i], "-nv")==0 )
            verbose=0;
    }
    if(N_capas==2)
    {
        neuronas_por_capa=(long*)malloc(sizeof(long)*N_capas);
        neuronas_por_capa[0]=784;
        neuronas_por_capa[N_capas-1]=10;
    }
    

    entrenamiento = leerMNISTdatabase(entren_labels_name, entren_images_name);
    
    if(trainset==0)
        trainset=entrenamiento->N;
    
    subset = mallocEntren(trainset, 784, 10);
    
    gsl_ran_shuffle(idum, entrenamiento->shuffling, entrenamiento->N, sizeof(long));
    
    for(i=0; i<trainset; i++)
    {
        gsl_matrix_memcpy(subset->entrada[i], entrenamiento->entrada[entrenamiento->shuffling[i]]);
        gsl_matrix_memcpy(subset->salida[i], entrenamiento->salida[entrenamiento->shuffling[i]]);
    }
    
    freeEntren(entrenamiento);
    
    test = leerMNISTdatabase(test_labels_name, test_images_name);
    

    if(cargar)
        red = cargarRedneu(file_name_c);
    else
    {
        red = mallocRedneu(N_capas, neuronas_por_capa);
        
        inicializarRed(red);
    }
    
    if(verbose) printf("epoca\tcoste\taciertos(%%)\n");
    for(i=0; i<iteraciones; i++)
    {
        gsl_ran_shuffle(idum, subset->shuffling, subset->N, sizeof(long));
       
        for(j=0; j<subset->N/lote; j++)
            algoritmoAprendizaje_gradientDescent(red, subset, j*lote, (j+1)*lote-1, velocidad, reg_factor, fcoste, activacionSigmoide, DactivacionSigmoide);
        
        aciertos=0;
        coste=0;
        for(j=0; j<test->N; j++)
        {
            coste+= funcionCoste(red, test->entrada[j], test->salida[j], fcoste, activacionSigmoide);
            gsl_matrix_max_index(test->salida[j], &etiqueta, &aux);
        
            propagacionRed(red, test->entrada[j], activacionSigmoide);
            gsl_matrix_max_index(red->activacion[red->N_capas-1], &numero, &aux);

            if(numero==etiqueta)
                aciertos+=1.0;
            else if(debug)
            {
                pintarImagen(test, j, stdout); 
                printf("%ld (la red dice que es un %ld)\tpulsar INTRO", etiqueta, numero);
                printf("\n"); 
                getchar();
            }
        }
        coste/=test->N;
        
        if(verbose)
            printf("%ld\t%.3lf\t%.3lf\n",i, coste, aciertos/test->N*100);
        else
            printf("%ld\t%lf\t%lf\n",i, coste, aciertos/test->N*100);
    }
    
    if(guardar) guardarRedneu(red, file_name_g);
    
    if(guardar_trainset) guardarEntren(subset, file_name_trainset);
    if(guardar_test) guardarEntren(test, file_name_test);
    
    free(neuronas_por_capa);
    freeEntren(subset);
    freeEntren(test);
    freeRedneu(red);
    
    return 0;
}
