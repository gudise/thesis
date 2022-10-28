
#include "main.h"
#include "matematicas.h"
#include "digital.h"
#include "redneuronal.h"


int main(int N_opcion, char** opcion)
{
    int i, j, k, l, n, Nretos=1, Ninstancias=1, Nrepeticiones=1, Ncell=1, parvec=0;
    char output[256]={"putty.pex"}, colapsar=0;
    double aux, delay_total_a, delay_total_b, bitout;
    double cteT=0;
    MATRIZ **cell_delay, **reto, **parity;
    PUFEXP* experimento;
    FILE* punte;

	//lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nPrograma de simulacion y analisis de PUF de arbitro: este programa genera un fichero .mtz (y/o un par de ficheros crpx, crpy) con un conjunto de retos-respuestas. Los retos son generados aleatoriamente, y las respuestas correspondientes producidas en funcion del modelo de APUF. Los valores numericos de este modelo (i.e., los retardos de cada fase) tambien son producidos aleatoriamente\n----------------------------------\n\n");
            printf("-out [\"putty.pex\"], si esta opcion esta presente el programa escribe todos los pares reto-respuesta producidos en un fichero putty simulado en formato '.pex'\n\n");
            printf("-Ncell [1], tamano de la matriz PUF en la direccion X\n\n");
            printf("-Nretos [1], numero de retos (permutaciones) utilizadas para cada instancia\n\n");
            printf("-Ninst [1]\n\n");
            printf("-Nrep [1]\n\n");
			printf("-cteT [0.0], constante de acoplo termico. Este valor se utiliza como factor para generar un numero aleatorio que modifica el valor nominal de cada fase de APUF.\n\n");
            printf("-parvec, si esta opcion esta presente se escribe el vector de paridad en lugar del reto\n\n");
            printf("-colapsar, si esta opcion esta presente la simulacion utiliza toda la secuencia de 'Nretos' retos como si fuera uno solo, dando lugar a una respuesta de 'Nretos' bits. En este caso el numero de retos efectivos siempre sera 1\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-out")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
                    sscanf(opcion[i+1], "%s", output);
            }
        }
        
        if( strcmp(opcion[i], "-Ncell")==0 )
            sscanf(opcion[i+1], "%d", &Ncell);
        
        if( strcmp(opcion[i], "-Nretos")==0 )
            sscanf(opcion[i+1], "%d", &Nretos);
        
        if( strcmp(opcion[i], "-Ninst")==0 )
            sscanf(opcion[i+1], "%d", &Ninstancias);
        
        if( strcmp(opcion[i], "-Nrep")==0 )
            sscanf(opcion[i+1], "%d", &Nrepeticiones);
        
        if( strcmp(opcion[i], "-cteT")==0 )
            sscanf(opcion[i+1], "%lf", &cteT);
        
        if( strcmp(opcion[i], "-parvec")==0 )
            parvec=1;
        
        if( strcmp(opcion[i], "-colapsar")==0 )
            colapsar=1;
    }

    if(colapsar)
        experimento = mallocPufexp(1, Ninstancias, Nrepeticiones, 0, Nretos);
    else
        experimento = mallocPufexp(Nretos, Ninstancias, Nrepeticiones, Ncell+parvec, 1);
    
    cell_delay = malloc(sizeof(MATRIZ**)*Ninstancias);
    for(i=0; i<Ninstancias; i++)
    {
        cell_delay[i] = mallocMatriz(Ncell, 2);
        
        for(j=0; j<Ncell; j++)
        {
            cell_delay[i]->elemento[j][0] = ran1(); //distribucion plana
            cell_delay[i]->elemento[j][1] = ran1();
        }
    }
    
	reto = (MATRIZ**)malloc(sizeof(MATRIZ*)*Nretos);
    for(i=0; i<Nretos; i++)
    {
		reto[i] = mallocMatriz(Ncell, 1);
        for(j=0; j<Ncell; j++)
        {
            if(dado(2))
                reto[i]->elemento[j][0]=1;
            else
                reto[i]->elemento[j][0]=-1;
        }
    }
    
    if(parvec)
    {
        parity = (MATRIZ**)malloc(sizeof(MATRIZ*)*Nretos);
        for(i=0; i<Nretos; i++)
        {
            parity[i]=mallocMatriz(Ncell+1, 1);
            for(j=0; j<Ncell+1; j++)
                parity[i]->elemento[j][0]=1;
        }
    }

    for(i=0; i<Nretos; i++)
    {
        for(j=0; j<Ninstancias; j++)
        {
            for(k=0; k<Nrepeticiones; k++)
            {
                delay_total_a=0;
                delay_total_b=0;
                for(l=0; l<Ncell; l++)
                {
                    if(reto[i]->elemento[l][0]>0)
                    {
                        delay_total_a+=cell_delay[j]->elemento[l][0]+cteT*ran1_gauss(&aux);
                        delay_total_b+=cell_delay[j]->elemento[l][1]+cteT*aux; //ruido termico
                    }
                    else
                    {
                        delay_total_a+=cell_delay[j]->elemento[l][1]+cteT*ran1_gauss(&aux);
                        delay_total_b+=cell_delay[j]->elemento[l][0]+cteT*aux; //ruido termico
                    }
                }
                if(delay_total_a>delay_total_b)
                    bitout=-1;
                else
                    bitout=1;

                if(parvec)
                {
                    for(l=0; l<Ncell; l++)
                    {
                        for(n=l; n<Ncell; n++)
                        {
                            if(reto[i]->elemento[n][0]<0)
                                parity[i]->elemento[l][0]*=1;
                            else
                                parity[i]->elemento[l][0]*=-1;
                        }
                    } //calculo el vector de paridad
                }
                if(!colapsar)
                {
                    if(parvec)
                    {
                        for(l=0; l<Ncell+1; l++)
                            experimento->elementox[i][j][k][l]=parity[i]->elemento[l][0];
                        experimento->elementoy[i][j][k][0]=(double)bitout;
                    }
                    else
                    {
                        for(l=0; l<Ncell; l++)
                            experimento->elementox[i][j][k][l]=reto[i]->elemento[l][0];
                        experimento->elementoy[i][j][k][0]=(double)bitout;  
                    }
                }
                else
                    experimento->elementoy[0][j][k][i]=(double)bitout;
                
            }
        }
    }

    punte = fopen(output, "w");
    pintarPufexp(experimento, punte);
    fclose(punte);

	//liberar memoria
	for(i=0; i<Ninstancias; i++)
        freeMatriz(cell_delay[i]);
	free(cell_delay);
	//freeBinario(bitstring);
    for(i=0; i<Nretos; i++)
		freeMatriz(reto[i]);
	free(reto);
    if(parvec)
    {
        for(i=0; i<Nretos; i++)
            freeMatriz(parity[i]);
        free(parity);
    }
    freePufexp(experimento);

//end
    

    return 0;
}

