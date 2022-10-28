#include <main.h>
#include <matematicas.h>
#include <digital.h>


int main(int N_opcion, char** opcion)
{
    long mpfr_prec=53;
    char input[1024]={"input.mtz"}, output[1024]={"output.mtz"}, contraer_filas=0, contraer_columnas=0, barajar_filas=0, barajar_columnas=0, ordenar_filas=0, orden_var=0, decimar=0;
    int i, j, contador_i, contador_j, x_fila=-1, y_fila=-1, z_fila=-1, x_col=-1, y_col=-1, z_col=-1, N_filas_result, N_columnas_result, *index_filas, *index_columnas, decim_length=-1;
    double factor=1.0, aux;
    FILE *punte;
	MATRIZ* data, *data_new, *result, *auxmtz;
    MPFR_MATRIZ* result_decimal;
    DISTRI* auxdist;
    BINARIO* binaux;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa permite manipular la forma de una matriz (en formato '.mtz') para generar un nuevo fichero '.mtz'; el programa permite extraer una submatriz de la matriz original, la cual puede obtenerse de filas y/o columnas salteadas. Una vez seleccionada esta submatriz, el programa puede contraer (i.e., promediar) las filas (para crear una matriz fila), contraer las columnas (para crear una matriz columna), o ambas (lo que da lugar a un numero). Antes de escribirla el programa también puede desordenarla (tanto en filas como en columnas). Por ultimo, el programa tambien puede multiplicar cada elemento por un factor antes de escribir la salida.\n----------------------------------------------------------------------\n\n");
            printf("-in [input.mtz], nombre del archivo de entrada\n\n");
            printf("-out [output.mtz], nombre del archivo de salida\n\n");
            printf("-filas [x] [y] [z], esta opcion permite determinar las filas de la matriz original que se utilizaran para construir la submatriz: desde la fila 'x' hasta la 'y' saltando cada 'z' filas. La numeracion va desde 0 hasta N-1. Si se utiliza un valor negativo para estos numeros (por defecto), se interpreta como 'el valor extremo': x<0=desde la fila 0; y<0=hasta la fila N-1; z<0=saltando cada 1 fila (i.e., todas las filas).\n\n");
            printf("-columnas [x] [y] [z], esta opcion permite determinar las columnas de la matriz original que se utilizaran para construir la submatriz: desde la columna 'x' hasta la 'y' saltando cada 'z' columnas. La numeracion va desde 0 hasta N-1. Si se utiliza un valor negativo para estos numeros (por defecto), se interpreta como 'el valor extremo': x<0=desde la columna 0; y<0=hasta la columna N-1; z<0=saltando cada 1 columnas (i.e., todas las columnas). \n\n");
			printf("-contraerFilas, si esta opcion esta presente el programa contraera (promediara) las filas de la submatriz entre si, dando lugar a una nueva matriz de una sola fila.\n\n");
			printf("-contraerColumnas, si esta opcion esta presente el programa contraera (promediara) las columnas de la submatriz entre si, dando lugar a una nueva matriz de una sola columna.\n\n");
            printf("-barajarFilas, si esta opcion esta presente el programa barajara aleatoriamente las filas de la matriz de entrada (es decir, lee las filas aleatoriamente en vez de en orden). Notar que no tiene sentido utilizar esta opcion junto con '-contraerFilas' ni '-ordenarFilas'\n\n");
            printf("-barajarColumnas, si esta opcion esta presente el programa barajara aleatoriamente las columnas de la matriz de entrada (es decir, lee las columnas aleatoriamente en vez de en orden). Notar que no tiene sentido utilizar esta opcion junto con '-contraerColumnas'\n\n");
            printf("-factor [1.0], esta opcion permite indicar un factor multiplicativo que se aplicara a todos los elementos de la submatriz antes de escribirlos. Utilizar para cambios de unidades\n\n");
            printf("-ordenarFilas [var], si esta opcion esta presente el programa reordenara las filas de la matriz de entrada de menor a mayor. Notar que no tiene sentido utilizar esta opcion junto con '-contraerFilas' ni '-barajarFilas'. Anadir el flag 'var' es opcional, y sirve para modificar el criterio empleado para ordenar las filas, que pasan a ordenarse de menor a mayor en funcion de la desviacion (en modulo) de cada valor en la fila respecto del promedio de las filas.\n\n");
            printf("-decimar [nbits], si esta opcion esta presente el programa toma la matriz de entrada como si fuera una matriz de una sola columna formada por palabras binarias de 'nbits' bits (por defecto = 'N_columnas'), leidos en orden de izda a dcha y de arriba a abajo. Si 'N_filas'x'N_columnas' no es multiplo de 'nbits', los bits que sobren se descartan. Esta transformacion se realiza en el ultimo paso del programa (de forma que cualquier otra opcion habra tenido efecto). Se toman los valores mayores que cero de la entrada original como '1', y los valores menores o iguales como '0'\n\n");

            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
            sscanf(opcion[i+1], "%s", input);
        
        if(strcmp(opcion[i], "-out")==0)
            sscanf(opcion[i+1], "%s", output);
		
        if(strcmp(opcion[i], "-filas")==0)
        {
            sscanf(opcion[i+1], "%d", &x_fila);
            sscanf(opcion[i+2], "%d", &y_fila);
            sscanf(opcion[i+3], "%d", &z_fila);
        }
        
        if(strcmp(opcion[i], "-columnas")==0)
        {
            sscanf(opcion[i+1], "%d", &x_col);
            sscanf(opcion[i+2], "%d", &y_col);
            sscanf(opcion[i+3], "%d", &z_col);
        }
        
        if(strcmp(opcion[i], "-contraerFilas")==0)
            contraer_filas=1;
        
        if(strcmp(opcion[i], "-contraerColumnas")==0)
            contraer_columnas=1;
        
        if(strcmp(opcion[i], "-barajarFilas")==0)
            barajar_filas=1;
        
        if(strcmp(opcion[i], "-barajarColumnas")==0)
            barajar_columnas=1;
        
        if(strcmp(opcion[i], "-factor")==0)
            sscanf(opcion[i+1], "%lf", &factor);
        
        if(strcmp(opcion[i], "-ordenarFilas")==0)
        {
            ordenar_filas=1;
            
            if(i<N_opcion-1)
            {                
                if(strcmp(opcion[i+1], "var")==0)
                    orden_var=1;
            }
        }
        
        if(strcmp(opcion[i], "-decimar")==0)
        {
            decimar=1;
            
            if(i<N_opcion-1)
            {                
                if(opcion[i+1][0] != '-')
                    sscanf(opcion[i+1], "%d", &decim_length);
            }
        }
    }
    
    data = leerMatriz(input);
    
    if(x_fila<0) x_fila = 0;
    if(y_fila<0) y_fila = data->N_filas-1;
    if(z_fila<0) z_fila = 1;
    
    if(x_col<0) x_col = 0;
    if(y_col<0) y_col = data->N_columnas-1;
    if(z_col<0) z_col = 1;
        
	index_filas = malloc(sizeof(int)*data->N_filas);
	for(i=0; i<data->N_filas; i++)
		index_filas[i] = i;
		
	index_columnas = malloc(sizeof(int)*data->N_columnas);
	for(i=0; i<data->N_columnas; i++)
		index_columnas[i] = i;
    
	if(barajar_filas) gsl_ran_shuffle(idum, index_filas, data->N_filas, sizeof(int));
	if(barajar_columnas) gsl_ran_shuffle(idum, index_columnas, data->N_columnas, sizeof(int));
    
    if(ordenar_filas)
    {
        auxdist = mallocDistribucion(data->N_filas);
        if(!orden_var)
        {
            for(j=0; j<data->N_columnas; j++)
            {
                for(i=0; i<data->N_filas; i++)
                {
                    auxdist->ejex[i]=i+0.1;
                    auxdist->ejey[i]=data->elemento[i][j];
                }
                GSL_ordenarData_distribucion(auxdist);
                for(i=0; i<data->N_filas; i++)
                    data->elemento[i][j]=auxdist->ejey[i];
            }
        }
        else
        {
            for(j=0; j<data->N_columnas; j++)
            {
                for(i=0; i<data->N_filas; i++)
                {
                    auxdist->ejex[i]=i+0.1;
                    auxdist->ejey[i]=data->elemento[i][j];
                }
                aux = calcular_promedio(auxdist->ejey, auxdist->N);
                for(i=0; i<data->N_filas; i++)
                    auxdist->ejey[i] = fabs(auxdist->ejey[i]-aux);
                
                GSL_ordenarData_distribucion(auxdist);
                for(i=0; i<data->N_filas; i++)
                    auxdist->ejey[i]=data->elemento[i][j];
                for(i=0; i<data->N_filas; i++)
                    data->elemento[i][j]=auxdist->ejey[(int)(auxdist->ejex[i])];
            }        
        }
        
        freeDistribucion(auxdist);
    }
    
    data_new = mallocMatriz((y_fila-x_fila+1)/z_fila, (y_col-x_col+1)/z_col);
    
    contador_i=0;
    for(i=x_fila; i<=y_fila; i+=z_fila)
    {
        contador_j=0;
        for(j=x_col; j<=y_col; j+=z_col)
        {
            data_new->elemento[contador_i][contador_j] = data->elemento[index_filas[i]][index_columnas[j]];
            contador_j++;
        }
        contador_i++;
    }
    
    if(contraer_filas)
    {
		N_filas_result = 1;
		
        for(i=1; i<data_new->N_filas; i++)
		{
			for(j=0; j<data_new->N_columnas; j++)
				data_new->elemento[0][j]+=data_new->elemento[i][j];
		}
		
		for(j=0; j<data_new->N_columnas; j++)
			data_new->elemento[0][j]/=data_new->N_filas;
    }
	else N_filas_result = data_new->N_filas;
    
    if(contraer_columnas)
    {
		N_columnas_result = 1;
		
        for(i=0; i<data_new->N_filas; i++)
		{
			for(j=1; j<data_new->N_columnas; j++)
				data_new->elemento[i][0]+=data_new->elemento[i][j];
		}
		
		for(i=0; i<data_new->N_filas; i++)
			data_new->elemento[i][0]/=data_new->N_columnas;   
    }
	else N_columnas_result = data_new->N_columnas;
	
	result = mallocMatriz(N_filas_result, N_columnas_result);
	for(i=0; i<result->N_filas; i++)
	{
		for(j=0; j<result->N_columnas; j++)
			result->elemento[i][j]=factor*data_new->elemento[i][j];
	}
    
    if(!decimar)
    {
        punte = fopen(output, "w");
        pintarMatriz(result, punte);
        fclose(punte);
    }
    else
    {
        if(decim_length<0)
            decim_length = result->N_columnas;
        
        binaux = mallocBinario(decim_length);
        result_decimal = MPFR_mallocMatriz((result->N_filas*result->N_columnas)/decim_length, 1);
        
        contador_i=0;
        for(i=0; i<(result->N_filas*result->N_columnas)/decim_length; i++)
        {
            for(j=0; j<decim_length; j++)
            {
                if(result->elemento[contador_i/result->N_columnas][contador_i%result->N_columnas]>EPS)
                    binaux->digito[j]='1';
                else
                    binaux->digito[j]='0';
                
                contador_i++;
            }
            mpfra_set_str(result_decimal->elemento[i][0], binaux->digito, 2);
        }
        
        punte = fopen(output, "w");
        fprintf(punte, "#[N_filas] %ld\n#[N_columnas] %d\n\n#[matriz]\n", result_decimal->N_filas, 1);
        
        for(i=0; i<result_decimal->N_filas; i++)
        {
            mpfra_out_str(punte, 10, 0, result_decimal->elemento[i][0]);
            fprintf(punte, "\n");
        }
        fprintf(punte, "#[fin]\n");
        fclose(punte);
        
        freeBinario(binaux);
        MPFR_freeMatriz(result_decimal);
    }
	
	freeMatriz(data);
	freeMatriz(data_new);
	freeMatriz(result);
	free(index_filas);
	free(index_columnas);

    return 0;
}
