/*
 * NOTAS:   . Las funciones MPFR_free() no funcionan muy bien
 * */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <omp.h>
#include <gmp.h>
#include <mpfr.h>
#include "alias/mpfr.h" //redefinicion de las funciones mpfr
#include "alias/gsl.h"
 

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

#define M_TAU 2*M_PI //TAU = 2*PI

#define MTZ_INDEX(m, i, j)  i*m->N_columnas+j   
#define MTZ_FIL_INDEX(m, n) n/m->N_columnas
#define MTZ_COL_INDEX(m, n) n%m->N_columnas

#define fseekset(x, y)  fseek(x, y, SEEK_SET)



gsl_rng* idum;
int percent;
int indexec; //esta variable contiene el indice de ejecucion del programa (i.e. el indice de la iteracion, en caso de que se este llamando iterativamente). Para que un programa deje traza de su indice y por tanto pueda ser recogida por una iteracion posterior se utiliza el flag especial '!'

void beforeMain(void) __attribute__((constructor));
void beforeMain(void)
{
    FILE* punte;
    int aux=0;
    
    punte=fopen(".indexec", "r");
    if(punte)
    {
        fscanf(punte, "%d", &aux);
        fclose(punte);
    }
    
    indexec=aux+1;
    
    idum = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(idum, (unsigned long) time(NULL)+indexec); //precision de SEGUNDOS!
        
}//en esta funcion se ejecutan las inicializaciones globales previas a 'main'. OJO! Esto SOLO FUNCIONA CON EL COMPILADOR gcc!


/*ESTRUCTURAS*/

typedef struct MATRIZ_STRUCT
{
    double** elemento;
    unsigned long N_filas;
    unsigned long N_columnas;

} MATRIZ;

typedef gsl_matrix GSL_MATRIZ;

typedef struct MPFR_MATRIZ_STRUCT
{
	mpfr_t** elemento;
	unsigned long N_filas;
	unsigned long N_columnas;

} MPFR_MATRIZ;

typedef struct TABLA_STRUCT
{
    int N_filas;
    int N_columnas;
    char** encabezado;
    double** data;
} TABLA;

typedef struct DISTRIBUCION_STRUCT
{
    unsigned long N;
    double* ejex;
    double* ejey;

    char string[256];

} DISTRI;

typedef struct MPFR_DISTRIBUCION_STRUCT
{
    unsigned long N;
    mpfr_t* ejex;
    mpfr_t* ejey;

} MPFR_DISTRI;

typedef struct SDISTRIBUCION_STRUCT
{
    unsigned long N, M;
    double* ejex;
    double* ejey;
    double** ejez;
    
    char string[256];
    
} SDISTRI;



/*ENCABEZADOS*/

//gestion de memoria
MATRIZ*         mallocMatriz(unsigned long N_filas, unsigned long N_columnas);

MPFR_MATRIZ*    MPFR_mallocMatriz(unsigned long N_filas, unsigned long N_columnas);

TABLA*          mallocTabla(unsigned long N_filas, unsigned long N_columnas);

DISTRI*         mallocDistribucion(unsigned long N);

MPFR_DISTRI*    MPFR_mallocDistribucion(unsigned long N);

SDISTRI*        mallocSDistribucion(unsigned long N, unsigned long M);

void            freeMatriz(MATRIZ* A);

void 	        MPFR_freeMatriz(MPFR_MATRIZ* A);

void            freeTabla(TABLA* A);

void            freeDistribucion(DISTRI* result);

void	        MPFR_freeDistribucion(MPFR_DISTRI* A);

void            freeSDistribucion(SDISTRI* result);


//auxiliares
double   dividirLimite(double a, double b, double epsilon);

int      compararFloat(double a, double b ,double epsilon);

int      compararDouble(double *a, double *b);

void     bajarLinea(FILE* punte); //mueve el puntero del stream abierto 'punte' al proximo salto de linea

char     pasarColumna(FILE* punte); //mueve el puntero del stream abierto 'punte' a la proxima columna. Si termina con exito devuelve 1, en otro caso devuelve 0

char     buscarPalabra(FILE* punte, char *palabra);

void     encontrarUltimaLinea(FILE* punte); // mueve el puntero del 'stream' abierto a la ultima linea (con texto) del fichero

void     percentage(int index, int N_total, char frase[256]);

double   ran1(); //distribucion plana (0, 1)

double   ran1_gauss(double mu, double sigma); //distribucion gaussiana de media 'mu' y desviacion 'sigma'

