/* ESTRUCTURAS */

typedef struct PUFARRAY_STRUCT
{
	double  *****elemento;
	int N_retos;
	int N_inst;
	int N_rep;
	int N_pdl;
	int N_cells;
	
} PUFARRAY;

typedef struct PUFEXP_STRUCT
{
    int N_retos, N_instancias, N_repeticiones, tamano_reto, tamano_respuesta;
    double**** elementox;
    double**** elementoy;

} PUFEXP;

typedef struct BINARIO_STRUCT
{
    int Nbits; //numero de bits    
    char* digito;
    
} BINARIO; //estructura de numero binario en punto fijo

typedef struct CMTOPOL_STRUCT
{
    int N_celdas;
    int N_bits;
    int** matriz;
    
} CMTOPOL; //estructura de una "matriz" que contiene la topologia de una PUF de medida compensada



/*ENCABEZADOS*/

//gestion de memoria
PUFARRAY*	mallocPufarray(int N_retos, int N_inst, int N_rep, int N_pdl, int N_cells);

PUFEXP*     mallocPufexp(int N_retos, int N_instancias, int N_repeticiones, int tamano_reto, int tamano_respuesta);

BINARIO*    mallocBinario(int Nbits);

CMTOPOL*    mallocCmtopol(int N_celdas);

void		freePufarray(PUFARRAY* A);

void        freePufexp(PUFEXP* A);

void        freeBinario(BINARIO* A);

void        freeCmtopol(CMTOPOL* A);


//algebra booleana
double      binarioAdouble(BINARIO* a);

BINARIO*    longAbinario(long a, int N_max);

BINARIO*    charAbinario(char a);

void        convertirAGray(MATRIZ *data);

double      distanciaHamming(double* data1, double* data2, int N);

double      distanciaEuclidea(double* data1, double* data2, int N);

int         invertirBit(int bit);


//aritmetica
void    copiarBinario(BINARIO* result, BINARIO* original);

int     bincmp(BINARIO* a, BINARIO* b);

void    sumaBinaria(BINARIO* a, BINARIO* b, BINARIO* result);

void    restaBinaria(BINARIO* a, BINARIO* b, BINARIO* result);

void    productoBinario(BINARIO* a, BINARIO* b, BINARIO* result);

double  correlacionBitstring(BINARIO* bitstring_1, BINARIO* bitstring_2); //esta funcion calcula la autocorrelacion de un bitstream de longitud N_filas x N_columnas (leido de izda a dcha y de arriba a abajo), a distancia 'longitud' con condiciones periodicas de contorno.


//PUFs
MATRIZ*     interdistanciaSet(PUFEXP* A, char realdata);

void        interdistanciaStat(PUFEXP* A, double* media, double* desv);

MATRIZ*     intradistanciaSet(PUFEXP* A, char realdata);

void        intradistanciaStat(PUFEXP* A, double* media, double* desv);

MATRIZ*		analisisvSet(PUFEXP* A, MATRIZ* goldenkey, char realdata);

void		analisisvStat(PUFEXP* A, MATRIZ* goldenkey, double* media, double* desv);

PUFEXP*     mtz_to_pex(MATRIZ* retos, MATRIZ* respuestas, int N_retos, int N_instancias, int N_repeticiones);

MATRIZ*     pex_to_mtz(PUFEXP* input);

MPFR_MATRIZ* pex_to_datamtz(PUFEXP* input); // esta funcion crea una matriz (de una sola columna) con los valores decimales almacenados en un experimento.

PUFARRAY*	copiarPufarray(PUFARRAY* entrada);

PUFEXP*     copiarPufexp(PUFEXP* entrada);

PUFEXP*     resize_pex(PUFEXP* entrada, int resize_retos, int resize_inst, int resize_rep, int resize_y); //cambia 'entrada' a un experimento de numero_de_respuestas,tamano_respuesta.

PUFEXP*     truncate_pex(PUFEXP* entrada, int trunc); //trunca 'entrada' a 'trunc' numero de bits (on-the-fly)

PUFEXP*		prom_pex(PUFEXP* entrada); //promedia 'pufexp' para obtener un 'pufexp' de N_repeticiones=1

PUFEXP*		zeropadPufexp(PUFEXP* entreada, int n_zeroes);

void        digitalizarComparacion(BINARIO* result, long medida);

CMTOPOL*    generarTopologia(int N_osciladores, int N_bits, int N_modulos); //genera una topologia CM aleatoriamente. Si N_bits>0, la opcion N_modulos se ignora, en caso contrario el numero de bits no estara fijo



//lectura/escritura
PUFEXP*     leerPufexp(char* input);

CMTOPOL*    leerCmtopol(char* input);

void        pintarPufexp(PUFEXP *A, FILE* punte);

void        pintarCmtopol(CMTOPOL* A, char* output);



/*FUNCIONES*/

//gestion de memoria
PUFARRAY* mallocPufarray(int N_retos, int N_inst, int N_rep, int N_pdl, int N_cells)
{
	PUFARRAY *result;

	result = (PUFARRAY*)malloc(sizeof(PUFARRAY));
	
	result->N_retos = N_retos;
	result->N_inst = N_inst;
	result->N_rep = N_rep;
	result->N_pdl = N_pdl;
	result->N_cells = N_cells;
	
	result->elemento = (double*****)malloc(sizeof(double****)*N_retos);
	for(int i=0; i<N_retos; i++)
	{
		result->elemento[i] = (double****)malloc(sizeof(double***)*N_inst);
		for(int j=0; j<N_inst; j++)
		{
			result->elemento[i][j] = (double***)malloc(sizeof(double**)*N_rep);
			for(int k=0; k<N_rep; k++)
			{
				result->elemento[i][j][k] = (double**)malloc(sizeof(double*)*N_pdl);
				for(int l=0; l<N_pdl; l++)
					result->elemento[i][j][k][l] = (double*)malloc(sizeof(double)*N_cells);
			}
		}
	}
	
	for(int i=0; i<N_retos; i++)
	{
		for(int j=0; j<N_inst; j++)
		{
			for(int k=0; k<N_rep; k++)
			{
				for(int l=0; l<N_pdl; l++)
				{
					for(int m=0; m<N_cells; m++)
						result->elemento[i][j][k][l][m] = 0;
				}
			}
		}
	}
	
	return result;
}

