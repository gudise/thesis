#include <main.h>
#include <digital.h>


int main(int N_opcion, char** opcion)
{
    long i, j, k, l, N_claves, N_items, N_items_new, Nbits=32, puntero, N_instancias=1, N_baraja=1, *index_array;
    double factor=1.0, aux;
    char input[1024]={"putty.log"}, output[1024], formato=0, putty=0, barajar=0, filas_name[1024]={"putty_promedio_claves.txt"}, columnas_name[1024]={"putty_promedio_items.txt"}, promedio_filas=0, promedio_columnas=0, new_putty=0, partir=0, Ninst=0;
    FILE *punte;
	MATRIZ* data, *promedio, *varianza;
    BINARIO* bitstring;

    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nEste programa calcula la frecuencia promediada sobre 'N_claves' medidas de frecuencia para 'N_items' diferentes. El formato 'putty' debe ser:\n\n\tclave 1) item_1 item_2 ... item_N\n\tclave 2) item_1 item_2 ... item_N\n\t.\n\t.\n\t.\n\tclave M) item_1 item_2 ... item_N\n----------------------------------------------------------------------\n\n");
            printf("-in [putty.log], \n\n");
            printf("-formato {[decimal], [binario (32)]}, formato del sistema de numeracion de los datos de entrada. Si se utiliza el formato binario por defecto se esperaran palabras de 32 bits, sin embargo esto puede cambiarse anadiendo un numero (opcional) inmediatamente despues de 'binario'\n\n");
			printf("-promedio_claves [\"putty_promedio_claves.txt\"], esta opcion promedia las 'N_claves' filas del archivo 'putty'. El formato de salida estandar sera una tabla con cuatro columnas: indice de item \\t promedio \\t desviacion tipica \\t error tipico. Si la opcion '-p' esta presente, el formato de salida sera en su lugar un archivo 'putty' con N_claves=1\n\n");
			printf("-promedio_items [\"putty_promedio_items.txt\"], esta opcion promedia las 'N_items' columnas del archivo 'putty'. El formato de salida estandar sera una tabla con cuatro columnas: indice de item \\t promedio \\t desviacion tipica \\t error tipico. Si la opcion '-p' esta presente, el formato de salida sera en su lugar un archivo 'putty' con N_items=1\n\n");
			printf("-p [putty out name], si esta opcion esta presente el formato de salida sera en formato 'putty'. Si despues de la opcion 'p' se introduce el nombre de un fichero, el programa escribira en formato decimal el fichero putty leido\n\n");
			printf("-barajar [1], si esta opcion esta presente el programa barajara la salida en formato 'putty'(siempre que se haya incluido la opcion '-p') antes de escribirla. Opcionalmente puede indicarse el numero de permutaciones que se escribira. Cada permuacion sera equiprobable\n\n");
			printf("-factor [1.0], esta opcion permite indicar un factor multiplicativo que se aplicara a todos los elementos leidos de 'putty'. Utilizar para cambios de unidades\n\n");
            printf("-partir [N_bits], si esta opcion esta presente el programa partira las claves leidas en trozos de tamano 'N_bits', i.e. por cada clave leida escribira N_items/N_bits claves de longitud N_bits cada una, antes de escribirlas en formato putty (por lo tanto solo tiene efecto si la opcion '-p' esta presente). La accion de partir las claves es la ultima en tener lugar (es decir que la accion de barajar ocurre primero). Notar tambien que esta opcion no tiene sentido en el caso de -'promedio_items', ya que ahi las claves son de longitud unidad (en ese caso se ignora la opcion '-partir'). Alternativamente puede escribirse la etiqueta 'Ninst' despues de '-partir', en cuyo caso el programa escribira N_bits claves de tamano N_items/N_bits cada una. e.g. si introducimos un fichero putty con una unica clave de 99 items:\n\t-partir 33 --> escribira 3 claves de 33 bits\n\t-partir Ninst 33 --> escribir 33 claves de 3 bits cada una\n\n");

            return 0;
        }

        if(strcmp(opcion[i], "-in")==0)
            sprintf(input, "%s", opcion[i+1]);
        
        if(strcmp(opcion[i], "-formato")==0)
        {
            if(strcmp(opcion[i+1], "binario")==0)
            {
                formato=1;
                if(i+2<N_opcion)
                {
                    if(opcion[i+2][0]!='-')
                        sscanf(opcion[i+1], "%ld", &Nbits);
                }
            }
        }

        if(strcmp(opcion[i], "-promedio_claves")==0)
        {
			promedio_filas=1;
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
					sscanf(opcion[i+1], "%s", filas_name); 
            }
        }
		
        if(strcmp(opcion[i], "-promedio_items")==0)
        {
			promedio_columnas=1;
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
					sscanf(opcion[i+1], "%s", columnas_name);
            }
        }
        
        if(strcmp(opcion[i], "-p")==0)
		{
			putty=1;
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
				{
					sscanf(opcion[i+1], "%s", output);
					new_putty=1;
				}
            }
		}
		
        if(strcmp(opcion[i], "-factor")==0)
            sscanf(opcion[i+1], "%lf", &factor);
			
        if(strcmp(opcion[i], "-barajar")==0)
		{
            barajar=1;
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
					sscanf(opcion[i+1], "%ld", &N_baraja); 
            }
		}
		
        if(strcmp(opcion[i], "-partir")==0)
		{
            partir=1;
            if(strcmp(opcion[i+1], "Ninst")!= 0)
                sscanf(opcion[i+1], "%ld", &N_items_new);
            else
            {
                sscanf(opcion[i+2], "%ld", &N_instancias);
                Ninst=1;
            }
		}
    }

    puntero=leerParametrosPutty(&N_claves, &N_items, input);
    
    if(partir)
    {
        if(Ninst)
            N_items_new=N_items/N_instancias;
        else
            N_instancias=N_items/N_items_new;
    }
	
	data = mallocMatriz(N_items, N_claves);

    punte = fopen(input, "r");

	fseek(punte, puntero, SEEK_SET);
      
    if(formato==0) //entrada decimal
    {
        for(i=0; i<N_claves; i++)
        {
            for(j=0; j<N_items;j++)
			{
                fscanf(punte, "%lf", &(data->elemento[j][i]));
				data->elemento[j][i]*=factor;
			}
        }
    }
    else //entrada binaria
    {
        bitstring = mallocBinario(Nbits);
        
        for(i=0; i<N_claves; i++)
        {
            for(j=0; j<N_items;j++)
			{
                fscanf(punte, "%s", bitstring->digito);
                data->elemento[j][i] = binarioAdouble(bitstring, 1);
				data->elemento[j][i]*=factor;
            }
        }
    }
	
	//Aqui ya hemos leido el archivo 'putty'
	
	if(promedio_filas)
	{
		promedio = mallocMatriz(N_items, 1);
		for(i=0; i<N_items; i++)
			promedio->elemento[i][0] = 0;
		
		for(i=0; i<N_claves; i++)
		{
			for(j=0; j<N_items; j++)
				promedio->elemento[j][0]+=data->elemento[j][i];
		}
		
		for(i=0; i<N_items; i++)
			promedio->elemento[i][0]/=N_claves;
			
		varianza = mallocMatriz(N_items, 1);
		for(i=0; i<N_items; i++)
			varianza->elemento[i][0] = 0;
			
		for(i=0; i<N_claves; i++)
		{
			for(j=0; j<N_items; j++)
				varianza->elemento[j][0]+=(data->elemento[j][i]-promedio->elemento[j][0])*(data->elemento[j][i]-promedio->elemento[j][0]);
		}
		
		for(i=0; i<N_items; i++)
			varianza->elemento[i][0]/=(N_claves-1);
			
			
		punte=fopen(filas_name, "w");
		if(putty)
		{
			index_array = malloc(sizeof(long)*N_items);
			for(i=0; i<N_items; i++)
				index_array[i] = i;
			
			fprintf(punte, "--archivo putty creado sobre un promedio de filas--\nN_claves = %ld\n", N_baraja* N_instancias);
			
			for(i=0; i<N_baraja; i++)
			{
				if(barajar)
					gsl_ran_shuffle(idum, index_array, N_items, sizeof(long));
				if(partir)
                {
                    for(j=0; j<N_instancias; j++)
                    {
                        for(k=0; k<N_items_new; j++)
                            fprintf(punte, "%lf ",promedio->elemento[index_array[j*N_items_new+k]][0]); 
                        fprintf(punte, "\n");
                    }
                }
                else
                {
                    for(j=0; j<N_items; j++)
                        fprintf(punte, "%lf ",promedio->elemento[index_array[j]][0]);
                    fprintf(punte, "\n");
                }
			}
			fprintf(punte, "-fin-\n");
			
			free(index_array);
		}
		else
		{
			fprintf(punte, "Item\tpromedio\tdesviacion\terror\n");
			for(i=0; i<N_items; i++)
				fprintf(punte, "%ld\t%lf\t%lf\t%lf\n",i+1, promedio->elemento[i][0], sqrt(varianza->elemento[i][0]), sqrt(varianza->elemento[i][0])/sqrt(N_claves));
		}
		fclose(punte);
		
		freeMatriz(promedio);
		freeMatriz(varianza);
	}
	
	if(promedio_columnas)
	{
		promedio = mallocMatriz(N_claves, 1);
		
		for(i=0; i<N_claves; i++)
			promedio->elemento[i][0] = 0;
		
		for(i=0; i<N_claves; i++)
		{
			for(j=0; j<N_items; j++)
				promedio->elemento[i][0]+=data->elemento[j][i];
		}
		
		for(i=0; i<N_claves; i++)
			promedio->elemento[i][0]/=N_items;
			
		varianza = mallocMatriz(N_claves, 1);
		for(i=0; i<N_claves; i++)
			varianza->elemento[i][0] = 0;
			
		for(i=0; i<N_claves; i++)
		{
			for(j=0; j<N_items; j++)
				varianza->elemento[i][0]+=(data->elemento[j][i]-promedio->elemento[i][0])*(data->elemento[j][i]-promedio->elemento[i][0]);
		}
		
		for(i=0; i<N_claves; i++)
			varianza->elemento[i][0]/=(N_items-1);
			
			
		punte=fopen(columnas_name, "w");
		if(putty)
		{
			index_array = malloc(sizeof(long)*N_items);
			for(i=0; i<N_items; i++)
				index_array[i] = i;
				
			fprintf(punte, "--archivo putty creado sobre un promedio de filas--\nN_claves = %ld\n", N_claves*N_baraja);
			
			for(i=0; i<N_baraja; i++)
			{
				if(barajar)
					gsl_ran_shuffle(idum, index_array, N_items, sizeof(long));
			
				for(j=0; j<N_claves; j++)
					fprintf(punte, "%lf\n",promedio->elemento[index_array[j]][0]);
			}
			fprintf(punte, "-fin-\n");
			
			free(index_array);
		}
		else
		{
			fprintf(punte, "Item\tpromedio\tdesviacion\terror\n");
			for(i=0; i<N_claves; i++)
				fprintf(punte, "%ld\t%lf\t%lf\t%lf\n",i+1, promedio->elemento[i][0], sqrt(varianza->elemento[i][0]), sqrt(varianza->elemento[i][0])/sqrt(N_items));
		}
		fclose(punte);
		
		freeMatriz(promedio);
		freeMatriz(varianza);
	}
	
	if(new_putty)
	{
		punte=fopen(output, "w");
		
		index_array = malloc(sizeof(long)*N_items);
		for(i=0; i<N_items; i++)
			index_array[i] = i;
		
		fprintf(punte, "--archivo putty creado por 'manipularPutty'--\nN_claves = %ld\n", N_claves*N_baraja*N_instancias);
		for(i=0; i<N_claves; i++)
		{
			for(j=0; j<N_baraja; j++)
			{
				if(barajar)
					gsl_ran_shuffle(idum, index_array, N_items, sizeof(long));
                
                if(partir)
                {
                    for(k=0; k<N_instancias; k++)
                    {
                        for(l=0; l<N_items_new; l++)
                            fprintf(punte, "%lf ", data->elemento[index_array[k*N_items_new+l]][i]);
                        fprintf(punte, "\n");
                    }
                }
                else
                {
                    for(k=0; k<N_items; k++)
                        fprintf(punte, "%lf ", data->elemento[index_array[k]][i]);
                    fprintf(punte, "\n");
                }
			}
		}
		fprintf(punte, "-fin-\n");
		
		free(index_array);
	
		fclose(punte);
		
	}


    freeMatriz(data);
    if(formato==1) freeBinario(bitstring);

    return 0;
}
