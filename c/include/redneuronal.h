/*ESTRUCTURAS*/

typedef double (*fActivacion_punte)(double, char);

typedef struct REDNEURONAL_STRUCT
{
    long N_capas;
    long* capa; //capa[i] = nº de neuronas en la capa "i".
    gsl_matrix **peso, **gradiente_coste_peso;
    gsl_matrix **sesgo, **gradiente_coste_sesgo;
    gsl_matrix **activacion, **estimulo;
    gsl_matrix **error, **aux;
    
    fActivacion_punte *fActivacion;
    
} REDNEU;

typedef struct ENTRENAMIENTO_STRUCT
{
    long N; //numero de items de entrada  
    long tamano_entrada; //numero de elementos de cada array de entrada.
    long tamano_salida; //numero de elementos de cada array de salida. Si el objeto ENTREN no contiene datos de salida esta cantidad sera 0
    long* shuffling;
    gsl_matrix** entrada;
    gsl_matrix** salida;

} ENTREN;



/*ENCABEZADOS*/

//gestion de memoria
REDNEU* mallocRedneu(long N_capas, long *neuronas_por_capa, fActivacion_punte *fActivacion_por_capa);

ENTREN* mallocEntren(long N, long tamano_entrada, long tamano_salida); //si se trat de un conjunto de test utilizar tamano_salida=0

void    freeRedneu(REDNEU *red);

void    freeEntren(ENTREN *result);



//redes neuronales artificiales
void    inicializarRed(REDNEU* red, double media, double sigma); //inicializa la red con valores aleatorios distribuidos gausianamente

void    propagacionRed(REDNEU* red, gsl_matrix* entrada); //actualiza la activacion y el estimulo de cada capa para una 'entrada'. La activacion de la ultima capa (red->activacion[red->N_capas-1]) es la salida de la red

void    gradiente_costeCuadratico(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida); //Calcula el gradiente de la funcion de coste cuadratica para una entrada, utilizando el algoritmo backup_substitution

void    gradiente_costeCrossEntropy(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida);

void    algoritmoAprendizaje_gradientDescent(REDNEU* red, ENTREN* entrenamiento, long begin, long end, double velocidad, double reg_factor, void (*gradiente_coste)(REDNEU*, gsl_matrix*, gsl_matrix*)); //entrena a la red con un lote de entrenamientos etiquetados entre [begin, end]

//double  algoritmoAprendizaje_simulatedAnnealing(REDNEU* red, ENTREN* entrenamiento, long begin, long end, double velocidad, double reg_factor, double beta);

void    guardarRedneu(REDNEU* red, char* name);

REDNEU* cargarRedneu(char* name);

void    guardarEntren(ENTREN* entrenamiento, char* name);

ENTREN* cargarEntren(char* name);

long    checkRespuestaRed(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida, double tolerancia); //saca '1' si la red acierta con la respuesta, '0' en otro caso. Si tolerancia<0 la funcion actua en modo 'clasificador' y sacara el indice de la capa de salida cuyo valor sea maximo

void    copiarRedneu(REDNEU* dest, REDNEU* src); //copia todo el contenido de 'src' en 'dest'. Deben teenr las mismas dimensiones

REDNEU* clonarRedneu(REDNEU* src); //clona la red 'src' en un puntero a una nueva red


//funciones de activacion
double  activacionSigmoide(double x, char derivada); //funcion sigmoide. Si 'derivada'==0, se extrae la "derivada de orden 0", i.e. la funcion. Si 'derivada'==1 se extrae la derivada

double  activacionSwish(double x, char derivada);

double  activacionTanh(double x, char derivada); //funcion tangente hiperbolica. Si 'derivada'==0, se extrae la "derivada de orden 0", i.e. la funcion. Si 'derivada'==1 se extrae la derivada

double  activacionRELU(double x, char derivada); //funcion REctified Linear Unit. Si 'derivada'==0, se extrae la "derivada de orden 0", i.e. la funcion. Si 'derivada'==1 se extrae la derivada


//regresion lineal polinomial



//auxiliares
double  costeCuadratico(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida);

double  costeCrossEntropy(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida);

ENTREN* leerMNISTdatabase(char* archivo_etiquetas, char* archivo_imagen); //produce un set de entrenamiento a partir de la base de datos de numeros manuscritos MNIST

