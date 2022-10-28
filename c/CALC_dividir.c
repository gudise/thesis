#include "include/main.h"



int main(int N_opcion, char** opcion)
{   
    int Nprec=8*sizeof(double);
    mpfr_t input;
    mpfr_t result;



//prologo
	//lectura de opciones
    if( strcmp(opcion[1], "-help")==0 )
    {
        printf("\ndividir x y [prec], este programa calcula la division x/y. El calculo se realiza con precision 'prec' (medido en numero d ebits. Si no se pasa la cantidad 'prec' la precision sera 'double'\n");
            
        exit(0);
    }
    
    if(N_opcion>3)
        sscanf(opcion[3], "%d", &Nprec);
    
    //setting
    mpfra_set_default_prec(Nprec);
    
    mpfr_init(input);
    mpfr_init(result);
    
    mpfra_set_str(result, opcion[1], 10);
    mpfra_set_str(input, opcion[2], 10);
    
    mpfra_div(result, result, input);
    
    mpfra_out_str(stdout, 10, 6,result);
    
    mpfr_clear(input);
    
    mpfr_clear(result);
    

    return 0;
}