double   ran1_lineal(double parametro); //distribucion linealmente creciente entre (0.1). "parametro" mide la inclinacion de la distribucion, su valor debe ser (0,1]

unsigned long dado(unsigned long N_caras); //produce un numero aleatorio entre [0, N_caras-1]

int     quickSort_particion(MATRIZ* data, int low, int high);

void    quickSort(MATRIZ* data, int low, int high);

int     quickSort_particion_distribucion(DISTRI* data, int low, int high);

void    quickSort_distribucion(DISTRI* data, int low, int high);

void    ordenarData(MATRIZ* data); //ordena el vector columna 'data' de menor a mayor.

void    GSL_ordenarData(MATRIZ* data); //ordena el vector columna 'data' de menor a mayor.

void    ordenarData_distribucion(DISTRI* data);

void    GSL_ordenarData_distribucion(DISTRI* data);

int     MPFR_quickSort_particion(MPFR_MATRIZ* data, int low, int high);

void    MPFR_quickSort(MPFR_MATRIZ* data, int low, int high);

void    MPFR_ordenarData(MPFR_MATRIZ* data);//ordena el vector 'data' de menor a mayor usando el algoritmo quickSort

void    MPFR_GSL_ordenarData(MPFR_MATRIZ* data); 

int     MPFR_quickSort_particion_distribucion(MPFR_DISTRI* data, int low, int high);

void    MPFR_quickSort_distribucion(MPFR_DISTRI* data, int low, int high);//esta funcion ordena la distribucion 'data' dirigida por la columna 'ejey'

void    MPFR_ordenarData_distribucion(MPFR_DISTRI* data);

void    MPFR_GSL_ordenarData_distribucion(MPFR_DISTRI* data);

void    swap(double *x, double *y);

void    delay(double seconds);

char    wordIsInSet(char* word, char* diccionario);


//lectura
MATRIZ*         leerMatriz(char* file_name);

MPFR_MATRIZ*    MPFR_leerMatriz(char* file_name);

TABLA*          leerTabla(char* file_name);

gsl_matrix*     GSL_leerMatriz(FILE* punte);

MPFR_DISTRI*    MPFR_leerDistribucion(char* file_name, char encabezado);

void            leerParametrosTabla(long* N_filas_out, long* N_columnas_out, char* input, char encabezado);


//escritura
void    pintarMatriz(MATRIZ *A, FILE* punte);

void    GSL_pintarMatriz(gsl_matrix *A, FILE* punte);

void    pintarTabla(TABLA* tabla, char* fname);

void    pintarDistribucion(DISTRI *blanco, char* output);

void    MPFR_pintarDistribucion(MPFR_DISTRI* blanco, char* output);//pinta la distribucion. Si 'invertir' pinta la imagen especular de la distribucion

void    animacionGnuplot(char* input, int N_frames, int N_dim, int N_repeticiones);



/*FUNCIONES*/

//gestion de memoria
MATRIZ* mallocMatriz(unsigned long N_filas, unsigned long N_columnas)
{
    MATRIZ* result;
    unsigned long i;

    result = (MATRIZ*)malloc(sizeof(MATRIZ));
    
    result->N_filas = N_filas;
    result->N_columnas = N_columnas;
    result->elemento = (double**)malloc(sizeof(double*)*N_filas);
    for(i=0;i<N_filas;i++)
        result->elemento[i] = calloc(N_columnas, sizeof(double));

    return result;
}

MPFR_MATRIZ* MPFR_mallocMatriz(unsigned long N_filas, unsigned long N_columnas)
{
	unsigned long i, j;
	MPFR_MATRIZ* result;

	result = (MPFR_MATRIZ*)malloc(sizeof(MPFR_MATRIZ));

	result->N_filas = N_filas;
	result->N_columnas = N_columnas;
	result->elemento = (mpfr_t**)malloc(sizeof(mpfr_t*)*N_filas);
	for(i=0; i<N_filas; i++)
	{
		result->elemento[i] = (mpfr_t*)malloc(sizeof(mpfr_t)*N_columnas);
		for(j=0; j<N_columnas; j++)
			mpfra_init_set_ui(result->elemento[i][j], 0);
	}	
	
	return result;
}

TABLA* mallocTabla(unsigned long N_filas, unsigned long N_columnas)
{
    int i;
    TABLA* result;
    
    result = (TABLA*)malloc(sizeof(TABLA));
    
    result->N_filas = N_filas;
    
    result->N_columnas = N_columnas;
    
    result->encabezado = (char**)malloc(sizeof(char*)*N_columnas);
    for(i=0; i<N_columnas; i++)
        result->encabezado[i] = (char*)malloc(sizeof(char)*64);
    
    result->data = (double**)malloc(sizeof(double*)*(N_filas-1));
    for(i=0; i<N_filas-1; i++)
        result->data[i] = (double*)malloc(sizeof(double)*N_columnas);
    
    return result;
}

