/*ENCABEZADOS*/

int		N_bits_MUX(int N);

void    cablearOscilador(FILE* punte, int N_inversores, int i);

void    transfiereNumeroOut(FILE* punte, int Nbits);

void    transfiereNumeroIn(FILE* punte, int Nbits);

void    configOsciladores(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap);

void	configOsciladores_LUT2(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel);

void	configOsciladores_LUT3(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel);

void	configOsciladores_LUT3_multirouting(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap);

void	configOsciladores_LUT4(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel);

void	configOsciladores_LUT5(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel);

void	configOsciladores_LUT6(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel);

void	configOsciladores_LUT6_multirouting(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap);

void	configOsciladores_LUT6_2(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel);

void    pintarOscilador(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap);

void	pintarOscilador_LUT2(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel);

void	pintarOscilador_LUT3(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel);

void 	pintarOscilador_LUT3_multirouting(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap);

void	pintarOscilador_LUT4(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel);

void	pintarOscilador_LUT5(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel);

void	pintarOscilador_LUT6(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel);

void	pintarOscilador_LUT6_multirouting(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap);

void	pintarOscilador_LUT6_2(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel);


const char *bel_array[] = {"C6LUT","A6LUT","D6LUT","B6LUT"};



/*FUNCIONES*/

int N_bits_MUX(int N) //nº de bits necesarios para el selector del multiplexor
{
    if(N<=0)
        return 1;
    else
    {
        if(log(N)/log(2)>(int)(log(N)/log(2)))
            return (int)(log(N)/log(2))+1;
        else
            return (int)(log(N)/log(2));
    }
}

void configOsciladores(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap)
{
    int i;
    
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    wire[%d:0] w_%d;",N_inv-1,i);
    
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
    fprintf(punte,"\n\n");
    
    for(i=0;i<N_osciladores;i++)
		pintarOscilador(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap);
	
	fprintf(punte,"endmodule\n");
}


void pintarOscilador(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(out_ro[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i);
    
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT1 #(2'b01) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,N_inversores-1,i,i,N_inversores-1);
            
            ocupacion_celda++;
            
            break;
        }
        
        fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT1 #(2'b01) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux);
        
        ocupacion_celda++;
        
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}


void configOsciladores_LUT2(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel)
{
    int i;
        
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	if(!minsel)
		fprintf(punte,"    input[%d:0] sel,\n",N_inv-1);
	else
		fprintf(punte,"    input[%d:0] sel,\n",0);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\"*) wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    wire[%d:0] w_%d;",N_inv-1,i);				        
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
	fprintf(punte,"\n\n");
	
	for(i=0;i<N_osciladores;i++)
		pintarOscilador_LUT2(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap, minsel);
	
	fprintf(punte,"endmodule\n");
}


void pintarOscilador_LUT2(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux, contador;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(out_ro[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i);
    
	contador=0;
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            if(!minsel)
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT2 #(4'h5) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,contador);
            else
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT2 #(4'h5) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,0);
            
            ocupacion_celda++;
            
            break;
        }
        
        if(!minsel)
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT2 #(4'h5) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,contador);
        else
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT2 #(4'h5) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,0);
            
        contador++;
		
        ocupacion_celda++;
        
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}


void configOsciladores_LUT3(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel)
{
    int i;
    
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	if(!minsel)
		fprintf(punte,"    input[%d:0] sel,\n",2*N_inv-1);
	else
		fprintf(punte,"    input[%d:0] sel,\n",1);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    wire[%d:0] w_%d;",N_inv-1,i);				        
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
	fprintf(punte,"\n\n");
	
	for(i=0;i<N_osciladores;i++)
		pintarOscilador_LUT3(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap, minsel);
    
    fprintf(punte,"endmodule\n");
}


void pintarOscilador_LUT3(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux, contador;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(out_ro[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i);
    
	contador=0;
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            if(!minsel)
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT3 #(8'h55) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,contador,contador+1);
                    
            else
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT3 #(8'h55) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,0,1);
            
            ocupacion_celda++;
            
            break;
        }
        
        if(!minsel)
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT3 #(8'h55) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,contador,contador+1);
                
        else
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT3 #(8'h55) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,0,1); 
        
        contador+=2;
		
        ocupacion_celda++;
        
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}