PUFEXP* mallocPufexp(int N_retos, int N_instancias, int N_repeticiones, int tamano_reto, int tamano_respuesta)
{
    int i,j,k;
    PUFEXP* result;

    result = (PUFEXP*)malloc(sizeof(PUFEXP));

    result->N_retos = N_retos;
    result->N_instancias = N_instancias;
    result->N_repeticiones = N_repeticiones;
    result->tamano_reto = tamano_reto;
    result->tamano_respuesta = tamano_respuesta;

    result->elementox = (double****)malloc(sizeof(double***)*N_retos);
    for(i=0; i<N_retos; i++)
    {
        result->elementox[i] = (double***)malloc(sizeof(double**)*N_instancias);
        for(j=0; j<N_instancias; j++)
        {
            result->elementox[i][j] = (double**)malloc(sizeof(double*)*N_repeticiones);
            for(k=0; k<N_repeticiones; k++)
                result->elementox[i][j][k] = (double*)malloc(sizeof(double)*tamano_reto);
        }
    }
    
    result->elementoy = (double****)malloc(sizeof(double***)*N_retos);
    for(i=0; i<N_retos; i++)
    {
        result->elementoy[i] = (double***)malloc(sizeof(double**)*N_instancias);
        for(j=0; j<N_instancias; j++)
        {
            result->elementoy[i][j] = (double**)malloc(sizeof(double*)*N_repeticiones);
            for(k=0; k<N_repeticiones; k++)
                result->elementoy[i][j][k] = (double*)malloc(sizeof(double)*tamano_respuesta);
        }
    }

    return result;
}

BINARIO* mallocBinario(int Nbits)
{
    int i;
    BINARIO* result;
    
    result = (BINARIO*)malloc(sizeof(BINARIO));
    result->Nbits = Nbits;
    result->digito = (char*)malloc(sizeof(char)*(Nbits+1));
    for(i=0; i<Nbits; i++)
        result->digito[i] = '0';
    result->digito[Nbits] = 0; // esto permite manejar el array 'digito' como un string formateado. No debe tocarse!
    
    return result;
    
}

CMTOPOL* mallocCmtopol(int N_celdas)
{
    int i;
    CMTOPOL* result;
   
    result = (CMTOPOL*)malloc(sizeof(CMTOPOL));
    result->N_celdas=N_celdas;
    result->N_bits=-1;
    result->matriz = (int**)malloc(sizeof(int*)*N_celdas);
    for(i=0; i<N_celdas; i++)
        result->matriz[i]=(int*)malloc(sizeof(int)*N_celdas);
    
    return result;
}

void freePufarray(PUFARRAY *A)
{
		for(int i=0; i<A->N_retos; i++)
		{
			for(int j=0; j<A->N_inst; j++)
			{
				for(int k=0; k<A->N_rep; k++)
				{
					for(int l=0; l<A->N_pdl; l++)
						free(A->elemento[i][j][k][l]);
				}
			}
		}
		for(int i=0; i<A->N_retos; i++)
		{
			for(int j=0; j<A->N_inst; j++)
			{
				for(int k=0; k<A->N_rep; k++)
					free(A->elemento[i][j][k]);
			}
		}
		for(int i=0; i<A->N_retos; i++)
		{
			for(int j=0; j<A->N_inst; j++)
				free(A->elemento[i][j]);
		}
		for(int i=0; i<A->N_retos; i++)
			free(A->elemento[i]);
			
		free(A->elemento);
}

void freePufexp(PUFEXP* A)
{
    int i,j,k;

    for(i=0; i< A->N_retos; i++)
    {
        for(j=0; j<A->N_instancias; j++)
        {
            for(k=0; k<A->N_repeticiones; k++)
            {
                free(A->elementox[i][j][k]);
                free(A->elementoy[i][j][k]);
            }
        }
    }
    for(i=0; i<A->N_retos; i++)
    {
        for(j=0; j<A->N_instancias; j++)
        {
            free(A->elementox[i][j]);
            free(A->elementoy[i][j]);
        }
    }
    for(i=0; i<A->N_retos; i++)
    {
        free(A->elementox[i]);
        free(A->elementoy[i]);
    }

    free(A->elementox);
    free(A->elementoy);
    
    free(A);
}

void freeBinario(BINARIO* A)
{
    free(A->digito);
    free(A);
}

void freeCmtopol(CMTOPOL* A)
{
    int i;
    
    for(i=0; i<A->N_celdas; i++)
        free(A->matriz[i]);
    free(A->matriz);
    
    free(A);
}


//algebra booleana
double  binarioAdouble(BINARIO* a)
{
    double result = 0;
    
    for(int i=0; i<a->Nbits; i++)
    {
        if(a->digito[i]=='1')
            result+=1<<i;
    }
    
    return result;
}


BINARIO* longAbinario(long a, int N_max)
{
    int Nbits, i;
    BINARIO* result;
    
    if(N_max<0)
        Nbits=8*(int)sizeof(long);
    else
        Nbits = 1+(int)log2(N_max);
        
    result = mallocBinario(Nbits);
    
    for(i=0; i<Nbits; i++)
    {
        if(a%2==1) result->digito[i]='1';
        else result->digito[i]='0';
        
        a = a/2;
    }
    
    return result;
}

