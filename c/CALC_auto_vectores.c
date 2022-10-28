
#include "include/main.h"
#include "include/matematicas.h"

int main(int N_opcion, char** opcion)
{
    int i,j,plot=0,N_lineas;
    char input[256]={"input.mtz"},output[256]={"autovec.mtz"}, flagin=0, flagout=0, autoval_name[256]={"autoval.txt"}, real=0, autoval=0, tras=0;
	gsl_matrix* entrada;
	gsl_eigen_nonsymmv_workspace* workspace;
	gsl_vector_complex* eval;
	gsl_matrix_complex* evec;
	gsl_complex caux;
    FILE *punte;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nCalculo de autovectores y autovalores de una matriz utilizando el metodo double-shift de Francis (implementado por la biblioteca gsl) \n----------------------------------\nOpciones:\n\n");
            printf("-in [input.mtz], si esta opcion esta presente el programa leera la entrada de un fichero. En caso contrario leera la entrada de stdin\n\n");
            printf("-out [autovec.mtz], si esta opcion esta presente el programa volcara la salida en un fichero. En caso contrario volcara la salida en stdout. El formato sera 'mtz', donde cada columna representara un autovector de la matriz 'input.mtz'. Notar que esta matriz 'autovec.mtz' corresponde a la matriz de cambio de base: auto_base-->base_canonica, en el sentido de: autovec.mtz*vectorcolumna_en_autobase=vectorcolumna_en_basecanonica.\n\n");
            printf("-autoval [autoval.txt], si esta opcion esta presente el programa escribira los autovalores en un fichero (en orden, en relacion a las columnas de la matriz 'autovec.mtz'\n\n");
			printf("-real, si esta opcion esta presente el programa solo escribira las partes reales de los autovectores/autovalores (usar si se sabe que son numeros reales)\n\n");
            printf("-tras, si esta opcion eta presente el programa escribe la matriz de autovectores traspuesta: cada fila sera un autovector (esto puede ser util por ejemplo para proyectar directamente una trayectoria sobre la base de autovectores)\n\n");
            
            exit(0);
        }
        
        {
            flagin=1;
            if(i+1 < N_opcion)
            {
                if(opcion[i+1][0]!='-')
                    sprintf(input, "%s", opcion[i+1]);
            }
        }
        
        if( strcmp(opcion[i], "-out")==0 )
        {
            flagout=1;
            if(i+1 < N_opcion)
            {
                if(opcion[i+1][0]!='-')
                    sprintf(output, "%s", opcion[i+1]);
            }
        }
			
        if( strcmp(opcion[i], "-autoval")==0 )
		{
            autoval = 1;
			if(i<N_opcion-1)
			{
				if(opcion[i+1][0]!='-')
					sscanf(opcion[i+1], "%s", autoval_name);
			}
		}
        
        if( strcmp(opcion[i], "-real")==0 )
            real = 1;
        
        if( strcmp(opcion[i], "-tras")==0 )
            tras = 1;        
    }
    
    if(flagin)
        punte = fopen(input, "r");
    else
        punte = stdin;

    entrada = GSL_leerMatriz(punte);
    if(flagin)
        fclose(punte);
	
	workspace = gsl_eigen_nonsymmv_alloc(entrada->size1);
	
	eval = gsl_vector_complex_alloc(entrada->size1);
	
	evec = gsl_matrix_complex_alloc(entrada->size1, entrada->size1);
	
	gsl_eigen_nonsymmv(entrada, eval, evec, workspace);
	
	gsl_eigen_nonsymmv_sort(eval, evec, GSL_EIGEN_SORT_ABS_DESC);
        
    if(flagout)
        punte = fopen(output, "w");
    else
        punte = stdout;
    fprintf(punte, "# [N_filas]\t%ld\n\n# [N_columnas]\t%ld\n\n# [matriz]\n", (long)evec->size1, (long)evec->size2);
    for(i=0; i<(int)entrada->size1; i++)
    {
        for(j=0; j<(int)entrada->size1; j++)
		{
			if(tras==0)
                caux = gsl_matrix_complex_get(evec, i, j);
            else
                caux = gsl_matrix_complex_get(evec, j, i);
			if(real)
				fprintf(punte, "%g\t", GSL_REAL(caux));
			else
				fprintf(punte, "%g+%gi\t", GSL_REAL(caux), GSL_IMAG(caux));
		}
        fprintf(punte, "\n");
    }
    if(flagout)
        fclose(punte);
	
	if(autoval)
	{
		punte = fopen(autoval_name, "w");
		fprintf(punte, "[autovalores]\n");
		for(i=0; i<(int)entrada->size1; i++)
		{
			caux = gsl_vector_complex_get(eval, i);
			if(real)
				fprintf(punte, "%g\t", GSL_REAL(caux));
			else
				fprintf(punte, "%g+%gi\t", GSL_REAL(caux), GSL_IMAG(caux));
		}
		fclose(punte);
	}
	
	gsl_eigen_nonsymmv_free(workspace);
	gsl_vector_complex_free(eval);
	gsl_matrix_complex_free(evec);
	gsl_matrix_free(entrada);
    
    return 0;
}
