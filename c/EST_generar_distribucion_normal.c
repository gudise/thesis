#include "include/main.h"
#include "include/matematicas.h"
#include "include/digital.h"

int main(int N_opcion, char** opcion)
{
	int i, j, entrop=0, N, N_filas=10, N_columnas=10, verbose=1, binario=0;
	char out_name[256]={"normal.mtz"};
	double noprima, prima, sigma=1.0, media=0.0;
	double entropia;
	FILE* punte;
    BINARIO** bitstring;

    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa produce una matriz de valores generados aleatoriamente siguiendo una distribucion gausiana.\n----------------------------------\n\n");
            printf("-out [normal.mtz],  nombre del fichero de salida.\n\n");
            printf("-media [0.0], valor del promedio de la distribucion\n\n");
			printf("-sigma [1.0], sigma de la distribucion gaussiana\n\n");
			printf("-Nx [10], numero de columnas generadas\n\n");
            printf("-Ny [10], numero de filas generadas\n\n");
			printf("-entropia, si esta opcion esta presente el programa devolvera la entropia teorica de la distribucion\n\n");
            printf("-binario, si esta opcion esta presente la distribucion de salida se dara en formato binario. Solo las partes enteras se convierten a binario, el usuario es responsable de que la distribucion pedida no contenga numeros negativos ni  decimales (para ello utilizar valores altos de media y sigma, e.g. media = 2e6, sigma = 1e6)\n\n");
			printf("-nv, si esta opcion esta presente, el programa solo escribira las cantidades elegidas. Esta opcion esta pensada para utilizar el programa como parte de un script\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-out")==0 )
            sscanf(opcion[i+1], "%s", out_name);
        
		if(strcmp(opcion[i], "-media")==0)
			sscanf(opcion[i+1], "%lf", &media);
		
		if(strcmp(opcion[i], "-sigma")==0)
			sscanf(opcion[i+1], "%lf", &sigma);
        
        if( strcmp(opcion[i], "-Ny")==0 )
            sscanf(opcion[i+1], "%d", &N_filas);
        
        if( strcmp(opcion[i], "-Nx")==0 )
            sscanf(opcion[i+1], "%d", &N_columnas);

        if( strcmp(opcion[i], "-entropia")==0 )
            entrop = 1;
        
        if( strcmp(opcion[i], "-binario")==0 )
            binario = 1;

        if( strcmp(opcion[i], "-nv")==0 )
            verbose = 0;
    }
    
    N=N_filas*N_columnas;
    
    if(binario) bitstring = (BINARIO**)malloc(sizeof(BINARIO*)*N);

	punte = fopen(out_name, "w");
	fprintf(punte, "#[N_filas] %d\n#[N_columnas] %d\n\n#[matriz]\n",N_filas, N_columnas);
        
    for(i=0; i<N_filas; i+=1)
    {
        for(j=0; j<N_columnas; j++)
        {
            noprima = sigma*ran1_gauss(&prima)+media;
            prima = sigma*prima+media;
            
            if(binario==0)
                fprintf(punte, "%lf\t", noprima);
            else
            {
                bitstring[i*N_columnas+j] = longAbinario((unsigned long)noprima, -1);
                fprintfbin(punte, bitstring[i*N_columnas+j]);
            }
        }
        fprintf(punte, "\n");
    }
    fprintf(punte, "\n#[fin]\n");
    
    if(binario)
    {
        for(i=0; i<N; i+=2)
        {
            freeBinario(bitstring[i]);
            if(i<N-2) freeBinario(bitstring[i+1]);
        }
        
        free(bitstring);
    }
        
    

	if(entrop)
	{
		if(verbose) printf("Entropia = %lf\n", log2(sigma*sqrt(M_TAU*M_E)));
		else printf("%lf", log2(sigma*sqrt(M_TAU*M_E)));
	}

	fclose(punte);

	return 0;
}