void    pintarImagen(ENTREN* entrenamiento, long i, FILE* punte); //pinta el numero manuscrito etiquetado 'i' en el conjunto 'entrenamiento' sobre el flujo 'punte'





/*FUNCIONES*/

//gestion de memoria
REDNEU* mallocRedneu(long N_capas, long *neuronas_por_capa, fActivacion_punte *fActivacion_por_capa)
{
    long i;
    REDNEU *red;
    
    red=(REDNEU*)malloc(sizeof(REDNEU));
    
    red->N_capas=N_capas;
    red->peso=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*red->N_capas);
    red->gradiente_coste_peso=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*red->N_capas);
    red->sesgo=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*red->N_capas);
    red->gradiente_coste_sesgo=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*red->N_capas);
    red->activacion=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*red->N_capas);
    red->estimulo=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*red->N_capas);
    red->error=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*red->N_capas);
    red->aux=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*red->N_capas);
    
    red->capa=(long*)malloc(sizeof(long)*N_capas);
    red->fActivacion=(fActivacion_punte*)malloc(sizeof(fActivacion_punte)*N_capas);
    for(i=0; i<red->N_capas; i++)
    {
        red->capa[i]=neuronas_por_capa[i];
        red->fActivacion[i]=fActivacion_por_capa[i];
    }
    
    for(i=0; i<red->N_capas; i++)
    {
        if(i>0)//else
        {
            red->peso[i]=gsl_matrix_alloc(red->capa[i], red->capa[i-1]);
            red->gradiente_coste_peso[i]=gsl_matrix_alloc(red->capa[i], red->capa[i-1]);
            
            red->sesgo[i]=gsl_matrix_alloc(red->capa[i], 1);
            red->gradiente_coste_sesgo[i]=gsl_matrix_alloc(red->capa[i], 1);
            
            red->error[i]=gsl_matrix_alloc(red->capa[i], 1);
            
            red->aux[i]=gsl_matrix_alloc(red->capa[i], 1);
        }
        
        red->activacion[i]=gsl_matrix_alloc(red->capa[i], 1);
        red->estimulo[i]=gsl_matrix_alloc(red->capa[i], 1);
    }
    
    return red;
}

ENTREN* mallocEntren(long N, long tamano_entrada, long tamano_salida)
{
    long i;
    ENTREN* result;
    
    result = (ENTREN*)malloc(sizeof(ENTREN));
    
    result->N = N;
    result-> tamano_entrada = tamano_entrada;
    result->tamano_salida = tamano_salida;
    
    result->shuffling=(long*)malloc(sizeof(long)*N);
    for(i=0; i<N; i++)
        result->shuffling[i]=i;
    
    result->entrada = (gsl_matrix**)malloc(sizeof(gsl_matrix*)*N);
    for(i=0; i<N; i++)
        result->entrada[i] = gsl_matrix_alloc(tamano_entrada, 1);
    
    if(tamano_salida!=0)
    {
        result->salida = (gsl_matrix**)malloc(sizeof(gsl_matrix*)*N);
        for(i=0; i<N; i++)
            result->salida[i] = gsl_matrix_alloc(tamano_salida, 1);
    }
    
    return result;
}

void freeRedneu(REDNEU* red)
{
    long i;
    
    for(i=0; i<red->N_capas; i++)
    {
        if(i>0)
        {
            gsl_matrix_free(red->peso[i]);
            gsl_matrix_free(red->gradiente_coste_peso[i]);
        
            gsl_matrix_free(red->sesgo[i]);
            gsl_matrix_free(red->gradiente_coste_sesgo[i]);
        
            gsl_matrix_free(red->error[i]);
        
            gsl_matrix_free(red->aux[i]);
        }
        
        gsl_matrix_free(red->activacion[i]);
        gsl_matrix_free(red->estimulo[i]);
    }
    
    free(red->peso);
    free(red->gradiente_coste_peso);
    free(red->sesgo);
    free(red->gradiente_coste_sesgo);
    free(red->error);
    free(red->aux);
    free(red->activacion);
    free(red->estimulo);
    free(red->capa);
    free(red->fActivacion);
    free(red);
}