void configOsciladores_LUT3_multirouting(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap)
{
    int i;
    
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	fprintf(punte,"    input[%d:0] sel,\n",N_inv-1);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    wire[%d:0] w_%d;",N_inv-1,i);				        
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
	fprintf(punte,"\n\n");
	
	for(i=0;i<N_osciladores;i++)
		pintarOscilador_LUT3_multirouting(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap);
    
    fprintf(punte,"endmodule\n");
}


void pintarOscilador_LUT3_multirouting(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux, contador;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(out_ro[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i);
    
	contador=0;
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT3 #(8'h1B) inv_%d_%d(.O(out_ro[%d]), .I0(sel[%d]), .I1(w_%d[%d]), .I2(w_%d[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,contador,i,aux,i,aux);
            
            ocupacion_celda++;
            
            break;
        }
        
        fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT3 #(8'h1B) inv_%d_%d(.O(w_%d[%d]), .I0(sel[%d]), .I1(w_%d[%d]), .I2(w_%d[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,contador,i,aux,i,aux);
        
        ocupacion_celda++;
        contador++;
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}


void configOsciladores_LUT4(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel)
{
    int i, j, aux, XOS, YOS, result=0;
    
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	if(!minsel)
		fprintf(punte,"    input[%d:0] sel,\n",3*N_inv-1);
	else
		fprintf(punte,"    input[%d:0] sel,\n",2);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    wire[%d:0] w_%d;",N_inv-1,i);				        
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
	fprintf(punte,"\n\n");
	
	for(i=0;i<N_osciladores;i++)
		pintarOscilador_LUT4(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap, minsel);
    
    fprintf(punte,"endmodule\n");
}


void pintarOscilador_LUT4(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux, contador;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(out_ro[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i);
    
	contador=0;
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            if(!minsel)
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT4 #(16'h5555) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,contador,contador+1,contador+2);
                    
            else
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT4 #(16'h5555) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,0,1,2);
            
            ocupacion_celda++;
            
            break;
        }
        
        if(!minsel)
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT4 #(16'h5555) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,contador,contador+1,contador+2);
                
        else
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT4 #(16'h5555) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,0,1,2);
        
        ocupacion_celda++;
        contador+=3;
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}


void configOsciladores_LUT5(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel)
{
    int i;
    
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	if(!minsel)
		fprintf(punte,"    input[%d:0] sel,\n",4*N_inv-1);
	else
		fprintf(punte,"    input[%d:0] sel,\n",3);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    wire[%d:0] w_%d;",N_inv-1,i);				        
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
	fprintf(punte,"\n\n");
	
	for(i=0;i<N_osciladores;i++)
		pintarOscilador_LUT5(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap, minsel);

    fprintf(punte,"endmodule\n");    
}


void pintarOscilador_LUT5(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux, contador;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(out_ro[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i);
    
	contador=0;
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            if(!minsel)
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT5 #(32'h55555555) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,contador,contador+1,contador+2,contador+3);
                    
            else
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT5 #(32'h55555555) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,0,1,2,3);
            
            ocupacion_celda++;
            
            break;
        }

        if(!minsel)
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT5 #(32'h55555555) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,contador,contador+1,contador+2,contador+3);
	
        else
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT5 #(32'h55555555) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,0,1,2,3);
        
        ocupacion_celda++;
        contador+=4;
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}


void configOsciladores_LUT6(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel)
{
    int i;
    
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	if(!minsel)
		fprintf(punte,"    input[%d:0] sel,\n",5*N_inv-1);
	else
		fprintf(punte,"    input[%d:0] sel,\n",4);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    wire[%d:0] w_%d;",N_inv-1,i);				        
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
	fprintf(punte,"\n\n");
	
	for(i=0;i<N_osciladores;i++)
		pintarOscilador_LUT6(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap, minsel);
    
    fprintf(punte,"endmodule\n");    
}


void pintarOscilador_LUT6(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux, contador;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(out_ro[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i);
    
	contador=0;
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            if(!minsel)
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6 #(64'h5555555555555555) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]), .I5(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,contador,contador+1,contador+2,contador+3,contador+4);
                
            else
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6 #(64'h5555555555555555) inv_%d_%d(.O(out_ro[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]), .I5(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux,0,1,2,3,4);
            
            ocupacion_celda++;
            
            break;
        }

        if(!minsel)
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6 #(64'h5555555555555555) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]), .I5(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,contador,contador+1,contador+2,contador+3,contador+4);
				
        else
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6 #(64'h5555555555555555) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]), .I5(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,0,1,2,3,4);
        
        ocupacion_celda++;
        contador+=5;
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}