DISTRI* mallocDistribucion(unsigned long N)
{
    unsigned long i;
    DISTRI* result;

    result = (DISTRI*)malloc(sizeof(DISTRI));

    result ->N = N;
    result->ejex = calloc(N, sizeof(double));
    result->ejey = calloc(N, sizeof(double));
    if(result->ejex == NULL || result->ejey == NULL)
        printf("\nERROR: Tamano del heap excedido al alojar memoria en 'Allocate_distribucion'.\nTratando de alojar %.1lf MB\n",sizeof(double)*N/1000000.0);

    return result;
}

MPFR_DISTRI* MPFR_mallocDistribucion(unsigned long N)
{
    unsigned long i;
    MPFR_DISTRI* result;

    result = (MPFR_DISTRI*)malloc(sizeof(MPFR_DISTRI));

    result ->N = N;
    result->ejex = (mpfr_t*)malloc(sizeof(mpfr_t)*N);
    result->ejey = (mpfr_t*)malloc(sizeof(mpfr_t)*N);

    for(i=0;i<N;i++)
	{
		mpfra_init_set_ui(result->ejex[i], 0);
        
		mpfra_init_set_ui(result->ejey[i], 0); 
	}
	
    return result;
}

SDISTRI* mallocSDistribucion(unsigned long N, unsigned long M)
{
    unsigned long i, j;
    SDISTRI *result;
    
    result = (SDISTRI*)malloc(sizeof(SDISTRI));
    
    result->N = N;
    result->M = M;
    
    result->ejex = (double*)calloc(N, sizeof(double));
    result->ejey = (double*)calloc(M, sizeof(double));
    
    result->ejez = (double**)malloc(sizeof(double*)*N);
    for(i=0; i<N; i++)
        result->ejez[i] = (double*)calloc(M, sizeof(double));
    
    if(result->ejez == NULL)
        printf("\nERROR: Tamano del heap excedido al alojar memoria en 'Allocate_distribucion'.\nTratando de alojar %.1lf MB\n",sizeof(double)*N*M/1000000.0);
    
    return result;
}

void freeMatriz(MATRIZ* A)
{
    int i;

    for(i=0;i<A->N_filas;i++)
        free(A->elemento[i]);
    free(A->elemento);
    free(A);
}

void MPFR_freeMatriz(MPFR_MATRIZ* A)
{
	int i, j;
	for(i=0; i<A->N_filas; i++)
	{
		for(j=0; j<A->N_columnas; j++)
			mpfr_clear(A->elemento[i][j]);
	}
	for(i=0; i<A->N_filas; i++)
		free(A->elemento[i]);
	free(A->elemento);
	free(A);
}

void freeTabla(TABLA* A)
{
    int i;
    
    for(i=0; i<A->N_columnas; i++)
        free(A->encabezado[i]);
    free(A->encabezado);
    
    for(i=0; i<A->N_filas-1; i++)
        free(A->data[i]);
    free(A->data);
            
    free(A);
}

void freeDistribucion(DISTRI* result)
{
    free(result->ejex);
    free(result->ejey);
    free(result);
}

void MPFR_freeDistribucion(MPFR_DISTRI* A)
{
	int i;
	for(i=0; i<A->N; i++)
	{
		mpfr_clear(A->ejex[i]);
		mpfr_clear(A->ejey[i]);
	}
	free(A->ejex);
	free(A->ejey);
	free(A);

}

void freeSDistribucion(SDISTRI* result)
{
    int i;
    
    for(i=0; i<result->N; i++)
        free(result->ejez[i]);
    
    free(result->ejez);
    free(result->ejey);
    free(result->ejex);
    free(result);
}


//auxiliares
double dividirLimite(double a, double b, double epsilon)
{
    double izquierda, derecha;

    izquierda = (a-epsilon)/(b-epsilon);
    derecha = (a+epsilon)/(b+epsilon);

    if(compararFloat(izquierda, derecha, epsilon)!=0)
    {
        printf("\nERROR: division entre '0'\n");
        return 1/epsilon;
    }

    if(fabs(a)<EPS && fabs(b)<EPS) return 1;
    else return a/b;

    return 0.5*(izquierda+derecha);
}