void freeEntren(ENTREN *result)
{
    long i;
    
    for(i=0; i<result->N; i++)
        gsl_matrix_free(result->entrada[i]);
    
    if(result->tamano_salida!=0)
    {
        for(i=0; i<result->N; i++)
            gsl_matrix_free(result->salida[i]);
        
        free(result->salida);
    }
    
    free(result->shuffling);
    
    free(result->entrada);
    free(result);
}



//redes neuronales artificiales
void inicializarRed(REDNEU* red, double media, double sigma)
{
    long i, j, k;
    
    for(i=1; i<red->N_capas; i++)
    {
        for(j=0; j<red->capa[i]; j++)
        {
            if(sigma>0)//inicializacion manual
            {
                for(k=0; k<red->capa[i-1]; k++)
                    gsl_matrix_set(red->peso[i], j, k, ran1_gauss(media, sigma));
                gsl_matrix_set(red->sesgo[i], j, 0, ran1_gauss(media, sigma));
            }
            else if(sigma<0)//inicializacion automatica
            {
                for(k=0; k<red->capa[i-1]; k++)
                    gsl_matrix_set(red->peso[i], j, k, ran1_gauss(0, 1)/sqrt(red->capa[i-1]));
                gsl_matrix_set(red->sesgo[i], j, 0, ran1_gauss(0, 1));
            }
        }
    }
}


void propagacionRed(REDNEU* red, gsl_matrix* entrada)
{
    long i, j;
    
    gsl_matrix_memcpy(red->activacion[0], entrada);
    
    for(i=1; i<red->N_capas; i++)
    {
        gsl_matrix_memcpy(red->estimulo[i], red->sesgo[i]);  
        
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, red->peso[i], red->activacion[i-1], 1, red->estimulo[i]);
            
        for(j=0; j<red->capa[i];j++)
            gsl_matrix_set(red->activacion[i], j, 0, red->fActivacion[i](gsl_matrix_get(red->estimulo[i], j, 0), 0));
    }
}


void gradiente_costeCuadratico(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida)
{
    long i, j;
    double aux;
    
    propagacionRed(red, entrada);
    
    for(i=red->N_capas-1; i>0; i--)
    {
        if(i==red->N_capas-1)
        {
            gsl_matrix_memcpy(red->error[red->N_capas-1], red->activacion[red->N_capas-1]);
            gsl_matrix_sub(red->error[red->N_capas-1], salida);//'red->error' guarda el gradiente para la funcion de coste cuadratica respecto de las activaciones de la ultima capa
        }
        else
            gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1, red->peso[i+1], red->error[i+1], 0, red->error[i]);
        
        for(j=0; j<red->capa[i]; j++)
        {
            aux = gsl_matrix_get(red->estimulo[i], j, 0);
            gsl_matrix_set(red->aux[i], j, 0, red->fActivacion[i](aux, 1));//'red->aux' guarda la derivada de la funcion de activacion para la capa 'i'
        }

        gsl_matrix_mul_elements(red->error[i], red->aux[i]);//este es el error de verdad para la capa 'i'
        
        gsl_matrix_add(red->gradiente_coste_sesgo[i], red->error[i]);
        
        gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1, red->error[i], red->activacion[i-1], 1, red->gradiente_coste_peso[i]);
    }
}


