/*ESTRUCTURAS*/

typedef struct TENSOR_STRUCT
{
	long Nup;
	long largest_up;
	
	long Ndown;
	long largest_down;
	
	long Nindex;
	long largest_index;
	
	long Ndim;
	double *elemento;
	
} TENSOR;




/*ENCABEZADOS*/

//gestion de memoria
TENSOR  *mallocTensor(long Nup, long Ndown, long Ndim);

void    freeTensor(TENSOR* T);


//algebra lineal
MATRIZ  *autovectorMaxPowerMethod(MATRIZ *input, double tolerancia);

MATRIZ  *autovectorMaxInversePowerMethod(MATRIZ *input, MATRIZ *autov_aprox, double tolerancia);

MATRIZ  **autovectorWielandDeflation(MATRIZ *input, double tolerancia);

int     compararMatriz(MATRIZ *A, MATRIZ *B);

MATRIZ  *conjugarMatriz(MATRIZ *A);

void    copiarMatriz(MATRIZ *result, MATRIZ *original);

void    MPFR_copiarMatriz(MPFR_MATRIZ* result, MPFR_MATRIZ* original);

int     deltaKronecker(int i, int j);

double  determinante(MATRIZ *input);

double   GSL_determinante(gsl_matrix *input);

MATRIZ  *eliminacionGausiana(MATRIZ *input);

void    escalarMatriz(double escalar, MATRIZ *A);

void    gramSchmidt(MATRIZ *input);

void    iniciarMatriz(MATRIZ *A, char flag[256]);

void    invertirMatriz(MATRIZ *A);

void    GSL_invertirMatriz(gsl_matrix *A);

void    matrizEquivalente(MATRIZ *A, int fila_substituida, int fila_0, double const_0, int fila_1, double const_1);

double  moduloVector(MATRIZ *A);

void    multiplicarMatrices(MATRIZ *A,  MATRIZ *B, MATRIZ *result);

void    multiplicarMatricesStrassen(MATRIZ *A, MATRIZ *B, MATRIZ *result);

//void  multiplicarMatricesOMP(MATRIZ *A,  MATRIZ *B, MATRIZ *result);

void    normalizarMatriz(MATRIZ *input);

void    permutarColumnas(MATRIZ *A, int columna_a, int columna_b);

void    permutarFilas(MATRIZ *A, int fila_a, int fila_b);

void    pivotarMatriz(MATRIZ *MATRIZ_extendida);

double  productoEscalar(MATRIZ *A, MATRIZ *B);

void    productoHadamard(MATRIZ *A, MATRIZ *B, MATRIZ *result);

//void  productoHadamardParalelo(MATRIZ *A, MATRIZ *B, MATRIZ *result);

//void  productoHadamardOMP(MATRIZ *A, MATRIZ *B, MATRIZ *result);

//void  productoTensorial(MATRIZ *A, MATRIZ *B, MATRIZ *result);

void    restarMatrices(MATRIZ *A, MATRIZ *B, MATRIZ *result);

MATRIZ  *sistemaEcuaciones_EliminacionGaussRetroSub(MATRIZ *A, MATRIZ *b);

void    sumarMatrices( MATRIZ *A, MATRIZ *B, MATRIZ *result);

void    trasponerMatriz(MATRIZ *A, MATRIZ* result);

void    vaciarMatriz(MATRIZ *A);



//calculo
double derivadaParcial(double (*funcion)(), int indf, int coordenada, MATRIZ *x0, void *parametros); //calcula d_indf / d_coordenada. El argumento 'parametros' puede ser el puntero nulo NULL si 'funcion' no necesita una lista de parametros externos.

MATRIZ *solucionCampoNulo_steepestDescendent(int N_funciones, double (*funcion)(), MATRIZ *x0, double tolerancia, void* parametros); //calcula una solucion para funcion(x, parametros)=0; en general 'funcion' sera un campo vectorial que dependera de un indice 'i' para extraer cada componente. El argumento 'parametros' puede ser el puntero NULL si la 'funcion' no necesita una lista de parametros externos.

MATRIZ* integracionEDO_RKkernel(int stages, double* A, double* beta, double *x_ini, double dt, int N, int n_func, double (*funcion)());

MATRIZ* integracionEDS_RKkernel(int stages, int n_rand, double* A, double* beta, double* lambda, double *x_ini, double dt, double* chi, int N, int n_func, double (*funcion)());

DISTRI* integracionEDO2_diferenciasFinitas(double (*Px)(double x), double (*Qx)(double x), double (*Rx)(double x), double x_ini, double x_fin, long N, double y_ini, double y_fin); // dy2/dx2 + P(x)*dy/dx + Q(x)*y + R(x)=0, x-ini <x< x_fin, y_ini <y< y_fin, O(x3)

double  interpolarDistribucion(DISTRI blanco, double x);

MATRIZ *gradiente(double (*funcion)(void* parametros, MATRIZ *microestado),void* parametros, MATRIZ *microestado, double epsilon);

MATRIZ *maximaPendiente(double (*funcion)(void* parametros, MATRIZ *microestado),void* parametros, MATRIZ *micro_inicial, double epsilon);

void    normalizarDistribucion(DISTRI* data, char tipo); //tipo=1 normaliza por area; tipo=2 normaliza al maximo valor

void    MPFR_normalizarDistribucion(MPFR_DISTRI* data, char tipo); //tipo=1 normaliza por area; tipo=2 normaliza al maximo valor

DISTRI* acumularDistribucion(DISTRI* entrada); //devuelve una distribucion con el mismo ejex de la entrada, y el ejey la distribucion acumulada.

DISTRI* desacumularDistribucion(DISTRI* entrada); //devuelve una distribucion con el mismo ejex de la entrada, y el ejey 1 menos la distribucion acumulada ('desacumular').

//estadistica y probabilidad
DISTRI*         ajustarBinomial(DISTRI *histograma, int N_bits, int N_iteraciones);

void            calculaBinomial(DISTRI* blanco, unsigned int n, double p);

DISTRI*         construyeHistograma(MATRIZ *data, double caja, double x_min_in, double x_max_in, char normalizar, char ordenar, char verbose);

DISTRI*         construyeHistograma_Sspace(MATRIZ *data, char normalizar, char ordenar);

MPFR_DISTRI*    MPFR_construyeHistograma(MPFR_MATRIZ *data, mpfr_t caja, mpfr_t x_min_in, mpfr_t x_max_in, char normalizar, char ordenar, char verbose);

MPFR_DISTRI*    MPFR_construyeHistograma_Sspace(MPFR_MATRIZ *data, int resolucion, char normalizar, char ordenar);

double 	        calculaEntropiaPlugin(DISTRI *distribucion); //calcula la entropia de una distribucion

double 	        MPFR_calculaEntropiaPlugin(MPFR_DISTRI* distribucion);

double 	        MPFR_calculaEntropiaGrassberger(MPFR_MATRIZ* data, char ordenar);

double 	        MPFR_calculaEntropiaSpacingEstimate(MPFR_MATRIZ* data, int resolucion, char ordenar);

double          calculaMinEntropiaPlugin(DISTRI *distribucion);

double          MPFR_calculaMinEntropiaPlugin(MPFR_DISTRI *distribucion);

double 	        MPFR_calculaMinEntropiaGrassberger(MPFR_MATRIZ* data, char ordenar);

double 	        MPFR_calculaMinEntropiaSpacingEstimate(MPFR_MATRIZ* data, int resolucion, char ordenar);

double          MPFR_calculaGuesswork(MPFR_DISTRI* distribucion, char ordenar);

double          minimosCuadrados(DISTRI *func1, DISTRI *func2);

double          calcular_promedio(double* input, int N);

double          calcular_promedio_varianza(double *input, int N, double* var);

double          calcular_promedio_varianza_filtrando(double* input, int N, double factor_varianza, double* var);

double          FreedmanDiaconis(MATRIZ* data, char ordenar); //esta funcion calcula el tamano de caja optimo para un histograma siguiendo la regla de Freedman-Diaconis

void            MPFR_FreedmanDiaconis(mpfr_t *result, MPFR_MATRIZ* data, char ordenar);

MATRIZ*         matrizCovarianza(MATRIZ* input);



//combinatoria
double  combinatorio(int arriba, int abajo);

void    escriboTablaPerm(int N_osciladores, int N_retos, int verbose);

double  factorial(int N);


///calculo tensorial
long tensorIndice(TENSOR* T, long* index_array); //index_array contiene los valores (0, Ndim-1) de cada indice

void tensorRevIndice(long* index_array, long index, TENSOR*T); //esta funcion convierte 'index' en su correspondiente arreglo de indices y lo mete en 'index_array'

void tensorSet(TENSOR* T, long* index, double new);

double tensorGet(TENSOR* T, long* index);

void tensorFila(gsl_vector* result, TENSOR *T, long* index_fijo, long index_var);

TENSOR* productoTensorial(TENSOR* A, TENSOR* B); //esta funcion devuelve el producto tensorial AxB

TENSOR* autoContraerIndicesTensor(TENSOR *A, long indexA_up, long indexA_down);

TENSOR* contraerIndicesTensor(TENSOR* A, TENSOR* B, long indexA, long indexB);




/*FUNCIONES*/

//gestion de memoria
TENSOR* mallocTensor(long Nup, long Ndown, long Ndim)
{
	long aux;
	TENSOR* result;
	
	result = malloc(sizeof(TENSOR));
	
	aux = (long)pow((double)Ndim, (double)(Nup+Ndown));
	
	result->elemento=malloc(sizeof(double)*aux);
	
	result->Nup = Nup;
	result->largest_up=(long)(pow((double)Ndim, (double)Nup));
	
	result->Ndown = Ndown;
	result->largest_down=(long)(pow((double)Ndim, (double)Ndown));
	
	result->Nindex = Nup+Ndown;
	result->largest_index=aux;
	
	result->Ndim = Ndim;
	
	
	return result;
}

void freeTensor(TENSOR* T)
{
	free(T->elemento);
	free(T);
}


//algebra lineal
MATRIZ *autovectorMaxPowerMethod(MATRIZ *input, double tolerancia)
{
    int i, j, N;
    double aux, autovalor, error;
    MATRIZ *result, *result_mem;
    
    if(input->N_filas != input->N_columnas)
    {
        printf("\nERROR: en la funcion 'autovector'; la MATRIZ introducida debe ser cuadrada\n");
        
        return NULL;
    }
    else
    {
        N = input->N_filas;
        
        result = mallocMatriz(N, 1);
        for(i=0; i<N; i++)
            result->elemento[i][0] = ran1(&idum);
        normalizarMatriz(result);
        
        result_mem = mallocMatriz(N, 1);
        copiarMatriz(result_mem, result);
        
        for(i=0; i>=0; i++)
        {
            if(i>0) error = autovalor;
            
            multiplicarMatrices(input, result, result); //Esta operacion se esta ejecutando MAL!!!
            aux = productoEscalar(result_mem, result_mem);
            autovalor = productoEscalar(result_mem, result)/aux;
            
            normalizarMatriz(result);
            
            copiarMatriz(result_mem, result);
            
            if(i>0)
            {
                if( fabs(error - autovalor) < tolerancia) break;
            }
        }
        freeMatriz(result_mem);
        
        escalarMatriz(autovalor, result);
        
        return result;
    }
}//Devuelve el autovector asociado al autovalor de modulo maximo, normalizado y escalado por dicho autovalor


MATRIZ *autovectorMaxInversePowerMethod(MATRIZ *input, MATRIZ *autov_aprox, double tolerancia)
{
    int i, j, N;
    double aux, autovalor, valor_q, error;
    MATRIZ *result, *result_mem, *inverse_input;
    
    if(input->N_filas != input->N_columnas)
    {
        printf("\nERROR: en la funcion 'autovector'; la MATRIZ introducida debe ser cuadrada\n");
        
        return NULL;
    }
    else
    {
        N = input->N_filas;
        
        result = mallocMatriz(N, 1);
        
        multiplicarMatrices(input, autov_aprox, result);
        
        valor_q = productoEscalar(autov_aprox, result)/productoEscalar(autov_aprox, autov_aprox);
        if( fabs(valor_q-moduloVector(autov_aprox))<tolerancia ) 
        {
            for(i=0; i<N; i++)
                result->elemento[i][0] = autov_aprox->elemento[i][0];
            normalizarMatriz(result);
            escalarMatriz(valor_q, result);
        
            return result;
            
        }//El aurovector introducido ya es una buena aproximacion
        
        inverse_input = mallocMatriz(N, N);
        iniciarMatriz(inverse_input, "identidad");
        escalarMatriz(valor_q, inverse_input);
        restarMatrices(input, inverse_input, inverse_input);
        
        invertirMatriz(inverse_input); 
        
        for(i=0; i<N; i++)
            result->elemento[i][0] = autov_aprox->elemento[i][0];
        normalizarMatriz(result);
        
        result_mem = mallocMatriz(N, 1);
        copiarMatriz(result_mem, result);
        
        for(i=0; i>=0; i++)
        {
            if(i>0) error = autovalor;
            
            multiplicarMatrices(inverse_input, result, result); //Esta operacion se esta ejecutando MAL!!!
            aux = productoEscalar(result_mem, result_mem);
            autovalor = productoEscalar(result_mem, result)/aux;
            
            normalizarMatriz(result);
            
            copiarMatriz(result_mem, result);
            
            if(i>0)
            {
                if( fabs(error - autovalor) < tolerancia) break;
            }
        }
        freeMatriz(result_mem);
        freeMatriz(inverse_input);
        autovalor = 1/autovalor + valor_q; 
        escalarMatriz(autovalor, result);
        
        return result;
    }
}