int compararFloat(double a, double b ,double epsilon)
{
    if(fabs(a-b)<epsilon)
        return 0;
    else return 1;
}

int compararDouble(double *a, double *b)
{
    if(*a > *b)
        return 1;
    else if(*a < *b)
        return -1;
    else
        return 0;
}


void bajarLinea(FILE* punte)
{
    while(fgetc(punte)!='\n'){}
}


char pasarColumna(FILE* punte)
{
    char basura[256];
    int puntero, puntero1;
    
    puntero = ftell(punte);
    
    bajarLinea(punte);
    
    puntero1=ftell(punte);
    
    fseekset(punte, puntero);
    
    fscanf(punte, "%s", basura);
    
    if(ftell(punte)>=puntero1)
    {
        fseekset(punte, puntero);
        
        return 0;
    }
    else
        return 1;
}


char buscarPalabra(FILE* punte, char *palabra)
{
    //Esta funcion busca la palabra 'palabra' en el stream abierto por 'punte', desde la posicion actual en adelante. Si tiene exito (i.e., encuentra la palabra), la funcion termina con '1' y el puntero del archivo se mueve a la posicion inmediatamente posterior a la palabra 'palabra'. Si por el contrario la funcion falla (i.e., no encuentra la palabra), la funcion termina con '0' y devuelve el puntero del archivo al valor que tenia al entrar en la funcion.
    
    char string[256];
    long puntero = ftell(punte);

    fscanf(punte, "%s", string);
    while(strcmp(string, palabra)!=0)
    {
        fscanf(punte, "%s", string);
        if(feof(punte))
        {
            fseek(punte, puntero, SEEK_SET);
            return 0;
        }
    }
    return 1;
}


void encontrarUltimaLinea(FILE* punte)
{
    fseek(punte, -2, SEEK_END);
    for(int i=3; i>=0; i++)
    {
        if(fgetc(punte)=='\n')
        {
            if(fgetc(punte)!='\n') // estamos al comienzo de la primera fila empezando por el final
                break; 
        }
        else
            fseek(punte, -i, SEEK_END);
    }
}


void percentage(int index, int N_total, char frase[256])
{
    int aux = N_total/100, i;
    if(percent==0)
    {
        printf("%s\n", frase);

        printf("\r |");
        for(i=0;i<100;i++)
            printf(".");
        printf("|\t0%%");
        fflush(stdout);

        percent++;
    }

    if(index>=percent*aux && index-1<percent*aux)
    {
        printf("\r |");
        for(i=0;i<(100*index)/N_total;i++)
            printf("#");

        for(i=(100*index)/N_total; i<100;i++)
            printf(".");

        printf("|\t%.0f%%",(100.0*index)/N_total);
        fflush(stdout);

        percent++;
    }

    if(index==N_total-1)
    {
        printf("\r |");
        for(i=0;i<100;i++)
            printf("#");

        printf("|\t100%%");
        fflush(stdout);

        printf("\n");

        percent=0;
    }
}

double ran1()
{
    return gsl_rng_uniform(idum);
}


double ran1_gauss(double mu, double sigma)
{	
	return gsl_ran_gaussian(idum, sigma)+mu;
}


double   ran1_lineal(double parametro)
{
    double result;
    double x;
    
    x = ran1();
    
    if(parametro<EPS) result = x;
    
    else result = (parametro-1)/(2*parametro) + sqrt(((1-parametro)*(1-parametro))/(4*parametro*parametro) + x/parametro);
    
    return result;
}


unsigned long dado(unsigned long N_caras)
{
    return gsl_rng_uniform_int(idum, N_caras);
}


void swap(double *x, double *y)
{
    double temp;
    temp = *x;
    *x = *y;
    *y = temp;
}


int quickSort_particion(MATRIZ* data, int low, int high)
{
    int i, j;
    double aux;
    
    i=low-1;
    for(j=low; j<high; j++)
    {
        if(data->elemento[j][0]<data->elemento[high][0])
        {
            i++;
            swap(&(data->elemento[i][0]), &(data->elemento[j][0]));
        }
    }
    swap(&(data->elemento[i+1][0]), &(data->elemento[high][0]));
    
    return (i+1);
}

void quickSort(MATRIZ* data, int low, int high)
{
    int p;
    
    if(low<high)
    {
        p = quickSort_particion(data, low, high);
        
        quickSort(data, low, p-1);
        quickSort(data, p+1, high);
    }
}


void ordenarData(MATRIZ* data)
{
    quickSort(data, 0, data->N_filas-1);
}