void configOsciladores_LUT6_2(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap, char minsel)
{
    int i;
    
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	if(!minsel)
		fprintf(punte,"    input[%d:0] sel,\n",5*N_inv-1);
	else
		fprintf(punte,"    input[%d:0] sel,\n",4);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[%d:0] w_%d;",N_inv,i);				        
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
	fprintf(punte,"\n\n");
	
	for(i=0;i<N_osciladores;i++)
		pintarOscilador_LUT6_2(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap, minsel);
    
    fprintf(punte,"endmodule\n");    
}


void pintarOscilador_LUT6_2(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap, char minsel)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux, contador;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(w_%d[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i,N_inversores);
    
	contador=0;
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            if(!minsel)
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6_2 #(64'h5555555555555555) inv_%d_%d(.O5(out_ro[%d]), .O6(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]), .I5(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux+1,i,aux,contador,contador+1,contador+2,contador+3,contador+4);
                
            else
                fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6_2 #(64'h5555555555555555) inv_%d_%d(.O5(out_ro[%d]), .O6(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]), .I5(sel[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,i,aux+1,i,aux,0,1,2,3,4);
            
            ocupacion_celda++;
            
            break;
        }

        if(!minsel)
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6 #(64'h5555555555555555) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]), .I5(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,contador,contador+1,contador+2,contador+3,contador+4);
				
        else
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6 #(64'h5555555555555555) inv_%d_%d(.O(w_%d[%d]), .I0(w_%d[%d]), .I1(sel[%d]), .I2(sel[%d]), .I3(sel[%d]), .I4(sel[%d]), .I5(sel[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,i,aux,0,1,2,3,4);
        
        ocupacion_celda++;
        contador+=5;
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}


void configOsciladores_LUT6_multirouting(FILE* punte, char config, int N_inv, int* XOS_i, int* YOS_i, int N_osciladores, char* pinmap)
{
    int i;
    
	fprintf(punte,"module ROMATRIX (\n");
	fprintf(punte,"    input clock,\n");
	if(N_osciladores>1) 
		fprintf(punte,"    input[%d:0] sel_ro,\n",N_bits_MUX(N_osciladores)-1);
	fprintf(punte,"    input[%d:0] sel,\n",2*N_inv-1);
	fprintf(punte,"    input enable,\n");
	fprintf(punte,"    output out\n");
	fprintf(punte,"    );\n\n");
	fprintf(punte,"    (* ALLOW_COMBINATORIAL_LOOPS = \"true\", DONT_TOUCH = \"true\" *) wire[%d:0] out_ro;\n",N_osciladores-1);
	fprintf(punte,"    reg[%d:0] enable_ro;\n",N_osciladores-1);
	for(i=0; i<N_osciladores; i++)
		fprintf(punte,"\n    wire[%d:0] w_%d;",N_inv-1,i);				        
	if(N_osciladores>1)
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[sel_ro] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[sel_ro] = 1;\n");
        fprintf(punte, "    end\n");
    }
	else
    {
		fprintf(punte,"\n\n    assign out = enable? out_ro[0] : clock;\n");
        fprintf(punte, "\n    always @(*)begin\n");
        fprintf(punte, "          enable_ro = 0;\n");
        fprintf(punte, "          if(enable) enable_ro[0] = 1;\n");
    	fprintf(punte, "    end\n");
    }
    
	fprintf(punte,"\n\n");
	
	for(i=0;i<N_osciladores;i++)
		pintarOscilador_LUT6_multirouting(punte, config, N_inv, i, XOS_i[i], YOS_i[i], pinmap);
    
    fprintf(punte,"endmodule\n");    
}