BINARIO* charAbinario(char a)
{
    int Nbits, i;
    BINARIO* result;
    
    Nbits=8*(int)sizeof(char);
    
    result = mallocBinario(Nbits);
    
    for(i=0; i<Nbits; i++)
    {
        if(a%2==1) result->digito[Nbits-i-1]='1';
        else result->digito[Nbits-i-1]='0';
        
        a = a/2;
    }
    
    return result;
}

double distanciaHamming(double* data1, double* data2, int N)
{
    long i;
    double result = 0;

    for(i=0;i<N;i++)
    {
        if(fabs(data1[i]-data2[i])>EPS)
            result+=1; 
    }

    return result;
}

double distanciaEuclidea(double* data1, double* data2, int N)
{
    int i;
    double result=0;
    
    for(i=0; i<N; i++)
        result+=(data1[i]-data2[i])*(data1[i]-data2[i]);
    
    return sqrt(result/N);
}

void ConvertirAGray(MATRIZ *data)
{
    int i, j, N_filas, N_columnas;
    MATRIZ *auxiliar;

    N_filas = data->N_filas;
    N_columnas = data->N_columnas;

    auxiliar = mallocMatriz(N_filas, N_columnas);

    for(i=0;i<N_filas;i++)
    {
        auxiliar->elemento[i][0]=data->elemento[i][0];
        for(j=1;j<N_columnas;j++)
        {
            if(data->elemento[i][j]==data->elemento[i][j-1])
                auxiliar->elemento[i][j]=0;
            else
                auxiliar->elemento[i][j]=1;
        }
    }

    for(i=0;i<N_filas;i++)
    {
        for(j=0;j<N_columnas;j++)
            data->elemento[i][j]=auxiliar->elemento[i][j];
    }

    freeMatriz(auxiliar);
}

int invertirBit(int bit)
{
    if(bit==0) return 1;
    else return 0;
}


//aritmetica
void    copiarBinario(BINARIO* result, BINARIO* original)
{
    int i, N;
    
    N = result->Nbits;
    
    for(i=0; i<N; i++)
        result->digito[i] = original->digito[i];
}


int bincmp(BINARIO* a, BINARIO* b) //devuelve: 1 si a>b, -1 si a<b y 0 si son iguales
{
    int i, Nbits;
    
    if(a->Nbits != b->Nbits)
    {
        printf("\nERROR: en funcion 'sumaBinaria'. Ambos bits de entrada deben tener igual numero de bits\n");
    }
    
    Nbits = a->Nbits;
    
    for(i=Nbits-1; i>=0; i--)
    {
        if(a->digito[i]=='1' && b->digito[i]=='0') return 1;
        else if(a->digito[i]=='0' && b->digito[i]=='1') return -1;
    }
    
    return 0;
}


void sumaBinaria(BINARIO* a, BINARIO* b, BINARIO* result)
{
    int i, Nbits, padded=0;
    char carry=0;
    
    if(a->Nbits != b->Nbits)
    {
        printf("\nERROR: en funcion 'sumaBinaria'. Ambos bits de entrada deben tener igual numero de bits\n");
    }
    Nbits = a->Nbits;
            
    carry=0;
    padded=0;
    for(i=0; i<Nbits; i++)
    {
        if(a->digito[i] != b->digito[i])
        {
            padded=0;
            if(!carry) result->digito[i] = '1';
            else result->digito[i] = '0';
        }
        else if(a->digito[i]=='0')
        {
            padded++;
            if(!carry) result->digito[i]='0';
            else 
            {
                result->digito[i]='1';
                carry=0;
            }
        }
        else
        {
            padded=0;
            if(!carry)
            {
                result->digito[i] = '0';
                carry=1;
            }
            else result->digito[i] = '1';
        }
    }
    if(carry)
        result->digito[Nbits-padded] = '1';
}

void restaBinaria(BINARIO* a, BINARIO* b, BINARIO* result)
{
    int i, Nbits;
    char carry=0;
    
    if(a->Nbits != b->Nbits)
    {
        printf("\nERROR: en funcion 'sumaBinaria'. Ambos bits de entrada deben tener igual numero de bits\n");
    }
    Nbits = a->Nbits;
    
    i=bincmp(a, b);
    
    if(i==0)
    {
        for(i=0; i<Nbits; i++)
            result->digito[i]='0';
    }
    
    else if(i==-1)
    {
        printf("\nERROR: en funcion 'restaBinaria'. Numeros binarios negativos no estan soportados\n");
    }
    else
    {
        carry=0;
        for(i=0; i<Nbits; i++)
        {
            if(a->digito[i] == b->digito[i])
            {
                if(!carry) result->digito[i] = '0';
                else result->digito[i] = '1';
            }
            else if(a->digito[i]=='1')
            {
                if(!carry) result->digito[i]='1';
                else 
                {
                    result->digito[i]='0';
                    carry=0;
                }
            }
            else
            {
                if(!carry)
                {
                    result->digito[i] = '1';
                    carry=1;
                }
                else result->digito[i] = '0';
            }
        }
    }
}

/*void    productoBinario(BINARIO* a, BINARIO* b, BINARIO* result)
{
    
}*/


