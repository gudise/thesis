/* CONSTANTES */

#define e_kbT   38.941431328 //e/(KbT), T=25ºC    constante de la ecuacion caracteristica del diodo

#define VT 0.6 //Parametro de tension del transistor. Este es variable y en general dependera de la tecnologia, por lo tanto solo lo introduzco en forma de define con findes de depuracion. En sucesivas actualizaciones de la libreria debera introducirse como parametro externamente.
#define beta_r 10.0
#define beta_f 125.0


/*ESTRUCTURAS*/

typedef struct CIRCUITO_STRUCT
{
    int     N_nodos; //numero de nodos totales = N_activos + N_pasivos
    int     N_activos; //numero de nodos activos
    int     N_pasivos; //numero de nodos pasivos
    double  *tension_nodo; //matriz 1D con las tensiones de cada nodo
    double  *intensidad_nodo;
    double  ***intensidad_malla; //matriz NxN con los valores de la intensidad que fluye entre los nodos i,j; convenio: + si fluye hacia dentro del nodo
    char    ***topologia;
    double  ****valor;
    int     **multiplicidad; //multiplicidad[i][j] = numero de mallas que unen los nodos i,j
    char    *status_nodo; //array de tamano N que marca los nodos activos('a', de tension fija) y pasivos('p', de tension variable por el programa).
    
} CIRCUITO;



/*ENCABEZADOS*/

//gestion de memoria
CIRCUITO *mallocCircuito(unsigned int N, unsigned int N_activos, unsigned int N_pasivos, int **multiplicidad_input, char ***topologia_input);

void        freeCircuito(CIRCUITO *input);


//funciones
void        calcularIntensidades(CIRCUITO *input, CIRCUITO *memoria, double d_tiempo); //calcula las intensidades que circulan por cada malla (i.e. actualiza la matriz 'intensidad_nodo') dada la distribucion de intensidades presente en el vector 'tension_nodo'

CIRCUITO    *leerTopologia(char *input);

CIRCUITO    *copiarCircuito(CIRCUITO *original);

void        resolverKirchoff_biseccion(CIRCUITO *input, double d_tiempo, double tolerancia); //resuelve las ecuaciones de Kirchoff en un instante temporal



/*FUNCIONES*/