MATRIZ  **autovectorWielandDeflation(MATRIZ *input, double tolerancia)
{
    int i,j,n,indice,N;
    double  aux, *autovalor;
    MATRIZ **autovector, **x_vector, *matriz_A, *matriz_B, *escalar_aux, *columna_aux;
    
    //inicializar vectores y matrices
    if(input->N_filas != input->N_columnas)
    {
        printf("\nERROR: en funcion 'autovectorWielandDeflation'. La matriz introducida no es cuadrada\n");
        exit(0);
    }
    N = input->N_filas;
        
    matriz_A = mallocMatriz(N, N);
    
    matriz_B = mallocMatriz(N, N);
    
    columna_aux = mallocMatriz(N, 1);
    escalar_aux = mallocMatriz(1,1);
    
    x_vector = (MATRIZ**)malloc(sizeof(MATRIZ*)*N);
    for(i=0; i<N; i++)
        x_vector[i] = mallocMatriz(1, N);
    
    autovalor = (double*)malloc(sizeof(double)*N);
    
    autovector = (MATRIZ**)malloc(sizeof(MATRIZ*)*N);
    
    copiarMatriz(matriz_A, input);

    //autovalores/vectores
    for(n=0; n<N; n++)
    {
        //calcular autovectores/valores
        columna_aux = autovectorMaxPowerMethod(matriz_A, tolerancia);
        autovector[n] = autovectorMaxInversePowerMethod(matriz_A, columna_aux, tolerancia);
        autovalor[n] = moduloVector(autovector[n]);
        
        //x_vector
        for(i=0; i<N; i++)
        {
            if( compararFloat(autovector[n]->elemento[i][0], 0, EPS) != 0)
            {
                aux = autovector[n]->elemento[i][0];
                indice = i;
                break;
            }
        }
        for(i=0; i<N; i++)
            x_vector[n]->elemento[0][i] = matriz_A->elemento[indice][i] / (aux*autovalor[n]);
        
        //construir B
        multiplicarMatrices(autovector[n], x_vector[n], matriz_B);
        escalarMatriz(autovalor[n], matriz_B);
        restarMatrices(matriz_A, matriz_B, matriz_B);
        
        copiarMatriz(matriz_A, matriz_B);
    }
    
    //construir autovector de la matriz original
    for(n=N-1; n>0; n--)
    {
        for(i=n; i<N; i++)
        {
            multiplicarMatrices(x_vector[n-1], autovector[i], escalar_aux);
            copiarMatriz(columna_aux, autovector[n-1]);
            escalarMatriz(autovalor[n-1]*escalar_aux->elemento[0][0], columna_aux);
            
            escalarMatriz(autovalor[i]-autovalor[n-1], autovector[i]);
            
            sumarMatrices(autovector[i], columna_aux, autovector[i]);
        }
    }
    
    for(i=0; i<N; i++)
    {
        normalizarMatriz(autovector[i]);
        escalarMatriz(autovalor[i], autovector[i]);
    }
    
    freeMatriz(matriz_A);
    freeMatriz(matriz_B);
    freeMatriz(columna_aux);
    freeMatriz(escalar_aux);
    free(autovalor);
    for(i=0; i<N; i++) freeMatriz(x_vector[i]);
    free(x_vector);
    
    return autovector;
}


int compararMatriz(MATRIZ *A, MATRIZ *B)
{
    int i,j;

    if(A->N_filas!=B->N_filas || A->N_columnas!=B->N_columnas)
        return 1;
    else
    {
        for(i=0;i<A->N_filas;i++)
        {
            for(j=0;j<A->N_columnas;j++)
            {
                if(compararFloat(A->elemento[i][j],B->elemento[i][j],EPS))
                    return 1;
            }
        }
        return 0;
    }
}


MATRIZ *conjugarMatriz(MATRIZ *A)
{
    int i, j, k, l, N;
    MATRIZ *result, *conjugado;
    
    if(A->N_filas != A->N_columnas)
    {
        printf("\n\nERROR: en funcion 'conjugarMatriz'. La matriz introducida no es cuadrada\n\n");
        exit(0);
    }
    
    N = A->N_filas;
    
    result = mallocMatriz(N, N);
    conjugado = mallocMatriz(N-1, N-1);
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
        {
            for(k=0; k<i; k++)
            {
                for(l=0; l<j; l++)
                    conjugado->elemento[k][l] = A->elemento[k][l];
                for(l=j+1; l<N; l++)
                    conjugado->elemento[k][l-1] = A->elemento[k][l];
            }
            for(k=i+1; k<N; k++)
            {
                for(l=0; l<j; l++)
                    conjugado->elemento[k-1][l] = A->elemento[k][l];
                for(l=j+1; l<N; l++)
                    conjugado->elemento[k-1][l-1] = A->elemento[k][l];
            }
            if((i+j)%2==0) result->elemento[i][j] = determinante(conjugado);
            else result->elemento[i][j] = -determinante(conjugado);
        }
    }
    freeMatriz(conjugado);
    
    return result;
}


void copiarMatriz(MATRIZ *result, MATRIZ *original)
{
    int i,j;

    if(original->N_filas!=result->N_filas || original->N_columnas!=result->N_columnas)
    {
        printf("\nERROR: en funcion %ccopiarMatriz%c: intentando copiar una MATRIZ a una cascara de distinto tamaño\n",34,34);
        exit(1);
    }

    for(i=0;i<original->N_filas;i++)
    {
        for(j=0;j<original->N_columnas;j++)
            result->elemento[i][j]=original->elemento[i][j];
    }
}


void MPFR_copiarMatriz(MPFR_MATRIZ* result, MPFR_MATRIZ* original)
{
    int i, j;
    
    if(original->N_filas!=result->N_filas || original->N_columnas!=result->N_columnas)
    {
        printf("\nERROR: en funcion %ccopiarMatriz%c: intentando copiar una MATRIZ a una cascara de distinto tamaño\n",34,34);
        exit(1);
    }
    
    for(i=0; i<original->N_filas; i++)
    {
        for(j=0; j<original->N_columnas; j++)
            mpfra_set(result->elemento[i][j], original->elemento[i][j]);
    }
}


int deltaKronecker(int i, int j)
{
    if(i==j)
        return 1;
    else
        return 0;
}

double determinante(MATRIZ *input)
{
    int N, i, j, k;
    double result = 0;
    MATRIZ *aux;

    N = input->N_filas;

    if(N==1)
        return input->elemento[0][0];
    else
    {
        aux = mallocMatriz(N-1, N-1);

        for(i=0;i<N;i++) ///Esto son columnas!
        {
            for(j=0;j<N-1;j++)
            {
                for(k=0;k<i;k++)
                    aux->elemento[j][k] = input->elemento[j+1][k];

                for(k=i+1;k<N;k++)
                    aux->elemento[j][k-1] = input->elemento[j+1][k];
            }
            if(i%2==0)
                result+=input->elemento[0][i]*determinante(aux);
            else
                result-=input->elemento[0][i]*determinante(aux);
        }
        freeMatriz(aux);
        return result;
    }
}


double GSL_determinante(gsl_matrix *input)
{
    double result;
    gsl_matrix* LU;
    gsl_permutation* p;
    int signum;
    
    LU = gsl_matrix_alloc(input->size1, input->size2);
    
    p = gsl_permutation_alloc(input->size1);
    
    gsl_matrix_memcpy(LU, input);
    
    gsl_linalg_LU_decomp(LU, p, &signum);
    
    result = gsl_linalg_LU_det(LU, signum);
    
    gsl_matrix_free(LU);
    gsl_permutation_free(p);

    return result;
}



MATRIZ *eliminacionGausiana(MATRIZ *input)
{
    int i, j, k, index, N_filas, N_columnas;
    double aux;
    MATRIZ *result;
    
    N_filas = input->N_filas;
    N_columnas = input->N_columnas;
    
    result = mallocMatriz(N_filas, N_columnas);
    
    for(i=0;i<N_filas;i++)
    {
        for(j=0;j<N_columnas;j++)
            result->elemento[i][j] = input->elemento[i][j];
    }
    
    for(i=0; i<N_columnas-2; i++)
    {
        index = i;
        for(j=i+1; j<N_filas;j++)
        {
            if(fabs(result->elemento[j][i]) > 1e-15)
            {
                if(j<index)
                    index = j;
            }
        }
        if(index!=i)
            permutarFilas(result, index, i);
        
        for(j=i+1; j<N_filas; j++)
        {
            aux = result->elemento[j][i] / result->elemento[i][i];
            
            for(k=0;k<N_columnas;k++)
                result->elemento[j][k] = result->elemento[j][k]-aux*result->elemento[i][k];
        }
    }
    return result;
}

void escalarMatriz(double escalar, MATRIZ *A)
{
    int i,j;

    for(i=0;i<A->N_filas;i++)
    {
        for(j=0;j<A->N_columnas;j++)
            A->elemento[i][j]=escalar*(A->elemento[i][j]);
    }
}

void gramSchmidt(MATRIZ *input)
{
    int i, j, N;
    double aux;
    MATRIZ **base_vieja, **base_nueva, *vector_resta, *vector_aux;
    
    if(input->N_filas != input->N_columnas)
    {
        printf("\nERROR: en funcion 'gramSchmidt. La MATRIZ introducida debe ser cuadrada'\n");
    }
    else
    {
        N = input->N_filas;
        
        base_vieja = malloc(sizeof(MATRIZ*)*N);
        for(i=0; i<N; i++)
        {
            base_vieja[i] = mallocMatriz(N, 1);
            for(j=0; j<N; j++)
                base_vieja[i]->elemento[j][0] = input->elemento[i][j];
        }
        
        base_nueva = malloc(sizeof(MATRIZ*)*N);
        for(i=0; i<N; i++)
        {
            base_nueva[i] = mallocMatriz(N, 1);
            for(j=0; j<N;j++)
                base_nueva[i]->elemento[j][0] = 0;
        }
        
        vector_resta = mallocMatriz(N, 1);
        vector_aux = mallocMatriz(N, 1);
        for(i=0; i<N; i++)
        {
            vaciarMatriz(vector_resta);
            for(j=0; j<i; j++)
            {
                aux = productoEscalar(base_nueva[j], base_vieja[i])/productoEscalar(base_nueva[j], base_nueva[j]);
                
                copiarMatriz(vector_aux, base_nueva[j]);
                
                escalarMatriz(aux, vector_aux);
                
                sumarMatrices(vector_aux, vector_resta, vector_resta);
            }
            
            restarMatrices(base_vieja[i], vector_resta, base_nueva[i]);
        }
        
        for(i=0; i<N; i++)
        {
            for(j=0; j<N; j++)
                input->elemento[i][j] = base_nueva[i]->elemento[j][0];
        }
    }
    
    for(i=0; i<N; i++)
    {
        freeMatriz(base_nueva[i]);
        freeMatriz(base_vieja[i]);
    }
    free(base_nueva);
    free(base_vieja);
    
    freeMatriz(vector_aux);
    freeMatriz(vector_resta);
}

void iniciarMatriz(MATRIZ *A, char flag[256])
{
    int i,j;

    for(i=0;i<A->N_filas;i++)
    {
        for(j=0;j<A->N_columnas;j++)
        {
            if(strcmp(flag, "identidad")==0 || strcmp(flag, "Identidad")==0 || strcmp(flag, "1")==0)
            {
                if(i==j) A->elemento[i][j]=1;
                else A->elemento[i][j]=0;
            }
            
            else if(strcmp(flag, "nula")==0 || strcmp(flag, "Nula")==0 || strcmp(flag, "0")==0) 
				A->elemento[i][j]=0;
			
			else
				A->elemento[i][j]=ran1(&idum);
        }
            
    }
}


void invertirMatriz(MATRIZ *A)
{
    int i, j, k, l, N;
    double A_determinante;
    MATRIZ *A_conjugada, *A_traspuesta;
    
    if(A->N_filas != A->N_columnas)
    {
        printf("\nERROR: en funcion 'invertirMatriz': la matriz introducida no es cuadrada\n");
        exit(0);
    }
    
    A_determinante = determinante(A);
    N = A-> N_filas;
    
    A_conjugada = conjugarMatriz(A);
    
    A_traspuesta = mallocMatriz(A_conjugada->N_columnas, A_conjugada->N_filas);
    trasponerMatriz(A_conjugada, A_traspuesta);
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
            A->elemento[i][j] = A_traspuesta->elemento[i][j]/A_determinante;
    }
    
    freeMatriz(A_conjugada);
    freeMatriz(A_traspuesta);
}