double correlacionBitstring(BINARIO* bitstring_1, BINARIO* bitstring_2)
{
    long i;
    double result;
    
    if(bitstring_1->Nbits != bitstring_2->Nbits)
    {
        printf("\nERROR: en funcion 'correlacionBitstring': los bitstrings introducidos deben ser de igual tamano\n");
        return -1;
    }
    
    result=0;
    for(i=0; i<bitstring_1->Nbits; i++)
    {
        if(bitstring_1->digito[i]==bitstring_2->digito[i])
            result+=1.0;
        //else
            //result+=-1.0; 
    }
    
    return fabs(result)/bitstring_1->Nbits;
}


//PUFs
MATRIZ* interdistanciaSet(PUFEXP* A, char realdata)
{
    long i, j, k, j_prima, N, contador;
    MATRIZ* result;

    if(A->N_instancias==1)
    {
        printf("\nERROR: en funcion 'interdistanciaDistribucion', El numero de instancias (N_instancias) del experimento debe ser mayor que '1' para calcular este observable\n");
        return NULL;
    }

    N = (A->N_instancias*A->N_retos*A->N_repeticiones*(A->N_instancias-1))/2;

    result = mallocMatriz(N, 1);

    contador=0;
    for(i=0; i<A->N_retos; i++)
    {
        for(j=0; j<A->N_instancias; j++)
        {
            for(k=0; k<A->N_repeticiones; k++)
            {
                for(j_prima=j+1; j_prima<A->N_instancias; j_prima++)
                {
                    if(!realdata)
                        result->elemento[contador][0] = distanciaHamming(A->elementoy[i][j][k], A->elementoy[i][j_prima][k], A->tamano_respuesta);
                    else
                        result->elemento[contador][0] = distanciaEuclidea(A->elementoy[i][j][k], A->elementoy[i][j_prima][k], A->tamano_respuesta);
                    
                    contador++;
                }
            }
        }
    }

    return result;
}


void interdistanciaStat(PUFEXP* A, double* media, double* desv)
{
    *media=0;
    *desv=0;

    if(A->N_instancias==1)
    {
        printf("\nERROR: en funcion 'interdistanciaPromedio', El numero de instancias (N_instancias) del experimento debe ser mayor que '1' para calcular este observable\n");
        exit(0);
    }

    for(int i=0; i<A->N_retos; i++)
    {
        for(int j=0; j<A->N_instancias; j++)
        {
            for(int k=0; k<A->N_repeticiones; k++)
            {
                for(int j_prima=j+1; j_prima<A->N_instancias; j_prima++)
                    *media+=distanciaHamming(A->elementoy[i][j][k], A->elementoy[i][j_prima][k], A->tamano_respuesta);
            }
        }
    }
    *media /= A->N_instancias*A->N_retos*A->N_repeticiones*(A->N_instancias-1)*0.5;
    
    for(int i=0; i<A->N_retos; i++)
    {
        for(int j=0; j<A->N_instancias; j++)
        {
            for(int k=0; k<A->N_repeticiones; k++)
            {
                for(int j_prima=j+1; j_prima<A->N_instancias; j_prima++)
                    *desv+=(distanciaHamming(A->elementoy[i][j][k], A->elementoy[i][j_prima][k], A->tamano_respuesta)-*media)*(distanciaHamming(A->elementoy[i][j][k], A->elementoy[i][j_prima][k], A->tamano_respuesta)-*media);
            }
        }
    }
    *desv /= (A->N_instancias*A->N_retos*A->N_repeticiones*(A->N_instancias-1)*0.5-1);
    *desv = sqrt(*desv);
}

MATRIZ* intradistanciaSet(PUFEXP* A, char realdata)
{
    long i, j, k, k_prima, N, contador;
    MATRIZ* result;

    if(A->N_repeticiones==1)
    {
        printf("\nERROR: en funcion 'intradistanciaPromedio', El numero de repeticiones (N_repeticiones) del experimento debe ser mayor que '1' para calcular este observable\n");
        return NULL;
    }

    N = (A->N_instancias*A->N_retos*A->N_repeticiones*(A->N_repeticiones-1))/2;

    result = mallocMatriz(N, 1);

    contador=0;
    for(i=0; i<A->N_retos; i++)
    {
        for(j=0; j<A->N_instancias; j++)
        {
            for(k=0; k<A->N_repeticiones; k++)
            {
                for(k_prima=k+1; k_prima<A->N_repeticiones; k_prima++)
                {
                    if(!realdata)
                        result->elemento[contador][0] = distanciaHamming(A->elementoy[i][j][k], A->elementoy[i][j][k_prima], A->tamano_respuesta);
                    else
                        result->elemento[contador][0] = distanciaEuclidea(A->elementoy[i][j][k], A->elementoy[i][j][k_prima], A->tamano_respuesta);
                    
                    contador++;
                }
            }
        }
    }

    return result;
}

void intradistanciaStat(PUFEXP* A, double* media, double* desv)
{   
    *media=0;
    *desv=0;

    if(A->N_repeticiones==1)
    {
        printf("\nERROR: en funcion 'intradistanciaPromedio', El numero de repeticiones (N_repeticiones) del experimento debe ser mayor que '1' para calcular este observable\n");
        exit(0);
    }

    for(int i=0; i<A->N_retos; i++)
    {
        for(int j=0; j<A->N_instancias; j++)
        {
            for(int k=0; k<A->N_repeticiones; k++)
            {
                for(int k_prima=k+1; k_prima<A->N_repeticiones; k_prima++)
                    *media+=distanciaHamming(A->elementoy[i][j][k], A->elementoy[i][j][k_prima], A->tamano_respuesta);
            }
        }
    }
    *media /= A->N_instancias*A->N_retos*A->N_repeticiones*(A->N_repeticiones-1)*0.5;
    
    for(int i=0; i<A->N_retos; i++)
    {
        for(int j=0; j<A->N_instancias; j++)
        {
            for(int k=0; k<A->N_repeticiones; k++)
            {
                for(int k_prima=k+1; k_prima<A->N_repeticiones; k_prima++)
                    *desv+=(distanciaHamming(A->elementoy[i][j][k], A->elementoy[i][j][k_prima], A->tamano_respuesta)-*media)*(distanciaHamming(A->elementoy[i][j][k], A->elementoy[i][j][k_prima], A->tamano_respuesta)-*media);
            }
        }
    }
    *desv /= (A->N_instancias*A->N_retos*A->N_repeticiones*(A->N_repeticiones-1)*0.5-1);
    *desv = sqrt(*desv);
}