//gestion de memoria
CIRCUITO *mallocCircuito(unsigned int N, unsigned int N_activos, unsigned int N_pasivos, int **multiplicidad_input, char ***topologia_input)
{
    int i, j, k, l, ***multivalor;
    CIRCUITO *result;
    
    multivalor = (int***)malloc(sizeof(int**)*N);
    for(i=0; i<N; i++)
    {
        multivalor[i]=(int**)malloc(sizeof(int*)*N);
        for(j=0; j<N; j++)
        {
            multivalor[i][j] = (int*)malloc(sizeof(int)*multiplicidad_input[i][j]);
            
            for(k=0; k<multiplicidad_input[i][j]; k++)
            {
                switch(topologia_input[i][j][k])
                {
                    case 'V':
                        multivalor[i][j][k]=1;
                        break;
                        
                    case 'R':
                        multivalor[i][j][k]=1;
                        break;
                        
                    case 'C':
                        multivalor[i][j][k]=1;
                        break;
                        
                    case 'L':
                        multivalor[i][j][k]=1;
                        break;
                        
                    case 'D':
                        multivalor[i][j][k]=1;
                        break;
                        
                    case 'P':
                        multivalor[i][j][k]=3;
                        break;
                        
                    case 'N':
                        multivalor[i][j][k]=3;
                        break;
                        
                    case 'E':
                        multivalor[i][j][k]=4;
                        break;
                }
            }
        }
    }
    
    result = (CIRCUITO*)malloc(sizeof(CIRCUITO));
    
    result->intensidad_nodo = (double*)malloc(sizeof(double)*N);
    
    result->tension_nodo = (double*)malloc(sizeof(double)*N);
    
    result->status_nodo = (char*)malloc(sizeof(char)*N);
    
    result->intensidad_malla = (double***)malloc(sizeof(double**)*N);
    for(i=0; i<N; i++)
    {
        result->intensidad_malla[i] = (double**)malloc(sizeof(double*)*N);
        for(j=0; j<N; j++)
            result->intensidad_malla[i][j] = (double*)malloc(sizeof(double)*multiplicidad_input[i][j]);
    }
    
    result->topologia = (char***)malloc(sizeof(char**)*N);
    for(i=0; i<N; i++)
    {
        result->topologia[i] = (char**)malloc(sizeof(char*)*N);
        for(j=0; j<N; j++)
            result->topologia[i][j] = (char*)malloc(sizeof(char)*multiplicidad_input[i][j]);
    }
    
    result->valor = (double****)malloc(sizeof(double***)*N);
    for(i=0; i<N; i++)
    {
        result->valor[i] = (double***)malloc(sizeof(double**)*N);
        for(j=0; j<N; j++)
        {
            result->valor[i][j] = (double**)malloc(sizeof(double*)*multiplicidad_input[i][j]);
            for(k=0; k<multiplicidad_input[i][j]; k++)
                result->valor[i][j][k] = (double*)malloc(sizeof(double)*multivalor[i][j][k]);
        }
    }
    
    result->multiplicidad = (int**)malloc(sizeof(int*)*N);
    for(i=0; i<N; i++)
        result->multiplicidad[i] = (int*)malloc(sizeof(int)*N);
    
    for(i=0; i<N; i++)
    {
        result->intensidad_nodo[i]=0;
        result->tension_nodo[i]=0;
        result->status_nodo[i]=0;
        for(j=0; j<N; j++)
        {
            result->multiplicidad[i][j]=0;
            
            for(k=0; k<multiplicidad_input[i][j]; k++)
            {
                result->intensidad_malla[i][j][k]=0;
                result->topologia[i][j][k]=0;
                
                for(l=0; l<multivalor[i][j][k]; l++)
                    result->valor[i][j][k][l]=0;
            }
        }
    }
    
    result->N_nodos = N;
    
    result->N_pasivos = N_pasivos;
    
    result->N_activos = N_activos;
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
            free(multivalor[i][j]);
        free(multivalor[i]);
    }
    free(multivalor);
    
    
    return result;
}

void freeCircuito(CIRCUITO *input)
{
    int i, j, k, N;
    
    N = input->N_nodos;
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
        {
            for(k=0; k<input->multiplicidad[i][j]; k++)
                free(input->valor[i][j][k]);
            
            free(input->intensidad_malla[i][j]);
            free(input->topologia[i][j]);
            free(input->valor[i][j]);
        }
        free(input->intensidad_malla[i]);
        free(input->topologia[i]);
        free(input->valor[i]);
        free(input->multiplicidad[i]);
    }
    free(input->intensidad_malla);
    free(input->topologia);
    free(input->valor);
    free(input->multiplicidad);
    free(input->intensidad_nodo);
    free(input->tension_nodo);
    free(input->status_nodo);
    
    free(input);
}


