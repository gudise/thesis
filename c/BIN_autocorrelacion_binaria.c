#include <main.h>
#include <digital.h>


int main(int N_opcion, char** opcion)
{
    long    i, j, contador, Nbits1, Nbits2, dist_min=1, dist_max=1;
    char    input_name[256]={"input1.mtz"}, verbose=1;
    BINARIO* bitstring1, *bitstring2;
    MATRIZ* input;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa calcula la autocorrelacion de una palabra binaria introducida en forma de mstriz (.mtz) para un rango de distancias de correlacion. En este formato, cada elemento de la matriz sera un bit (1 si >0, 0 si <0); la matriz de entrada se considera un unico bitstring de tamano N_filas x N_columnas bits.\n\n----------------------------------------------------------------------\n\n");
            printf("-in [input.mtz], \n\n");
            printf("-min [1], distancia minima (por defecto 1.\n\n");
            printf("-max [1], distancia maxima (por defecto 1.\n\n");            
            printf("-nv, no verbose\n\n");

            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
            sprintf(input_name, "%s", opcion[i+1]);
        
        if(strcmp(opcion[i], "-min")==0)
            sscanf(opcion[i+1], "%ld", &dist_min);
        
        if(strcmp(opcion[i], "-max")==0)
            sscanf(opcion[i+1], "%ld", &dist_max);
                  
        if(strcmp(opcion[i], "-nv")==0)
            verbose = 0;
    }
    
    input = leerMatriz(input_name);
	
	Nbits1 = input->N_filas*input->N_columnas;
    Nbits2 = Nbits1;
    
    bitstring1 = mallocBinario(Nbits1);
    bitstring2 = mallocBinario(Nbits2);
    
    contador=0;
    for(i=0; i<input->N_filas; i++)
    {
        for(j=0; j<input->N_columnas; j++)
        {
            if(input->elemento[i][j]<0)
                bitstring1->digito[contador]='0';
            else
                bitstring1->digito[contador]='1';
            
            contador++;
        }
    }
    if(verbose)
        printf("dist\tautocorr\n");
    
    for(i=dist_min; i<=dist_max; i++)
    {
        for(j=0; j<bitstring1->Nbits; j++)
            bitstring2->digito[j] = bitstring1->digito[(j+i)%bitstring1->Nbits];
        
        if(verbose)
            printf("%ld\t%lg\n", i, correlacionBitstring(bitstring1, bitstring2));
        else
            printf("%lg", correlacionBitstring(bitstring1, bitstring2));
    }
    

    freeMatriz(input);
    freeBinario(bitstring1);
    freeBinario(bitstring2);

    return 0;
}