void GSL_ordenarData(MATRIZ* data)
{
    long i;
    double* p;
    
    p = malloc(sizeof(double)*data->N_filas);
    for(i=0; i<data->N_filas; i++)
        p[i]=data->elemento[i][0];
    
    gsl_heapsort(p, data->N_filas, sizeof(p[0]), (gsl_comparison_fn_t)compararDouble);
    
    for(i=0; i<data->N_filas; i++)
        data->elemento[i][0] = p[i];
    
    free(p);
}


int quickSort_particion_distribucion(DISTRI* data, int low, int high)
{
    int i, j;
    
    i=low-1;
    for(j=low; j<high; j++)
    {
        if(data->ejey[j]<data->ejey[high])
        {
            i++;
            swap(&(data->ejey[i]), &(data->ejey[j]));
            swap(&(data->ejex[i]), &(data->ejex[j]));
        }
    }
    swap(&(data->ejey[i+1]), &(data->ejey[high]));
    swap(&(data->ejex[i+1]), &(data->ejex[high]));
    
    return (i+1);
}

void quickSort_distribucion(DISTRI* data, int low, int high)
{
    int p;
    
    if(low<high)
    {
        p = quickSort_particion_distribucion(data, low, high);
        
        quickSort_distribucion(data, low, p-1);
        quickSort_distribucion(data, p+1, high);
    }
}


void ordenarData_distribucion(DISTRI* data)
{
    quickSort_distribucion(data, 0, data->N-1);
}


void GSL_ordenarData_distribucion(DISTRI* data)
{
    long i;
    DISTRI* copia;
    size_t* p;
    
    copia = mallocDistribucion(data->N);
    for(i=0; i<data->N; i++)
    {
        copia->ejex[i] = data->ejex[i];
        copia->ejey[i] = data->ejey[i];
    }
    
    p = malloc(sizeof(size_t)*data->N);
    
    gsl_heapsort_index(p, copia->ejey, data->N, sizeof(data->ejey[0]), (gsl_comparison_fn_t)compararDouble);
    
    for(i=0; i<data->N; i++)
    {
        data->ejex[i]=copia->ejex[p[i]];
        data->ejey[i]=copia->ejey[p[i]];
    }
    
    freeDistribucion(copia);
    free(p);
}


int MPFR_quickSort_particion(MPFR_MATRIZ* data, int low, int high)
{
    int i, j;
    
    i=low-1;
    for(j=low; j<high; j++)
    {
        if(mpfr_less_p(data->elemento[j][0], data->elemento[high][0]))
        {
            i++;
            mpfr_swap(data->elemento[i][0], data->elemento[j][0]);
        }
    }
    mpfr_swap(data->elemento[i+1][0], data->elemento[high][0]);
    
    return (i+1);
}

void MPFR_quickSort(MPFR_MATRIZ* data, int low, int high)
{
    int p;
    
    if(low<high)
    {
        p = MPFR_quickSort_particion(data, low, high);
        
        MPFR_quickSort(data, low, p-1);
        MPFR_quickSort(data, p+1, high);
    }
}


void MPFR_ordenarData(MPFR_MATRIZ* data)
{
    MPFR_quickSort(data, 0, data->N_filas-1);
}


void MPFR_GSL_ordenarData(MPFR_MATRIZ* data)
{
    long i;
    mpfr_t* p;
    
    p = malloc(sizeof(mpfr_t)*data->N_filas);
    for(i=0; i<data->N_filas; i++)
        mpfra_init_set(p[i], data->elemento[i][0]);
    
    gsl_heapsort(p, data->N_filas, sizeof(p[0]), (gsl_comparison_fn_t)mpfr_cmp);
    
    for(i=0; i<data->N_filas; i++)
        mpfra_set(data->elemento[i][0], p[i]);
    
    for(i=0; i<data->N_filas; i++)
        mpfr_clear(p[i]);
    free(p);
}


int MPFR_quickSort_particion_distribucion(MPFR_DISTRI* data, int low, int high)
{
    int i, j;
    
    i=low-1;
    for(j=low; j<high; j++)
    {
        if(mpfr_less_p(data->ejey[j], data->ejey[high]))
        {
            i++;
            mpfr_swap(data->ejey[i], data->ejey[j]);
            mpfr_swap(data->ejex[i], data->ejex[j]);
        }
    }
    mpfr_swap(data->ejey[i+1], data->ejey[high]);
    mpfr_swap(data->ejex[i+1], data->ejex[high]);
    
    return (i+1);
}