void GSL_invertirMatriz(gsl_matrix *A)
{
    gsl_matrix* LU;
    gsl_permutation* p;
    int signum;
    
    LU = gsl_matrix_alloc(A->size1, A->size2);
    
    p = gsl_permutation_alloc(A->size1);
    
    gsl_matrix_memcpy(LU, A);
    
    gsl_linalg_LU_decomp(LU, p, &signum);
    
    gsl_linalg_LU_invert(LU, p, A);
        
    gsl_matrix_free(LU);
    gsl_permutation_free(p);

}


void matrizEquivalente(MATRIZ *A, int fila_substituida, int fila_0, double const_0, int fila_1, double const_1)
{
    int j;

    for(j=0;j<A->N_columnas;j++)
        A->elemento[fila_substituida][j]=const_0*(A->elemento[fila_0][j])+const_1*(A->elemento[fila_1][j]);
}

double moduloVector(MATRIZ *A)
{
    double result;
    int i,j;

    result = 0;
    for(i=0;i<A->N_filas;i++)
    {
        for(j=0;j<A->N_columnas;j++)
            result+=A->elemento[i][j]*A->elemento[i][j];
    }
    result = sqrt(result);

    return result;
}

void multiplicarMatrices(MATRIZ *A,  MATRIZ *B, MATRIZ *result)
{
    MATRIZ *result_aux;
    int i,j,k;

    if(A->N_columnas != B->N_filas)
    {
        printf("\nERROR: en funcion 'multiplicarMatrices': operandos de dimension incorrecta\n");
        exit(1);
    }

    else
    {
        if(result->N_filas!=A->N_filas || result->N_columnas!=B->N_columnas)
        {
            printf("\nERROR: en funcion 'multiplicarMatrices': result de dimension incorrecta\n");
            exit(1);
        }

        else
        {
            result_aux = mallocMatriz(result->N_filas, result->N_columnas);
            
            copiarMatriz(result_aux, result);

            for(i=0;i<A->N_filas;i++)
            {
                for(j=0;j<B->N_columnas;j++)
                    result_aux->elemento[i][j]=0;
            }

            for(i=0;i<A->N_filas;i++)
            {
                for(j=0;j<B->N_columnas;j++)
                {
                    for(k=0;k<A->N_columnas;k++)
                        result_aux->elemento[i][j]+=A->elemento[i][k]*B->elemento[k][j];
                }
            }
            copiarMatriz(result, result_aux);
            freeMatriz(result_aux);
        }
    }
}


void multiplicarMatricesStrassen(MATRIZ *A, MATRIZ *B, MATRIZ *result)
{
    MATRIZ*** sub_A, ***sub_B, ***sub_result, **M;
    MATRIZ *aux0, *aux1;
    int N, sub_N, i, j, k, l, A_filas_extra, A_columnas_extra, B_filas_extra, B_columnas_extra, elemento_k, elemento_l;
    double exponente;

    N=A->N_filas;
    if(A->N_columnas>N)
        N=A->N_columnas;
    if(B->N_filas>N)
        N=B->N_filas;
    if(B->N_columnas>N)
        N=B->N_columnas;

    exponente=(log(N)/log(2));
    if(compararFloat(exponente-(int)exponente, 0, 0.00000001)!=0)
    {
        if(exponente-(int)exponente<0.5)
            N=pow(2,(int)exponente);

        else
            N=pow(2,(int)exponente+1);

        A_filas_extra=N-A->N_filas;
        A_columnas_extra=N-A->N_columnas;
        B_filas_extra=N-B->N_filas;
        B_columnas_extra=N-B->N_columnas;
    }

    sub_N=N/2;
    sub_A=(MATRIZ***)malloc(sizeof(MATRIZ**)*2);
    sub_B=(MATRIZ***)malloc(sizeof(MATRIZ**)*2);
    sub_result=(MATRIZ***)malloc(sizeof(MATRIZ**)*2);
    M=(MATRIZ**)malloc(sizeof(MATRIZ*)*8);
    aux0=mallocMatriz(sub_N,sub_N);
    aux1=mallocMatriz(sub_N,sub_N);

    for(i=0;i<2;i++)
    {
        sub_A[i]=(MATRIZ**)malloc(sizeof(MATRIZ*)*2);

        sub_B[i]=(MATRIZ**)malloc(sizeof(MATRIZ*)*2);

        sub_result[i]=(MATRIZ**)malloc(sizeof(MATRIZ*)*2);
    }

    for(i=0;i<2;i++)
    {
        for(j=0;j<2;j++)
        {
            sub_A[i][j]=mallocMatriz(sub_N,sub_N);

            sub_B[i][j]=mallocMatriz(sub_N,sub_N);

            sub_result[i][j]=mallocMatriz(sub_N,sub_N);
        }
    }

    for(i=0;i<7;i++)
        M[i]=mallocMatriz(sub_N,sub_N);

    for(i=0;i<2;i++)
    {
        for(j=0;j<2;j++)
        {
            for(k=0;k<sub_N;k++)
            {
                elemento_k=sub_N*i+k;

                for(l=0;l<sub_N;l++)
                {
                    elemento_l=sub_N*j+l;

                    if(elemento_k<A->N_filas && elemento_l<A->N_columnas)
                        sub_A[i][j]->elemento[k][l]=A->elemento[elemento_k][elemento_l];
                    else
                        sub_A[i][j]->elemento[k][l]=0;

                    if(elemento_k<B->N_filas && elemento_l<B->N_columnas)
                        sub_B[i][j]->elemento[k][l]=B->elemento[elemento_k][elemento_l];
                    else
                        sub_B[i][j]->elemento[k][l]=0;
                }
            }
        }
    }

    sumarMatrices(sub_A[0][0],sub_A[1][1],aux0);
    sumarMatrices(sub_B[0][0],sub_B[1][1],aux1);
    multiplicarMatrices(aux0,aux1,M[0]);

    sumarMatrices(sub_A[1][0], sub_A[1][1], aux0);
    multiplicarMatrices(aux0,sub_B[0][0],M[1]);

    restarMatrices(sub_B[0][1], sub_B[1][1], aux0);
    multiplicarMatrices(sub_A[0][0],aux0,M[2]);

    restarMatrices(sub_B[1][0], sub_B[0][0], aux0);
    multiplicarMatrices(sub_A[1][1],aux0,M[3]);

    sumarMatrices(sub_A[0][0],sub_A[0][1],aux0);
    multiplicarMatrices(aux0,sub_B[1][1],M[4]);

    restarMatrices(sub_A[1][0],sub_A[0][0],aux0);
    sumarMatrices(sub_B[0][0],sub_B[0][1],aux1);
    multiplicarMatrices(aux0,aux1,M[5]);

    restarMatrices(sub_A[0][1],sub_A[1][1],aux0);
    sumarMatrices(sub_B[1][0], sub_B[1][1],aux1);
    multiplicarMatrices(aux0,aux1,M[6]);

    for(i=0;i<sub_N;i++)
    {
        for(j=0;j<sub_N;j++)
        {
            sub_result[0][0]->elemento[i][j]=M[0]->elemento[i][j]+M[3]->elemento[i][j]-M[4]->elemento[i][j]+M[6]->elemento[i][j];

            sub_result[0][1]->elemento[i][j]=M[2]->elemento[i][j]+M[4]->elemento[i][j];

            sub_result[1][0]->elemento[i][j]=M[1]->elemento[i][j]+M[3]->elemento[i][j];

            sub_result[1][1]->elemento[i][j]=M[0]->elemento[i][j]-M[1]->elemento[i][j]+M[2]->elemento[i][j]+M[5]->elemento[i][j];
        }
    }

    for(i=0;i<2;i++)
    {
        for(j=0;j<2;j++)
        {
            for(k=0;k<sub_N;k++)
            {
                elemento_k=sub_N*i+k;
                if(elemento_k>=result->N_filas)
                    break;

                for(l=0;l<sub_N;l++)
                {
                    elemento_l=sub_N*j+l;
                    if(elemento_l>=result->N_columnas)
                        break;

                    result->elemento[elemento_k][elemento_l]=sub_result[i][j]->elemento[k][l];
                }
            }
        }
    }

    for(i=0;i<2;i++)
    {
        for(j=0;j<2;j++)
        {
            freeMatriz(sub_A[i][j]);
            freeMatriz(sub_B[i][j]);
            freeMatriz(sub_result[i][j]);
        }
    }
    for(i=0;i<2;i++)
    {
        free(sub_A[i]);

        free(sub_B[i]);

        free(sub_result[i]);
    }

    free(sub_A);
    free(sub_B);
    free(sub_result);

    for(i=0;i<7;i++)
        freeMatriz(M[i]);
    free(M);

    freeMatriz(aux0);
    freeMatriz(aux1);
}

/*void multiplicarMatricesOMP(MATRIZ *A,  MATRIZ *B, MATRIZ *result)
 {

 int i,j,k;

 if(A->N_columnas != B->N_filas)
 {
 printf("\nERROR: en funcion %cmultiplicarMatrices%c: operandos de dimension incorrecta\n",34,34);
 exit(1);
 }

 else
 {
 if(result->N_filas!=A->N_filas || result->N_columnas!=B->N_columnas)
 {
 printf("\nERROR: en funcion %cmultiplicarMatrices%c: result de dimension incorrecta\n",34,34);
 exit(1);
 }

 else
 {

 for(i=0;i<A->N_filas;i++)
 {
 for(j=0;j<B->N_columnas;j++)
 result->elemento[i][j]=0;
 }

 omp_set_dynamic(1);
 #pragma omp parallel private (i, j, k) shared (A, B, result)
 {
 #pragma omp for nowait schedule (auto)
 for(i=0;i<A->N_filas;i++)
 {
 for(j=0;j<B->N_columnas;j++)
 {
 for(k=0;k<A->N_columnas;k++)
 result->elemento[i][j]+=(A->elemento)[i][k]*(B->elemento)[k][j];
 }
 }
 }
 }
 }
 }*/

void normalizarMatriz(MATRIZ *input)
{
    escalarMatriz(1/sqrt(productoEscalar(input, input)), input);
}

void permutarColumnas(MATRIZ *A, int columna_a, int columna_b)
{
    int i;
    double* aux;

    aux=(double*)malloc(sizeof(double)*A->N_filas);

    for(i=0;i<A->N_filas;i++)
        aux[i]=A->elemento[i][columna_a];

    for(i=0;i<A->N_filas;i++)
        A->elemento[i][columna_a]=A->elemento[i][columna_b];

    for(i=0;i<A->N_filas;i++)
        A->elemento[i][columna_b]=aux[i];

    free(aux);
}

void permutarFilas(MATRIZ *A, int fila_a, int fila_b)
{
    int i;
    double* aux;

    aux=(double*)malloc(sizeof(double)*A->N_columnas);

    for(i=0;i<A->N_columnas;i++)
        aux[i]=A->elemento[fila_a][i];

    for(i=0;i<A->N_columnas;i++)
        A->elemento[fila_a][i]=A->elemento[fila_b][i];

    for(i=0;i<A->N_columnas;i++)
        A->elemento[fila_b][i]=aux[i];

    free(aux);
}

void pivotarMatriz(MATRIZ *matriz_extendida)
{
    int i,j;
    int N; //numero de veces que se puede reducir la MATRIZ
    double aux;
    int aux_i, aux_j;

    if(matriz_extendida->N_filas<=matriz_extendida->N_columnas)
        N=matriz_extendida->N_filas;
    else
        N=matriz_extendida->N_columnas;

    for(j=0;j<N;j++)
    {
        aux=0;

        for(i=j;i<N;i++)
        {
            if(fabs(matriz_extendida->elemento[i][j])>aux)
            {
                aux=fabs(matriz_extendida->elemento[i][j]);
                aux_i=i;
            }
        }
        permutarFilas(matriz_extendida,aux_i,j);
    }
}

double productoEscalar(MATRIZ *A, MATRIZ *B)
{
    int i, j, N_filas, N_columnas;
    double result = 0;
    MATRIZ *matriz_auxiliar;
    
    N_filas = A->N_filas;
    N_columnas = A->N_columnas;
    
    matriz_auxiliar = mallocMatriz(N_filas, N_columnas);
    productoHadamard(A, B, matriz_auxiliar);
    
    for(i=0;i<N_filas;i++)
    {
        for(j=0;j<N_columnas;j++)
            result+=matriz_auxiliar->elemento[i][j];
    }
    
    freeMatriz(matriz_auxiliar);
    
    return result;
}

void productoHadamard(MATRIZ *A, MATRIZ *B, MATRIZ *result)
{
    int i, j;

    if(A->N_filas !=B->N_filas || A->N_columnas!=B->N_columnas)
    {
        printf("\nERROR en funcion %cproductoHadamard%c: matrices de distinta dimension\n",34,34);
        exit(1);
    }

    else
    {
        if(result->N_filas!=A->N_filas || result->N_columnas!=A->N_columnas)
        {
            printf("\nERROR en funcion %cproductoHadamard%c: result de dimension incorrecta\n",34,34);
            exit(1);
        }
        else
        {
            for(i=0;i<A->N_filas;i++)
            {
                for(j=0;j<A->N_columnas;j++)
                    result->elemento[i][j]=A->elemento[i][j]*B->elemento[i][j];
            }
        }
    }
}

