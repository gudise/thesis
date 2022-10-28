#include "main.h"
#include "matematicas.h"

int main(int N_opcion, char** opcion)
{
    int n, i, j, N=10000;
    char input[256]={"input.mtz"}, verbose=1;
    double A=0, tau=1, b=0, A_new, tau_new, b_new, coste, coste_new, A_grad, tau_grad, b_grad, aux;
	MATRIZ* entrada;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa calcula el mejor ajuste exponencial y=A*exp(x/tau)+b para los adtos de entrada. Los datos de entrada se introduciran en forma de matriz con dos columnas: la primera representa el eje 'x', y la segunda el eje 'y' (si la matriz introducida tiene mas columnas, se ignoraran todas salvo las dos\n----------------------------------\nOpciones:\n\n");
            printf("-in [input.mtz]  fichero con la matriz de entrada (ver 'doc/my_doc')\n\n");
            printf("-nv, si esta eopcion esta presente el programa solo pintara un valor: la longitud de correlacion. Esta opcion esta pensada para usarse en shell scripts que lean directamente la salida de este programa a una variable\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
            sprintf(input, "%s", opcion[i+1]);
        
        if( strcmp(opcion[i], "-nv")==0 )
            verbose=0;
    }
    
	
	entrada = leerMatriz(input);
    
    for(n=0; n<10000; n++)
    {
        tau = (entrada->elemento[1][0]-entrada->elemento[0][0])/log((entrada->elemento[0][1]-b)/(entrada->elemento[1][1]-b));
        A = (entrada->elemento[0][1]-b)*exp(entrada->elemento[0][0]/tau);
//         coste=0;
//         for(i=0; i<entrada->N_filas; i++)
//         {
//             aux=A*exp(-entrada->elemento[i][0]/tau)+b-entrada->elemento[i][1];
//             coste+=aux*aux*exp(-10*i);
//         }
//         A_new = A+ran1_gauss(&aux); //(2*ran1()-1);
//         coste_new=0;
//         for(i=0; i<entrada->N_filas; i++)
//         {
//             aux=A_new*exp(-entrada->elemento[i][0]/tau)+b-entrada->elemento[i][1];
//             coste_new+=aux*aux*exp(-10*i);
//         }
//         if(coste_new<coste)
//             A=A_new;
        
//         coste=0;
//         for(i=0; i<entrada->N_filas; i++)
//         {
//             aux=A*exp(-entrada->elemento[i][0]/tau)+b-entrada->elemento[i][1];
//             coste+=aux*aux;
//         }
//         tau_new = tau+ran1_gauss(&aux); //(2*ran1()-1);
//         coste_new=0;
//         for(i=0; i<entrada->N_filas; i++)
//         {
//             aux=A*exp(-entrada->elemento[i][0]/tau_new)+b-entrada->elemento[i][1];
//             coste_new+=aux*aux;
//         }
//         if(coste_new<coste)
//             tau=tau_new;
        
        coste=0;
        for(i=0; i<entrada->N_filas; i++)
        {
            aux=A*exp(-entrada->elemento[i][0]/tau)+b-entrada->elemento[i][1];
            coste+=aux*aux;
        }
        b_new = b+ran1_gauss(&aux); //(2*ran1()-1);
        coste_new=0;
        for(i=0; i<entrada->N_filas; i++)
        {
            aux=A*exp(-entrada->elemento[i][0]/tau)+b_new-entrada->elemento[i][1];
            coste_new+=aux*aux;
        }
        if(coste_new<coste)
            b=b_new;
        
        
        if(coste_new<0.001) break;
    } //con este codigo encontramos unos valores iniciales decentes
    
    for(n=0; n<1000; n++)
    {
        A_grad=0;
        tau_grad=0;
        b_grad=0;
        coste=0;
        for(i=0; i<entrada->N_filas; i++)
        {
            aux = exp(-entrada->elemento[i][0]/tau);
            
            A_grad+=aux*aux*(A+(b-entrada->elemento[i][1])*exp(entrada->elemento[i][0]/tau));
            
            tau_grad+=A*entrada->elemento[i][0]*aux*aux*(A+(b-entrada->elemento[i][1])*exp(entrada->elemento[i][0]))/(tau*tau);
            
            b_grad+=A*aux+b-entrada->elemento[i][1];
            
            coste+=(A*aux+b-entrada->elemento[i][1])*(A*aux+b-entrada->elemento[i][1])/(2*entrada->N_filas);
        }
        
        A_grad/=entrada->N_filas;
        tau_grad/=entrada->N_filas;
        b_grad/=entrada->N_filas;
        
        if(sqrt(A_grad*A_grad+tau_grad*tau_grad+b_grad*b_grad)<0.00001)
            break;
        
        A = A-0.001*A_grad;
        tau = tau-0.001*tau_grad;
        b = b-0.001*b_grad;
    } //descenso de gradiente
    

	freeMatriz(entrada);
    
    if(verbose)
        printf("\ny = A*exp(-x/tau)+b:\n\n\tA = %lg\n\ttau = %lg\n\tb = %lf\n\ncoste = %lg\n", A, tau, b, coste);
    else
        printf("%lg", tau);
	
    return 0;
}