MATRIZ* analisisvSet(PUFEXP* A, MATRIZ* goldenkey, char realdata)
{
    long i, j, k, N, contador;
    MATRIZ* result;
	
	if(A->N_retos*A->N_instancias != goldenkey->N_filas)
	{
		printf("\nERROR: en función 'analisisvSet'. El número de filas de la matriz 'goldenkey' no es formateable a nretos*ninst.\n");
		return NULL;
	}
	if(A->tamano_respuesta != goldenkey->N_columnas)
	{
		printf("\nERROR: en función 'analisisvSet'. El número de columnas de la matriz 'goldenkey' no coincide con el número de bits de las respuestas de 'pufexp'.\n");
		return NULL;
	}

    N = A->N_instancias*A->N_retos*A->N_repeticiones;

    result = mallocMatriz(N, 1);

    contador=0;
    for(i=0; i<A->N_retos; i++)
    {
        for(j=0; j<A->N_instancias; j++)
        {
            for(k=0; k<A->N_repeticiones; k++)
            {
				if(!realdata)
					result->elemento[contador][0] = distanciaHamming(A->elementoy[i][j][k], goldenkey->elemento[i*A->N_instancias+j], A->tamano_respuesta);
				else
					result->elemento[contador][0] = distanciaEuclidea(A->elementoy[i][j][k], goldenkey->elemento[i*A->N_instancias+j], A->tamano_respuesta);
					
				contador++;
            }
        }
    }

    return result;
}

void analisisvStat(PUFEXP* A, MATRIZ* goldenkey, double* media, double* desv)
{
    *media=0;
    *desv=0;

	if(A->N_retos*A->N_instancias != goldenkey->N_filas)
	{
		printf("\nERROR: en función 'analisisvSet'. El número de filas de la matriz 'goldenkey' no es formateable a nretos*ninst.\n");
		exit(0);
	}
	if(A->tamano_respuesta != goldenkey->N_columnas)
	{
		printf("\nERROR: en función 'analisisvSet'. El número de columnas de la matriz 'goldenkey' no coincide con el número de bits de las respuestas de 'pufexp'.\n");
		exit(0);
	}

    for(int i=0; i<A->N_retos; i++)
    {
        for(int j=0; j<A->N_instancias; j++)
        {
            for(int k=0; k<A->N_repeticiones; k++)
                *media+=distanciaHamming(A->elementoy[i][j][k], goldenkey->elemento[i*A->N_instancias+j], A->tamano_respuesta);
        }
    }
    *media /= A->N_instancias*A->N_retos*A->N_repeticiones;
    
    for(int i=0; i<A->N_retos; i++)
    {
        for(int j=0; j<A->N_instancias; j++)
        {
            for(int k=0; k<A->N_repeticiones; k++)
                *desv+=(distanciaHamming(A->elementoy[i][j][k], goldenkey->elemento[i*A->N_instancias+j], A->tamano_respuesta)-*media)*(distanciaHamming(A->elementoy[i][j][k], goldenkey->elemento[i*A->N_instancias+j], A->tamano_respuesta)-*media);
        }
    }
    *desv /= (A->N_instancias*A->N_retos*A->N_repeticiones-1);
    *desv = sqrt(*desv);
}

PUFEXP *mtz_to_pex(MATRIZ* retos, MATRIZ* respuestas, int N_retos, int N_instancias, int N_repeticiones)
{
    int i, j, N, contador_reto=0, contador_instancia=0, contador_repeticion=0;
    PUFEXP *result;
    
    N = N_retos*N_instancias*N_repeticiones;
    
    if(retos->N_filas != N || respuestas->N_filas != N)
    {
        printf("\nERROR: en funcion 'mtz_to_pex': los tamanos de las matrices no coinciden con el numero de retos X instancias X repeticiones\n");
        return NULL;
    }

    result = mallocPufexp(N_retos, N_instancias, N_repeticiones, retos->N_columnas, respuestas->N_columnas);
    for(i=0; i<N; i++)
    {
        for(j=0; j<retos->N_columnas; j++)
            result->elementox[contador_reto][contador_instancia][contador_repeticion][j] = retos->elemento[i][j];
        
        for(j=0; j<respuestas->N_columnas; j++)
            result->elementoy[contador_reto][contador_instancia][contador_repeticion][j] = respuestas->elemento[i][j];
        
        contador_repeticion++;
        if(contador_repeticion==N_repeticiones)
        {
            contador_repeticion=0;
            contador_instancia++;
            
            if(contador_instancia==N_instancias)
            {
                contador_instancia=0;
                contador_reto++;
                
                if(contador_reto==N_retos)
                    break;
            }
        }
    }
    return result;
}

MATRIZ* pex_to_mtz(PUFEXP* input)
{
    int i, j, k, l, contador;
    MATRIZ* result;
    
    result = mallocMatriz(input->N_retos*input->N_instancias*input->N_repeticiones, input->tamano_respuesta);
    
    contador=0;
    for(i=0; i<input->N_retos; i++)
    {
        for(j=0; j<input->N_instancias; j++)
        {
            for(k=0; k<input->N_repeticiones; k++)
            {
                for(l=0; l<input->tamano_respuesta; l++)
                    result->elemento[contador][l] = input->elementoy[i][j][k][l];
                
                contador++;
            }
        }
    }
    
    return result;
}