/*void productoHadamardOMP(MATRIZ *A, MATRIZ *B, MATRIZ *result)
 {
 int i, j;

 if(A->N_filas !=B->N_filas || A->N_columnas!=B->N_columnas)
 {
 printf("\nERROR en funcion %cproductoHadamard%c: matrices de distinta dimension\n",34,34);
 exit(1);
 }

 else
 {
 if(result->N_filas!=A->N_filas || result->N_columnas!=A->N_columnas)
 {
 printf("\nERROR en funcion %cproductoHadamard%c: result de dimension incorrecta\n",34,34);
 exit(1);
 }
 else
 {
 omp_set_dynamic(1);
 #pragma omp parallel private (i, j) shared (A, B, result)
 {
 #pragma for schedule (auto) nowait
 for(i=0;i<A->N_filas;i++)
 {
 for(j=0;j<A->N_columnas;j++)
 result->elemento[i][j]=A->elemento[i][j]*B->elemento[i][j];
 }
 }
 }
 }
 }*/

/*void productoTensorial(MATRIZ *A, MATRIZ *B, MATRIZ *result)
{
    int i,j,k,l;

    if(result->N_filas!=(A->N_filas*B->N_filas) || result->N_columnas!=(A->N_columnas*B->N_columnas))
    {
        printf("\nERROR en funcion %cproductoTensorial%c: result de dimension incorrecta\n",34,34);
        exit(1);
    }
    else
    {
        for(i=0;i<A->N_filas;i++)
        {
            for(j=0;j<B->N_filas;j++)
            {
                for(k=0;k<A->N_columnas;k++)
                {
                    for(l=0;l<B->N_columnas;l++)
                        result->elemento[i*B->N_filas+j][k*B->N_columnas+l] = A->elemento[i][k]*B->elemento[j][l];
                }
            }
        }
    }
}*/

void restarMatrices(MATRIZ *A, MATRIZ *B, MATRIZ *result)
{
    int i,j,k;

    if( A->N_filas!=B->N_filas || A->N_columnas!=B->N_columnas)
    {
        printf("\nERROR: en funcion %csumarMatrices%c: operandos de dimension incorrecta\n",34,34);
        exit(1);
    }
    else
    {
        if(result->N_filas!=A->N_filas || result->N_columnas!=A->N_columnas)
        {
            printf("\nERROR: en funcion %csumarMatrices%c: result de dimension incorrecta\n",34,34);
            exit(1);
        }
        else
        {
            for(i=0;i<A->N_filas;i++)
            {
                for(j=0;j<A->N_columnas;j++)
                    result->elemento[i][j]=(A->elemento)[i][j]-(B->elemento)[i][j];
            }
        }
    }
}

MATRIZ *sistemaEcuaciones_EliminacionGaussRetroSub(MATRIZ *A, MATRIZ *b) //resuleve Ax = b
{
    int i, j, k, index;
    int N_filas, N_columnas;
    double aux;
    MATRIZ *result;
    MATRIZ *matriz_auxiliar, *sistema;

    N_filas = A->N_filas;
    N_columnas = A->N_columnas+1;
    
    sistema = mallocMatriz(N_filas, N_columnas);
    for(i=0; i<N_filas; i++)
    {
        for(j=0; j<N_filas; j++)
            sistema->elemento[i][j] = A->elemento[i][j];
        
        sistema->elemento[i][N_columnas-1] = b->elemento[i][0];    
    
    }

    if(N_columnas-N_filas!=1)
        printf("\nERROR: en la funcion 'GaussEliminationBackSubs'. La MATRIZ 'A' introducida es cuadrada\n");
    else
        matriz_auxiliar = eliminacionGausiana(sistema);

    if(matriz_auxiliar->elemento[N_filas-1][N_columnas-2] == 0)
    {
        printf("\nERROR: en funcion 'GaussEliminationBackSubs', no existe solucion unica al sistema de ecuaciones introducido\n");
        freeMatriz(sistema);
        return NULL;
    }

    else
    {
        result  =   mallocMatriz(N_filas,1);

        result->elemento[N_filas-1][0] = matriz_auxiliar->elemento[N_filas-1][N_columnas-1]/matriz_auxiliar->elemento[N_filas-1][N_columnas-2];
        for(i=N_filas-2; i>=0; i--)
        {
            aux=0;
            for(j=i+1; j<N_filas; j++)
                aux+=matriz_auxiliar->elemento[i][j]*result->elemento[j][0];
            
            result->elemento[i][0]=(matriz_auxiliar->elemento[i][N_columnas-1]-aux)/matriz_auxiliar->elemento[i][i];
        }
        freeMatriz(sistema);
        return result;
    } ///Backwards substitution
}

void sumarMatrices(MATRIZ *A, MATRIZ *B, MATRIZ *result)
{
    int i,j,k;

    if( A->N_filas!=B->N_filas || A->N_columnas!=B->N_columnas)
    {
        printf("\nERROR: en funcion %csumarMatrices%c: operandos de dimension incorrecta\n",34,34);
        exit(1);
    }
    else
    {
        if(result->N_filas!=A->N_filas || result->N_columnas!=A->N_columnas)
        {
            printf("\nERROR: en funcion %csumarMatrices%c: result de dimension incorrecta\n",34,34);
            exit(1);
        }
        else
        {
            for(i=0;i<A->N_filas;i++)
            {
                for(j=0;j<A->N_columnas;j++)
                    result->elemento[i][j]=(A->elemento)[i][j]+(B->elemento)[i][j];
            }
        }
    }
}


void trasponerMatriz(MATRIZ *A, MATRIZ* result)
{
    long i, j;
    
    for(i=0;i<A->N_columnas;i++)
    {
        for(j=0;j<A->N_filas;j++)
            result->elemento[i][j] = A->elemento[j][i];
    }
        
    //Introduce la traspuesta de A en result. Las dimensiones de result deben ser result->Nfilas=A->Ncolumnas, result->Ncolumnas=A->Nfilas
} 


void vaciarMatriz(MATRIZ *A)
{
    int i,j;

    for(i=0;i<A->N_filas;i++)
    {
        for(j=0;j<A->N_columnas;j++)
            A->elemento[i][j]=0;
    }
}


//calculo

double derivadaParcial(double (*funcion)(), int indf, int coordenada, MATRIZ *x0, void *parametros)
{
    int i, N;
    double dx = 0.01, result;
    MATRIZ *x1;
    
    N = x0->N_filas;
    
    x1 = mallocMatriz(N, 1);
    
    for(i=0; i<N; i++)
        x1->elemento[i][0] = x0->elemento[i][0];
    x1->elemento[coordenada][0]+=dx;
    
    result = (funcion(x1, indf, parametros)-funcion(x0, indf, parametros))/dx;
    
    freeMatriz(x1);
    
    return result;
}


MATRIZ *solucionCampoNulo_steepestDescendent(int N_funciones, double (*funcion)(), MATRIZ *x0, double tolerancia, void* parametros)
{
    int i, j, k, N;
    double alfa, alfa1, alfa2, d_alfa=0.01, modulo, modulo1, modulo2;
    MATRIZ *result, *result1, *result2, *jacobiano_traspuesto, *gradiente;
    
    N = N_funciones;
    
    result = mallocMatriz(N, 1);
    result1 = mallocMatriz(N, 1);
    result2 = mallocMatriz(N, 1);
    jacobiano_traspuesto = mallocMatriz(N,N);
    gradiente = mallocMatriz(N, 1);
    
    for(i=0; i<N; i++)
        result->elemento[i][0]=x0->elemento[i][0];
    
    for(k=0; k<1000; k++)
    {
        modulo=0;
        for(i=0; i<N; i++)
            modulo+=funcion(result, i, parametros)*funcion(result, i, parametros);
        
        if(modulo<tolerancia) break;
        
        for(i=0; i<N; i++)
        {
            gradiente->elemento[i][0] = funcion(result, i, parametros);
            for(j=0; j<N; j++)
                jacobiano_traspuesto->elemento[i][j] = derivadaParcial(funcion, j, i, result, parametros);
        }
        
        multiplicarMatrices(jacobiano_traspuesto, gradiente, gradiente);
        
        normalizarMatriz(gradiente);
        
        //buscamos el valor alfa que minimiza la funcion h(alfa)=modulo(x-alfa*gradiente)
        //empezamo con alfa2 = 1;
        alfa2=1;
        while(alfa2>d_alfa)
        {
            for(i=0; i<N; i++)
                result2->elemento[i][0]=result->elemento[i][0]-alfa2*gradiente->elemento[i][0];
            
            modulo2=0;
            for(i=0; i<N; i++)
                modulo2+=funcion(result2, i, parametros)*funcion(result2, i, parametros);
            
            if(modulo2<modulo) break;
            else alfa2-=d_alfa;
        }
        
        alfa1=0.5*alfa2;
        
        for(i=0; i<N; i++)
            result1->elemento[i][0]=result->elemento[i][0]-alfa1*gradiente->elemento[i][0];
        
        modulo1=0;
        for(i=0; i<N; i++)
            modulo1+=funcion(result1, i, parametros)*funcion(result1, i, parametros);
        
        alfa = -0.5*alfa1*(4*modulo1-modulo2-3*modulo)/(modulo2+modulo-2*modulo1);
        
        for(i=0; i<N; i++)
            result->elemento[i][0]-=alfa*gradiente->elemento[i][0];
    }
    
    freeMatriz(jacobiano_traspuesto);
    freeMatriz(gradiente);
    freeMatriz(result1);
    freeMatriz(result2);
    
    return result;
}


DISTRI* integracionEDO1_diferenciasFinitas(double (*Qx)(double x), double (*Rx)(double x), double x_ini, double x_fin, long N, double y_ini, double y_fin) 
{   
    long i, j, N_filas, N_columnas;
    double dx;
    DISTRI* result;
    MATRIZ *soluciones;
    MATRIZ *sistemaLineal, *A, *b;
    
    result=mallocDistribucion(N);
    for(i=0; i<result->N; i++)
        result->ejex[i]=(x_fin-x_ini)/(result->N-1)*i+x_ini;
    
    dx = result->ejex[1]-result->ejex[0];
    
    N_filas = result->N;
    
    N_columnas=N_filas+1;
    
    sistemaLineal   =   mallocMatriz(N_filas, N_columnas);
    
    for(i=0; i<N_filas; i++)
    {
        sistemaLineal->elemento[0][i]=(double)deltaKronecker(0,i);
        sistemaLineal->elemento[N_filas-1][i]=(double)deltaKronecker(N_filas-1,i);
    }///Se rellenan la primera y utltima ecuacion
    
    for(i=1;i<N_filas-1;i++)
    {
        for(j=0;j<N_filas;j++)
            sistemaLineal->elemento[i][j] = deltaKronecker(i,j-1)   -
                                            deltaKronecker(i,j)     *(2*Qx(result->ejex[i])*dx)-
                                            deltaKronecker(i,j+1);
    }
    
    sistemaLineal->elemento[0][N_columnas-1]=y_ini;
    for(i=1;i<N_filas-1;i++)
        sistemaLineal->elemento[i][N_columnas-1]=2*dx*Rx(result->ejex[i]);
    sistemaLineal->elemento[N_filas-1][N_columnas-1]=y_fin; ///Se rellena el vector de igualdades para terminar con la MATRIZ ampliada
    
    A = mallocMatriz(sistemaLineal->N_filas, sistemaLineal->N_columnas-1);
    b = mallocMatriz(sistemaLineal->N_filas, 1);
    
    for(i=0; i<sistemaLineal->N_filas; i++)
    {
        for(j=0; j<sistemaLineal->N_columnas-1; j++)
            A->elemento[i][j] = sistemaLineal->elemento[i][j];
        b->elemento[i][0] = sistemaLineal->elemento[i][sistemaLineal->N_columnas-1];
    }
    
    soluciones=sistemaEcuaciones_EliminacionGaussRetroSub(A, b);
    
    for(i=0;i<N_filas;i++)
        result->ejey[i]=soluciones->elemento[i][0];
    
    freeMatriz(sistemaLineal);
    freeMatriz(soluciones);
    freeMatriz(A);
    freeMatriz(b);
    
    return result;
}


MATRIZ* integracionEDO_RKkernel(int stages, double* A, double* beta, double *x_ini, double dt, int N, int n_func, double (*funcion)())
{
//     Resuelve un sistema de n_func EDOs de la forma: dx[i]/dt = f_i(double x[n_func+1]); Las funciones f_i toman n_func+1 parametros, donde x[0] es el tiempo. La funcion devuelve un array de distribuciones x[i](t). El algoritmo RK realizara 'stages' pasos. Los parametros del algoritmo 'A' y 'beta' se introducen en forma de arrays unidimensionales: A={A_1, A_2, ..., A_stages}, beta={beta_2_1, beta_3_2, beta_3_2, ..., beta_stages_stages-1}. Ver '1981-Greenside' ecuaciones (50) y (51)
    
    int i, j, l, k, contador_beta;
    MATRIZ *g, *x, *x_aux;
    MATRIZ *result;

    g = mallocMatriz(stages, n_func);
    x = mallocMatriz(1, n_func);
	x_aux = mallocMatriz(1, n_func);

    result = mallocMatriz(N, n_func);

	for(k=0; k<n_func; k++)
	{   x->elemento[0][k] = x_ini[k];
		result->elemento[0][k]=x->elemento[0][k];
	}

	for(i=1; i<N; i++)
    {
        for(l=0; l<stages; l++)
        {
			contador_beta=(l*(l+1))/2;
            for(k=0; k<n_func; k++)
            {
                x_aux->elemento[0][k]=x->elemento[0][k];
                for(j=0; j<l-1; j++)
                    x_aux->elemento[0][k]+=dt*beta[contador_beta+j]*g->elemento[j][k];
            }
                
			for(k=0; k<n_func; k++)
                g->elemento[l][k]=funcion(k, x_aux->elemento[0]);
        }
        
        for(k=0; k<n_func; k++)
        {
            for(l=0; l<stages; l++)
                x->elemento[0][k]+=dt*A[l]*g->elemento[l][k];
        }
        
        for(k=0; k<n_func; k++)
			result->elemento[i][k]=x->elemento[0][k];
    }

    freeMatriz(g);
    freeMatriz(x);
    freeMatriz(x_aux);    
    
    return result;
}


