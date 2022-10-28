#include "include/main.h"
#include "include/matematicas.h"
#include "include/digital.h"

int main(int N_opcion, char** opcion)
{
	int i, j, entrop=0, N=100, n=10, verbose=1, binario=0, numero;
	char out_name[256]={"binomial.mtz"};
	double p = 0.5;
	double entropia;
	FILE* punte;
    BINARIO* bitstring;

    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa produce una matriz de valores (enteros de '0' a 'N') generados aleatoriamente siguiendo una distribucion binomial.\n----------------------------------\n\n");
            printf("-out [binomial.mtz],  nombre del fichero de salida.\n\n");
            printf("-p [0.5], probabilidad de que un suceso sea exitoso para la binomial\n\n");
			printf("-n [10], numero sucesos para la binomial\n\n");
            printf("-N [100], numero de valores extraidos de la distribucion\n\n");
			printf("-entropia, si esta opcion esta presente el programa devolvera la entropia teorica de la distribucion\n\n");
            printf("-binario, si esta opcion esta presente la distribucion de salida se dara en formato binario.\n\n");
			printf("-nv, si esta opcion esta presente, el programa solo escribira las cantidades elegidas. Esta opcion esta pensada para utilizar el programa como parte de un script\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-out")==0 )
            sscanf(opcion[i+1], "%s", out_name);
        
		if(strcmp(opcion[i], "-p")==0)
			sscanf(opcion[i+1], "%lf", &p);
        
        if( strcmp(opcion[i], "-n")==0 )
            sscanf(opcion[i+1], "%d", &n);
        
        if( strcmp(opcion[i], "-N")==0 )
            sscanf(opcion[i+1], "%d", &N);

        if( strcmp(opcion[i], "-entropia")==0 )
            entrop = 1;
        
        if( strcmp(opcion[i], "-binario")==0 )
            binario = 1;

        if( strcmp(opcion[i], "-nv")==0 )
            verbose = 0;
    }
    
    bitstring = mallocBinario(n);

	punte = fopen(out_name, "w");
	if(!binario)
        fprintf(punte, "#[N_filas] %d\n#[N_columnas] %d\n\n#[matriz]\n", N, 1);
    else
        fprintf(punte, "#[N_filas] %d\n#[N_columnas] %d\n\n#[matriz]\n", N, n);
        
    for(i=0; i<N; i+=1)
    {
        for(j=0; j<n; j++)
        {
            if(ran1()>p)
                bitstring->digito[j]='0';
            else 
                bitstring->digito[j]='1';
        }
        
        if(!binario)
            fprintf(punte, "%lg\t", binarioAdouble(bitstring));
        else
            fprintfbin(punte, bitstring);
        
        fprintf(punte, "\n");
    }
    fprintf(punte, "\n#[fin]\n");
    
    freeBinario(bitstring);

	if(entrop)
	{
		if(verbose) printf("Entropia = %lf\n", 0.5*log2(M_TAU*M_E*n*p*(1-p)));
		else printf("%lf", 0.5*log2(M_TAU*M_E*n*p*(1-p)));
	}

	fclose(punte);

	return 0;
}