void pintarOscilador_LUT6_multirouting(FILE* punte, char config, int N_inversores, int i, int XOS, int YOS, char* pinmap)
{
    int X,Y, celda[2], ocupacion_celda=0;
    int j, aux, contador;
    
    celda[0]=XOS;
    celda[1]=YOS;
    
    
    fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"I0:A1,I1:A2\" *) LUT2 #(4'b1000) AND_%d(.O(w_%d[0]), .I0(enable_ro[%d]), .I1(out_ro[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],i,i,i,i);
    
	contador=0;
    aux=0;
    ocupacion_celda=1;
    for(j=0;j<1;j++)
    {
        if(ocupacion_celda==4)
        {
            if(config=='y') celda[1]++;
            else celda[0]++;
            
            ocupacion_celda=0;
        }
        
        if(aux==N_inversores-1)
        {
            fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6 #(64'h123456789ABCDEF) inv_%d_%d(.O(out_ro[%d]), .I0(sel[%d]), .I1(sel[%d]), .I2(w_%d[%d]), .I3(w_%d[%d]), .i4(w_%d[%d]), .i5(w_%d[%d]));\n\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,contador,contador+1,i,aux,i,aux,i,aux,i,aux);
            
            ocupacion_celda++;
            
            break;
        }
        
        fprintf(punte,"    (* BEL=\"%s\", LOC=\"SLICE_X%dY%d\", DONT_TOUCH=\"true\", LOCK_PINS=\"%s\" *) LUT6 #(64'h123456789ABCDEF) inv_%d_%d(.O(w_%d[%d]), .I0(sel[%d]), .I1(sel[%d]), .I2(w_%d[%d]), .I3(w_%d[%d]), .i4(w_%d[%d]), .i5(w_%d[%d]));\n",bel_array[ocupacion_celda],celda[0],celda[1],pinmap,i,aux,i,aux+1,contador,contador+1,i,aux,i,aux,i,aux,i,aux);
        
        ocupacion_celda++;
        contador+=2;
        aux++;
        
        j=-1;
    }
    fprintf(punte,"\n");
}