//funciones
void calcularIntensidades(CIRCUITO *input, CIRCUITO *memoria, double d_tiempo)
{
    int i, j, k;
    double VGS, VDS, IC, IB, VBE, VBC;

    for(i=0; i<input->N_nodos; i++)
    {
        input->intensidad_nodo[i]=0;
        
        for(j=0; j<input->N_nodos; j++)
        {
            for(k=0; k<input->multiplicidad[i][j]; k++)
            {
                switch(input->topologia[i][j][k])
                {
                    case 'R':
                        input->intensidad_malla[i][j][k] = ( input->tension_nodo[i]-input->tension_nodo[j] )/input->valor[i][j][k][0];
                        
                        input->intensidad_malla[j][i][k] = -input->intensidad_malla[i][j][k];
                        
                        break;
                    
                    case 'C':
                        input->intensidad_malla[i][j][k] = ((input->tension_nodo[i]-input->tension_nodo[j])-(memoria->tension_nodo[i]-memoria->tension_nodo[j]))*input->valor[i][j][k][0]/d_tiempo;
                        
                        input->intensidad_malla[j][i][k] = -input->intensidad_malla[i][j][k];
                        
                        break;
                    
                    case 'L':
                        input->intensidad_malla[i][j][k] = ( input->tension_nodo[i]-input->tension_nodo[j] )*d_tiempo/input->valor[i][j][k][0]+memoria->intensidad_malla[i][j][k];
                        
                        input->intensidad_malla[j][i][k] = -input->intensidad_malla[i][j][k];
                        
                        break;
                
                    case 'D':
                        input->intensidad_malla[i][j][k] = input->valor[i][j][k][0]*(exp(e_kbT*( input->tension_nodo[i]-input->tension_nodo[j] ))-1);
                        
                        input->intensidad_malla[j][i][k] = -input->intensidad_malla[i][j][k];
                        
                        break;
                        
                    case 'P':
                        VGS = input->tension_nodo[i+1]-input->tension_nodo[j];
                        VDS = input->tension_nodo[i]-input->tension_nodo[j];
                            
                        if(VGS>input->valor[i][j][k][1]) //region de cutoff
                            input->intensidad_malla[i][j][k]=0;
                        
                        else
                        {
                            if(VDS>VGS-input->valor[i][j][k][1]) //region lineal
                                input->intensidad_malla[i][j][k]=input->valor[i][j][k][0]*VDS*(2*(VGS-input->valor[i][j][k][1])-VDS);
                            
                            else //region saturacion
                                input->intensidad_malla[i][j][k]=input->valor[i][j][k][0]*(VGS-input->valor[i][j][k][1])*(VGS-input->valor[i][j][k][1]);
                        }
                        
                        input->intensidad_malla[j][i][k] = -input->intensidad_malla[i][j][k];
                        
                        input->intensidad_malla[i][i+1][k] = 0;
                        input->intensidad_malla[i+1][i][k] = 0;
                        
                        input->intensidad_malla[i+1][j][k] = 0;
                        input->intensidad_malla[j][i+1][k] = 0;
                        
                        break;
                        
                    case 'N':
                        VGS = input->tension_nodo[i+1]-input->tension_nodo[j];
                        VDS = input->tension_nodo[i]-input->tension_nodo[j];
                            
                        if(VGS<=input->valor[i][j][k][1]) //region de cutoff
                            input->intensidad_malla[i][j][k]=0;
                        
                        else
                        {
                            if(VDS<=VGS-input->valor[i][j][k][1]) //region lineal
                                input->intensidad_malla[i][j][k]=input->valor[i][j][k][0]*VDS*(2*(VGS-input->valor[i][j][k][1])-VDS);
                            
                            else //region saturacion
                                input->intensidad_malla[i][j][k]=input->valor[i][j][k][0]*(VGS-input->valor[i][j][k][1])*(VGS-input->valor[i][j][k][1]);
                        }
                        
                        input->intensidad_malla[j][i][k] = -input->intensidad_malla[i][j][k];
                        
                        input->intensidad_malla[i][i+1][k] = 0;
                        input->intensidad_malla[i+1][i][k] = 0;
                        
                        input->intensidad_malla[i+1][j][k] = 0;
                        input->intensidad_malla[j][i+1][k] = 0;
                        
                        break;
                        
                    case 'E':
                        VBE = input->tension_nodo[i+1]-input->tension_nodo[j];
                        VBC = input->tension_nodo[i+1]-input->tension_nodo[i];
                        IC = input->valor[i][j][k][0]*(exp(VBE/input->valor[i][j][k][1])-exp(VBC-input->valor[i][j][k][1])-1/input->valor[i][j][k][3]*(exp(VBC/input->valor[i][j][k][1])-1));
                        IB = input->valor[i][j][k][0]*(1/input->valor[i][j][k][2]*(exp(VBE/input->valor[i][j][k][1])-1)+1/input->valor[i][j][k][3]*(exp(VBC/input->valor[i][j][k][1])-1));
                        
                        input->intensidad_malla[i][j][k] = (2.0*IC+IB)/3.0;
                        input->intensidad_malla[j][i][k] = -input->intensidad_malla[i][j][k];
                        
                        input->intensidad_malla[i][i+1][k] = (IC-IB)/3.0;
                        input->intensidad_malla[i+1][i][k] = -input->intensidad_malla[i][i+1][k];
                        
                        input->intensidad_malla[i+1][j][k] = (IC+2.0*IB)/3.0;
                        input->intensidad_malla[j][i+1][k] = -input->intensidad_malla[i+1][j][k];
                        
                        break;
                    
                    default: input->intensidad_malla[i][j][k] = 0;
                }
            }
        }
        
        
        for(j=0; j<input->N_nodos; j++)
        {
            for(k=0; k<input->N_nodos; k++)
                input->intensidad_nodo[i]+=input->intensidad_malla[j][i][k]-input->intensidad_malla[i][j][k];
        }
    }
}


