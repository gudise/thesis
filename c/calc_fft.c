#include "include/main.h"
#include "include/matematicas.h"

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

int main(int N_opcion, char** opcion)
{
    int i, j, col=1, N, desde=0, hasta=0;
    char input[256]={"input.mtz"},output[256]={"fft.mtz"}, modulo=0;
	double* data, freq, timestep, freqstep;
	MATRIZ* entrada;
	gsl_fft_complex_wavetable* wavetable;
	gsl_fft_complex_workspace* workspace;
    FILE *punte;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nCalculo de la transformada rapida de Fourier para una serie temporal (real) \n----------------------------------\nOpciones:\n\n");
            printf("-in [input.mtz]  fichero con la matriz de entrada (ver 'doc/my_doc')\n\n");
            printf("-out [fft.mtz],  nombre del fichero de salida. El formato sera 'mtz' y constara de cuatro columnas: la columna '0' sera la frecuencia (calculadas a partir del 'timestep' de los datos de entrada, ojo con las unidades!), la columna '1' sera la parte real de la fft, la columna '2' sera la parte imaginaria de la fft, la columna '3' contiene la amplitud de la fft\n\n");
			printf("-col [1], esta opcion define la columna de 'input.mtz' que se usa para la transformada. Las columnas empiezan a numerarse por '0' (i.e., por defecto se utiliza la segunda columna de la matriz de entrada)\n\n");
			printf("-desde [0], esta opcion indica la primera fila que se utilizara para fft (siendo '0' la primera fila)\n\n");
			printf("-hasta [N_filas-1], esta opcion indica la ultima fila que se utilizara para fft\n\n");
            printf("-modulo, si esta opcion esta presente el programa pinta una cuarta columna con el modulo de la transformada (esto es util para representarlas graficamente, pero no si se quieren realizar operaciones sobre el fichero .mtz como por ejemplo calcular promedios)\n\n");
			
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
            sprintf(input, "%s", opcion[i+1]);
        
        if( strcmp(opcion[i], "-out")==0 )
            sprintf(output, "%s", opcion[i+1]);
			
        if( strcmp(opcion[i], "-col")==0 )
            sscanf(opcion[i+1], "%d", &col); 
			
        if( strcmp(opcion[i], "-desde")==0 )
            sscanf(opcion[i+1], "%d", &desde); 
			
        if( strcmp(opcion[i], "-hasta")==0 )
            sscanf(opcion[i+1], "%d", &hasta);

		if( strcmp(opcion[i], "-modulo")==0 )
			modulo=1;
    }
    
    entrada = leerMatriz(input);
	if(hasta==0)
		hasta=entrada->N_filas-1;
	N = hasta-desde+1;
	timestep = entrada->elemento[1][0]-entrada->elemento[0][0];
	freq = -1/(2*timestep);
	freqstep = 1/(timestep*N);

	
	data = malloc(sizeof(double)*2*N);
	for(i=0; i<N; i++)
	{
		REAL(data, i) = entrada->elemento[i+desde][col];
		IMAG(data, i) = 0;
	}
	
	workspace = gsl_fft_complex_workspace_alloc(N);
	wavetable = gsl_fft_complex_wavetable_alloc(N);
	
	gsl_fft_complex_forward(data, 1, N, wavetable, workspace);
	    
    punte = fopen(output, "w");
    if(modulo)
		fprintf(punte, "# [N_filas]\t%d\n\n# [N_columnas]\t%d\n\n# [matriz]\n", N, 4);
	else
		fprintf(punte, "# [N_filas]\t%d\n\n# [N_columnas]\t%d\n\n# [matriz]\n", N, 3);
    for(i=N/2; i<N; i++)
	{
		if(modulo)
			fprintf(punte, "%lg\t%lg\t%lg\t%lg\n", freq, REAL(data,i), IMAG(data,i), sqrt(REAL(data,i)*REAL(data,i)+IMAG(data,i)*IMAG(data,i)));
		else
			fprintf(punte, "%lg\t%lg\t%lg\n", freq, REAL(data,i), IMAG(data,i));
		
		freq+=freqstep;
	}
	for(i=0; i<N/2; i++)
	{
		if(modulo)
			fprintf(punte, "%lg\t%lg\t%lg\t%lg\n", freq, REAL(data,i), IMAG(data,i), sqrt(REAL(data,i)*REAL(data,i)+IMAG(data,i)*IMAG(data,i)));
		else
			fprintf(punte, "%lg\t%lg\t%lg\n", freq, REAL(data,i), IMAG(data,i));
		
		freq+=freqstep;
	}
    fclose(punte);
	
	
	gsl_fft_complex_wavetable_free(wavetable);
	gsl_fft_complex_workspace_free(workspace);
	freeMatriz(entrada);
    
    return 0;
}
