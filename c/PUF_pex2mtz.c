#include <main.h>
#include <digital.h>


int main(int N_opcion, char** opcion)
{
    char input[1024]={"input.pex"}, output[1024]={"output.mtz"};
    long i, j, k, l, n, N_retos=0, N_instancias=0, N_repeticiones=0, N_bits, contador=0;
    FILE *punte;
    MATRIZ* data_out;
    PUFEXP* data_in;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa toma un experimento '.pex' y lo convierte en una matriz '.mtz'\n----------------------------------------------------------------------\n\n");
            printf("-in [input.pex], nombre del archivo de entrada\n\n");
            printf("-out [output.mtz], nombre del archivo de salida\n\n");
            
            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
            sscanf(opcion[i+1], "%s", input);
        
        if(strcmp(opcion[i], "-out")==0)
            sscanf(opcion[i+1], "%s", output);
    }
    
    data_in= leerPufexp(input);
    
    data_out= pex_to_mtz(data_in);
    
    punte = fopen(output, "w");
    pintarMatriz(data_out, punte);
    fclose(punte);
	
	freeMatriz(data_out);
	freePufexp(data_in);

    return 0;
}