void gradiente_costeCrossEntropy(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida)
{
    long i, j;
    double aux, aux1, aux2;
    
    propagacionRed(red, entrada);
    
    for(i=red->N_capas-1; i>0; i--)
    {
        if(red->fActivacion[i]==activacionSigmoide)//si la activacion es sigmoide esta funcion de coste tiene un gradiente particularmente simple
        {
            if(i==red->N_capas-1)
            {            
                gsl_matrix_memcpy(red->error[red->N_capas-1], red->activacion[red->N_capas-1]);
                gsl_matrix_sub(red->error[red->N_capas-1], salida);
            }
            else
            {
                gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1, red->peso[i+1], red->error[i+1], 0, red->error[i]);
            
                for(j=0; j<red->capa[i]; j++)
                {
                    aux = gsl_matrix_get(red->estimulo[i], j, 0);
                    gsl_matrix_set(red->aux[i], j, 0, red->fActivacion[i](aux, 1));//'red->aux' guarda la derivada de la funcion de activacion para la capa 'i'
                }
            
                gsl_matrix_mul_elements(red->error[i], red->aux[i]);
            }
            
            gsl_matrix_add(red->gradiente_coste_sesgo[i], red->error[i]);
            
            gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1, red->error[i], red->activacion[i-1], 1, red->gradiente_coste_peso[i]);
        }
        
        else
        {
            if(i==red->N_capas-1)
            {            
                for(j=0; j<red->capa[red->N_capas-1]; j++)
                {
                    aux = gsl_matrix_get(red->activacion[red->N_capas-1], j, 0);
                    aux1 = aux*(1-aux);
                    aux2 = gsl_matrix_get(salida, j, 0);
                    
                    if(fabs(aux1)<EPS)
                    {
                        if(aux1<0)
                            aux1=-EPS;
                        else
                            aux1=EPS;
                    }
                    
                    gsl_matrix_set(red->error[red->N_capas-1], j, 0, (aux-aux2)/aux1);
                }
            }
            else
                gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1, red->peso[i+1], red->gradiente_coste_sesgo[i+1], 0, red->error[i]);
            
            for(j=0; j<red->capa[i]; j++)
            {
                aux = gsl_matrix_get(red->estimulo[i], j, 0);
                gsl_matrix_set(red->aux[i], j, 0, red->fActivacion[i](aux, 1));//'red->aux' guarda la derivada de la funcion de activacion para la capa 'i'
            }
            
            gsl_matrix_mul_elements(red->error[i], red->aux[i]);
            
            gsl_matrix_add(red->gradiente_coste_sesgo[i], red->error[i]);
            
            gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1, red->error[i], red->activacion[i-1], 1, red->gradiente_coste_peso[i]);
        }
    }
}


void algoritmoAprendizaje_gradientDescent(REDNEU* red, ENTREN* entrenamiento, long begin, long end, double velocidad, double reg_factor, void (*gradiente_coste)(REDNEU*, gsl_matrix*, gsl_matrix*))
{
    long i, lote=end-begin+1, Npesos=0;
    
    for(i=1; i<red->N_capas; i++)
    {
        gsl_matrix_set_zero(red->gradiente_coste_sesgo[i]);
        gsl_matrix_set_zero(red->gradiente_coste_peso[i]);
        
        Npesos+=red->capa[i-1]*red->capa[i];
    }
        
    for(i=begin; i<=end; i++)
        gradiente_coste(red, entrenamiento->entrada[entrenamiento->shuffling[i]], entrenamiento->salida[entrenamiento->shuffling[i]]);
    
    for(i=1; i<red->N_capas; i++)
    {
        gsl_matrix_scale(red->gradiente_coste_sesgo[i], velocidad/lote);
        gsl_matrix_sub(red->sesgo[i], red->gradiente_coste_sesgo[i]);

        gsl_matrix_scale(red->gradiente_coste_peso[i], velocidad/lote);
        gsl_matrix_scale(red->peso[i], 1-velocidad*reg_factor/Npesos);
        gsl_matrix_sub(red->peso[i], red->gradiente_coste_peso[i]);  
    }
}