void transfiereNumeroOut(FILE* punte, int Nbits)
{
    /*
        Esta funcion genera un modulo para leer numeros de la fpga usando arduino.
     */
    
    fprintf(punte,"module TRANSN_OUT(clock, reset, solicitud, busy, numero, out);\n");
    fprintf(punte,"    //Sentido de la comunicacion:\n\t\t//modo = 0 --> leer de FPGA: cada vez que se solicita, 'pin_comunicacion' extrae un nuevo bit de 'numero'\n\t\t//modo = 1 --> escribir en FPGA: cada vez que se solicita, 'numero[i]' se carga con el bit de 'pin_comunicacion'");
    fprintf(punte,"    //I/O estandar\n");
    fprintf(punte,"    input clock;\n");
    fprintf(punte,"    input reset; \n");
    fprintf(punte,"    input solicitud; \n");
    fprintf(punte,"    output reg busy=1;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //variables internas estandsr\n");
    fprintf(punte,"    reg[1:0] state=`RST;\n");
    fprintf(punte,"    reg[1:0] resetreg=`LOW;\n");
    fprintf(punte,"    reg[1:0] solicitudreg=`LOW;\n");
    fprintf(punte,"    reg[1:0] estable=`LOW;\n");
    fprintf(punte,"    reg[1:0] estacionario=`LOW;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //I/O custom\n");
    fprintf(punte,"    input[%d:0] numero;\n", Nbits-1);
    fprintf(punte,"    output reg out;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //variables internas custom\n");
    fprintf(punte,"    reg [0:%d] i=%d;\n", N_bits_MUX(Nbits)-1, Nbits-1);
    fprintf(punte,"    reg calc_end=0;\n");
    fprintf(punte,"\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        resetreg[0] <= reset;\n");
    fprintf(punte,"        resetreg[1] <= resetreg[0];\n");
    fprintf(punte,"\n");
    fprintf(punte,"        solicitudreg[0] <= solicitud;\n");
    fprintf(punte,"        solicitudreg[1] <= solicitudreg[0];\n");
    fprintf(punte,"    end\n");
    fprintf(punte,"\n");
    fprintf(punte,"    //definicion de estados\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        estable[1] <= estable[0];\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        case (state) \n");
    fprintf(punte,"            `RST: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'RST'\n");
    fprintf(punte,"                if(out==0 && i==%d) estable[0] <= 1;\n", Nbits-1);
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'RST'\n");
    fprintf(punte,"                out <= 0;\n");
    fprintf(punte,"                i <= %d;\n", Nbits-1);
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `RDY: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'RDY'\n");
    fprintf(punte,"                if(calc_end==0) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'RDY'\n");
    fprintf(punte,"                calc_end<=0;\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `CALC: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'CALC'\n");
    fprintf(punte,"                if(calc_end==1) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'CALC'\n");
    fprintf(punte,"                if(calc_end==0) begin\n");
    fprintf(punte,"                    i <= i + 1;\n");
    fprintf(punte,"                    calc_end<=1;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"             end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `OUT: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'OUT'\n");
    fprintf(punte,"                if(out==numero[i]) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'OUT'\n");
    fprintf(punte,"                out <= numero[i];\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"        endcase\n");
    fprintf(punte,"    end\n");
    fprintf(punte,"\n");
    fprintf(punte,"    //bucle dinamico estandar (NO TOCAR!)\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        if(resetreg==`LOW && solicitudreg==`LOW && estable==`HIGH && estacionario==`HIGH) busy <= 0;\n");
    fprintf(punte,"        else busy <= 1;\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        estacionario[1] <= estacionario[0];\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        case(state)\n");
    fprintf(punte,"            `RST: begin\n");
    fprintf(punte,"                if(busy==0) begin\n");
    fprintf(punte,"                    state <= `RDY;\n");
    fprintf(punte,"                    estacionario[0] <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"                else begin\n");
    fprintf(punte,"                    state <= `RST;\n");
    fprintf(punte,"                    estacionario[0] <= 1;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"\n");
    fprintf(punte,"            `RDY: begin\n");
    fprintf(punte,"                if(resetreg==`FALLING) begin\n");
    fprintf(punte,"                    state <= `RST;\n");
    fprintf(punte,"                    estacionario[0] <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"                else begin\n");
    fprintf(punte,"                    if(solicitudreg==`FALLING) begin\n");
    fprintf(punte,"                        state <= `CALC;\n");
    fprintf(punte,"                        estacionario[0] <= 0;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                    else begin\n");
    fprintf(punte,"                        state <= `RDY;\n");
    fprintf(punte,"                        estacionario[0] <= 1;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"\n");
    fprintf(punte,"            `CALC: begin\n");
    fprintf(punte,"                if(resetreg==`FALLING) begin\n");
    fprintf(punte,"                    state <= `RST;\n");
    fprintf(punte,"                    estacionario[0] <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"                else begin\n");
    fprintf(punte,"                    if(busy==0) begin\n");
    fprintf(punte,"                        state <= `OUT;\n");
    fprintf(punte,"                        estacionario[0] <= 0;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                    else begin\n");
    fprintf(punte,"                        state <= `CALC;\n");
    fprintf(punte,"                        estacionario[0] <= 1;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"\n");
    fprintf(punte,"            `OUT: begin\n");
    fprintf(punte,"                if(resetreg==`FALLING) begin\n");
    fprintf(punte,"                    state <= `RST;\n");
    fprintf(punte,"                    estacionario[0] <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"                else begin\n");
    fprintf(punte,"                    if(solicitudreg==`FALLING) begin\n");
    fprintf(punte,"                        state <= `RDY;\n");
    fprintf(punte,"                        estacionario[0] <= 0;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                    else begin\n");
    fprintf(punte,"                        state <= `OUT;\n");
    fprintf(punte,"                        estacionario[0] <= 1;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"        endcase\n");
    fprintf(punte,"    end\n");
    fprintf(punte,"\n");
    fprintf(punte,"endmodule\n");

}


void transfiereNumeroIn(FILE* punte, int Nbits)
{
    /*
        Esta funcion genera un modulo para leer numeros de la fpga usando arduino.
     */
    
    fprintf(punte,"module TRANSN_IN(clock, reset, solicitud, busy, numero, in);\n");
    fprintf(punte,"    //Sentido de la comunicacion:\n\t\t//modo = 0 --> leer de FPGA: cada vez que se solicita, 'pin_comunicacion' extrae un nuevo bit de 'numero'\n\t\t//modo = 1 --> escribir en FPGA: cada vez que se solicita, 'numero[i]' se carga con el bit de 'pin_comunicacion'");
    fprintf(punte,"    //I/O estandar\n");
    fprintf(punte,"    input clock;\n");
    fprintf(punte,"    input reset; \n");
    fprintf(punte,"    input solicitud; \n");
    fprintf(punte,"    output reg busy=1;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //variables internas estandsr\n");
    fprintf(punte,"    reg[1:0] state=`RST;\n");
    fprintf(punte,"    reg[1:0] resetreg=`LOW;\n");
    fprintf(punte,"    reg[1:0] solicitudreg=`LOW;\n");
    fprintf(punte,"    reg[1:0] estable=`LOW;\n");
    fprintf(punte,"    reg[1:0] estacionario=`LOW;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //I/O custom\n");
    fprintf(punte,"    output reg[%d:0] numero;\n", Nbits-1);
    fprintf(punte,"    input in;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //variables internas custom\n");
    fprintf(punte,"    reg[0:%d] i=%d;\n", N_bits_MUX(Nbits)-1, Nbits-1);
    fprintf(punte,"    reg calc_end=0;\n");
    fprintf(punte,"\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        resetreg[0] <= reset;\n");
    fprintf(punte,"        resetreg[1] <= resetreg[0];\n");
    fprintf(punte,"\n");
    fprintf(punte,"        solicitudreg[0] <= solicitud;\n");
    fprintf(punte,"        solicitudreg[1] <= solicitudreg[0];\n");
    fprintf(punte,"    end\n");
    fprintf(punte,"\n");
    fprintf(punte,"    //definicion de estados\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        estable[1] <= estable[0];\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        case (state) \n");
    fprintf(punte,"            `RST: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'RST'\n");
    fprintf(punte,"                if(i==%d) estable[0] <= 1;\n", Nbits-1);
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'RST'\n");
    fprintf(punte,"                numero <= 0;\n");
    fprintf(punte,"                i <= %d;\n", Nbits-1);
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `RDY: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'RDY'\n");
    fprintf(punte,"                if(calc_end==0) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'RDY'\n");
    fprintf(punte,"                calc_end<=0;\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `CALC: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'CALC'\n");
    fprintf(punte,"                if(calc_end==1) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'CALC'\n");
    fprintf(punte,"                if(calc_end==0) begin\n");
    fprintf(punte,"                    i <= i + 1;\n");
    fprintf(punte,"                    calc_end<=1;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"             end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `OUT: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'OUT'\n");
    fprintf(punte,"                if(in==numero[i]) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'OUT'\n");
    fprintf(punte,"                numero[i] <= in;\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"        endcase\n");
    fprintf(punte,"    end\n");
    fprintf(punte,"\n");
    fprintf(punte,"    //bucle dinamico estandar (NO TOCAR!)\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        if(resetreg==`LOW && solicitudreg==`LOW && estable==`HIGH && estacionario==`HIGH) busy <= 0;\n");
    fprintf(punte,"        else busy <= 1;\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        estacionario[1] <= estacionario[0];\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        case(state)\n");
    fprintf(punte,"            `RST: begin\n");
    fprintf(punte,"                if(busy==0) begin\n");
    fprintf(punte,"                    state <= `RDY;\n");
    fprintf(punte,"                    estacionario[0] <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"                else begin\n");
    fprintf(punte,"                    state <= `RST;\n");
    fprintf(punte,"                    estacionario[0] <= 1;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"\n");
    fprintf(punte,"            `RDY: begin\n");
    fprintf(punte,"                if(resetreg==`FALLING) begin\n");
    fprintf(punte,"                    state <= `RST;\n");
    fprintf(punte,"                    estacionario[0] <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"                else begin\n");
    fprintf(punte,"                    if(solicitudreg==`FALLING) begin\n");
    fprintf(punte,"                        state <= `CALC;\n");
    fprintf(punte,"                        estacionario[0] <= 0;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                    else begin\n");
    fprintf(punte,"                        state <= `RDY;\n");
    fprintf(punte,"                        estacionario[0] <= 1;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"\n");
    fprintf(punte,"            `CALC: begin\n");
    fprintf(punte,"                if(resetreg==`FALLING) begin\n");
    fprintf(punte,"                    state <= `RST;\n");
    fprintf(punte,"                    estacionario[0] <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"                else begin\n");
    fprintf(punte,"                    if(busy==0) begin\n");
    fprintf(punte,"                        state <= `OUT;\n");
    fprintf(punte,"                        estacionario[0] <= 0;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                    else begin\n");
    fprintf(punte,"                        state <= `CALC;\n");
    fprintf(punte,"                        estacionario[0] <= 1;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"\n");
    fprintf(punte,"            `OUT: begin\n");
    fprintf(punte,"                if(resetreg==`FALLING) begin\n");
    fprintf(punte,"                    state <= `RST;\n");
    fprintf(punte,"                    estacionario[0] <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"                else begin\n");
    fprintf(punte,"                    if(solicitudreg==`FALLING) begin\n");
    fprintf(punte,"                        state <= `RDY;\n");
    fprintf(punte,"                        estacionario[0] <= 0;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                    else begin\n");
    fprintf(punte,"                        state <= `OUT;\n");
    fprintf(punte,"                        estacionario[0] <= 1;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"        endcase\n");
    fprintf(punte,"    end\n");
    fprintf(punte,"\n");
    fprintf(punte,"endmodule\n");

}