MATRIZ* integracionEDS_RKkernel(int stages, int n_rand, double* A, double* beta, double* lambda, double *x_ini, double dt, double* chi, int N, int n_func, double (*funcion)())
{
/*Resuelve un sistema de N EDSs de la forma: 
    dx[i]/dt = funcion(double x[n_func], int i) + ruido_i, 0<=i<=n_func; 
    La funcion 'funcion' es de hecho un campo vectorial: funcion(i, x) = f_i(x)
    Las funciones f_i toman n_func parametros, donde x[0] es el tiempo. La funcion devuelve un array de distribuciones x[i](t)*/
    
    int i, j, l, k, m, contador_beta;
    MATRIZ *g, *x, *x_aux, *Y;
    MATRIZ *result;

    g = mallocMatriz(stages, n_func);
    x = mallocMatriz(1, n_func);
	x_aux = mallocMatriz(1, n_func);
	Y = mallocMatriz(stages+1, n_func);

    result = mallocMatriz(N, n_func);

	for(k=0; k<n_func; k++)
	{   x->elemento[0][k] = x_ini[k];
		result->elemento[0][k]=x->elemento[0][k];
	}

	for(i=1; i<N; i++)
    {
		iniciarMatriz(Y, "0");
		for(l=0; l<stages+1; l++)
		{
			for(k=0; k<n_func; k++)
			{
				for(m=0; m<n_rand; m++)
					Y->elemento[l][k]+=ran1_gauss(0, 1)*lambda[l*n_rand+m];
			}
		}
        for(l=0; l<stages; l++)
        {
			contador_beta=(l*(l+1))/2;
            for(k=0; k<n_func; k++)
            {
                x_aux->elemento[0][k]=x->elemento[0][k];
                for(j=0; j<l-1; j++)
                    x_aux->elemento[0][k]+=dt*beta[contador_beta+j]*g->elemento[j][k];
                x_aux->elemento[0][k]+=sqrt(dt*chi[k])*Y->elemento[l+1][k];
            }
                
			for(k=0; k<n_func; k++)
                g->elemento[l][k]=funcion(k, x_aux->elemento[0]);
        }
        
        for(k=0; k<n_func; k++)
        {
            for(l=0; l<stages; l++)
				x->elemento[0][k]+=dt*A[l]*g->elemento[l][k];
            x->elemento[0][k]+=sqrt(dt*chi[k])*Y->elemento[0][k];
        }
        
        for(k=0; k<n_func; k++)
			result->elemento[i][k]=x->elemento[0][k];
    }

    freeMatriz(g);
    freeMatriz(x);
    freeMatriz(x_aux);
	freeMatriz(Y);
    
    return result;
}


DISTRI* integracionEDO2_diferenciasFinitas(double(*Px)(double x), double (*Qx)(double x), double (*Rx)(double x), double x_ini, double x_fin, long N, double y_ini, double y_fin) 
{  
    long i, j, N_filas, N_columnas;
    double dx;
    DISTRI* result;
    MATRIZ *soluciones;
    MATRIZ *sistemaLineal, *A, *b;
    
    result=mallocDistribucion(N);
    for(i=0; i<result->N; i++)
        result->ejex[i]=(x_fin-x_ini)/(result->N-1)*i+x_ini;
    
    dx = result->ejex[1]-result->ejex[0];
    
    N_filas = result->N;
    
    N_columnas=N_filas+1;
    
    sistemaLineal   =   mallocMatriz(N_filas, N_columnas);
    
    for(i=0; i<N_filas; i++)
    {
        sistemaLineal->elemento[0][i]=(double)deltaKronecker(0,i);
        sistemaLineal->elemento[N_filas-1][i]=(double)deltaKronecker(N_filas-1,i);
    }///Se rellenan la primera y utltima ecuacion
    
    for(i=1;i<N_filas-1;i++)
    {
        for(j=0;j<N_filas;j++)
            sistemaLineal->elemento[i][j]=(1-Px(result->ejex[i])*0.5*dx)*deltaKronecker(i,j-1) -
            (2+Qx(result->ejex[i])*dx*dx)*deltaKronecker(i,j)+
            (1+Px(result->ejex[i])*0.5*dx)*deltaKronecker(i,j+1);
    }
    
    sistemaLineal->elemento[0][N_columnas-1]=y_ini;
    for(i=1;i<N_filas-1;i++)
        sistemaLineal->elemento[i][N_columnas-1]=Rx(result->ejex[i])*dx*dx;
    sistemaLineal->elemento[N_filas-1][N_columnas-1]=y_fin; ///Se rellena el vector de igualdades para terminar con la MATRIZ ampliada
    
    A = mallocMatriz(sistemaLineal->N_filas, sistemaLineal->N_columnas-1);
    b = mallocMatriz(sistemaLineal->N_filas, 1);
    
    for(i=0; i<sistemaLineal->N_filas; i++)
    {
        for(j=0; j<sistemaLineal->N_columnas-1; j++)
            A->elemento[i][j] = sistemaLineal->elemento[i][j];
        b->elemento[i][0] = sistemaLineal->elemento[i][sistemaLineal->N_columnas-1];
    }
    
    soluciones=sistemaEcuaciones_EliminacionGaussRetroSub(A, b);
    
    for(i=0;i<N_filas;i++)
        result->ejey[i]=soluciones->elemento[i][0];
    
    freeMatriz(sistemaLineal);
    freeMatriz(soluciones);
    freeMatriz(A);
    freeMatriz(b);
    
    return result;
}

double interpolarDistribucion(DISTRI blanco, double x)
{
    double indice, result, dx;
    int i, i_previo, i_posterior;
    
    dx = blanco.ejex[1]-blanco.ejex[0];

    indice = (x-blanco.ejex[0])/dx;

    if(indice-(int)indice < 0.5)
    {
        i_previo = (int)indice;

        i_posterior = i_previo +1;
    }
    else
    {
        i_posterior = (int)indice;

        i_previo = i_posterior-1;
    }

    result = (blanco.ejey[i_posterior]-blanco.ejey[i_previo])/dx*(x-blanco.ejex[i_previo])+blanco.ejey[i_previo];

    return result;
}

MATRIZ *gradiente(double (*funcion)(void* parametros, MATRIZ *microestado),void* parametros, MATRIZ *microestado, double epsilon)
{
    int i,j,k,l,N_filas, N_columnas;
    double aux_pos, aux_neg;
    MATRIZ *result, *micro_incremento_pos,*micro_incremento_neg;

    N_filas=microestado->N_filas;
    N_columnas=microestado->N_columnas;


    micro_incremento_pos=mallocMatriz(N_filas, N_columnas);
    micro_incremento_neg=mallocMatriz(N_filas, N_columnas);
    for(i=0;i<N_filas;i++)
    {
        for(j=0;j<N_columnas;j++)
        {
            micro_incremento_pos->elemento[i][j]=microestado->elemento[i][j];
            micro_incremento_neg->elemento[i][j]=microestado->elemento[i][j];
        }
    }

    result = mallocMatriz(N_filas, N_columnas);

    printf("Calculo de gradiente:\n");
    for(i=0;i<N_filas;i++)
    {
        for(j=0;j<N_columnas;j++)
        {
            aux_pos=micro_incremento_pos->elemento[i][j];
            aux_neg=micro_incremento_neg->elemento[i][j];

            micro_incremento_pos->elemento[i][j]+=epsilon;
            micro_incremento_neg->elemento[i][j]-=epsilon;

            result->elemento[i][j]=(funcion(parametros,micro_incremento_pos)-funcion(parametros,micro_incremento_neg))/(2*epsilon);

            micro_incremento_pos->elemento[i][j]=aux_pos;
            micro_incremento_neg->elemento[i][j]=aux_neg;
            //if(100*(i*N_columnas+j)%(N_filas*N_columnas)==0)
            printf("%.3f %% completado\n", 100.0*(i*N_columnas+j)/(1.0*N_filas*N_columnas));
        }
    }

    freeMatriz(micro_incremento_pos);
    freeMatriz(micro_incremento_neg);
    free(micro_incremento_pos);
    free(micro_incremento_neg);

    return result;
}

MATRIZ *maximaPendiente(double (*funcion)(void* parametros, MATRIZ *microestado),void* parametros, MATRIZ *micro_inicial, double epsilon)
{
    int i,j,k,N_filas, N_columnas, contador;
    double modulo_maxima_pendiente;
    MATRIZ *micro_nuevo, *micro_viejo, *maxima_pendiente;

    //printf("\nCONTROL0\n");
    N_filas = micro_inicial->N_filas;
    N_columnas = micro_inicial->N_columnas;

    micro_nuevo = mallocMatriz(N_filas,N_columnas);
    //printf("\nCONTROL1\n");
    micro_viejo=mallocMatriz(micro_inicial->N_filas, micro_inicial->N_columnas);
    copiarMatriz(micro_viejo, micro_inicial);
    contador=0;
    for(i=0;i<1;i++)
    {
        maxima_pendiente=gradiente(funcion,parametros,micro_viejo,epsilon);
        modulo_maxima_pendiente=moduloVector(maxima_pendiente);
        //printf("\nCONTROL2\n");

        for(j=0;j<N_filas;j++)
        {
            for(k=0;k<N_columnas;k++)
                micro_nuevo->elemento[j][k]=micro_viejo->elemento[j][k]-epsilon*maxima_pendiente->elemento[j][k]/modulo_maxima_pendiente;
        }

        //printf("\nCONTROL3\n");

        freeMatriz(maxima_pendiente);
        freeMatriz(micro_viejo);
        //printf("\nCONTROL4\n");
        if(modulo_maxima_pendiente<epsilon)
        {
            freeMatriz(micro_viejo);
            break;
        }
        //printf("\nCONTROL5\n");
        micro_viejo=mallocMatriz(micro_nuevo->N_filas, micro_nuevo->N_columnas);
        copiarMatriz(micro_viejo, micro_nuevo);

        i=-1;
        contador++;
        printf("nº de iteraciones = %d\n",contador);
    }

    return micro_nuevo;
}


void normalizarDistribucion(DISTRI* data, char tipo)
{
    int i, N=data->N;
    double area_sup=0, area_inf=0, area, aux;
        
    if(tipo==1)
	{
		for(i=0; i<N-1; i++)
			area_inf+=(data->ejex[i+1]-data->ejex[i])*data->ejey[i];
        
		for(i=1; i<N; i++)
			area_sup+=(data->ejex[i]-data->ejex[i-1])*data->ejey[i];
        
        area = 0.5*(area_inf+area_sup);
        
		for(i=0; i<N; i++)
			data->ejey[i]/=area;
	}//Aqui se normaliza por area
	
    else if(tipo==2)
	{
        aux=data->ejey[0];
		for(i=1; i<N; i++)
		{
            if(aux<data->ejey[i])
                aux=data->ejey[i];
        }			
		for(i=0; i<N; i++)
			data->ejey[i]/=aux;
	}//Aqui se normaliza por valor maximo
}


void MPFR_normalizarDistribucion(MPFR_DISTRI* data, char tipo)
{
    unsigned long i, N=data->N;
    mpfr_t aux, area_inf, area_sup, area;
    
    mpfra_init_set_ui(area_inf, 0);
    mpfra_init_set_ui(area_sup, 0);
    mpfra_init_set_ui(area, 0);
    mpfra_init_set_ui(aux, 0);
    
    if(tipo==1)
	{
		for(i=0; i<N-1; i++)
		{
			mpfra_sub(aux, data->ejex[i+1], data->ejex[i]);
			mpfra_mul(aux, aux, data->ejey[i]); 
			mpfra_add(area_inf, area_inf, aux);
		}
		
		for(i=1; i<N; i++)
		{
			mpfra_sub(aux, data->ejex[i], data->ejex[i-1]);
			mpfra_mul(aux, aux, data->ejey[i]); 
			mpfra_add(area_sup, area_sup, aux);
		}
		
		mpfra_add(area, area_sup, area_inf);
        mpfra_div_ui(area, area, 2);
			
		for(i=0; i<N; i++)
			mpfra_div(data->ejey[i], data->ejey[i], area);
	}//Aqui se normaliza por area
	
    else if(tipo==2)
	{
        mpfra_set(aux, data->ejey[0]);
		for(i=1; i<N; i++)
		{
            if(mpfra_cmp(aux, data->ejey[i])<0)
                mpfra_set(aux, data->ejey[i]);
        }			
		for(i=0; i<N; i++)
			mpfra_div(data->ejey[i], data->ejey[i], aux);
	}//Aqui se normaliza por valor maximo
	
	mpfr_clear(area_inf);
    mpfr_clear(area_sup);
    mpfr_clear(area);
    mpfr_clear(aux);
}

