#include <main.h>
#include <digital.h>


int main(int N_opcion, char** opcion)
{
    long    i, j, contador, Nbits1, Nbits2;
    char    input1_name[256]={"input1.mtz"}, input2_name[256]={"input2.mtz"}, verbose=1;
    BINARIO* bitstring1, *bitstring2;
    MATRIZ* input1, *input2;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa calcula la correlacion entre dos palabras binarias introducidas en forma de mstriz (.mtz). En este formato, cada elemento de la matriz sera un bit (1 si >0, 0 si <0); cada matriz de entrada se considera un unico bitstring de tamano N_filas x N_columnas bits. Este programa devuelve un error si los tamanos de los dos bitstrings son diferentes.\n\n----------------------------------------------------------------------\n\n");
            printf("-in [input1.mtz input2.mtz], \n\n");
            printf("-nv, no verbose\n\n");

            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
        {
            sprintf(input1_name, "%s", opcion[i+1]);
            sprintf(input2_name, "%s", opcion[i+2]);
        }
        
        if(strcmp(opcion[i], "-nv")==0)
            verbose = 0;
    }
    
    input1 = leerMatriz(input1_name);
    input2 = leerMatriz(input2_name);
	
	Nbits1 = input1->N_filas*input1->N_columnas;
    Nbits2 = input2->N_filas*input2->N_columnas;
    
    bitstring1 = mallocBinario(Nbits1);
    bitstring2 = mallocBinario(Nbits2);
    
    contador=0;
    for(i=0; i<input1->N_filas; i++)
    {
        for(j=0; j<input1->N_columnas; j++)
        {
            if(input1->elemento[i][j]<0)
                bitstring1->digito[contador]='0';
            else
                bitstring1->digito[contador]='1';
            
            contador++;
        }
    }
    
    contador=0;
    for(i=0; i<input2->N_filas; i++)
    {
        for(j=0; j<input2->N_columnas; j++)
        {
            if(input2->elemento[i][j]<0)
                bitstring2->digito[contador]='0';
            else
                bitstring2->digito[contador]='1';
            
            contador++;
        }
    }
	
	if(verbose)
        printf("\ncorrelacion binaria = %lg\n", correlacionBitstring(bitstring1, bitstring2));
    else
        printf("%lg", correlacionBitstring(bitstring1, bitstring2));


    freeMatriz(input1);
    freeMatriz(input2);
    freeBinario(bitstring1);
    freeBinario(bitstring2);

    return 0;
}