/*double algoritmoAprendizaje_simulatedAnnealing(REDNEU* red, ENTREN* entrenamiento, long begin, long end, double velocidad, double reg_factor, double beta)
{
    long i, j, k, n, N=100;
    double energia, incr_energia, aceptancia=0;
    REDNEU* red_aux;
    
    red_aux = clonarRedneu(red); //inicializamos la red auxiliar
    
    /*for(n=0; n<100; n++)
    {
        energia=0;
        for(i=begin; i<=end; i++)
            energia+=funcionCoste(red, entrenamiento->entrada[i], entrenamiento->salida[i]);
        energia/=(end-begin+1);

        for(i=1; i<red->N_capas; i++)
        {
            for(j=0; j<red->capa[i]; j++)
            {
                gsl_matrix_set(red_aux->sesgo[i], j, 0, gsl_matrix_get(red_aux->sesgo[i], j, 0)+velocidad*(2*ran1()-1));
                
                for(k=0; k<red->capa[i-1]; k++)
                    gsl_matrix_set(red_aux->peso[i], j, k, gsl_matrix_get(red_aux->peso[i], j, k)+velocidad*(2*ran1()-1));
            }
        } //cambio tentativo
        
        incr_energia=0;
        for(i=begin; i<=end; i++)
            incr_energia+=funcionCoste(red_aux, entrenamiento->entrada[i], entrenamiento->salida[i]);
        incr_energia/=(end-begin+1);

        incr_energia-=energia;//incremento de energia

        beta+=fabs(incr_energia);
    }//este bucle se usa para inicializar beta  un valor b*E approx 1
    beta/=100; printf("var_energia_media = %lf\t-->\tbeta = %lf", beta, 1.0/beta); getchar();
    beta = 1/beta;
    
    copiarRedneu(red_aux, red); //inicializamos de nuevo las matrices auxiliares a sus valores originales
    
    for(n=0; n<N; n++)
    {
        energia=0;
        for(i=begin; i<=end; i++)
            energia+=funcionCoste(red, entrenamiento->entrada[i], entrenamiento->salida[i]);
        energia/=(end-begin+1);
        
        for(i=1; i<red->N_capas; i++)
        {
            for(j=0; j<red->capa[i]; j++)
            {
                gsl_matrix_set(red_aux->sesgo[i], j, 0, gsl_matrix_get(red_aux->sesgo[i], j, 0)+velocidad*(2*ran1()-1));
                
                for(k=0; k<red->capa[i-1]; k++)
                    gsl_matrix_set(red_aux->peso[i], j, k, gsl_matrix_get(red_aux->peso[i], j, k)+velocidad*(2*ran1()-1));
            }
        } //cambio tentativo
        
        incr_energia=0;
        for(i=begin; i<=end; i++)
            incr_energia+=funcionCoste(red_aux, entrenamiento->entrada[i], entrenamiento->salida[i]);
        incr_energia/=(end-begin+1);
        
        incr_energia-=energia;//incremento de energia

        if(exp(-beta*incr_energia)>ran1())
        {
            copiarRedneu(red, red_aux);
            aceptancia+=1.0;
        }//cambio aceptado
    }//Este es el bucle de simulated annealing
    
    freeRedneu(red_aux);//liberar memoria
    
    return aceptancia/N;
}*/


void guardarRedneu(REDNEU* red, char* name)
{
    long i, j, k;
    char basura[256];
    FILE* punte;
    
    punte = fopen(name, "w");
    
    fprintf(punte, "Numero_capas = %ld\n\n", red->N_capas);
    
    fprintf(punte, "--CAPAS--\n\n");
    for(i=0; i<red->N_capas; i++)
    {
        if(i==0)
            sprintf(basura, "null");
        else
        {
            if(red->fActivacion[i]==activacionSigmoide)
                sprintf(basura, "sigm");
            else if(red->fActivacion[i]==activacionSwish)
                sprintf(basura, "swish");
            else if(red->fActivacion[i]==activacionTanh)
                sprintf(basura, "tanh");
            else if(red->fActivacion[i]==activacionRELU)
                sprintf(basura, "relu");
        }
        
        fprintf(punte, "[%ld] %ld %s\n\n", i, red->capa[i], basura);
    }
    
    fprintf(punte, "\n");

    
    fprintf(punte, "--SESGOS--\n\n");
    for(i=1; i<red->N_capas; i++)
    {
        fprintf(punte, "[%ld]\n", i);
        for(j=0; j<red->capa[i]; j++)
            fprintf(punte,"%lf ", gsl_matrix_get(red->sesgo[i], j, 0));
        fprintf(punte, "\n\n");
    }
    fprintf(punte, "\n");
    
    fprintf(punte, "--PESOS--\n\n");
    for(i=1; i<red->N_capas; i++)
    {
        fprintf(punte, "[%ld]\n", i);
        for(j=0; j<red->capa[i]; j++)
        {
            for(k=0; k<red->capa[i-1]; k++)
                fprintf(punte,"%lf ", gsl_matrix_get(red->peso[i], j, k));
            fprintf(punte, "\n");
        }
        fprintf(punte, "\n");
    }
    fprintf(punte, "\n--FIN--\n");
    
    fclose(punte);
}