MPFR_MATRIZ* pex_to_datamtz(PUFEXP* input)
{
    int contador, N_claves, N_bits;
    MPFR_MATRIZ* result;
    BINARIO** bitstring;
    
    N_claves = input->N_retos*input->N_instancias*input->N_repeticiones;
    N_bits = input->tamano_respuesta;
    
    result = MPFR_mallocMatriz(N_claves, 1);
    
    bitstring = (BINARIO**)malloc(sizeof(BINARIO*)*N_claves);
    for(int i=0; i<N_claves; i++)
        bitstring[i] = mallocBinario(N_bits);
    
    contador=0;
    for(int i=0; i<input->N_retos; i++)
    {
        for(int j=0; j<input->N_instancias; j++)
        {
            for(int k=0; k<input->N_repeticiones; k++)
            {
                bitstring[contador] = mallocBinario(N_bits);
                for(int l=0; l<N_bits; l++)
                {
                    if(input->elementoy[i][j][k][l] < EPS)
                        bitstring[contador]->digito[l]='0';
                    else
                        bitstring[contador]->digito[l]='1';
                }
                mpfra_set_str(result->elemento[contador][0], bitstring[contador]->digito, 2);
                contador++;
            }
        }
    }
    
    for(int i=0; i<N_claves; i++)
        freeBinario(bitstring[i]);
    free(bitstring);
    
    return result;
}

PUFARRAY* copiarPufarray(PUFARRAY* entrada)
{
	PUFARRAY* result;
	
	result = mallocPufarray(entrada->N_retos, entrada->N_inst, entrada->N_rep, entrada->N_pdl, entrada->N_cells);
	
	for(int i=0; i<entrada->N_retos; i++)
	{
		for(int j=0; j<entrada->N_inst; j++)
		{
			for(int k=0; k<entrada->N_rep; k++)
			{
				for(int l=0; l<entrada->N_pdl; l++)
				{
					for(int m=0; m<entrada->N_cells; m++)
						result->elemento[i][j][k][l][m] = entrada->elemento[i][j][k][l][m];
				}
			}
		}
	}
	
	return result;
}

PUFEXP* copiarPufexp(PUFEXP* entrada)
{
    PUFEXP* result;
    
    result = mallocPufexp(entrada->N_retos, entrada->N_instancias, entrada->N_repeticiones, entrada->tamano_reto, entrada->tamano_respuesta);
    
    for(int i=0; i<entrada->N_retos; i++)
    {
        for(int j=0; j<entrada->N_instancias; j++)
        {
            for(int k=0; k<entrada->N_repeticiones; k++)
            {
                for(int l=0; l<entrada->tamano_reto; l++)
                    result->elementox[i][j][k][l] = entrada->elementox[i][j][k][l];
                for(int l=0; l<entrada->tamano_respuesta; l++)
                    result->elementoy[i][j][k][l] = entrada->elementoy[i][j][k][l];
            }
        }
    }
    
    return result;
}

PUFEXP* resize_pex(PUFEXP* entrada, int resize_retos, int resize_inst, int resize_rep, int resize_y)
{
    PUFEXP* result;
    int cont_retos, cont_inst, cont_rep, cont_y;
    
    if(entrada->N_retos*entrada->N_instancias*entrada->N_repeticiones*entrada->tamano_respuesta != resize_retos*resize_inst*resize_rep*resize_y)
        return 0;
    
    result = mallocPufexp(resize_retos, resize_inst, resize_rep, 0, resize_y);
    
    cont_retos=0;
    cont_inst=0;
    cont_rep=0;
    cont_y=0;
    for(int i=0; i<resize_retos; i++)
    {
        for(int j=0; j<resize_inst; j++)
        {
            for(int k=0; k<resize_rep; k++)
            {
                for(int l=0; l<resize_y; l++)
                {
                    result->elementoy[i][j][k][l] = entrada->elementoy[cont_retos][cont_inst][cont_rep][cont_y];
                    
                    cont_y++;
                    if(cont_y==entrada->tamano_respuesta)
                    {
                        cont_y=0;
                        cont_rep++;
                    }
                    if(cont_rep==entrada->N_repeticiones)
                    {
                        cont_rep=0;
                        cont_inst++;
                    }
                    if(cont_inst==entrada->N_instancias)
                    {
                        cont_inst=0;
                        cont_retos++;
                    }
                }
            }
        }
    }
    
    return result;
}

PUFEXP* truncate_pex(PUFEXP* entrada, int trunc)
{
    PUFEXP* result;

    if(trunc>entrada->tamano_respuesta)
        return 0;

    result = mallocPufexp(entrada->N_retos, entrada->N_instancias, entrada->N_repeticiones, 0, trunc);

    for(int i=0; i<entrada->N_retos; i++)
    {
        for(int j=0; j<entrada->N_instancias; j++)
        {
            for(int k=0; k<entrada->N_repeticiones; k++)
            {
                for(int l=0; l<trunc; l++)
                    result->elementoy[i][j][k][l] = entrada->elementoy[i][j][k][l];
            }
        }
    }
    
    return result;
}

