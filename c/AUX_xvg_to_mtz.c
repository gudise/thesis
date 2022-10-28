#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int main(int N_opcion, char** opcion)
{
    int i, j, Nfilas, Ncolumnas;
    char in_name[256]={"input.xvg"}, out_name[256]={"output.mtz"}, string[256], letra;
    FILE* punte, *punte1;
    
    for(i=0; i<N_opcion; i++)
    {
        if(strcmp(opcion[i], "-help")==0)
        {
            printf("\nPrograma para convertir un ficher '.xvg' en '.mtz': \n--------------------------\n\n");
            printf("-in [\"input.xvg\"], nombre del fichero de entrada\n\n");
            printf("-out [\"output.mtz\"], nombre del fichero de salida\n\n");
            
            exit(0);
        }
        
        if(strcmp(opcion[i], "-in")==0)
            sscanf(opcion[i+1], "%s", in_name);
        
        if(strcmp(opcion[i], "-out")==0)
            sscanf(opcion[i+1], "%s", out_name);
    }
    
    
    punte=fopen(in_name, "r");
    punte1=fopen(".temp", "w");
    while(1)
    {
    letra = fgetc(punte);
        if(feof(punte))
            break;
            
        else if(letra!='@' && letra!='&' && letra!='#')
        {
            fseek(punte, -1, SEEK_SET);
            fscanf(punte, "%s", string);
            fprintf(punte1, "%s", string);
            while(fgetc(punte)!='\n')
            {
                fscanf(punte, "%s", string);
                fprintf(punte1, "\t%s", string);
            }
            fprintf(punte1, "\n");
        }
        else
        {
            while(fgetc(punte)!='\n')
            {}
        }
        
        if(feof(punte))
            break;
        
    }
    fclose(punte);
    fclose(punte1);
    
    Ncolumnas=1;
    punte = fopen(".temp", "r");
    while(1)
    {
        fscanf(punte, "%s", string);
        if(fgetc(punte)!='\n')
            Ncolumnas++;
        else
            break;
    }
    fclose(punte);
    
    Nfilas=0;
    punte = fopen(".temp", "r");
    while(1)
    {
        fscanf(punte, "%s", string);
        if(fgetc(punte)=='\n')
            Nfilas++;
        
        if(feof(punte))
            break;
    }
    fclose(punte);
    
    
    punte = fopen(out_name, "w");
    punte1 = fopen(".temp", "r");
    fprintf(punte, "# [N_filas] %d\n# [N_columnas] %d\n# [matriz]\n\n", Nfilas, Ncolumnas);
    for(i=0; i<Nfilas; i++)
    {
        for(j=0; j<Ncolumnas; j++)
        {
           fscanf(punte1, "%s", string);
           fprintf(punte, "%s\t", string);
        }
        fprintf(punte, "\n");
    }
    fclose(punte);
    fclose(punte1);
    
    remove(".temp");

    return 0;
}