DISTRI* acumularDistribucion(DISTRI* entrada)
{
    DISTRI* result;
    result = mallocDistribucion(entrada->N);
    for(int i=0; i<entrada->N; i++)
    {
        result->ejex[i] = entrada->ejex[i];
        result->ejey[i]=0;
        for(int j=0; j<i; j++)
            result->ejey[i]+=entrada->ejey[j];
    }
    
    return result;
}

DISTRI* desacumularDistribucion(DISTRI* entrada)
{
    DISTRI* result;
    result = mallocDistribucion(entrada->N);
    for(int i=0; i<entrada->N; i++)
    {
        result->ejex[i] = entrada->ejex[i];
        result->ejey[i]=1;
        for(int j=0; j<i; j++)
            result->ejey[i]-=entrada->ejey[j];
    }
    
    return result;
}


//estadistica
DISTRI* ajustarBinomial(DISTRI *histograma, int N_bits, int N_iteraciones)
{
    int i, x;
    double dx, x_min, x_max, p, minimos_cuadrados, aux, p_result;
    DISTRI *result;

    dx = histograma->ejex[1]-histograma->ejex[0];
    x_min = histograma->ejex[0];
    x_max = histograma->ejex[histograma->N-1];
    if(fabs(dx-(int)dx) > EPS || fabs(x_min-(int)x_min) > EPS)
    {
        printf("\nERROR: el histograma que se intenta ajustar no esta definido en el conjunto de los naturales. Corregir esto antes de proceder\n");

        return NULL;
    }
    result = mallocDistribucion(histograma->N);
    for(i=0; i<result->N; i++)
        result->ejex[i]=(x_max-x_min)/(result->N-1)*i+x_min;

    p=0;
    calculaBinomial(result, N_bits, p);
    minimos_cuadrados = minimosCuadrados(histograma, result);
    aux = minimos_cuadrados;
    p_result=p;
    for(i=0;i<N_iteraciones;i++)
    {
        p=(1.0*(i+1))/N_iteraciones;
        calculaBinomial(result, N_bits, p);
        minimos_cuadrados = minimosCuadrados(histograma, result);
        if(minimos_cuadrados<aux)
        {
            aux = minimos_cuadrados;
            p_result = p;
        }
        percentage(i, N_iteraciones,"\n\nAjustando binomial...");
    }
    calculaBinomial(result, N_bits, p_result); 
    sprintf(result->string, "%lg", p_result);
    
    normalizarDistribucion(result, 1); //normalizamos el resultado por area (ya deberia estar normalizado, pero asi corregimos errores numericos).

    return result;
}


void calculaBinomial(DISTRI* blanco, unsigned int n, double p)
{
    unsigned int k;

    for(int i=0;i<blanco->N;i++)
    {
        k = (unsigned int)(blanco->ejex[i]);
        
        blanco->ejey[i] = gsl_ran_binomial_pdf(k, p, n);
    }
}

DISTRI* construyeHistograma(MATRIZ *data, double caja, double x_min_in, double x_max_in, char normalizar, char ordenar, char verbose)
{
    unsigned long i, j, contador, index, N_data, N_cajas;
    double aux, aux1, x_min, x_max, area;
    MATRIZ* data_ordenado;
    DISTRI *result;
    
    x_min=data->elemento[0][0];
    x_max=data->elemento[0][0];
    area=0;
    aux=0;
    aux1=0;

    N_data = data->N_filas;
    
	data_ordenado = mallocMatriz(N_data, 1);
	for(i=0; i<N_data; i++)
		data_ordenado->elemento[i][0]=data->elemento[i][0];
    
    if(ordenar)
        ordenarData(data_ordenado);
    
    if(fabs(x_min_in-x_max_in)<EPS)
    {
        x_min=data_ordenado->elemento[0][0];
        x_max=data_ordenado->elemento[N_data-1][0];
    }
    else
    {
        x_min=x_min_in;
        x_max=x_max_in;
    }

    aux=(x_max-x_min)/caja;
    
    N_cajas = (long)aux+1;
    
    result = mallocDistribucion(N_cajas);

    contador=0;
    for(i=0;i<N_cajas; i++)
    {
        result->ejex[i]=caja*i;
        result->ejex[i]+=x_min;
    
        result->ejey[i]=0;
        
        for(j=contador; j<N_data; j++)
        {
            aux=data_ordenado->elemento[j][0]-result->ejex[i];
            aux1=caja-aux;
            if( aux1 < EPS)
                break;
            else
            {
                result->ejey[i]++;
                contador++;
            }
        }
    }
    
    if(normalizar>0)
        normalizarDistribucion(result, normalizar);
//     if(normalizar==1)
// 	{
// 		for(i=1; i<N_cajas; i++)
// 			area+=(result->ejex[i]-result->ejex[i-1])*result->ejey[i];
// 			
// 		for(i=0; i<N_cajas; i++)
// 			result->ejey[i]/=area;
// 	}//Aqui se normaliza por area
// 	
//     else if(normalizar==2)
// 	{
//         aux=result->ejey[0];
// 		for(i=1; i<N_cajas; i++)
// 		{
//             if(aux<result->ejey[i])
//                 aux=result->ejey[i];
//         }			
// 		for(i=0; i<N_cajas; i++)
// 			result->ejey[i]/=aux;
// 	}//Aqui se normaliza por valor maximo

    return result;
}


MPFR_DISTRI* MPFR_construyeHistograma(MPFR_MATRIZ *data, mpfr_t caja, mpfr_t x_min_in, mpfr_t x_max_in, char normalizar, char ordenar, char verbose)
{
    unsigned long i, j, contador, index, N_data, N_cajas;
    mpfr_t aux, aux1, x_min, x_max, area;
    MPFR_MATRIZ* data_ordenado;
    MPFR_DISTRI *result;
    
    mpfra_init_set(x_min, data->elemento[0][0]);
    mpfra_init_set(x_max, data->elemento[0][0]);
    mpfra_init_set_ui(area, 0);
    mpfra_init_set_ui(aux, 0);
    mpfra_init_set_ui(aux1, 0);

    N_data = data->N_filas;
    
	data_ordenado = MPFR_mallocMatriz(N_data, 1);
	for(i=0; i<N_data; i++)
		mpfra_set(data_ordenado->elemento[i][0], data->elemento[i][0]);
    
    if(ordenar)
        MPFR_ordenarData(data_ordenado);
    
    if(mpfra_cmp(x_min_in, x_max_in)==0)
    {
        mpfra_set(x_min, data_ordenado->elemento[0][0]);
        mpfra_set(x_max, data_ordenado->elemento[N_data-1][0]);
    }
    else
    {
        mpfra_set(x_min, x_min_in);
        mpfra_set(x_max, x_max_in);
    }

    mpfra_sub(aux, x_max, x_min);
    mpfra_div(aux, aux, caja);
    
    N_cajas = mpfra_get_ui(aux)+1;
    
    result = MPFR_mallocDistribucion(N_cajas);

    contador=0;
    for(i=0;i<N_cajas; i++)
    {
        mpfra_mul_ui(result->ejex[i], caja, i);
        mpfra_add(result->ejex[i], result->ejex[i], x_min);
    
        mpfra_set_ui(result->ejey[i], 0);
        
        for(j=contador; j<N_data; j++)
        {
            mpfra_sub(aux, data_ordenado->elemento[j][0], result->ejex[i]);
            mpfra_sub(aux1, caja, aux);
            if( mpfr_cmp_d(aux1, EPS) < 0)
                break;
            else
            {
                mpfra_add_ui(result->ejey[i], result->ejey[i], 1);
                contador++;
            }
        }
    }

    if(normalizar>0)
        MPFR_normalizarDistribucion(result, normalizar);
//     if(normalizar==1)
// 	{
// 		for(i=1; i<N_cajas; i++)
// 		{
// 			mpfra_sub(aux, result->ejex[i], result->ejex[i-1]);
// 			mpfra_mul(aux, aux, result->ejey[i]); 
// 			mpfra_add(area, area, aux);
// 		}//aqui pueden probarse diversas variaciones del calculo de area (mayores ordenes de precision en la intgral)
// 			
// 		for(i=0; i<N_cajas; i++)
// 			mpfra_div(result->ejey[i], result->ejey[i], area);
// 	}//Aqui se normaliza por area
// 	
//     else if(normalizar==2)
// 	{
//         mpfra_set(aux, result->ejey[0]);
// 		for(i=1; i<N_cajas; i++)
// 		{
//             if(mpfra_cmp(aux, result->ejey[i])<0)
//                 mpfra_set(aux, result->ejey[i]);
//         }			
// 		for(i=0; i<N_cajas; i++)
// 			mpfra_div(result->ejey[i], result->ejey[i], aux);
// 	}//Aqui se normaliza por valor maximo
	
    mpfr_clear(x_min);
    mpfr_clear(x_max);
    mpfr_clear(area);
    mpfr_clear(aux);
    mpfr_clear(aux1);

    return result;
}


DISTRI* construyeHistograma_Sspace(MATRIZ *data, char normalizar, char ordenar)
{
	unsigned long i, j, N, contador;
	double aux, dmax, area;
	MATRIZ* data_ordenado;
	DISTRI* result;

	N = data->N_filas;

	data_ordenado = mallocMatriz(N,1);
	for(i=0; i<N; i++)
		data_ordenado->elemento[i][0] = data->elemento[i][0];

    if(ordenar)
        ordenarData(data_ordenado);
     
	dmax=0;
	for(i=1; i<N; i++)
	{
		aux = data_ordenado->elemento[i][0]-data_ordenado->elemento[i-1][0];
		if(aux>dmax) dmax = aux;
	}//dmax es el incremento maximo: lo utilizaremos para normalizar los valores de la derivada
    
	result = mallocDistribucion(N);
    for(i=0; i<result->N; i++)
        result->ejex[i]=(data_ordenado->elemento[N-1][0]-data_ordenado->elemento[0][0])/(result->N-1)*i+data_ordenado->elemento[0][0];
        
	for(i=0; i<N; i++)
    {
		if(i==0) aux = data_ordenado->elemento[i+1][0]-data_ordenado->elemento[i][0];
		else if(i==N-1) aux = data_ordenado->elemento[i][0] - data_ordenado->elemento[i-1][0];
		else aux = 0.5*(data_ordenado->elemento[i+1][0]-data_ordenado->elemento[i-1][0]);
        
		if(aux < EPS) aux = EPS;//Ojo! con esto: podria afectar al desempeño del programa
        
        aux/=dmax;
		
        result->ejex[i] = data_ordenado->elemento[i][0];
		result->ejey[i] = 1.0/aux;
    }

	if(normalizar)
	{
		area=0;
		for(i=1; i<N; i++)
			area+=result->ejey[i];

		for(i=0; i<N; i++)
			result->ejey[i]/= area; 
	}

	freeMatriz(data_ordenado);

	return result;
}


MPFR_DISTRI* MPFR_construyeHistograma_Sspace(MPFR_MATRIZ *data, int resolucion, char normalizar, char ordenar)
{
	unsigned long i, j, contador, N;
	mpfr_t area, aux;
	MPFR_MATRIZ* data_ordenado;
	MPFR_DISTRI* result;
    
	mpfra_init_set_ui(area, 0);
	mpfra_init_set_ui(aux, 0);

	N = data->N_filas;
    
    result = MPFR_mallocDistribucion(N);

	data_ordenado = MPFR_mallocMatriz(N, 1);
	for(i=0; i<N; i++)
		mpfra_set(data_ordenado->elemento[i][0], data->elemento[i][0]);
    if(ordenar)
        MPFR_ordenarData(data_ordenado);

    
	for(i=0; i<N; i++)
    {
		if(i==0) mpfra_sub(aux,  data_ordenado->elemento[1][0], data_ordenado->elemento[0][0]);
		else if(i==N-1) mpfra_sub(aux, data_ordenado->elemento[i][0], data_ordenado->elemento[i-1][0]);
		else
		{            
			mpfra_sub(aux, data_ordenado->elemento[i+1][0], data_ordenado->elemento[i-1][0]);
			mpfra_div_ui(aux, aux, 2);
		}
		
		if(mpfr_cmp_d(aux, EPS)<0)
            mpfra_set_d(aux, 0.5/pow(10, (double)resolucion));

		mpfra_set(result->ejex[i], data_ordenado->elemento[i][0]);
        mpfra_ui_div(result->ejey[i], 1, aux);
    }

    if(normalizar==1)
	{
		for(i=1; i<N; i++)
		{
			mpfra_sub(aux, result->ejex[i], result->ejex[i-1]);
			mpfra_mul(aux, aux, result->ejey[i]); 
			mpfra_add(area, area, aux);
		}//aqui pueden probarse diversas variaciones del calculo de area (mayores ordenes de precision en la intgral)
			
		for(i=0; i<N; i++)
			mpfra_div(result->ejey[i], result->ejey[i], area);
	}//Aqui se normaliza por area
	
    else if(normalizar==2)
	{
        mpfra_set(aux, result->ejey[0]);
		for(i=1; i<N; i++)
		{
            if(mpfra_cmp(aux, result->ejey[i])<0)
                mpfra_set(aux, result->ejey[i]);
        }			
		for(i=0; i<N; i++)
			mpfra_div(result->ejey[i], result->ejey[i], aux);
	}//Aqui se normaliza por valor maximo

	//liberar memoria
	mpfr_clear(aux);
	mpfr_clear(area);
	MPFR_freeMatriz(data_ordenado);
	

	return result;
}


