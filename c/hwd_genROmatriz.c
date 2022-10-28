#include "main.h"
#include "hardware.h"


int main(int N_opcion, char** opcion)
{
    FILE* punte;
    char out_name[256]={"romatrix"}, config='x', design_name[300], directriz='x', env[256], tipo=0, pinmap[64], pinmap_set=0, minsel=0, *string;
    int i, j, N_inv=3, XOS, YOS, XOS_i=0, YOS_i=0, XOS_incr=1, YOS_incr=1, XOS_max=20, YOS_max=20, N_osciladores=10, resolucion=1000000, N_claves=100;
	int* pos_oscilador[2], *pos_oscilador_fijado[2], N_osciladores_fijados, contador;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa crea todos los ficheros necesarios (.v .xdc .ino) para implementar una matriz de osciladores de anillo en FPGA y medir sus frecuencias de oscilacion utilizando una interfaz Arduino. Para medir la frecuencia de un oscilador se mide el tiempo que le cuesta a este realizar 'resolucion' oscilaciones\n----------------------------------\n\n");
            printf("-out [romatrix], todas las salidas del programa tendran este nombre seguido de la extension correspondiente\n\n");
			printf("-tipo [lut1|lut2|lut3|lut3mr|lut4|lut5|lut6|lut6mr|lut6_2], si esta opcion esta presente, se modifica la clase de LUT que constiuye un inversor (por defecto se toma 'lut1'). Las matrices de osciladores modificados admiten un array de configuracion (ademas de la seleccion de osciladores):\nBits de configuracion:\n\tlut3 --> 2*N_inv*N_osciladores\n\tlut3_mr (multirouting) --> N_inv*N_osciladores\n\tlut4 --> 3*N_inv_N_osciladores\n\tlut5 --> 4*N_inv_N_osciladores\n\tlut6 --> 5*N_inv*N_osciladores\n\tlut6_mr(multirouting) --> 4*N_inv_N_osciladores \n\n");
            printf("-config [x] {x, y}, direccion en la que se colocaran los inversores de cada oscilador\n\n");
            printf("-directriz [x] ¤ {x, y}, esta opcion indica la direccion de crecimiento de la matriz (por defecto a lo largo del eje 'x')\n\n");
            printf("-pinmap [I0:A1,I1:A2,...], esta opcion permite cambiar la asignacion de puertos de la LUT. La nomenclatura 'Ix:Ay' es lenguaje XDC y debe respetarse. Recordar que el numero de entradas 'Ix' depende del tipo de LUT utilizado. La nomenclatura de los puertos de cada LUT es A1,A2,A3,A4,A5,A6.\n\n");
            printf("-minsel [0], con esta opcion se realizará una seleccion minima (unicamente 5 bits), que comparten todos los inversores de cada oscilador. Para activar 'minsel' podemos escribir '-minsel 1', o simplemente '-minsel'. '-minsel 0' (la opción por defecto) equivale a no utilizar esta opción\n\n");
            printf("-Ninv [3], numero de inversores de cada oscilador\n\n");
            printf("-XOSincr [1], distancia entre las coordenadas X de dos osciladores adyacentes\n\n");
            printf("-YOSincr [1], distancia entre las coordenadas Y de dos osciladores adyacentes\n\n");
            printf("-XOSmax [20], coordenada X maxima de la matriz de osciladores implementada\n\n");
            printf("-YOSmax [20], coordenada Y maxima de la matriz de osciladores implementada\n\n");
            printf("-Nosc [10], numero de osciladores de la matriz. Si Nosc=-1, la matriz se llenara hasta completar las celdas indicadas en XOSmax, YOSmax\n\n");
			printf("-posmap [x0,y0 x1,y1 ...], esta opcion permite indicar explícitamente las posiciones de (la primera puerta, i.e., de la puerta AND) de cada oscilador. Si no se indican tantos pares x,y como osciladores haya, el resto de osciladores hasta llegar a Nosc se escribirán de acuerdo con los criterios establecidos en OSincr y OSmax. Si no se indica esta opción en basoluto se utiliza x0=0, y0=0\n\n");
            printf("-resolucion [1000000], numero de ciclos de medicion para cada oscilador\n\n");
            
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
        {
            pinmap_set=1;
            sscanf(opcion[i+1], "%s", pinmap);
        }
        
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
        
        if( strcmp(opcion[i], "-config")==0 )
        {
            if(strcmp(opcion[i+1], "y")==0 || strcmp(opcion[i+1], "Y")==0)
                config='y';
        }
        
        if( strcmp(opcion[i], "-directriz")==0 )
        {
            if(strcmp(opcion[i+1], "y")==0 || strcmp(opcion[i+1], "Y")==0)
                directriz='y';
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
			for(j=i+1; j>i; j++)
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

			for(j=0; j<N_osciladores_fijados; j++)
				sscanf(opcion[j+i+1], "%d,%d", &pos_oscilador_fijado[0][j], &pos_oscilador_fijado[1][j]);
		}
        
        if( strcmp(opcion[i], "-resolucion")==0 )
            sscanf(opcion[i+1], "%d", &resolucion);
    } // fin de lectura de opciones
    
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

    for(i=0;i<N_osciladores_fijados;i++)
    {	
		pos_oscilador[0][i]=pos_oscilador_fijado[0][i];
		pos_oscilador[1][i]=pos_oscilador_fijado[1][i];
	}

	XOS = pos_oscilador[0][N_osciladores_fijados-1];
	YOS = pos_oscilador[1][N_osciladores_fijados-1];
    contador=0;
	for(i=N_osciladores_fijados;i<N_osciladores;i++)
	{
		if(directriz=='x')
			XOS = XOS+XOS_incr;
		else
			YOS = YOS+YOS_incr;
		
		if(XOS>=XOS_max+1)
		{
			XOS = pos_oscilador_fijado[0][N_osciladores_fijados-1];
            
			if(config=='y') YOS+= N_inv/4+YOS_incr;
			else YOS+=YOS_incr;
            
			if(YOS>=YOS_max+1)
			{
				printf("\nERROR: en funcion 'hwd_genROmatriz'. YOSmax excedida. Como mucho se pueden implementar %d osciladores (en lugar de %d osciladores)\n", contador+N_osciladores_fijados, N_osciladores);
				exit(0);
			}
		}
		if(YOS>=YOS_max+1)
		{
			YOS = pos_oscilador_fijado[1][N_osciladores_fijados-1];
			
			if(config=='y') XOS+=XOS_incr;
			else XOS+= N_inv/4+XOS_incr;
            
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
    
	switch(tipo)
	{
		case 0:
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1");
            configOsciladores(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap);
			break;
		case 1:
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1,I1:A2");
            configOsciladores_LUT2(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 2: 
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1,I1:A2,I2:A3");
            configOsciladores_LUT3(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 3: 
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1,I1:A2,I2:A3");
            configOsciladores_LUT3_multirouting(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap);
			break;
		case 4: 
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1,I1:A2,I2:A3,I3:A4");
            configOsciladores_LUT4(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 5: 
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1,I1:A2,I2:A3,I3:A4,I4:A5");
            configOsciladores_LUT5(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 6: 
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1,I1:A2,I2:A3,I3:A4,I4:A5,I5:A6");
            configOsciladores_LUT6(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		case 7: 
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1,I1:A2,I2:A3,I3:A4,I4:A5,I5:A6");
            configOsciladores_LUT6_multirouting(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap);
			break;
		case 8: 
            if(!pinmap_set)
                sprintf(pinmap, "I0:A1,I1:A2,I2:A3,I3:A4,I4:A5,I5:A6");
            configOsciladores_LUT6_2(punte, config, N_inv, pos_oscilador[0], pos_oscilador[1], N_osciladores, pinmap, minsel);
			break;
		
	}
    
    fclose(punte);
	
	free(pos_oscilador[0]);
	free(pos_oscilador[1]);
	free(pos_oscilador_fijado[0]);
	free(pos_oscilador_fijado[1]);
    
    return 0;
}