REDNEU* cargarRedneu(char* name)
{
    long i, j, k, N_capas, *neuronas_por_capa;
    double aux;
    FILE* punte;
    char basura[256];
    fActivacion_punte* fActivacion_por_capa;
    REDNEU* result;
    
    punte = fopen(name, "r");
    
    fscanf(punte, "%s %s %ld",basura, basura, &N_capas);
    
    neuronas_por_capa = (long*)malloc(sizeof(long)*N_capas);
    fActivacion_por_capa=(fActivacion_punte*)malloc(sizeof(fActivacion_punte)*N_capas);
    
    fscanf(punte, "%s", basura);
    
    for(i=0; i<N_capas; i++)
    {
        fscanf(punte, "%s %ld %s", basura, &(neuronas_por_capa[i]), basura);
        
        if(i==0)
            fActivacion_por_capa[0]=NULL;
        
        else
        {
            if(strcmp(basura, "sigm")==0)
                fActivacion_por_capa[i]=activacionSigmoide;
            else if(strcmp(basura, "swish")==0)
                fActivacion_por_capa[i]=activacionSwish;
            else if(strcmp(basura, "tanh")==0)
                fActivacion_por_capa[i]=activacionTanh;
            else if(strcmp(basura, "relu")==0)
                fActivacion_por_capa[i]=activacionRELU;
        }
    }
    
    result = mallocRedneu(N_capas, neuronas_por_capa, fActivacion_por_capa);
    
    fscanf(punte, "%s", basura);
    
    for(i=1; i<N_capas; i++)
    {
        fscanf(punte, "%s", basura);
        for(j=0; j<neuronas_por_capa[i]; j++)
        {
            fscanf(punte, "%lf", &aux);
            gsl_matrix_set(result->sesgo[i], j, 0, aux);
        }
    }

    fscanf(punte, "%s", basura);
    
    for(i=1; i<N_capas; i++)
    {
        fscanf(punte, "%s", basura);
        for(j=0; j<neuronas_por_capa[i]; j++)
        {
            for(k=0; k<neuronas_por_capa[i-1]; k++)
            {
                fscanf(punte,"%lf ", &aux);
                gsl_matrix_set(result->peso[i], j, k, aux);
            }
        }
    }
    
    fclose(punte);
    free(neuronas_por_capa);
    free(fActivacion_por_capa);
    
    return result;
}


void guardarEntren(ENTREN* entrenamiento, char* name)
{
    long i, j;
    FILE* punte;
    
    punte = fopen(name, "w");
    fprintf(punte, "N_items = %ld\n", entrenamiento->N);
    fprintf(punte, "tamano_reto = %ld\n", entrenamiento->tamano_entrada);
    fprintf(punte, "tamano_respuesta = %ld\n\n", entrenamiento->tamano_salida);
    for(i=0; i<entrenamiento->N; i++)
    {
        for(j=0; j<entrenamiento->tamano_entrada; j++)
            fprintf(punte, "%lf ", gsl_matrix_get(entrenamiento->entrada[i], j, 0));
        fprintf(punte, "\n\n");
        
        for(j=0; j<entrenamiento->tamano_salida; j++)
            fprintf(punte, "%lf ", gsl_matrix_get(entrenamiento->salida[i], j, 0));
        fprintf(punte, "\n\n\n");
    }
    
    fclose(punte);
}


ENTREN* cargarEntren(char* name)
{
    long i, j, Nitems, tamano_entrada, tamano_salida;
    double aux;
    char basura[256];
    FILE* punte;
    ENTREN* result;
    
    punte=fopen(name, "r");
    
    buscarPalabra(punte, "N_items");
    fscanf(punte, "%s %ld", basura, &Nitems);
    
    buscarPalabra(punte, "tamano_reto");
    fscanf(punte, "%s %ld", basura, &tamano_entrada);
    
    buscarPalabra(punte, "tamano_respuesta");
    fscanf(punte, "%s %ld", basura, &tamano_salida);
    
    result = mallocEntren(Nitems, tamano_entrada, tamano_salida);
    
    for(i=0; i<Nitems; i++)
    {
        for(j=0; j<tamano_entrada; j++)
        {
            fscanf(punte, "%lf", &aux);
            gsl_matrix_set(result->entrada[i], j, 0, aux);
        }
        
        for(j=0; j<tamano_salida; j++)
        {
            fscanf(punte, "%lf", &aux);
            gsl_matrix_set(result->salida[i], j, 0, aux);
        }
    }
    
    fclose(punte);
    
    return result;
}


