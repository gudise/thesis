#include "include/main.h"



int main(int N_opcion, char** opcion)
{
    int Nprec = 8*sizeof(double);
    mpfr_t input;
    mpfr_t result;

//prologo
	//lectura de opciones
    if( strcmp(opcion[1], "-help")==0 )
    {
        printf("\nexp2 x [prec], esta funcion calcula 2^x. El calculo se realiza con precision 'prec' (medido en numero de bits. Si no se pasa la cantidad 'prec' la precision sera 'double'\n");
            
        exit(0);
    }
    
    if(N_opcion>2) sscanf(opcion[2], "%d", &Nprec);
    
    
    //setting
    mpfra_set_default_prec(Nprec);
    
    mpfr_init(input);
    mpfr_init(result);
    
    mpfra_set_str(input, opcion[1], 10);
    
    mpfr_exp2(result, input, MPFR_RNDN);
    
    mpfra_out_str(stdout, 10, 6,result);
    
    mpfr_clear(input);
    
    mpfr_clear(result);
    

    return 0;
}