void MPFR_quickSort_distribucion(MPFR_DISTRI* data, int low, int high)
{
    int p;
    
    if(low<high)
    {
        p = MPFR_quickSort_particion_distribucion(data, low, high);
        
        MPFR_quickSort_distribucion(data, low, p-1);
        MPFR_quickSort_distribucion(data, p+1, high);
    }
}


void MPFR_ordenarData_distribucion(MPFR_DISTRI* data)
{
    MPFR_quickSort_distribucion(data, 0, data->N-1);
}


void MPFR_GSL_ordenarData_distribucion(MPFR_DISTRI* data)
{
    long i;
    MPFR_DISTRI* copia;
    size_t* p;
    
    copia = MPFR_mallocDistribucion(data->N);
    for(i=0; i<data->N; i++)
    {
        mpfra_set(copia->ejex[i], data->ejex[i]);
        mpfra_set(copia->ejey[i], data->ejey[i]);
    }
    
    p = malloc(sizeof(size_t)*data->N);
    
    gsl_heapsort_index(p, copia->ejey, data->N, sizeof(data->ejey[0]), (gsl_comparison_fn_t)mpfr_cmp);
    
    for(i=0; i<data->N; i++)
    {
        mpfra_set(data->ejex[i], copia->ejex[p[i]]);
        mpfra_set(data->ejey[i], copia->ejey[p[i]]);
    }
    
    MPFR_freeDistribucion(copia);
    free(p);
}


void delay(double seconds)
{
	///Esta funcion crea un retardo medido en segundos (al modo de la funcion 'delay' de arduino)
	clock_t start;
	
	start = clock();
	while((double)(clock()-start)<seconds*CLOCKS_PER_SEC)
	{}
}

char wordIsInSet(char* word, char* diccionario)
{
    char aux[256];
    int n=0, i=0;

    while(diccionario[i]!=0)
    {
        sscanf(diccionario, "%s%n", aux, &n);
        diccionario+=n;
        i++;
        if(strcmp(aux, word)==0)
            return 1;
    }
    
    return 0;
}


//lectura
MATRIZ* leerMatriz(char* file_name)
{
    ///Esta funcion lee un archivo .mtz
    
    int n, m, i, j;
    MATRIZ* result;
    FILE* punte;
    
    punte = fopen(file_name, "r");
    
    buscarPalabra(punte, "#[N_filas]");
    fscanf(punte, "%d", &n);

    buscarPalabra(punte, "#[N_columnas]");
    fscanf(punte, "%d", &m);

    result = mallocMatriz(n, m);

    buscarPalabra(punte, "#[matriz]");
    
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
            fscanf(punte, "%lg", &(result->elemento[i][j]));
    }
    
    fclose(punte);
    
    return result;
}

MPFR_MATRIZ* MPFR_leerMatriz(char* file_name)
{
    int i, j, n, m;
    MPFR_MATRIZ* result;
    char basura[256];
    FILE* punte;
    
    punte = fopen(file_name, "r");
    buscarPalabra(punte, "#[N_filas]");
    fscanf(punte, "%d", &n);

    buscarPalabra(punte, "#[N_columnas]");
    fscanf(punte, "%d", &m);

    result = MPFR_mallocMatriz(n, m);

    buscarPalabra(punte, "#[matriz]");
    
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            fscanf(punte, "%s", basura);
            mpfra_set_str(result->elemento[i][j], basura, 10);
        }
    }
    fclose(punte);
    
    return result;
}


TABLA* leerTabla(char* file_name)
{
    int i, j, N_filas=0, N_columnas=1;
    char caux;
    FILE* punte;
    TABLA* result;
    
    punte = fopen(file_name, "r");
    while(!feof(punte))
    {
        caux = fgetc(punte);
        
        if(N_filas==0)
        {
            if(caux=='\t' || caux==' ')
                N_columnas++;
        }
        
        if(caux=='\n')
        {
            N_filas++;
            if(fgetc(punte)<33)
                break;
        }
    }

    result = mallocTabla(N_filas, N_columnas);

    fseek(punte, 0, SEEK_SET);
    
    for(i=0; i<N_columnas; i++)
        fscanf(punte, "%s", result->encabezado[i]);

    for(i=0; i<N_filas-1; i++)
    {
        for(j=0; j<N_columnas; j++)
            fscanf(punte, "%lf", &result->data[i][j]);
    }

    fclose(punte);

    return result;
}


