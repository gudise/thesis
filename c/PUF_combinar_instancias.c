#include <main.h>
#include <digital.h>

#define N_1             0
#define one_out_of_2    1
#define All_pairs       2
#define k_modular       3


int main(int N_opcion, char** opcion)
{
    char input[1024][256], output[256]={"output.pex"};
    long i, j, k, l, n, N_retos=0, N_instancias=0, N_repeticiones=0, tamano_reto=0, tamano_respuesta=0, contador=0;
    double daux;
    FILE *punte;
    PUFEXP **data, *experimento;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nDado que habitualmente se mide cada chip por separado y los resultados se guardan en formato '.pex', este programa esta pensado para integrar las medidas de varios chips en un unico archivo '.pex'. Este programa toma un conjunto de ficheros '.pex' tales que cada uno contiene 'N_retos', 'N_repeticiones' y una unica instancia 'N_instancias'=1, y los integra en un unico fichero '.pex' de 'N_retos', 'N_repeticiones' y tantas instancias como ficheros se introduzcan en '-in'. Los nombres de lo archivos de entrada deben estar separados por un espacio. Los archivos introducidos deben contener el mismo numero de retos y repeticiones.\n----------------------------------------------------------------------\n\n");
            printf("-in , nombres de los archivos de entrada\n\n");
            printf("-out [output.pex], nombre del archivo de salida\n\n");
            
            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
        {
            for(j=0; j<1024; j++)
            {
                if(i+j<N_opcion-1)
                {
                    if(opcion[i+1+j][0]!='-')
                    {
                        sscanf(opcion[i+1+j], "%s", input[j]);
                        N_instancias++;
                    }
                    else break;
                }
                else break;
            }
        }
        
        if(strcmp(opcion[i], "-out")==0)
            sscanf(opcion[i+1], "%s", output);
        
        if(strcmp(opcion[i], "-Nretos")==0)
            sscanf(opcion[i+1], "%ld", &N_retos);
        
        if(strcmp(opcion[i], "-Nrep")==0)
            sscanf(opcion[i+1], "%ld", &N_repeticiones);
    }

    data = (PUFEXP**)malloc(sizeof(PUFEXP*)*N_instancias);
    for(i=0; i<N_instancias; i++)
        data[i] = leerPufexp(input[i]); 
    N_retos = data[0]->N_retos;
    N_repeticiones = data[0]->N_repeticiones;
    tamano_reto = data[0]->tamano_reto;
    tamano_respuesta = data[0]->tamano_respuesta;
    
    experimento = mallocPufexp(N_retos, N_instancias, N_repeticiones, tamano_reto, tamano_respuesta);
    for(i=0; i<N_retos; i++)
    {
        for(j=0; j<N_instancias; j++)
        {
            for(k=0; k<N_repeticiones; k++)
            {
                for(l=0; l<tamano_reto; l++)
                    experimento->elementox[i][j][k][l] = data[j]->elementox[i][0][k][l];
                
                for(l=0; l<tamano_respuesta; l++)
                    experimento->elementoy[i][j][k][l] = data[j]->elementoy[i][0][k][l];
            }
        }
    }
	
	punte = fopen(output,"w");
	pintarPufexp(experimento, punte);
	fclose(punte);

	freePufexp(experimento);

    return 0;
}