long checkRespuestaRed(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida, double tolerancia)
{
    long i, index, numero;
    double aux, aux1;
    
    propagacionRed(red, entrada);
    
    if(tolerancia<0)
    {
        index=0;
        numero=0;
        aux = 0;
        aux1=0;
        for(i=0; i<red->capa[red->N_capas-1]; i++)
        {
            if(gsl_matrix_get(red->activacion[red->N_capas-1], i, 0)>aux)
            {
                index = i;
                aux = gsl_matrix_get(red->activacion[red->N_capas-1], i, 0);
            }
            
            if(gsl_matrix_get(salida, i, 0)>aux1)
            {
                numero = i;
                aux1 = gsl_matrix_get(salida, i, 0);
            }
        }
        
        if(index==numero)
            return 1;
        else
            return 0;        
    }
    
    else
    {
        for(i=0; i<red->capa[red->N_capas-1]; i++)
        {
            if (fabs(gsl_matrix_get(red->activacion[red->N_capas-1], i, 0)-gsl_matrix_get(salida, i, 0))>tolerancia)
                return 0;
        }
        return 1;
    }
}


void copiarRedneu(REDNEU* dest, REDNEU* src)
{
    long i;
    
    for(i=0; i<src->N_capas; i++)
    {
        if(i>0)
        {
            gsl_matrix_memcpy(dest->peso[i], src->peso[i]);
            gsl_matrix_memcpy(dest->gradiente_coste_peso[i], src->gradiente_coste_peso[i]);
            
            gsl_matrix_memcpy(dest->sesgo[i], src->sesgo[i]);
            gsl_matrix_memcpy(dest->gradiente_coste_sesgo[i], src->gradiente_coste_sesgo[i]);
            
            gsl_matrix_memcpy(dest->error[i], src->error[i]);
            
            gsl_matrix_memcpy(dest->aux[i], src->aux[i]);
        }
        gsl_matrix_memcpy(dest->activacion[i], src->activacion[i]);
        gsl_matrix_memcpy(dest->estimulo[i], src->estimulo[i]);
    }
}


REDNEU* clonarRedneu(REDNEU* src)
{
    long i;
    REDNEU* result;
    
    result = mallocRedneu(src->N_capas, src->capa, src->fActivacion);
    
    copiarRedneu(result, src);
    
    return result;
}


//funciones de ativacion
double activacionSigmoide(double x, char derivada)
{
    double result;

    result = 1.0/(1+exp(-x));

    if(derivada==0)
        return result;
    
    else if(derivada==1)
        return (result*(1-result));
}


double activacionSwish(double x, char derivada)
{
    double result;

    result = x/(1+exp(-x));

    if(derivada==0)
        return result;
    
    else if(derivada==1)
        return (result+activacionSigmoide(x, 0)*(1-result));
}


double  activacionTanh(double x, char derivada)
{
    double result;
    
    result = tanh(x);
    
    if(derivada==0)
        return result;
    
    else if(derivada==1)
        return (1-result*result);    
}


double activacionRELU(double x, char derivada)
{
    if(derivada==0)
    {
        if(x<EPS) return EPS;
        else return x;
        
    }
    
    else if(derivada==1)
    {
        if(x<EPS)
            return 0;
        else return 1;
        
    }
}


//auxiliares
double costeCuadratico(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida)
{
    long i;
    double result;
    gsl_vector *aux, *aux1;
    
    propagacionRed(red, entrada);
    
    aux = gsl_vector_alloc(red->capa[red->N_capas-1]);
    aux1 = gsl_vector_alloc(red->capa[red->N_capas-1]);
    
    gsl_matrix_get_col(aux, red->activacion[red->N_capas-1], 0);
    gsl_matrix_get_col(aux1, salida, 0);
    
    gsl_vector_sub(aux, aux1);
    result = gsl_blas_dnrm2(aux);
    
    gsl_vector_free(aux);
    gsl_vector_free(aux1);
        

    
    return result;
}


double costeCrossEntropy(REDNEU* red, gsl_matrix* entrada, gsl_matrix* salida)
{
    long i;
    double result, aux, aux1;
    
    propagacionRed(red, entrada);

    result=0;
    for(i=0; i<red->capa[red->N_capas-1]; i++)
    {
        aux = gsl_matrix_get(red->activacion[red->N_capas-1], i, 0);
        aux1 = gsl_matrix_get(salida, i, 0);
        
        if(fabs(aux)<EPS)
        {
            if(aux>0)
                aux=EPS;
            else
                aux=-EPS;
        }
        
        result-=aux1*log(aux)+(1-aux1)*log(1-aux);
    }
    
    return result;
}


