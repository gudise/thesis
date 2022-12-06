#include "main.h"
#include "hardware.h"

void extraerSlice(char* result, char* target, int ini, int fin) //copia en result el slice target[ini:fin]
{
	for (int i=0; i<fin-ini; i++)
		result[i]=target[i+ini];
	result[fin-ini]=0;
}


int main(int N_opcion, char** opcion)
{
    FILE* punte;
    char out_name[256] =	{"romatrix"},
		 directriz =		'y',
		 design_name[300],
		 env[256],
		 tipo=0,
		 **pinmap,
		 pinmap_opt[256]="no",
		 minsel = 0,
		 saux[256],
		 helptxt[256];
    int N_inv =		3,
		XOS, 
		YOS,
		XOS_i =		0,
		YOS_i =		0,
		XOS_incr =	1,
		YOS_incr =	1,
		XOS_max =	20,
		YOS_max =	20,
		N_osciladores =	10,
		resolucion =	1000000,
		N_claves =		100,
		*pos_oscilador[2],
		*pos_oscilador_fijado[2],
		N_osciladores_fijados,
		contador,
		intaux,
		N_pinmap;
    
    //lectura de opciones
    for(int i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
			sprintf(helptxt, "%s/c/help/gen_romatrix.help", getenv("REPO_fpga"));
			punte = fopen(helptxt, "r");
			while(!feof(punte))
				printf("%c", fgetc(punte));
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-out")==0 )
            sscanf(opcion[i+1], "%s", out_name);
		
        if( strcmp(opcion[i], "-tipo")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
                {
                    if(strcmp(opcion[i+1], "lut1")==0)
                        tipo = 0;
                    if(strcmp(opcion[i+1], "lut2")==0)
                        tipo = 1;
                    else if(strcmp(opcion[i+1], "lut3")==0)
                        tipo = 2;
                    else if(strcmp(opcion[i+1], "lut3mr")==0)
                        tipo = 3;
                    else if(strcmp(opcion[i+1], "lut4")==0)
                        tipo = 4;
                    else if(strcmp(opcion[i+1], "lut5")==0)
                        tipo = 5;
                    else if(strcmp(opcion[i+1], "lut6")==0)
                        tipo = 6;
                    else if(strcmp(opcion[i+1], "lut6mr")==0)
                        tipo = 7;
                    else if(strcmp(opcion[i+1], "lut6_2")==0)
                        tipo = 8;
                }
            }
        }
        
        if( strcmp(opcion[i], "-pinmap")==0 )
			sprintf(pinmap_opt, "%s;", opcion[i+1]);
        
        if( strcmp(opcion[i], "-minsel")==0 )
		{
			if(i<N_opcion-1)
			{
				if(opcion[i+1][0]!='-')
				{
					if(opcion[i+1][0]=='0')
						minsel=0;
					else
						minsel=1;
				}				
			}
		}
        
        if( strcmp(opcion[i], "-directriz")==0 )
        {
            if(strcmp(opcion[i+1], "x")==0 || strcmp(opcion[i+1], "X")==0)
                directriz='x';
        }
        
        if( strcmp(opcion[i], "-Ninv")==0 )
            sscanf(opcion[i+1], "%d", &N_inv);
        
        if( strcmp(opcion[i], "-XOSincr")==0 )
            sscanf(opcion[i+1], "%d", &XOS_incr);
        
        if( strcmp(opcion[i], "-YOSincr")==0 )
            sscanf(opcion[i+1], "%d", &YOS_incr);
        
        if( strcmp(opcion[i], "-XOSmax")==0 )
            sscanf(opcion[i+1], "%d", &XOS_max);
        
        if( strcmp(opcion[i], "-YOSmax")==0 )
            sscanf(opcion[i+1], "%d", &YOS_max);
        
        if( strcmp(opcion[i], "-Nosc")==0 )
            sscanf(opcion[i+1], "%d", &N_osciladores);
		
        if( strcmp(opcion[i], "-posmap")==0 )
		{
			N_osciladores_fijados=0;
			for(int j=i+1; j>i; j++)
			{
				if(j<N_opcion)
				{
					if(opcion[j][0]=='-')
						break;
					else
						N_osciladores_fijados++;
				}
				else
					break;
			}
			pos_oscilador_fijado[0]=(int*)malloc(sizeof(int)*N_osciladores_fijados);
			pos_oscilador_fijado[1]=(int*)malloc(sizeof(int)*N_osciladores_fijados);

			for(int j=0; j<N_osciladores_fijados; j++)
				sscanf(opcion[j+i+1], "%d,%d", &pos_oscilador_fijado[0][j], &pos_oscilador_fijado[1][j]);
		}
        
        if( strcmp(opcion[i], "-resolucion")==0 )
            sscanf(opcion[i+1], "%d", &resolucion);
    } 
	// fin de lectura de opciones
    
	
	if(N_osciladores_fijados==0)
	{
		N_osciladores_fijados=1;
				
		pos_oscilador_fijado[0]=(int*)malloc(sizeof(int));
		pos_oscilador_fijado[0][0]=XOS_i;
				
		pos_oscilador_fijado[1]=(int*)malloc(sizeof(int));
		pos_oscilador_fijado[1][0]=YOS_i;
	}
	pos_oscilador[0]=(int*)malloc(sizeof(int)*N_osciladores);
	pos_oscilador[1]=(int*)malloc(sizeof(int)*N_osciladores);

    for(int i=0;i<N_osciladores_fijados;i++)
    {	
		pos_oscilador[0][i]=pos_oscilador_fijado[0][i];
		pos_oscilador[1][i]=pos_oscilador_fijado[1][i];
	}

	XOS = pos_oscilador[0][N_osciladores_fijados-1];
	YOS = pos_oscilador[1][N_osciladores_fijados-1];
    contador=0;
	for(int i=N_osciladores_fijados;i<N_osciladores;i++)
	{
		if(directriz=='x')
			XOS = XOS+XOS_incr;
		else
			YOS = YOS+YOS_incr;
		
		if(XOS>=XOS_max+1)
		{
			XOS = pos_oscilador_fijado[0][N_osciladores_fijados-1];
            
			YOS+= N_inv/4+YOS_incr;
			if(YOS>=YOS_max+1)
			{
				printf("\nERROR: en funcion 'hwd_genROmatriz'. YOSmax excedida. Como mucho se pueden implementar %d osciladores (en lugar de %d osciladores)\n", contador+N_osciladores_fijados, N_osciladores);
				exit(0);
			}
		}
		if(YOS>=YOS_max+1)
		{
			YOS = pos_oscilador_fijado[1][N_osciladores_fijados-1];
			
			XOS+=XOS_incr;            
			if(XOS>=XOS_max+1)
			{
				printf("\nERROR: en funcion 'hwd_genROmatriz'. XOSmax excedida. Como mucho se pueden implementar %d osciladores (en lugar de %d osciladores)\n", contador+N_osciladores_fijados, N_osciladores);
				exit(0);
			}
		}
		
		pos_oscilador[0][i]=XOS;
		pos_oscilador[1][i]=YOS;
        contador++;
	} // Aqui poblamos el array 'pos_oscilador', utilizando: (i)Las posiciones prefijadas 'pos_oscilador_fijado', (ii)Las cantidades OS_incr y OS_max en caso de que queden osciladores sin fijar

    sprintf(design_name,"%s.v", out_name);
    punte=fopen(design_name,"w");
    fprintf(punte,"//N_osciladores: %d\n", N_osciladores);
	
	pinmap = (char**)malloc(sizeof(char*)*N_inv);
	for(int i=0; i<N_inv; i++)
		pinmap[i]=(char*)malloc(sizeof(char)*256);

	intaux=0;
	N_pinmap=0;
	for(int i=0; i<strlen(pinmap_opt); i++)
	{
		if(pinmap_opt[i]==';')
		{
			extraerSlice(saux, pinmap_opt,intaux, i);
			intaux=i+1;
			if(strcmp(saux, "no")==0)
				sprintf(pinmap[N_pinmap], " ");
			else
				sprintf(pinmap[N_pinmap], ", LOCK_PINS=\"%s\" ", saux);
			N_pinmap++;
		}
	}
	for(int i=N_pinmap; i<N_inv; i++)
		sprintf(pinmap[i], "%s", pinmap[i-1]);
	
	switch(tipo)
	{
		case 0:
            configOsciladores(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap);
			break;
		case 1:
            configOsciladores_LUT2(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 2: 
            configOsciladores_LUT3(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 3: 
            configOsciladores_LUT3_multirouting(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap);
			break;
		case 4: 
            configOsciladores_LUT4(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 5: 
            configOsciladores_LUT5(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 6: 
            configOsciladores_LUT6(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 7: 
            configOsciladores_LUT6_multirouting(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap);
			break;
		case 8: 
            configOsciladores_LUT6_2(punte, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		
	}
    
    fclose(punte);
	
	free(pos_oscilador[0]);
	free(pos_oscilador[1]);
	free(pos_oscilador_fijado[0]);
	free(pos_oscilador_fijado[1]);
	
	for(int i=0; i<N_inv; i++)
		free(pinmap[i]);
	free(pinmap);
    
    return 0;
}