PUFEXP* prom_pex(PUFEXP* entrada)
{
	PUFEXP* result;
	
    result = mallocPufexp(entrada->N_retos, entrada->N_instancias, 1, entrada->tamano_reto, entrada->tamano_respuesta);

    for(int i=0; i<entrada->N_retos; i++)
    {
        for(int j=0; j<entrada->N_instancias; j++)
        {
			for(int l=0; l<entrada->tamano_reto; l++)
				result->elementox[i][j][0][l] = entrada->elementox[i][j][0][l];
            
			for(int k=0; k<entrada->N_repeticiones; k++)
            {
                for(int l=0; l<entrada->tamano_respuesta; l++)
                    result->elementoy[i][j][0][l] += entrada->elementoy[i][j][k][l];
            }
			
			for(int l=0; l<entrada->tamano_respuesta; l++)
			{
				result->elementoy[i][j][0][l] /= entrada->N_repeticiones;
				if(result->elementoy[i][j][0][l] > 0.5)
					result->elementoy[i][j][0][l] = 1;
				else
					result->elementoy[i][j][0][l] = 0;
			}
        }
    }
    
    return result;
}

PUFEXP* zeropadPufexp(PUFEXP* entrada, int n_zeroes)
{
	PUFEXP* result;
	
	result = mallocPufexp(entrada->N_retos, entrada->N_instancias, entrada->N_repeticiones, entrada->tamano_reto, entrada->tamano_respuesta+n_zeroes);
	
    for(int i=0; i<entrada->N_retos; i++)
    {
        for(int j=0; j<entrada->N_instancias; j++)
        {
            for(int k=0; k<entrada->N_repeticiones; k++)
            {
                for(int l=0; l<entrada->tamano_reto; l++)
                    result->elementox[i][j][k][l] = entrada->elementox[i][j][k][l];
				for(int l=0; l<entrada->tamano_respuesta; l++)
					result->elementoy[i][j][k][l] = entrada->elementoy[i][j][k][l];
				for(int l=0; l<n_zeroes; l++)
					result->elementoy[i][j][k][entrada->tamano_respuesta+l] = 0;
            }
        }
    }
	
	return result;
}

void digitalizarComparacion(BINARIO* result, long medida)
{    
    long i, N_prec;
    
    N_prec = result->Nbits;
    
    if(medida<0)
    {
        result->digito[0]='1';
        medida*= -1;
    }
    else result->digito[0]='0';
    
    for(i=1; i<N_prec; i++)
    {
        if(medida%2==1) result->digito[i]='1';
        else result->digito[i]='0';
        
        medida = medida/2;
    }    
}

CMTOPOL* generarTopologia(int N_osciladores, int N_bits, int N_modulos)
{
    int n, i, j, i_0, j_0, i_new, j_new, i_old, j_old, intaux, pisado=0, contador_bits, flag=0;
    CMTOPOL* topologia;
    
    topologia = mallocCmtopol(N_osciladores);

    for(i=N_osciladores-1; i>=0; i--)
    {
        for(j=0; j<N_osciladores; j++)
        {
            if(i>j)
                topologia->matriz[i][j]=-1;
            else if(i==j)
                topologia->matriz[i][j]=-1;
            else
                topologia->matriz[i][j]=-1;
        }
    }
    
    if(N_bits<=0)
    {
        for(n=1; n<=N_modulos; n++)
        {
            while(1)
            {
                i_0=(int)dado((long)N_osciladores);
                j_0=(int)dado((long)N_osciladores);
                
                if(i_0>j_0 && topologia->matriz[i_0][j_0]<0)
                    break;
            }
            
            topologia->matriz[i_0][j_0]=n;
            i=i_0;
            j=j_0;
            while(1)
            {
                intaux=(int)dado(4);
                
                switch(intaux)
                {
                    case 0:
                        i_new=i;
                        j_new=j+1;
                        break;
                        
                    case 1:
                        i_new=i+1;
                        j_new=j;
                        break;
                        
                    case 2:
                        i_new=i;
                        j_new=j-1;
                        break;
                        
                    case 3:
                        i_new=i-1;
                        j_new=j;
                        break;
                }
                if(i_new<0)
                    i_new=1;
                
                if(i_new>N_osciladores-1)
                    i_new=N_osciladores-2;
                
                if(j_new<0)
                    j_new=1;
                
                if(j_new>N_osciladores-1)
                    j_new=N_osciladores-2;
                
                if(j_new==i_new)
                {
                    if(intaux==0)
                        j_new--;
                    else
                        i_new++;
                }
                //break;
                
                if(topologia->matriz[i_new][j_new]>0)
                {
                    if((i_new==i_old && j_new==j_old) || pisado)
                        pisado=1;
                    
                    else
                        break;

                }
                else pisado=0;
                
                i_old=i;
                j_old=j;
                
                i=i_new;
                j=j_new;
                
                topologia->matriz[i][j]=n;
            }
        }
    }
    
    else
    {
        contador_bits=N_bits;
        for(n=1; n<=1000; n++)
        {
            if(flag || contador_bits==0)
                break;

            while(1)
            {
                i_0=(int)dado((long)N_osciladores);
                j_0=(int)dado((long)N_osciladores);
                
                if(i_0>j_0 && topologia->matriz[i_0][j_0]<0)
                    break;
            }
            
            topologia->matriz[i_0][j_0]=n;
            contador_bits--;
            if(contador_bits==0)
                break;
            pisado=0;
            
            i=i_0;
            j=j_0;
            while(1)
            {
                intaux=(int)dado(4);
                
                switch(intaux)
                {
                    case 0:
                        i_new=i;
                        j_new=j+1;
                        break;
                        
                    case 1:
                        i_new=i+1;
                        j_new=j;
                        break;
                        
                    case 2:
                        i_new=i;
                        j_new=j-1;
                        break;
                        
                    case 3:
                        i_new=i-1;
                        j_new=j;
                        break;
                }
                if(i_new<0)
                    i_new=0;

                if(i_new>N_osciladores-1)
                    i_new=N_osciladores-1;

                if(j_new<0)
                    j_new=0;

                if(j_new>N_osciladores-1)
                    j_new=N_osciladores-1;

                if(j_new==i_new)
                {
                    if(intaux==0)
                        j_new--;
                    else
                        i_new++;
                }
                //break;

                if(topologia->matriz[i_new][j_new]<0)
                {
                    topologia->matriz[i_new][j_new]=n;
                    contador_bits--;
                    pisado=0;
                }
                
                else
                {
                    if((i_new==i_old && j_new==j_old) || pisado)
                        pisado=1;
                    
                    else
                        break;
                }

                i_old=i;
                j_old=j;
                
                i=i_new;
                j=j_new;
                
                if(contador_bits==0)
                {
                    flag=1;
                    break;
                }
            }
        }
    }
    
    return topologia;
}