ENTREN* leerMNISTdatabase(char* archivo_etiquetas, char* archivo_imagen)
{
    long i, j, k, N;
    long N_filas, N_columnas;
    unsigned char aux_MagicNumber[4], aux_N[4], aux_N_imagen[4], aux_N_filas[4], aux_N_columnas[4];
    long *etiquetas;
    gsl_matrix** imagen;
    ENTREN* result;
    FILE *punte;
    
    punte=fopen(archivo_etiquetas,"rb");

    aux_MagicNumber[0]=fgetc(punte);
    aux_MagicNumber[1]=fgetc(punte);
    aux_MagicNumber[2]=fgetc(punte);
    aux_MagicNumber[3]=fgetc(punte);

    aux_N[0]=fgetc(punte);
    aux_N[1]=fgetc(punte);
    aux_N[2]=fgetc(punte);
    aux_N[3]=fgetc(punte);
    
    N = aux_N[0]*256*256*256+aux_N[1]*256*256+aux_N[2]*256+aux_N[3];

    etiquetas=(long*)malloc(sizeof(long)*N);

    for(i=0;i<N;i++)
        etiquetas[i]=(long)fgetc(punte);

    fclose(punte);
    
    punte=fopen(archivo_imagen,"rb");

    aux_MagicNumber[0]=fgetc(punte);
    aux_MagicNumber[1]=fgetc(punte);
    aux_MagicNumber[2]=fgetc(punte);
    aux_MagicNumber[3]=fgetc(punte);

    aux_N[0]=fgetc(punte);
    aux_N[1]=fgetc(punte);
    aux_N[2]=fgetc(punte);
    aux_N[3]=fgetc(punte);
    N = aux_N[0]*256*256*256+aux_N[1]*256*256+aux_N[2]*256+aux_N[3];

    aux_N_filas[0]=fgetc(punte);
    aux_N_filas[1]=fgetc(punte);
    aux_N_filas[2]=fgetc(punte);
    aux_N_filas[3]=fgetc(punte);
    N_filas = aux_N_filas[0]*256*256*256+aux_N_filas[1]*256*256+aux_N_filas[2]*256+aux_N_filas[3];

    aux_N_columnas[0]=fgetc(punte);
    aux_N_columnas[1]=fgetc(punte);
    aux_N_columnas[2]=fgetc(punte);
    aux_N_columnas[3]=fgetc(punte);
    N_columnas = aux_N_columnas[0]*256*256*256+aux_N_columnas[1]*256*256+aux_N_columnas[2]*256+aux_N_columnas[3];

    imagen=(gsl_matrix**)malloc(sizeof(gsl_matrix*)*N);
    for(i=0;i<N;i++)
        imagen[i]=gsl_matrix_alloc(784,1);

    for(i=0;i<N;i++)
    {
        for(j=0; j<28; j++)
        {
            for(k=0; k<28; k++)
                gsl_matrix_set(imagen[i], j*28+k, 0, (double)fgetc(punte)/256);
        }
    }//cada pixel se representa con un valor (0,1) entre el blanco y el negro

    fclose(punte);    
    
    result = mallocEntren(N, 784, 10);
    for(i=0; i<N; i++)
    {
        gsl_matrix_set_zero(result->salida[i]);
        gsl_matrix_set(result->salida[i], etiquetas[i], 0, 1);
    
        gsl_matrix_memcpy(result->entrada[i], imagen[i]);
        
        gsl_matrix_free(imagen[i]);
    }
    free(imagen);
    free(etiquetas);

    return result;
}


void pintarImagen(ENTREN* entrenamiento, long i, FILE* punte)
{
    long j, k;
    
    for(j=0; j<28; j++)
    {
        for(k=0; k<28; k++)
        {
            if( gsl_matrix_get(entrenamiento->entrada[entrenamiento->shuffling[i]], j*28+k, 0)>0)
                fprintf(punte, "#");
            
            else fprintf(punte, " ");
        }
        fprintf(punte, "\n");
    }
}