double calculaEntropiaPlugin(DISTRI *distribucion)
{
    unsigned long i;
    double area, entropia;
    
    area = 0;
    for(i=1; i<distribucion->N; i++)
        area+=(distribucion->ejex[i]-distribucion->ejex[i-1])*distribucion->ejey[i];
    for(i=0; i<distribucion->N-1; i++)
        area+=(distribucion->ejex[i+1]-distribucion->ejex[i])*distribucion->ejey[i];
    area/=2;

    entropia=0;
    for(i=1; i<distribucion->N; i++)
    {
        if(distribucion->ejey[i] < EPS) entropia+=0; //calculo del limite			
        else entropia+=(distribucion->ejex[i]-distribucion->ejex[i-1])*(distribucion->ejey[i]*log2(distribucion->ejey[i]));
    }
    for(i=0; i<distribucion->N-1; i++)
    {
        if(distribucion->ejey[i] < EPS) entropia+=0; //calculo del limite			
        else entropia+=(distribucion->ejex[i+1]-distribucion->ejex[i])*(distribucion->ejey[i]*log2(distribucion->ejey[i]));
    }
    entropia/=2;
        
    entropia = log2(area)-entropia/area;
    
    return entropia;
}

double MPFR_calculaEntropiaPlugin(MPFR_DISTRI* distribucion)
{
    mpfr_t area, entropia, daux;
    int i;
    double result;
    
    mpfra_init_set_ui(daux, 0);
    
    mpfra_init_set_ui(area, 0);
    for(i=0; i<distribucion->N; i++)
        mpfra_add(area, area, distribucion->ejey[i]);
    
    mpfra_init_set_ui(entropia, 0);
    for(i=0; i<distribucion->N; i++)
    {
        if(mpfr_cmp_d(distribucion->ejey[i], EPS)>0) //calculo del limite
        {
            mpfra_log2(daux, distribucion->ejey[i]);
            mpfra_mul(daux, daux, distribucion->ejey[i]);
            mpfra_add(entropia, entropia, daux);
        }
    }
    
    mpfra_log2(daux, area);
    mpfra_div(entropia, entropia, area);
    mpfra_sub(entropia, daux, entropia);
    
    result = mpfra_get_d(entropia);
    
    mpfr_clear(area);
    mpfr_clear(daux);
    mpfr_clear(entropia);
    
    return result;
}


double MPFR_calculaEntropiaGrassberger(MPFR_MATRIZ* data, char ordenar)
{
    long i, j, N, contador, contador1;
    double result;
    mpfr_t aux, aux1, aux2, caja;
    MPFR_MATRIZ* data_ordenado;
    
    mpfra_init_set_ui(aux, 0);
    mpfra_init_set_ui(aux1, 0);
    mpfra_init_set_ui(aux2, 0);
    mpfra_init_set_ui(caja, 0);
    
    N = data->N_filas;
    
	data_ordenado = MPFR_mallocMatriz(N, 1);
	for(i=0; i<N; i++)
		mpfra_set(data_ordenado->elemento[i][0], data->elemento[i][0]);
    if(ordenar)
        MPFR_ordenarData(data_ordenado);

    for(i=1; i<data_ordenado->N_filas;i++)
    {
        mpfra_sub(aux, data_ordenado->elemento[i][0], data_ordenado->elemento[i-1][0]);
        if(mpfr_cmp(caja, aux)>0)
            mpfra_set(caja, aux);
    }
    mpfra_mul_d(caja, caja, 0.9);
    
    contador=0;
    while(contador<N)
    {
        contador1=0;
        for(j=contador; j<N; j++)
        {
            mpfra_sub(aux, data_ordenado->elemento[j][0], data_ordenado->elemento[contador][0]);
            if(mpfr_cmp(aux, caja)>0)
                break;
            else
                contador1++;
        }
        contador+=contador1;
        
        mpfra_set_ui(aux, N);
        mpfra_log(aux, aux);
        mpfra_mul_d(aux, aux, contador1/(1.0*N));
        
        mpfra_set_ui(aux1, contador1);
        mpfra_digamma(aux1, aux1);
        mpfra_mul_d(aux1, aux1, contador1/(1.0*N));
        
        mpfra_set_d(aux2, contador1/(1.0*N)*pow(-1, (double)contador1)/(contador1*(contador1+1)));
        
        mpfra_sub(aux, aux, aux1);
        mpfra_sub(aux, aux, aux2);
        
        result+=mpfra_get_d(aux);
    }
    
    mpfr_clear(aux);
    mpfr_clear(aux1);
    mpfr_clear(aux2);
    MPFR_freeMatriz(data_ordenado);
    
    return result/M_LN2;
}


double MPFR_calculaEntropiaSpacingEstimate(MPFR_MATRIZ* data, int resolucion, char ordenar)
{
	unsigned long i, j, contador, N;
	mpfr_t aux;
	MPFR_MATRIZ* data_ordenado;
	double result;
    mpfr_t entropia;
    
	mpfra_init_set_ui(aux, 0);
    mpfra_init_set_ui(entropia, 0);

	N = data->N_filas;
    
	data_ordenado = MPFR_mallocMatriz(N, 1);
	for(i=0; i<N; i++)
		mpfra_set(data_ordenado->elemento[i][0], data->elemento[i][0]);
    if(ordenar)
        MPFR_ordenarData(data_ordenado);

    result=0;
	for(i=0; i<N-1; i++)
    {
        mpfra_sub(aux, data_ordenado->elemento[i+1][0], data_ordenado->elemento[i][0]);
		
		if(mpfr_cmp_d(aux, EPS)<0)
            mpfra_set_d(aux, 0.5/pow(10, (double)resolucion));
        
        mpfra_mul_ui(aux, aux, N);
        mpfra_log(aux, aux);
        mpfra_add(entropia, entropia, aux);
    }
    
    result = mpfra_get_d(entropia);
    result/=N;
    result+=M_EULER;
    result/=M_LN2;

	//liberar memoria
	mpfr_clear(aux);
    mpfr_clear(entropia);
	MPFR_freeMatriz(data_ordenado);
	
	return result;
}


double calculaMinEntropiaPlugin(DISTRI *distribucion)
{
    unsigned long i;
    double area, min_entropia, ymax;
    
    area = 0;
    ymax=distribucion->ejey[0];
    for(i=1; i<distribucion->N; i++)
    {
        area+=(distribucion->ejex[i]-distribucion->ejex[i-1])*distribucion->ejey[i];
        if(distribucion->ejey[i]>ymax)
            ymax=distribucion->ejey[i];
    }
    for(i=0; i<distribucion->N-1; i++)
        area+=(distribucion->ejex[i+1]-distribucion->ejex[i])*distribucion->ejey[i];
    area/=2;
            
    min_entropia = log2(area)-log2(ymax);
    
    return min_entropia;
}


double MPFR_calculaMinEntropiaPlugin(MPFR_DISTRI *distribucion)
{
    unsigned long i;
    mpfr_t area, ymax, daux, daux1;
    double min_entropia;
    
    mpfra_init_set_ui(area, 0);
	mpfra_init_set(ymax, distribucion->ejey[0]);
    mpfra_init_set_ui(daux, 0);
    mpfra_init_set_ui(daux1, 0);
    
	for(i=1; i<distribucion->N; i++)
	{
		mpfra_sub(daux, distribucion->ejex[i], distribucion->ejex[i-1]);
		mpfra_mul(daux, daux, distribucion->ejey[i]); 
		mpfra_add(area, area, daux);
        
        if(mpfr_cmp(distribucion->ejey[i], ymax)>0)
            mpfra_set(ymax, distribucion->ejey[i]);

	}//integral inferior
	for(i=0; i<distribucion->N-1; i++)
	{
		mpfra_sub(daux, distribucion->ejex[i+1], distribucion->ejex[i]);
		mpfra_mul(daux, daux, distribucion->ejey[i]); 
		mpfra_add(area, area, daux);
	}//integral superior
	mpfra_div_ui(area, area, 2); 
            
    mpfra_log2(daux, area);
    mpfra_log2(daux1, ymax);
    mpfra_sub(daux, daux, daux1);
    
    min_entropia = mpfra_get_d(daux);
    
	mpfr_clear(area);
	mpfr_clear(daux);
	mpfr_clear(daux1);
	mpfr_clear(ymax);
    
    return min_entropia;
}


double MPFR_calculaMinEntropiaGrassberger(MPFR_MATRIZ* data, char ordenar)
{
    long i, j, N, contador, contador1;
    double result=0;
    mpfr_t aux, aux1, aux2, caja;
    MPFR_MATRIZ* data_ordenado;
    
    mpfra_init_set_ui(aux, 0);
    mpfra_init_set_ui(aux1, 0);
    mpfra_init_set_ui(aux2, 0);
    mpfra_init_set_ui(caja, 0);
    
    N = data->N_filas;
    
	data_ordenado = MPFR_mallocMatriz(N, 1);
	for(i=0; i<N; i++)
		mpfra_set(data_ordenado->elemento[i][0], data->elemento[i][0]);
    if(ordenar)
        MPFR_ordenarData(data_ordenado);
    
    for(i=1; i<data_ordenado->N_filas;i++)
    {
        mpfra_sub(aux, data_ordenado->elemento[i][0], data_ordenado->elemento[i-1][0]);
        if(mpfr_cmp(caja, aux)>0)
            mpfra_set(caja, aux);
    }
    mpfra_mul_d(caja, caja, 0.9);
    
    contador=0;
    while(contador<N)
    {
        contador1=0;
        for(j=contador; j<N; j++)
        {
            mpfra_sub(aux, data_ordenado->elemento[j][0], data_ordenado->elemento[contador][0]);
            if(mpfr_cmp(aux, caja)>0)
                break;
            else
                contador1++;
        }
        contador+=contador1;
        
        mpfra_set_ui(aux, N);
        mpfra_log(aux, aux);
        mpfra_mul_d(aux, aux, contador1/(1.0*N));
        
        mpfra_set_ui(aux1, contador1);
        mpfra_digamma(aux1, aux1);
        mpfra_mul_d(aux1, aux1, contador1/(1.0*N));
        
        mpfra_set_d(aux2, contador1/(1.0*N)*pow(-1, (double)contador1)/(contador1*(contador1+1)));
        
        mpfra_sub(aux, aux, aux1);
        mpfra_sub(aux, aux, aux2);
        
        if(mpfra_get_d(aux)>result)
            result=mpfra_get_d(aux);
    }
    
    mpfr_clear(aux);
    mpfr_clear(aux1);
    mpfr_clear(aux2);
    MPFR_freeMatriz(data_ordenado);
    
    return result/M_LN2;
}

double MPFR_calculaMinEntropiaSpacingEstimate(MPFR_MATRIZ* data, int resolucion, char ordenar)
{
	unsigned long i, j, contador, N;
	mpfr_t aux;
	MPFR_MATRIZ* data_ordenado;
	double result;
    mpfr_t entropia;
    
	mpfra_init_set_ui(aux, 0);
    mpfra_init_set_ui(entropia, 0);

	N = data->N_filas;
    
	data_ordenado = MPFR_mallocMatriz(N, 1);
	for(i=0; i<N; i++)
		mpfra_set(data_ordenado->elemento[i][0], data->elemento[i][0]);
    if(ordenar)
        MPFR_ordenarData(data_ordenado);

    result=0;
	for(i=0; i<N-1; i++)
    {
        mpfra_sub(aux, data_ordenado->elemento[i+1][0], data_ordenado->elemento[i][0]);
		
		if(mpfr_cmp_d(aux, EPS)<0)
            mpfra_set_d(aux, 0.5/pow(10, (double)resolucion));
        
        mpfra_mul_ui(aux, aux, N);
        mpfra_log(aux, aux);
        if(mpfra_cmp(aux, entropia)>0)
            mpfra_set(entropia, aux);
    }
    
    result = mpfra_get_d(entropia);
    result/=N;
    result+=M_EULER;
    result/=M_LN2;

	//liberar memoria
	mpfr_clear(aux);
    mpfr_clear(entropia);
	MPFR_freeMatriz(data_ordenado);

	return result;
}


double MPFR_calculaGuesswork(MPFR_DISTRI* distribucion, char ordenar)
{
    /*PENDIENTE*/
}


double minimosCuadrados(DISTRI *func1, DISTRI *func2)
{
    int i;
    double result = 0;

    if(func1->N != func2->N)
    {
        printf("\nError en el calculo de minimos cuadrados: las funciones no tienen igual numero de elementos\n");
        return -1;
    }

    else
    {
        for(i=0;i<func1->N;i++)
            result+=fabs(func1->ejey[i]-func2->ejey[i]);

        return result;
    }
}


