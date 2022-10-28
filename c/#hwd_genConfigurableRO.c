#include "main.h"
#include "hardware.h"


int main(int N_opcion, char** opcion)
{
    FILE* punte;
    char out_name[256]={"configurable_RO"}, config=1, design_name[300], contraints_name[300], env[256];
    long i, j, N_inv=5, XOS_i=0, YOS_i=0, XOS_incr=1, YOS_incr=1, XOS_max=20, YOS_max=20, XOS, YOS, X, Y, celda[2], ocupacion_celda=0, aux;
    
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa crea un fichero '.v' con un oscilador de anillo configurable implementado sobre las LUTs de 6 entradas una FPGA xilinx serie 7. La configuración se realiza mediante el bus \"selector\". Se utilizan dos bits de selección por inversor (LUT), d eforma que podemos elegir entre cuatro configuraciones diferentes de cada inversor. Esto proporciona 4^N_inv combinaciones posibles\n----------------------------------\n\n");
            printf("-out [configurable_RO.v], todas las salidas del programa tendran este nombre seguido de la extension correspondiente\n\n");
            printf("-config [vertical] {vertical, horizontal}, direccion en la que se colocaran los inversores de cada oscilador\n\n");
            printf("-Ninv [5], numero de inversores de cada oscilador\n\n");
            printf("-XOSi [0], coordenada X de la LUT a partir de la cual se construye la matriz\n\n");
            printf("-YOSi [0], coordenada Y de la LUT a partir de la cual se construye la matriz\n\n");
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-out")==0 )
            sscanf(opcion[i+1], "%s", out_name);
        
        if( strcmp(opcion[i], "-config")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
                {
                    if(strcmp(opcion[i+1], "horizontal")==0)
                        config = 0;
                    else
                        config = 1;
                }
            }
        }
        
        if( strcmp(opcion[i], "-Ninv")==0 )
            sscanf(opcion[i+1], "%ld", &N_inv);
        
        if( strcmp(opcion[i], "-XOSi")==0 )
            sscanf(opcion[i+1], "%ld", &XOS_i);
        
        if( strcmp(opcion[i], "-YOSi")==0 )
            sscanf(opcion[i+1], "%ld", &YOS_i);
    }
        
    sprintf(design_name,"%s.v", out_name);
    
    punte=fopen(design_name,"w");
    
    fprintf(punte,"module %s (\n\tinput[%ld:0] selector,\n\toutput out\n\t);\n", out_name, 2*N_inv-1);
    
    fprintf(punte,"\n  (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire out_ro;\n");
    
    fprintf(punte,"\n   wire [0:%ld] w;",N_inv-1);
    
    fprintf(punte,"\n\n    assign out = out_ro;\n");
    
    
    fprintf(punte,"\n\n");
    
    aux = N_inv;
    XOS = XOS_i;
    YOS = YOS_i;
    
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    
    if(punte!=NULL) fprintf(punte,"    (* BEL=\"A6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h8) AND(.O(w[0]), .I0(1'b1), .I1(out_ro), .I2(1'b0), .I3(1'b0), .I4(1'b0), .I5(1'b0));\n",celda[0],celda[1]);
    
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config) celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inv-1)
        {
            switch(ocupacion_celda)
            {
                case 0:
                    fprintf(punte,"    (* BEL=\"A6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h123456789ABCDEF) inv_%ld(.O(out_ro), .I0(selector[%ld]), .I1(selector[%ld]), .I2(w[%ld]), .I3(w[%ld]), .I4(w[%ld]), .I5(w[%ld]));\n\n",celda[0],celda[1],N_inv-1,2*N_inv-2,2*N_inv-1,N_inv-1,N_inv-1,N_inv-1,N_inv-1);
                    break;
                    
                case 1:
                    fprintf(punte,"    (* BEL=\"B6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h123456789ABCDEF) inv_%ld(.O(out_ro), .I0(selector[%ld]), .I1(selector[%ld]), .I2(w[%ld]), .I3(w[%ld]), .I4(w[%ld]), .I5(w[%ld]));\n\n",celda[0],celda[1],N_inv-1,2*N_inv-2,2*N_inv-1,N_inv-1,N_inv-1,N_inv-1,N_inv-1);
                    break;
                    
                case 2:
                    fprintf(punte,"    (* BEL=\"C6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h123456789ABCDEF) inv_%ld(.O(out_ro), .I0(selector[%ld]), .I1(selector[%ld]), .I2(w[%ld]), .I3(w[%ld]), .I4(w[%ld]), .I5(w[%ld]));\n\n",celda[0],celda[1],N_inv-1,2*N_inv-2,2*N_inv-1,N_inv-1,N_inv-1,N_inv-1,N_inv-1);
                    break;
                    
                case 3:
                    fprintf(punte,"    (* BEL=\"D6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h123456789ABCDEF) inv_%ld(.O(out_ro), .I0(selector[%ld]), .I1(selector[%ld]), .I2(w[%ld]), .I3(w[%ld]), .I4(w[%ld]), .I5(w[%ld]));\n\n",celda[0],celda[1],N_inv-1,2*N_inv-2,2*N_inv-1,N_inv-1,N_inv-1,N_inv-1,N_inv-1);
                    break;
            }
            ocupacion_celda++;
            
            break;
        }
        
        
        switch(ocupacion_celda)
        {
            case 0:
                fprintf(punte,"    (* BEL=\"A6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h123456789ABCDEF) inv_%ld(.O(w[%ld]), .I0(selector[%ld]), .I1(selector[%ld]), .I2(w[%ld]), .I3(w[%ld]), .I4(w[%ld]), .I5(w[%ld]));\n",celda[0],celda[1],aux,aux+1,2*aux,2*aux+1,aux,aux,aux,aux);
                break;
                
            case 1:
                fprintf(punte,"    (* BEL=\"B6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h123456789ABCDEF) inv_%ld(.O(w[%ld]), .I0(selector[%ld]), .I1(selector[%ld]), .I2(w[%ld]), .I3(w[%ld]), .I4(w[%ld]), .I5(w[%ld]));\n",celda[0],celda[1],aux,aux+1,2*aux,2*aux+1,aux,aux,aux,aux);
                break;
                
            case 2:
                fprintf(punte,"    (* BEL=\"C6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h123456789ABCDEF) inv_%ld(.O(w[%ld]), .I0(selector[%ld]), .I1(selector[%ld]), .I2(w[%ld]), .I3(w[%ld]), .I4(w[%ld]), .I5(w[%ld]));\n",celda[0],celda[1],aux,aux+1,2*aux,2*aux+1,aux,aux,aux,aux);
                break;
                
            case 3:
                fprintf(punte,"    (* BEL=\"D6LUT\", LOC=\"SLICE_X%ldY%ld\", DONT_TOUCH=\"true\" *) LUT6 #(64'h123456789ABCDEF) inv_%ld(.O(w[%ld]), .I0(selector[%ld]), .I1(selector[%ld]), .I2(w[%ld]), .I3(w[%ld]), .I4(w[%ld]), .I5(w[%ld]));\n",celda[0],celda[1],aux,aux+1,2*aux,2*aux+1,aux,aux,aux,aux);
                break;
        }
        ocupacion_celda++;
        
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");



    if(XOS>=XOS_max+1)
    {
        XOS = XOS_i;
        
        if(config) YOS+= N_inv/4+YOS_incr;
        else YOS+=YOS_incr;
        
    }
    if(YOS>=YOS_max+1)
    {
        if(config) XOS+=XOS_incr;
        else XOS+= N_inv/4+XOS_incr;
        
        YOS = YOS_i;
    }
    
    fprintf(punte,"endmodule\n");
    
    fclose(punte);
    
    return 0;
}