CIRCUITO  *leerTopologia(char *input)
{
    int i, j, breaking, N, N_activos=2, N_pasivos, inda, indb, puntero_topologia, puntero, **contador;
    double valor;
    char letra, string[256], ***topologia;
    CIRCUITO* result;
    FILE* punte;
    
    punte = fopen(input, "r");
    
    buscarPalabra(punte, "[N_nodos]"); fscanf(punte, "%d", &N);
    
    buscarPalabra(punte, "[topologia]"); puntero_topologia = (int)ftell(punte);
    
    fseek(punte, puntero_topologia, SEEK_SET);
    
    
    contador=(int**)malloc(sizeof(int*)*N);
    for(i=0; i<N; i++)
    {
        contador[i]=(int*)malloc(sizeof(int)*N);
        for(j=0; j<N; j++)
            contador[i][j] = 0;
    }
    
    puntero = puntero_topologia;
    breaking=0;
    while(breaking==0)
    {
        fseek(punte, puntero, SEEK_SET);
        fscanf(punte, "%d %c %d", &inda, &letra, &indb);
        switch(letra)
        {
            case 'V':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                
                if(inda != indb)
                    printf("\nERROR: en funcion 'leerTopologia'; elemento unario (nodo de tension fijo) definido entre dos nodos diferentes\n");
                else
                {
                    N_activos++;
                    contador[inda][indb]++;
                }
                    
                break;
                    
            case 'R':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                contador[inda][indb]++;
                    //contador[indb][inda]++;
                break;
                    
            case 'C':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                contador[inda][indb]++;
                    //contador[indb][inda]++;
                break;
                    
            case 'L':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                contador[inda][indb]++;
                    //contador[indb][inda]++;
                break;
                    
            case 'D':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                contador[inda][indb]++;
                    //contador[indb][inda]++;
                break;
                    
            case 'P':
                fscanf(punte, "%lf %lf", &valor, &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                contador[inda][indb]++;
                    //contador[indb][inda]++;
                    
                    //contador[inda][inda+1]++;
                    //contador[inda+1][inda]++;
                    
                    //contador[inda+1][indb]++;
                    //contador[indb][inda+1]++;
                break;
                    
            case 'N':
                fscanf(punte, "%lf %lf", &valor, &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                contador[inda][indb]++;
                    //contador[indb][inda]++;
                    
                    //contador[inda][inda+1]++;
                    //contador[inda+1][inda]++;
                    
                    //contador[inda+1][indb]++;
                    //contador[indb][inda+1]++;
                break;
                    
            case 'E':
                fscanf(punte, "%lf %lf %lf %lf", &valor, &valor, &valor, &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                contador[inda][indb]++;
                    //contador[indb][inda]++;
                    
                    //contador[inda][inda+1]++;
                    //contador[inda+1][inda]++;
                    
                    //contador[inda+1][indb]++;
                    //contador[indb][inda+1]++;
                break;
        }
    }
    
    N_pasivos = N-N_activos;
    
    topologia = (char***)malloc(sizeof(char**)*N);
    for(i=0; i<N; i++)
    {
        topologia[i]=(char**)malloc(sizeof(char*)*N);
        for(j=0; j<N; j++)
            topologia[i][j] = (char*)malloc(sizeof(char)*contador[i][j]);
    }
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
            contador[i][j]=0;
    }
    
    puntero = puntero_topologia;
    breaking=0;
    while(breaking==0)
    {
        fseek(punte, puntero, SEEK_SET);
        fscanf(punte, "%d %c %d", &inda, &letra, &indb);
        switch (letra)
        {
            case 'V':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                topologia[inda][indb][contador[inda][indb]]='V';
                break;
                
            case 'R':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                topologia[inda][indb][contador[inda][indb]]='R';
                    //topologia[indb][inda][contador[indb][inda]]='r';
                break;
                    
            case 'C':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                topologia[inda][indb][contador[inda][indb]]='C';
                    //topologia[indb][inda][contador[indb][inda]]='c';
                break;
                    
            case 'L':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                topologia[inda][indb][contador[inda][indb]]='L';
                    //topologia[indb][inda][contador[indb][inda]]='l';
                break;
                    
            case 'D':
                fscanf(punte, "%lf", &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                topologia[inda][indb][contador[inda][indb]]='D';
                    //topologia[indb][inda][contador[indb][inda]]='d';
                break;
                    
            case 'P':
                fscanf(punte, "%lf %lf", &valor, &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                topologia[inda][indb][contador[inda][indb]]='P';
                    //topologia[indb][inda][contador[indb][inda]]='p';
                    
                    //topologia[inda][inda+1][contador[inda][inda+1]]='p';
                    //topologia[inda+1][inda][contador[inda+1][inda]]='p';
                    
                    //topologia[inda+1][indb][contador[inda+1][indb]]='p';
                    //topologia[indb][inda+1][contador[indb][inda+1]]='p';
                    
                break;
                    
            case 'N':
                fscanf(punte, "%lf %lf", &valor, &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                topologia[inda][indb][contador[inda][indb]]='N';
                    //topologia[indb][inda][contador[indb][inda]]='n';
                    
                    //topologia[inda][inda+1][contador[inda][inda+1]]='n';
                    //topologia[inda+1][inda][contador[inda+1][inda]]='n';
                    
                    //topologia[inda+1][indb][contador[inda+1][indb]]='n';
                    //topologia[indb][inda+1][contador[indb][inda+1]]='n';
                break;
                    
            case 'E':
                fscanf(punte, "%lf %lf %lf %lf", &valor, &valor, &valor, &valor);
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                topologia[inda][indb][contador[inda][indb]]='E';
                    //topologia[indb][inda][contador[indb][inda]]='e';
                    
                    //topologia[inda][inda+1][contador[inda][inda+1]]='e';
                    //topologia[inda+1][inda][contador[inda+1][inda]]='e';
                    
                    //topologia[inda+1][indb][contador[inda+1][indb]]='e';
                    //topologia[indb][inda+1][contador[indb][inda+1]]='e';
            break;
        }
        contador[inda][indb]++;
                //contador[indb][inda]++;
    }
    
    result = mallocCircuito(N, N_activos, N_pasivos, contador, topologia);
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
        {
            result->multiplicidad[i][j] = contador[i][j];
            
            contador[i][j]=0;
        }
    }
    
    puntero = puntero_topologia;
    breaking=0;
    while(breaking==0)
    {
        fseek(punte, puntero, SEEK_SET);
        fscanf(punte, "%d %c %d", &inda, &letra, &indb);
        switch (letra)
        {
            case 'V':
                fscanf(punte, "%lf", &(result->valor[inda][indb][contador[inda][indb]][0]));
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                
                result->topologia[inda][indb][contador[inda][indb]]='V';
                result->status_nodo[inda]=1;
                result->tension_nodo[inda]=result->valor[inda][indb][contador[inda][indb]][0];
                
                break;
                    
            case 'R':
                fscanf(punte, "%lf", &(result->valor[inda][indb][contador[inda][indb]][0]));
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                result->topologia[inda][indb][contador[inda][indb]]='R';
                    //result->topologia[indb][inda][contador[indb][inda]]='r';
                break;
                    
            case 'C':
                fscanf(punte, "%lf", &(result->valor[inda][indb][contador[inda][indb]][0]));
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                result->topologia[inda][indb][contador[inda][indb]]='C';
                    //result->topologia[indb][inda][contador[indb][inda]]='c';
                break;
                    
            case 'L':
                fscanf(punte, "%lf", &(result->valor[inda][indb][contador[inda][indb]][0]));
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                result->topologia[inda][indb][contador[inda][indb]]='L';
                    //result->topologia[indb][inda][contador[indb][inda]]='l';
                break;
                    
            case 'D':
                fscanf(punte, "%lf", &(result->valor[inda][indb][contador[inda][indb]][0]));
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                result->topologia[inda][indb][contador[inda][indb]]='D';
                    //result->topologia[indb][inda][contador[indb][inda]]='d';
                break;
                    
            case 'P':
                fscanf(punte, "%lf %lf", &(result->valor[inda][indb][contador[inda][indb]][0]), &(result->valor[inda][indb][contador[inda][indb]][1]));
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                result->topologia[inda][indb][contador[inda][indb]]='P';
                    //result->topologia[indb][inda][contador[indb][inda]]='p';
                    
                    //result->topologia[inda][inda+1][contador[inda][inda+1]]='p';
                    //result->topologia[inda+1][inda][contador[inda+1][inda]]='p';
                    
                    //result->topologia[inda+1][indb][contador[inda+1][indb]]='p';
                    //result->topologia[indb][inda+1][contador[indb][inda+1]]='p';
                    
                break;
                    
            case 'N':
                fscanf(punte, "%lf %lf", &(result->valor[inda][indb][contador[inda][indb]][0]), &(result->valor[inda][indb][contador[inda][indb]][1]));
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                result->topologia[inda][indb][contador[inda][indb]]='N';
                    //result->topologia[indb][inda][contador[indb][inda]]='n';
                    
                    //result->topologia[inda][inda+1][contador[inda][inda+1]]='n';
                    //result->topologia[inda+1][inda][contador[inda+1][inda]]='n';
                    
                    //result->topologia[inda+1][indb][contador[inda+1][indb]]='n';
                    //result->topologia[indb][inda+1][contador[indb][inda+1]]='n';
                break;
                    
            case 'E':
                fscanf(punte, "%lf %lf %lf %lf", &(result->valor[inda][indb][contador[inda][indb]][0]), &(result->valor[inda][indb][contador[inda][indb]][1]), &(result->valor[inda][indb][contador[inda][indb]][2]), &(result->valor[inda][indb][contador[inda][indb]][3]));
                puntero = ftell(punte);
                fscanf(punte, "%s", string); if(strcmp(string, "end")==0) breaking=1;
                result->topologia[inda][indb][contador[inda][indb]]='E';
                    //result->topologia[indb][inda][contador[indb][inda]]='e';
                    
                    //result->topologia[inda][inda+1][contador[inda][inda+1]]='e';
                    //result->topologia[inda+1][inda][contador[inda+1][inda]]='e';
                    
                    //result->topologia[inda+1][indb][contador[inda+1][indb]]='e';
                    //result->topologia[indb][inda+1][contador[indb][inda+1]]='e';
                break;

            }
            contador[inda][indb]++;
                //contador[indb][inda]++;
    }
    
    result->status_nodo[0]=1;
    result->status_nodo[N-1]=1;
    
    for(i=0; i<N; i++)
        free(contador[i]);
    free(contador);
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
            free(topologia[i][j]);
    }
    for(i=0; i<N; i++)
        free(topologia[i]);
    free(topologia);
    
    fclose(punte);
    
    return result;
}


CIRCUITO *copiarCircuito(CIRCUITO *original)
{
    int i, j, k, l, N, N_activos, N_pasivos;
    //int **multiplicidad;
    CIRCUITO *result;
    
    N = original->N_nodos;
    
    N_activos = original->N_activos;
    
    N_pasivos = original->N_pasivos;
    
    /*multiplicidad=(int**)malloc(sizeof(int*)*N);
    for(i=0; i<N; i++)
    {
        multiplicidad[i]=(int*)malloc(sizeof(int)*N);
        for(j=0; j<N; j++)
            multiplicidad[i][j] = original->multiplicidad[i][j];
    }*/
    
    result = mallocCircuito(N, N_activos, N_pasivos, original->multiplicidad, original->topologia);
    
    for(i=0; i<N; i++)
    {
        result->tension_nodo[i] = original->tension_nodo[i];
        result->intensidad_nodo[i] = original->tension_nodo[i];
        result->status_nodo[i] = original->status_nodo[i];
        for(j=0; j<N; j++)
        {
            result->multiplicidad[i][j] = original->multiplicidad[i][j];
            
            for(k=0; k<original->multiplicidad[i][j]; k++)
            {
                result->intensidad_malla[i][j][k] = original->intensidad_malla[i][j][k];
                result->topologia[i][j][k] = original->topologia[i][j][k];
                
                switch(original->topologia[i][j][k])
                {
                    case 'V':
                        result->valor[i][j][k][0] = original->valor[i][j][k][0];
                        break;
                        
                    case 'R':
                        result->valor[i][j][k][0] = original->valor[i][j][k][0];
                        break;
                        
                    case 'C':
                        result->valor[i][j][k][0] = original->valor[i][j][k][0];
                        break;
                        
                    case 'L':
                        result->valor[i][j][k][0] = original->valor[i][j][k][0];
                        break;
                        
                    case 'D':
                        result->valor[i][j][k][0] = original->valor[i][j][k][0];
                        break;
                        
                    case 'P':
                        result->valor[i][j][k][0] = original->valor[i][j][k][0];
                        result->valor[i][j][k][1] = original->valor[i][j][k][1];
                        break;
                        
                    case 'N':
                        result->valor[i][j][k][0] = original->valor[i][j][k][0];
                        result->valor[i][j][k][1] = original->valor[i][j][k][1];
                        break;
                        
                    case 'E':
                        result->valor[i][j][k][0] = original->valor[i][j][k][0];
                        result->valor[i][j][k][1] = original->valor[i][j][k][1];
                        result->valor[i][j][k][2] = original->valor[i][j][k][2];
                        result->valor[i][j][k][3] = original->valor[i][j][k][3];
                        break;
                }
            }
        }
    }
    
    return result;
}


void resolverKirchoff_biseccion(CIRCUITO *input, double d_tiempo, double tolerancia)
{
    int i, j, N, N_pasivos, breaking;
    double *d_tension;
    CIRCUITO *input_original;
    
    N = input->N_nodos;
    N_pasivos = input->N_pasivos;
    
    d_tension = (double*)malloc(sizeof(double)*N);
    
    input_original = copiarCircuito(input);
   
    for(j=0; j<N; j++)
        d_tension[j] = (1-tolerancia)*tanh(fabs(input->intensidad_nodo[j]))+tolerancia;
    
    for(i=0; i<1000; i++)
    {
        breaking = 0;
        
        calcularIntensidades(input, input_original, d_tiempo);
        for(j=0; j<N; j++)
        {
            if(input->status_nodo[j]==0)
            {
                if(fabs(input->intensidad_nodo[j])<tolerancia && d_tension[j]<tolerancia && i>0) breaking++;
                else
                {
                    if(input->intensidad_nodo[j]*d_tension[j]<0) d_tension[j] = -0.5*d_tension[j];
                    
                    input->tension_nodo[j]+=d_tension[j];
                }
            }
        }
        
        if(breaking==N_pasivos) break;
    }
    
    free(d_tension);
    freeCircuito(input_original);
}