gsl_matrix* GSL_leerMatriz(FILE* punte)
{
    ///Esta funcion lee un archivo .mtz; aunque el formato '.mtz' especifica el nombre de los parametros [N_filas] y [N_columnas], la funcion es flexible y permite leer una variedad de nombres similares (ver abajo)
    
    int n, m, i, j;
    gsl_matrix* result;
    double aux;
        
    if(buscarPalabra(punte, "#[N_filas]"))
        fscanf(punte, "%d", &n);
    else if(buscarPalabra(punte, "#[Nfilas]"))
        fscanf(punte, "%d", &n);
    else if(buscarPalabra(punte, "#[n_filas]"))
        fscanf(punte, "%d", &n);
    else if(buscarPalabra(punte, "#[nfilas]"))
        fscanf(punte, "%d", &n);
    else if(buscarPalabra(punte, "#[filas]"))
        fscanf(punte, "%d", &n);

    if(buscarPalabra(punte, "#[N_columnas]"))
        fscanf(punte, "%d", &m);
    else if(buscarPalabra(punte, "#[Ncolumnas]"))
        fscanf(punte, "%d", &m);
    else if(buscarPalabra(punte, "#[n_columnas]"))
        fscanf(punte, "%d", &m);
    else if(buscarPalabra(punte, "#[ncolumnas]"))
        fscanf(punte, "%d", &m);
    else if(buscarPalabra(punte, "#[columnas]"))
        fscanf(punte, "%d", &m);

    result = gsl_matrix_calloc(n, m);

    buscarPalabra(punte, "#[matriz]");
    
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            fscanf(punte, "%lg", &aux);
            gsl_matrix_set(result, i, j, aux);
        }
    }
    
    return result;
}


MPFR_DISTRI* MPFR_leerDistribucion(char* file_name, char encabezado)
{
    long N=-1, i;
    char basura[256];
    MPFR_DISTRI* result;
    FILE* punte;
    
    punte = fopen(file_name, "r");
    
    while(!feof(punte))
    {
        fscanf(punte, "%s", basura);
        fscanf(punte, "%s", basura);
        N++;
    }
    fclose(punte);
    
    punte = fopen(file_name, "r");
    
    if(encabezado)
    {
        N--;
        
        result = MPFR_mallocDistribucion(N);
        
        fscanf(punte, "%s", basura);
        fscanf(punte, "%s", basura);
        
        for(i=0; i<N; i++)
        {
            fscanf(punte, "%s", basura);
            mpfra_set_str(result->ejex[i], basura, 10);
            
            fscanf(punte, "%s", basura);
            mpfra_set_str(result->ejey[i], basura, 10);
        }
    }
    
    else
    {
        result = MPFR_mallocDistribucion(N);
        
        for(i=0; i<N; i++)
        {
            fscanf(punte, "%s", basura);
            mpfra_set_str(result->ejex[i], basura, 10);
            
            fscanf(punte, "%s", basura);
            mpfra_set_str(result->ejey[i], basura, 10);
        }
    }
    
    fclose(punte);
    
    return result;
}


void leerParametrosTabla(long* N_filas_out, long* N_columnas_out, char* input, char encabezado)
{
    long N_filas=0, N_columnas=0;
    FILE* punte;
        
    punte = fopen(input, "r");
    while(1)
    {
        bajarLinea(punte);
        N_filas++;
        fgetc(punte); //hace falta una funcion que lea del fichero para poblar la variable EOF y que el loop termine!
        
        if(feof(punte)) break;
    }
    fclose(punte);
    if(encabezado) N_filas--;
	
    punte = fopen(input, "r");
    while(pasarColumna(punte))
        N_columnas++;
    fclose(punte);
    
    *N_filas_out = N_filas;
    *N_columnas_out = N_columnas;
}


//escritura
void pintarMatriz(MATRIZ *A, FILE* punte)
{
    ///Esta funcion escribe una matriz en formato .mtz
    
    int n, m, i, j;
    
    n= A->N_filas;
    m=A->N_columnas;
    
    fprintf(punte, "#[N_filas]\t%d\n#[N_columnas]\t%d\n\n#[matriz]\n", n, m);
    
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
            fprintf(punte, "%.17lg\t", A->elemento[i][j]);
        
        fprintf(punte, "\n");
    }
    fprintf(punte, "#[fin]\n");
}

void GSL_pintarMatriz(gsl_matrix *A, FILE* punte)
{
    ///Esta funcion escribe una matriz en formato .mtz
    
    int n, m, i, j;
    double aux;
    
    n=(int)(A->size1);
    m=(int)(A->size2);
    
    fprintf(punte, "#[N_filas]\t%d\n#[N_columnas]\t%d\n\n#[matriz]\n", n, m);
    
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            aux = gsl_matrix_get(A, i, j);
            fprintf(punte, "%.17lg\t", aux);
        }
        fprintf(punte, "\n");
    }
    fprintf(punte, "#[fin]\n");
}