double calcular_promedio(double* input, int N)
{
    int i;
    double result=0;
        
    for(i=0; i<N; i++)
        result+=input[i];
    
    result /= N;
    
    return result;
}

double calcular_promedio_varianza(double *input, int N, double* var)
{
    int i;
    double promedio, varianza=0;
    
    promedio = calcular_promedio(input, N);
    
    for(i=0; i<N; i++)
        varianza+=input[i]*input[i];
    
    varianza = varianza/N-promedio*promedio;
    
    *var = varianza;
        
    return promedio;
}

double calcular_promedio_varianza_filtrando(double* input, int N, double factor_varianza, double* var)
{
    int i, N_new, N_old;
    double result=0, promedio, varianza, *input_aux;
    
    N_new = N;
    input_aux = malloc(sizeof(double)*N);
    for(i=0; i<N; i++)
        input_aux[i] = input[i];
    
    while(1)
    {                
        promedio = calcular_promedio_varianza(input_aux, N, &varianza);
        promedio = N*promedio/N_new;
        varianza = (N-1)*varianza/(N_new-1)+N_new*(N_new-N)*promedio*promedio/(N*(N_new-1)); //No se si esta formula es exacta dado que uso el estimador sesgado de la varianza (para que las funciones de correlacion salgan bien)
        
        N_old = N_new;
        N_new = 0;
        for(i=0; i<N; i++)
        {
            if(fabs(input_aux[i]-promedio)<factor_varianza*sqrt(varianza))
                N_new++;
            else
                input_aux[i]=0;
        }
        if(N_new==N_old) break;
    }
    free(input_aux);
    
    *var = varianza;
    return promedio;
}


double FreedmanDiaconis(MATRIZ* data, char ordenar)
{
    int i, j, N;
    double Q1, Q3, cubic, result, aux;
    MATRIZ* data_ordenado;
    
    N=data->N_filas;
    cubic=pow((double)N, 1.0/3.0);
    
    if(ordenar)
    {
        data_ordenado = mallocMatriz(data->N_filas, 1);
        copiarMatriz(data_ordenado, data);
        ordenarData(data_ordenado);
        
        Q1=0;
        Q3=0;
        for(i=0; i<N/2; i++)
        {
            Q1+=data_ordenado->elemento[i][0];
            Q3+=data_ordenado->elemento[N-1-i][0];
        }
        Q1/=(double)(N/2);
        Q3/=(double)(N/2);
        
        freeMatriz(data_ordenado);
        
        result=2*(Q3-Q1)/cubic;
    }
    else
    {
        Q1=0;
        Q3=0;
        for(i=0; i<N/2; i++)
        {
            Q1+=data->elemento[i][0];
            Q3+=data->elemento[N-1-i][0];
        }
        Q1/=(double)(N/2);
        Q3/=(double)(N/2);
        
        result=2*(Q3-Q1)/cubic;
    }
    
    return result;
}


void MPFR_FreedmanDiaconis(mpfr_t *result, MPFR_MATRIZ* data, char ordenar)
{
    unsigned long i, j, N;
    mpfr_t Q1, Q3, cubic;
    MPFR_MATRIZ* data_ordenado;
    
    mpfra_init_set_ui(Q1, 0);
    mpfra_init_set_ui(Q3, 0);
    mpfra_init_set_ui(cubic, (unsigned long)data->N_filas);
    
    N=data->N_filas;
    mpfra_cbrt(cubic, cubic);
 
    if(ordenar)
    {
        data_ordenado = MPFR_mallocMatriz(data->N_filas, 1);
        MPFR_copiarMatriz(data_ordenado, data);
        MPFR_ordenarData(data_ordenado);
        
        for(i=0; i<N/2; i++)
        {
            mpfra_add(Q1, Q1, data_ordenado->elemento[i][0]);
            mpfra_add(Q3, Q3, data_ordenado->elemento[N-1-i][0]);
        }
        mpfra_div_ui(Q1, Q1, (N/2));
        mpfra_div_ui(Q3, Q3, (N/2));
        
        mpfra_sub(*result, Q3, Q1);
        mpfra_mul_ui(*result, *result, 2);
        mpfra_div(*result, *result, cubic);
        
        MPFR_freeMatriz(data_ordenado);
    }
    
    else
    {
        for(i=0; i<N/2; i++)
        {
            mpfra_add(Q1, Q1, data->elemento[i][0]);
            mpfra_add(Q3, Q3, data->elemento[N-1-i][0]);
        }
        mpfra_div_ui(Q1, Q1, (N/2));
        mpfra_div_ui(Q3, Q3, (N/2));
        
        mpfra_sub(*result, Q3, Q1);
        mpfra_mul_ui(*result, *result, 2);
        mpfra_div(*result, *result, cubic);
    }
    
    mpfr_clear(Q1);
    mpfr_clear(Q3);
    mpfr_clear(cubic);
}


MATRIZ* matrizCovarianza(MATRIZ* input)
{
    ///esta funcion calcula la matriz de covarianza de una 'trayectoria'. La trayectoria se introduce en forma de una matriz, donde cada columna representa una coordenada generalizada y cada fila un instante temporal distinto
    int i, j, k, N_frames, N_coord;
    double E_x, E_y, E_xy;
    MATRIZ* result;
    
    N_frames = input->N_filas;
    N_coord = input->N_columnas;
    
    result = mallocMatriz(N_coord, N_coord);
    
    for(i=0; i<N_coord; i++)
    {
        for(j=0; j<N_coord; j++)
        {
            E_x=0;
            E_y=0;
            E_xy=0;
            for(k=0; k<N_frames; k++)
            {
                E_x+=input->elemento[k][i];
                E_y+=input->elemento[k][j];
                E_xy+=input->elemento[k][i]*input->elemento[k][j];
            }
            E_x/=N_frames;
            E_y/=N_frames;
            E_xy/=N_frames;
            
            result->elemento[i][j]=E_xy-(E_x*E_y);
        }
    }
    
    return result;
}


//combinatoria
double combinatorio(int arriba, int abajo)
{
    int i, j, *productos, *divisores;
    double result;
    char permiso;


    productos = (int*)malloc(sizeof(int)*arriba);
    for(i=0;i<arriba;i++)
        productos[i]=i+1;

    divisores = (int*)malloc(sizeof(int)*arriba);
    for(i=0;i<abajo;i++)
        divisores[i]=i+1;
    for(i=0;i<arriba-abajo;i++)
        divisores[i+abajo]=i+1;


    for(i=0;i<arriba;i++)
    {
        for(j=0;j<arriba;j++)
        {
            if(productos[i] == divisores[j])
            {
                productos[i] = 1;

                divisores[j] = 1;
            }
        }
    }

    result = 1.0;
    for(i=0;i<arriba;i++)
        result*= (double)productos[i];

    for(i=0;i<arriba;i++)
        result/= (double)divisores[i];

    free(productos);
    free(divisores);

    return result;
}

void escriboTablaPerm(int N_osciladores, int N_retos, int verbose)
{
    int i,j,k,l,t;
    int** M_perm;
    int** columna_prohibida;
    int *result;
    FILE* punte;

    punte=fopen("tab_retos.txt","w");

    M_perm=(int**)malloc(sizeof(int*)*N_osciladores);
    for(i=0;i<N_osciladores;i++)
        M_perm[i]=(int*)malloc(sizeof(int)*N_osciladores);

    columna_prohibida=(int**)malloc(sizeof(int*)*N_osciladores);
    for(i=0;i<N_osciladores;i++)
        columna_prohibida[i]=(int*)malloc(sizeof(int)*(N_osciladores-i));

    result=(int*)malloc(sizeof(int)*N_osciladores);

    for(t=0;t<N_retos;t++)
    {
        for(i=0;i<N_osciladores;i++)
            columna_prohibida[0][i]=i;

        for(i=0;i<N_osciladores;i++)
        {
            for(j=0;j<N_osciladores;j++)
                M_perm[i][j]=0;
        }

        for(i=0;i<N_osciladores;i++)
        {
            j=dado(N_osciladores-i);
            k=columna_prohibida[i][j];
            M_perm[i][k]=1;

            if(i<N_osciladores-1)
            {
                for(l=0;l<j;l++)
                    columna_prohibida[i+1][l]=columna_prohibida[i][l];
                for(l=j+1;l<N_osciladores-i;l++)
                    columna_prohibida[i+1][l-1]=columna_prohibida[i][l];
            }
        }

        for(i=0;i<N_osciladores;i++)
        {
            result[i]=0;
            for(j=0;j<N_osciladores;j++)
                result[i]+=M_perm[i][j]*j;
        }

        for(i=0;i<N_osciladores;i++)
            fprintf(punte,"%d ",result[i]);
        fprintf(punte,"\n");

        if(verbose) percentage(t,N_retos,"\n\nCalculando espacio de retos...");
    }
    fclose(punte);
}

double factorial(int N)
{
    int i;
    double result = 0;

    for(i=1;i<=N;i++)
        result+=log((double)i);
    result = exp(result);

    return result;
}



///calculo tensorial
long tensorIndice(TENSOR* T, long* index_array) //index_array contiene los valores (0, Ndim-1) de cada indice
{
	long i, result=0;
	
	for(i=0; i<T->Nindex; i++)
		result+=(long)(pow(T->Ndim,i)*index_array[i]);
	
	return result;
}

void tensorRevIndice(long* index_array, long index, TENSOR*T) //esta funcion convierte 'index' en su correspondiente arreglo de indices y lo mete en 'index_array'
{
	long i, aux;
	
	aux = index;
	
	for(i=0; i<T->Nindex; i++)
	{
		index_array[i]=aux%T->Ndim;
		aux = aux/T->Ndim;
	}
}

void tensorSet(TENSOR* T, long* index, double new)
{		
	T->elemento[tensorIndice(T, index)]=new;
}

double tensorGet(TENSOR* T, long* index)
{	
	return T->elemento[tensorIndice(T, index)];
}

void tensorFila(gsl_vector* result, TENSOR *T, long* index_fijo, long index_var)
{
	long i, *index;
	double aux;
	
	index = malloc(sizeof(long)*(T->Nindex));
	
	for(i=0; i<index_var; i++)
		index[i]=index_fijo[i];
		
	for(i=index_var+1; i<T->Nindex; i++)
		index[i]=index_fijo[i];
		
	for(i=0; i<T->Ndim; i++)
	{
		index[index_var]=i;
		gsl_vector_set(result, i, tensorGet(T, index));
	}
	
	free(index);
}

TENSOR* productoTensorial(TENSOR* A, TENSOR* B) //esta funcion devuelve el producto tensorial AxB
{
	long i, j, k, *index_array_A, *index_array_B, *index_array_result;
	double aux;
	TENSOR* result;
	
	result = mallocTensor(A->Nup+B->Nup, A->Ndown+B->Ndown, A->Ndim);
	
	index_array_A=(long*)malloc(sizeof(long)*A->Nindex);
	index_array_B=(long*)malloc(sizeof(long)*B->Nindex);
	index_array_result=(long*)malloc(sizeof(long)*result->Nindex);
	
	k=0;
	for(i=0; i<A->largest_index; i++)
	{
		tensorRevIndice(index_array_A, i, A);
		for(j=0; j<B->largest_index; j++)
		{
			tensorRevIndice(index_array_B, j, B);
			
			aux = tensorGet(A, index_array_A)*tensorGet(B, index_array_B);
			
			tensorRevIndice(index_array_result, k, result);
			
			tensorSet(result, index_array_result, aux);
			
			k++;
		}
	}
	
	free(index_array_A);
	free(index_array_B);
	free(index_array_result);
	
	return result;
}

TENSOR* autoContraerIndicesTensor(TENSOR *A, long indexA_up, long indexA_down)
{
	long i, j, *index_result, *index_A;
	double aux=0;
	TENSOR* result;
	
	result = mallocTensor(A->Nup-1, A->Ndown-1, A->Ndim);
	
	index_A = malloc(sizeof(long)*A->Nindex);
	index_result = malloc(sizeof(long)*result->Nindex);
	
	for(i=0; i<result->largest_index; i++)
	{
		tensorRevIndice(index_result, i, result);
		
		for(j=0; j<indexA_up; j++)
			index_A[j]=index_result[j];
		
		for(j=indexA_up+1; j<indexA_down; j++)
			index_A[j]=index_result[j-1];
		
		for(j=indexA_down+1; j<A->Nindex; j++)
			index_A[j]=index_result[j-2];
		
		
		aux=0;
		for(j=0; j<result->Ndim; j++)
		{
			index_A[indexA_up]=j;
			index_A[indexA_down]=j;
			aux+=tensorGet(A, index_A);
		}
		
		tensorSet(result, index_result, aux);
	}
	
	free(index_A);
	free(index_result);
	
	return result;
}

TENSOR* contraerIndicesTensor(TENSOR* A, TENSOR* B, long indexA, long indexB)
{
	TENSOR* AB;
	TENSOR* result;
	
	AB = productoTensorial(A, B);
	
	result = autoContraerIndicesTensor(AB, indexA, AB->Nindex-B->Ndown+indexB);
	
	freeTensor(AB);
	
	return result;
	
	
}
