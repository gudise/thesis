#include <main.h>
#include <digital.h>

#define N_1             1
#define one_out_of_2    2
#define All_pairs       3
#define k_modular       4


int main(int N_opcion, char** opcion)
{
    char input[1024]={"input.mtz"}, output[1024]={"output.pex"}, topolfile[256]={"topol.cmt"};
    long i, j, k, l, n, N_retos=1, N_instancias=1, N_repeticiones=1, N_items=-1, topol=-1, k_mod=3, N_bits, contador=0;
    double daux;
    FILE *punte;
    MATRIZ *entrada, *data, *digit_data, *digit_retos;
    PUFEXP* experimento;
    CMTOPOL* topologia;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa toma una matriz '.mtz' y lo convierte a formato '.pex' usado para describir experimentos de PUFs. Este programa esta pensado para procesar los archivos producidos en las simulaciones o los experimentos, que típicamente estaran en formato de matriz. El programa garantiza que el numero de filas de la matriz de entrada sea = Nretos x Ninst x Nrep. Si esta propiedad no se cumple, el programa tomara los valores (Nretos=1, Ninst=1, Nrep=input->N_filas). Ademas la matriz de entraad tiene que estar bien formateada respetando el orden de los bucles de filas: el mas exterior el bucle de Nretos, el intermedio el bucle de Ninst, y el mas interno el bucle Nrep. El numero de items del experimento sera igual al numero de columnas de la matriz de entrada o, si la opcion -topol esta presente, se deducira de la topologia seleccionada\n----------------------------------------------------------------------\n\n");
            printf("-in [input.mtz], nombre del archivo de entrada\n\n");
            printf("-out [output.pex], nombre del archivo de salida\n\n");
            printf("-Nretos [1], numero de retos (por defecto se tomara 1\n\n");
            printf("-Ninst [1], numero de instancias (por defecto se tomara 1)\n\n");
            printf("-Nrep [1], numero de repeticiones (por defecto se tomara input->N_filas)\n\n");
            printf("-Nitems [input->N_columnas], numero de elementos que componen la respuesta (por defecto se tomara input->N_columnas)\n\n");
            printf("-topol [1|2|3|4|file [topol.cmt]], si esta opcion esta presente indica topologia que se utilizara para digitalizar la matriz de entrada. Actualmente las posibilidades son:\n\t1 --> N-1\n\t2 --> 1-out-of-2\n\t3 --> All-pairs\n\t4 [k] --> k-modular, esta opcion debe ir acompanada del valor 'k' (por defecto k=3). Tambien puede anadirse la palabra clave 'file' acompanada (opcionalmente) del nombre del fichero que contiene la topologia en formato '.cmt'. Si no se incluye un nombre, por defecto se buscara el archivo 'topol.cmt'\n\n");
            
            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
            sscanf(opcion[i+1], "%s", input);
        
        if(strcmp(opcion[i], "-out")==0)
            sscanf(opcion[i+1], "%s", output);
        
        if(strcmp(opcion[i], "-Nretos")==0)
            sscanf(opcion[i+1], "%ld", &N_retos);
        
        if(strcmp(opcion[i], "-Ninst")==0)
            sscanf(opcion[i+1], "%ld", &N_instancias);
        
        if(strcmp(opcion[i], "-Nrep")==0)
            sscanf(opcion[i+1], "%ld", &N_repeticiones);
        
        if(strcmp(opcion[i], "-Nitems")==0)
            sscanf(opcion[i+1], "%ld", &N_items);        
        
		if(strcmp(opcion[i], "-topol")==0)
        {
            if(strcmp(opcion[i+1], "file")==0)
            {
                if(i+2 < N_opcion)
                {
                    if(opcion[i+2][0]!='-')
                    {
                        sscanf(opcion[i+2], "%s", topolfile);
                        topol=0;
                    }
                }
            }
            else
            {
                sscanf(opcion[i+1], "%ld", &topol);
                if(topol==k_modular)
                    sscanf(opcion[i+2], "%ld", &k_mod);
            }
        }
    }
    
    entrada = leerMatriz(input);
    
    if(N_items<0)
        N_items = entrada->N_columnas;

    if(N_retos*N_instancias*N_repeticiones != entrada->N_filas)
    {
        N_retos = 1;
        N_instancias=1;
        N_repeticiones=entrada->N_filas;
    }
    
    data = mallocMatriz(entrada->N_filas, N_items);
    for(i=0; i<data->N_filas; i++)
    {
        for(j=0; j<data->N_columnas; j++)
            data->elemento[i][j] = entrada->elemento[i][j];
    }
    
    if(topol>=0)
    {
        switch(topol)
        {
            case 0: 
                topologia=leerCmtopol(topolfile);
                N_bits = topologia->N_bits;
                break;
                
            case N_1:  N_bits = data->N_columnas-1;
            break;
            
            case one_out_of_2: N_bits = data->N_columnas/2;
            break;
            
            case All_pairs: N_bits = (data->N_columnas*(data->N_columnas-1))/2;
            break;
            
            case k_modular: N_bits = (k_mod-1)*data->N_columnas/2;
            break;
        }
        
        digit_data = mallocMatriz(data->N_filas, N_bits);
        digit_retos = mallocMatriz(data->N_filas, 0); // por ahora NO esta implementada la forma de incluir retos
                
        for(i=0; i<digit_data->N_filas; i++)
        {
            switch(topol)
            {
                case 0:
                    contador=0;
                    
                    for(j=0; j<data->N_columnas; j++)
                    {
                        for(k=0; k<data->N_columnas; k++)
                        {
                            if(topologia->matriz[j][k]=='o')
                            {
                                daux = data->elemento[i][j]-data->elemento[i][k];
                            
                                if(daux<0) digit_data->elemento[i][contador] = 0;
                                else digit_data->elemento[i][contador] = 1; 
                            
                                contador++;
                            }
                        }
                    }
                    
                    freeCmtopol(topologia);
                    
                    break;
                    
                    
                case N_1:
                    contador=0;
                    for(j=0; j<data->N_columnas-1; j++)
                    {
                        daux = data->elemento[i][j] - data->elemento[i][j+1];
                        
                        if(daux<0) digit_data->elemento[i][contador] = 0;
                        else digit_data->elemento[i][contador] = 1;
                        
                        contador++;
                    }
                    break;
                    
                    
                case one_out_of_2:
                    contador=0;
                    for(j=0; j<data->N_columnas; j+=2)
                    {
                        daux = data->elemento[i][j] - data->elemento[i][j+1];
                        
                        if(daux<0) digit_data->elemento[i][contador] = 0;
                        else digit_data->elemento[i][contador] = 1;
                        
                        contador++;
                    }
                    break;
                    
                    
                case All_pairs:
                    contador=0;
                    for(j=0; j<data->N_columnas; j++)
                    {
                        for(k=j+1; k<data->N_columnas; k++)
                        {
                            daux = data->elemento[i][j]-data->elemento[i][k];
                            
                            if(daux<0) digit_data->elemento[i][contador] = 0;
                            else digit_data->elemento[i][contador] = 1; 
                            
                            contador++;
                        }
                    }
                    break;
                    
                    
                case k_modular:
                    contador=0;
                    for(j=0; j<data->N_columnas/k_mod; j++)
                    {
                        for(k=0; k<k_mod; k++)
                        {
                            for(l=k+1; l<k_mod; l++)
                            {
                                daux = data->elemento[i][j*k_mod+k]-data->elemento[i][j*k_mod+l];
                                
                                if(daux<0) digit_data->elemento[i][contador] = 0;
                                else digit_data->elemento[i][contador] = 1;
                                
                                contador++;
                            }
                        }
                    }
                    break;
            }
        }
        
        experimento = mtz_to_pex(digit_retos, digit_data, N_retos, N_instancias, N_repeticiones);
    }
    else
        experimento = mtz_to_pex(digit_retos, data, N_retos, N_instancias, N_repeticiones);

	
	punte = fopen(output,"w");
	pintarPufexp(experimento, punte);
	fclose(punte);
	
	freeMatriz(data);
    if(topol>=0) freeMatriz(digit_data);
	freePufexp(experimento);
    freeMatriz(entrada);

    return 0;
}