void pintarTabla(TABLA* tabla, char* file_name)
{
    int i, j;
    FILE *punte;
    
    punte = fopen(file_name, "w");
    
    fprintf(punte, "%s", tabla->encabezado[0]);
    for(i=1; i<tabla->N_columnas; i++)
        fprintf(punte, "\t%s", tabla->encabezado[i]);
    fprintf(punte, "\n");
    for(i=0; i<tabla->N_filas-1; i++)
    {
        fprintf(punte, "%lg", tabla->data[i][0]);
        for(j=1; j<tabla->N_columnas; j++)
            fprintf(punte, "\t%lg", tabla->data[i][j]);
        fprintf(punte, "\n");
    }
    
    fclose(punte);
}


void pintarDistribucion(DISTRI *blanco, char* output)
{
    int i;
    FILE* punte;

    punte = fopen(output,"w"); 

    fprintf(punte,"x\ty");
    for(i=0;i<blanco->N;i++)
        fprintf(punte,"\n%lg\t%lg",blanco->ejex[i], blanco->ejey[i]);

    fclose(punte);
}


void MPFR_pintarDistribucion(MPFR_DISTRI* blanco, char* output)
{
    int i;
    FILE* punte;
    
    punte = fopen(output, "w");
    fprintf(punte, "x\ty\n");
    for(i=0; i<blanco->N; i++)
    {
        mpfra_out_str(punte, 10, 6, blanco->ejex[i]);
        fprintf(punte, "\t");

        mpfra_out_str(punte, 10, 6, blanco->ejey[i]);
        fprintf(punte, "\n");
    }
}


void animacionGnuplot(char* input, int N_frames, int N_dim, int N_repeticiones)
{
    ///Esta funcion pinta en bucle un fichero de texto. El formato para gnuplot debe ser tal que cada frame diferente este separado por tres saltos de linea: [plot1]\n\n\n[plot2]...
	int i, j;
	double x, y, z, x_min, x_max, y_min, y_max, z_min, z_max;
	FILE *punte, *pipe;
	
	punte = fopen(input, "r");
		
	if(N_dim==1 || N_dim==2)
	{
		fscanf(punte, "%lf %lf", &x_max, &y_max);
		x_min = x_max;
		y_min = y_min;
		while(1)
		{
			fscanf(punte, "%lf %lf", &x, &y);
			if(x>x_max)
				x_max = x;
			if(x<x_min)
				x_min = x;
			if(y>y_max)
				y_max = y;
			if(y<y_min)
				y_min = y;
				
			if(feof(punte))
				break;

		}
	}
	else
	{
		fscanf(punte, "%lf %lf %lf", &x_max, &y_max, &z_max);
		x_min = x_max;
		y_min = y_max;
		z_min = z_max;
		while(1)
		{
			fscanf(punte, "%lf %lf", &x, &y);
			if(x>x_max)
				x_max = x;
			if(x<x_min)
				x_min = x;
			if(y>y_max)
				y_max = y;
			if(y<y_min)
				y_min = y;
			if(z>z_max)
				z_max = z;
			if(z<z_min)
				z_min = z;
				
			if(feof(punte))
				break;
		}
	}
	
	fclose(punte);
	
	pipe = popen("gnuplot\n", "w");
	fprintf(pipe, "unset key\n");
	fflush(pipe);
	fprintf(pipe, "set xrange [%lf:%lf]\n", x_min-0.1, x_max+0.1);
	fflush(pipe);
	fprintf(pipe, "set yrange [%lf:%lf]\n", y_min-0.1, y_max+0.1);
	fflush(pipe);
	if(N_dim==3)
	{
		fprintf(pipe, "set zrange [%lf:%lf]\n", z_min-0.1, z_max+0.1);
		fflush(pipe);
	}
	
		
    for(i=0; i<N_repeticiones; i++)
    {
        for(j=0; j<N_frames; j++)
        {
            if(N_dim==1 || N_dim==2)
                fprintf(pipe, "plot \"%s\" i %d u 1:2 w p pt 5 ps 3\n", input, j);
            else
                fprintf(pipe, "splot \"%s\" i %d u 1:2:3 w p pt 5 ps 3\n", input, j);
            fflush(pipe);
            delay(0.1);
        }
			
    }
	
	pclose(pipe);
}