//lectura/escritura
PUFEXP* leerPufexp(char* input)
{
    int i, j, k, l, N_retos, N_instancias, N_repeticiones, tamano_reto, tamano_respuesta;
    long contador=0;
    char string[256];
    FILE* punte;
    PUFEXP* result;
    
    punte = fopen(input, "r");
    
    buscarPalabra(punte, "#[N_retos]");
    fscanf(punte, "%d", &N_retos);
    
    buscarPalabra(punte, "#[N_instancias]");
    fscanf(punte, "%d", &N_instancias);
    
    buscarPalabra(punte, "#[N_repeticiones]");
    fscanf(punte, "%d", &N_repeticiones);
    
    buscarPalabra(punte, "#[tamano_reto]");
    fscanf(punte, "%d", &tamano_reto);
    
    buscarPalabra(punte, "#[tamano_respuesta]");
    fscanf(punte, "%d", &tamano_respuesta);
    
    result = mallocPufexp(N_retos, N_instancias, N_repeticiones, tamano_reto, tamano_respuesta);
    
    for(i=0; i<N_retos; i++)
    {
        for(j=0; j<N_instancias; j++)
        {
            for(k=0; k<N_repeticiones; k++)
            {
                sprintf(string, "#[%d,%d,%d]", i, j, k);
                
                buscarPalabra(punte, string);

                for(l=0; l<tamano_reto; l++)
                    fscanf(punte,"%lf", &result->elementox[i][j][k][l]);
                
                for(l=0; l<tamano_respuesta; l++)
                    fscanf(punte,"%lf", &result->elementoy[i][j][k][l]);
            }
        }
    }
    
    fclose(punte);
    
    return result;
}

CMTOPOL* leerCmtopol(char* input)
{
    int i,j, N_celdas, N_bits=0;
    FILE* punte;
    char aux[1];
    CMTOPOL* result;
    
    punte = fopen(input, "r");
    
    buscarPalabra(punte, "#[N_celdas]");
    fscanf(punte, "%d", &N_celdas);
    result = mallocCmtopol(N_celdas);
    
    buscarPalabra(punte, "#[matriz]");
    for(i=N_celdas-1; i>=0; i--)
    {
        for(j=0; j<N_celdas; j++)
        {
           fscanf(punte, "%s", aux);
           if(aux[0]=='o')
           {
               result->matriz[i][j]=1;
               N_bits++;
           }
           else result->matriz[i][j]=-1;
        }
    }
    
    fclose(punte);
    
    result->N_bits=N_bits;
    
    return result;
}


void pintarPufexp(PUFEXP *A, FILE* punte)
{
    int i, j, k, l;
    
    fprintf(punte, "#[N_retos]\t%d\n#[N_instancias]\t%d\n#[N_repeticiones]\t%d\n#[tamano_reto]\t%d\n#[tamano_respuesta]\t%d\n\n", A->N_retos, A->N_instancias, A->N_repeticiones, A->tamano_reto, A->tamano_respuesta);
    
    for(i=0; i<A->N_retos; i++)
    {
        for(j=0; j<A->N_instancias; j++)
        {
            for(k=0; k<A->N_repeticiones; k++)
            {
                fprintf(punte, "#[%d,%d,%d]\n", i, j, k);
                for(l=0; l<A->tamano_reto; l++)
                    fprintf(punte, "%lg ", A->elementox[i][j][k][l]);
                
                fprintf(punte, "\n\n");
                
                for(l=0; l<A->tamano_respuesta; l++)
                    fprintf(punte, "%lg ", A->elementoy[i][j][k][l]);
                
                fprintf(punte, "\n\n\n");
            }
        }
    }
    fprintf(punte, "#[fin]\n");
}

void pintarCmtopol(CMTOPOL* A, char* output)
{
    int i, j;
    FILE* punte;
    
    punte = fopen(output, "w");
    
    fprintf(punte, "#[N_celdas] %d\n\n#[matriz]\n\n", A->N_celdas);
    for(i=A->N_celdas-1; i<=0; i--)
    {
        for(j=0; j<A->N_celdas; j++)
        {
            if(i==j)
                fprintf(punte, "%c ", 'x');
            else if(A->matriz[i][j]>0)
                fprintf(punte, "%c ", 'o');
            else
                fprintf(punte, "%c ", '*');
        }
        fprintf(punte, "\n");
    }
    fprintf(punte, "\n#[fin]\n");
    
    fclose(punte);
}

void printfbin(BINARIO* a)
{
    int i;
    for(i=a->Nbits-1; i>=0; i--) printf("%c", a->digito[i]);
}

void fprintfbin(FILE* punte, BINARIO* a)
{
    int i;
    for(i=a->Nbits-1; i>=0; i--)
    {
        if(a->digito[i]=='0')
            fprintf(punte, "0 ");
        else
            fprintf(punte, "1 ");
    }
}
