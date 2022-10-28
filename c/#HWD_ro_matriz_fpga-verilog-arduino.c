#include "main.h"
#include "hardware.h"


int main(int N_opcion, char** opcion)
{
    FILE* punte;
    char out_name[256]={"wrapper"}, config=1, puertos=0, arduino=0, design_name[300], contraints_name[300], arduino_name[300], directriz='x', env[256], *string;
    long i, N_inv=5, XOS_i=0, YOS_i=0, XOS_incr=1, YOS_incr=1, XOS_max=20, YOS_max=20, N_osciladores=10, resolucion=1000000, N_claves=100;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa crea todos los ficheros necesarios (.v .xdc .ino) para implementar una matriz de osciladores de anillo en FPGA y medir sus frecuencias de oscilacion utilizando una interfaz Arduino. Para medir la frecuencia de un oscilador se mide el tiempo que le cuesta a este realizar 'resolucion' oscilaciones\n----------------------------------\n\n");
            printf("-out [wrapper], todas las salidas del programa tendran este nombre seguido de la extension correspondiente\n\n");
            printf("-config [vertical] {vertical, horizontal}, direccion en la que se colocaran los inversores de cada oscilador\n\n");
            printf("-directriz [x] ¤ {x, y}, esta opcion indica la direccion de crecimiento de la matriz (por defecto a lo largo del eje 'x')\n\n");
            printf("-Ninv [5], numero de inversores de cada oscilador\n\n");
            printf("-XOSi [0], coordenada X de la LUT a partir de la cual se construye la matriz\n\n");
            printf("-YOSi [0], coordenada Y de la LUT a partir de la cual se construye la matriz\n\n");
            printf("-XOSincr [1], distancia entre las coordenadas X de dos osciladores adyacentes\n\n");
            printf("-YOSincr [1], distancia entre las coordenadas Y de dos osciladores adyacentes\n\n");
            printf("-XOSmax [20], coordenada X maxima de la matriz de osciladores implementada\n\n");
            printf("-YOSmax [20], coordenada Y maxima de la matriz de osciladores implementada\n\n");
            printf("-Nosc [10], numero de osciladores de la matriz. Si Nosc=-1, la matriz se llenara hasta completar las celdas indicadas en XOSmax, YOSmax\n\n");
            printf("-resolucion [1000000], numero de ciclos de medicion para cada oscilador\n\n");
            printf("-puertos [pynq] {pynq, zybo, spartan}, si esta opcion esta presente el programa creara un fichero de restricciones (.xdc o .ucf) con la relación de puertos E/S de la placa \n\n");
            printf("-arduino [100], si esta opcion esta presente el programa genera un archivo '.ino' para programar un arduino. Si esta opcion es acompanada de un numero, este sera el numero de veces que se ejecute la funcion 'loop'. Arduino escribe la medida de cada iteracion del bucle  en una linea nueva, y escribe '-fin-' al terminar\n\n");
            
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
        
        if( strcmp(opcion[i], "-directriz")==0 )
        {
            if(strcmp(opcion[i+1], "y")==0 || strcmp(opcion[i+1], "Y")==0)
                directriz='y';
        }
        
        if( strcmp(opcion[i], "-Ninv")==0 )
            sscanf(opcion[i+1], "%ld", &N_inv);
        
        if( strcmp(opcion[i], "-XOSi")==0 )
            sscanf(opcion[i+1], "%ld", &XOS_i);
        
        if( strcmp(opcion[i], "-YOSi")==0 )
            sscanf(opcion[i+1], "%ld", &YOS_i);
        
        if( strcmp(opcion[i], "-XOSincr")==0 )
            sscanf(opcion[i+1], "%ld", &XOS_incr);
        
        if( strcmp(opcion[i], "-YOSincr")==0 )
            sscanf(opcion[i+1], "%ld", &YOS_incr);
        
        if( strcmp(opcion[i], "-XOSmax")==0 )
            sscanf(opcion[i+1], "%ld", &XOS_max);
        
        if( strcmp(opcion[i], "-YOSmax")==0 )
            sscanf(opcion[i+1], "%ld", &YOS_max);
        
        if( strcmp(opcion[i], "-Nosc")==0 )
            sscanf(opcion[i+1], "%ld", &N_osciladores);
        
        if( strcmp(opcion[i], "-resolucion")==0 )
            sscanf(opcion[i+1], "%ld", &resolucion);
        
        if( strcmp(opcion[i], "-puertos")==0 )
        {
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
                {
                    if(strcmp(opcion[i+1],"pynq")==0)
                        puertos = 1;
                    
                    else if(strcmp(opcion[i+1],"zybo")==0)
                        puertos = 2;
                    
                    else if(strcmp(opcion[i+1],"spartan")==0)
                        puertos = 3;
                }
            }
        }
        
        if( strcmp(opcion[i], "-arduino")==0 )
        {
            arduino=1;
            if(i<N_opcion-1)
            {
                if(opcion[i+1][0]!='-')
                    sscanf(opcion[i+1], "%ld", &N_claves);
            }
        }
    }
    if(N_osciladores<0)
        N_osciladores=1000000000;
    
    N_osciladores = configOsciladores(NULL, config, N_inv, XOS_i, YOS_i, XOS_incr, YOS_incr, N_osciladores, XOS_max, YOS_max, directriz);
    
    sprintf(design_name,"%s.v", out_name);
    
    punte=fopen(design_name,"w");
    
    fprintf(punte,"//N_osciladores: %ld\n", N_osciladores);
    fprintf(punte,"`timescale 1ns / 1ps\n");
    fprintf(punte,"\n");
    fprintf(punte,"`define LOW 2'b00\n");
    fprintf(punte,"`define RISING 2'b01\n");
    fprintf(punte,"`define FALLING 2'b10\n");
    fprintf(punte,"`define HIGH 2'b11\n");
    fprintf(punte,"\n");
    fprintf(punte,"`define RST 2'd0\n");
    fprintf(punte,"`define RDY 2'd1\n");
    fprintf(punte,"`define CALC 2'd2\n");
    fprintf(punte,"`define OUT 2'd3\n");
    fprintf(punte,"\n");
    fprintf(punte,"\n");
    fprintf(punte,"module wrapper(clock, reset, solicitud, busy, solicitud_out, solicitud_in, busy_out, busy_in, leer_fpga, escribir_fpga);\n");
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
    fprintf(punte,"    input solicitud_out;\n");
    fprintf(punte,"    output busy_out;\n");
    fprintf(punte,"    output leer_fpga;\n");
    fprintf(punte,"    input solicitud_in;\n");
    fprintf(punte,"    output busy_in;\n");
    fprintf(punte,"    input escribir_fpga;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //variables internas custom\n");
    fprintf(punte,"    reg[0:31] count=0;\n");
    fprintf(punte,"    reg[0:31] clock_count=0;\n");
    fprintf(punte,"    reg rst_count=0;\n");
    fprintf(punte,"    reg enable_ro=0;\n");
    fprintf(punte,"    wire out_mux;\n");
    fprintf(punte,"    reg calc_end=0;\n");
    fprintf(punte,"    reg[0:%ld] selector=0;\n", N_bits_MUX(N_osciladores)-1);
    fprintf(punte,"    wire[0:31] numero_in;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //registro de input\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        resetreg[0] <= reset;\n");
    fprintf(punte,"        resetreg[1] <= resetreg[0];\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        solicitudreg[0] <= solicitud;\n");
    fprintf(punte,"        solicitudreg[1] <= solicitudreg[0];\n");
    fprintf(punte,"    end\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //asignaciones combinacionales/submodulos\n");
    fprintf(punte,"    (* KEEP_HIERARCHY = \"yes\" *) ROPUF_OSC ropuf_osc(clock, selector, out_mux, enable_ro);\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    TRANSN_IN(clock, reset, solicitud_in, busy_in, numero_in, escribir_fpga);\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    TRANSN_OUT(clock, reset, solicitud_out, busy_out, count, leer_fpga);\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    always @(posedge out_mux) begin\n");
    fprintf(punte,"        if(rst_count==1)\n");
    fprintf(punte,"            count <= 0;\n");
    fprintf(punte,"        else if(enable_ro==1)\n");
    fprintf(punte,"            count <= count +1; \n");
    fprintf(punte,"    end\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //definicion de estados\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        estable[1] <= estable[0];\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        case (state) \n");
    fprintf(punte,"            `RST: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'RST'\n");
    fprintf(punte,"                if(enable_ro==0 && rst_count==1) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'RST'\n");
    fprintf(punte,"                enable_ro <= 0;\n");
    fprintf(punte,"                selector <= 0;\n");
    fprintf(punte,"                rst_count <= 1;\n");
    fprintf(punte,"                clock_count <= 0;\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `RDY: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'RDY'\n");
    fprintf(punte,"                if(calc_end==0 && enable_ro==0 && rst_count==1) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'RDY'\n");
    fprintf(punte,"                calc_end <= 0;\n");
    fprintf(punte,"                enable_ro <= 0;\n");
    fprintf(punte,"                rst_count <= 1;\n");
    fprintf(punte,"                clock_count <= 0;\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `CALC: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'CALC'\n");
    fprintf(punte,"                if(calc_end==1) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'CALC'\n");
    fprintf(punte,"                if(calc_end==0) begin\n");
    fprintf(punte,"                    enable_ro<=1;\n");
    fprintf(punte,"                    rst_count <= 0;\n");
    fprintf(punte,"                    clock_count <= clock_count+1;\n");
    fprintf(punte,"                    if(clock_count==%ld) begin\n", resolucion);
    fprintf(punte,"                        calc_end<=1;\n");
    fprintf(punte,"                        enable_ro<=0;\n");
    fprintf(punte,"                    end\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `OUT: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'OUT'\n");
    fprintf(punte,"                if(!calc_end) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'OUT'\n");
    fprintf(punte,"                if(calc_end) begin\n");
    fprintf(punte,"                    selector <= numero_in;\n");
    fprintf(punte,"                    calc_end <= 0;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"        endcase\n");
    fprintf(punte,"    end\n");
    fprintf(punte,"    \n");
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
    fprintf(punte,"    \n");
    fprintf(punte,"endmodule\n");
    fprintf(punte," \n");
    fprintf(punte," \n");

    transfiereNumeroIn(punte, 32);
    fprintf(punte," \n");
    fprintf(punte," \n");
    
    transfiereNumeroOut(punte, 32);
    fprintf(punte," \n");
    fprintf(punte," \n");
    
    
    configOsciladores(punte, config, N_inv, XOS_i, YOS_i, XOS_incr, YOS_incr, N_osciladores, XOS_max, YOS_max, directriz);
    
    fclose(punte);
    
    
    switch(puertos)
    {
        case 1:
            sprintf(contraints_name,"%s.xdc", out_name);
            punte=fopen(contraints_name,"w");
            fprintf(punte,"#Xilinx part: xc7z020clg400-1\n\n##Clock signal\n");
            fprintf(punte,"set_property -dict {PACKAGE_PIN H16 IOSTANDARD LVCMOS33} [get_ports clock]\n");
            fprintf(punte,"create_clock -period 8.000 -name clock -waveform {0.000 4.000} -add [get_ports clock]\n");
            fprintf(punte,"\n");
            fprintf(punte,"##Arduino IO\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN U13   IOSTANDARD LVCMOS33 } [get_ports { reset }]; #ar[2]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN V13   IOSTANDARD LVCMOS33 } [get_ports { solicitud }]; #ar[3]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN V15   IOSTANDARD LVCMOS33 } [get_ports { busy }]; #ar[4]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN T15   IOSTANDARD LVCMOS33 } [get_ports { solicitud_out }]; #ar[5]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN R16   IOSTANDARD LVCMOS33 } [get_ports { busy_out }]; #ar[6]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN U17   IOSTANDARD LVCMOS33 } [get_ports { leer_fpga }]; #ar[7]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN V17   IOSTANDARD LVCMOS33 } [get_ports { solicitud_in }]; #ar[8]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN V18   IOSTANDARD LVCMOS33 } [get_ports { busy_in }]; #ar[9]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN T16   IOSTANDARD LVCMOS33 } [get_ports { escribir_fpga }]; #ar[10]\n");
            fprintf(punte,"\n");
            fclose(punte);
            break;
            
        case 2:
            sprintf(contraints_name,"%s.xdc", out_name);
            punte=fopen(contraints_name,"w");
            fprintf(punte,"#Xilinx part: xc7z010clg400-1\n\n##Clock signal\n");
            fprintf(punte,"set_property -dict {PACKAGE_PIN L16 IOSTANDARD LVCMOS33} [get_ports clock]\n");
            fprintf(punte,"create_clock -period 8.000 -name clock -waveform {0.000 4.000} -add [get_ports clock]\n");
            fprintf(punte,"\n");
            fprintf(punte,"##Pmod Header JB\n");
            fprintf(punte,"set_property -dict {PACKAGE_PIN T20 IOSTANDARD LVCMOS33} [get_ports reset]\n");
            fprintf(punte,"set_property -dict {PACKAGE_PIN U20 IOSTANDARD LVCMOS33} [get_ports solicitud]\n");
            fprintf(punte,"set_property -dict {PACKAGE_PIN V20 IOSTANDARD LVCMOS33} [get_ports busy]\n");
            fprintf(punte,"set_property -dict {PACKAGE_PIN W20 IOSTANDARD LVCMOS33} [get_ports solicitud_transfer]\n");
            fprintf(punte,"\n");
            fclose(punte);
            break;
            
        case 3:
            sprintf(contraints_name,"%s.ucf", out_name);
            punte=fopen(contraints_name,"w");
            fprintf(punte,"#Xilinx part: spartan\n\n##Clock signal\n");
            fprintf(punte,"NET %cclock%c LOC = A10;\n",34,34);
            fprintf(punte,"\n");
            fprintf(punte,"##IO signals\n");
            fprintf(punte,"NET %creset%c LOC = A14; #u8-7, arduino 2\n",34,34);
            fprintf(punte,"NET %csolicitud%c LOC = C13; #u8-9, arduino 3\n",34,34);
            fprintf(punte,"NET %creadout%c LOC = B12; #u8-11, arduino 4\n",34,34);
            fprintf(punte,"NET %cbusy%c LOC = C11; #u8-13, arduino 5\n",34,34);
            fprintf(punte,"NET %ccomienza_medida%c LOC = B10; #u8-15, arduino 6\n",34,34);
            fprintf(punte,"NET %cacaba_medida%c LOC = C9;  #u8-17, arduino 7\n",34,34);
            fprintf(punte,"NET %cfin%c LOC = B8;  #u8-19, arduino 8\n",34,34);
            fclose(punte);
            break;
    }
    
    if(arduino)
    {
        sprintf(arduino_name,"%s.ino", out_name);
        punte=fopen(arduino_name,"w");
		
		sprintf(env, "%s", getenv("COMPUTACION0"));
		string=env+7;

		for(i=0; i<strlen(string); i++)
		{
			if(string[i]=='/')
				string[i]='\\';
		}
	
		fprintf(punte,"#include \"C:\\%s\\..\\recursos_consulta\\diseno electronico\\FSM\\fsm_arduino\\fsm.h\"\n", string);
        fprintf(punte,"#define Nfilas %ld\n", N_claves);
		fprintf(punte,"#define Ncolumnas %ld\n", N_osciladores);
		fprintf(punte,"#define Nbits %d\n", 32);
        fprintf(punte,"\n");
		fprintf(punte,"const int reset=2,\n");
		fprintf(punte,"          solic=3,\n");
		fprintf(punte,"          busy=4,\n");
		fprintf(punte,"          solic_out=5,\n");
		fprintf(punte,"          busy_out=6,\n");
		fprintf(punte,"          leer_fpga=7,\n");
		fprintf(punte,"          solic_in=8,\n");
		fprintf(punte,"          busy_in=9,\n");
		fprintf(punte,"          escribir_fpga=10;\n");
        fprintf(punte,"int i, j;\n");
        fprintf(punte,"\n");
        fprintf(punte,"void setup() {\n");
        fprintf(punte,"  //put your setup code here, to run once:\n");
		fprintf(punte,"  pinMode(reset, OUTPUT);\n");
		fprintf(punte,"  pinMode(solic, OUTPUT);\n");
		fprintf(punte,"  pinMode(busy, INPUT);\n");
		fprintf(punte,"  pinMode(solic_out, OUTPUT);\n");
		fprintf(punte,"  pinMode(busy_out, INPUT);\n");
		fprintf(punte,"  pinMode(leer_fpga, INPUT);\n");
		fprintf(punte,"  pinMode(solic_in, OUTPUT);\n");
		fprintf(punte,"  pinMode(busy_in, INPUT);\n");
		fprintf(punte,"  pinMode(escribir_fpga, OUTPUT);\n");
        fprintf(punte,"  \n");
        fprintf(punte,"  Serial.begin(9600);\n");
        fprintf(punte,"}\n");
        fprintf(punte,"\n");
        fprintf(punte,"void loop() {\n");
        fprintf(punte,"    // put your main code here, to run repeatedly:\n");
		fprintf(punte,"    Serial.flush();\n");
		fprintf(punte,"    Serial.println();\n");
        fprintf(punte,"    Serial.print(\"#[N_filas] \"); Serial.println(Nfilas);\n");
		fprintf(punte,"    Serial.print(\"#[N_columnas] \"); Serial.println(Ncolumnas);\n");
		fprintf(punte,"    Serial.println(\"#[matriz]\\n\");\n");
        fprintf(punte,"\n");
        fprintf(punte,"  for(i=0; i<Nfilas; i++)\n");
        fprintf(punte,"  {\n");
        fprintf(punte,"    FSM_reset_to_ready(reset, busy);\n");
        fprintf(punte,"\n");
        fprintf(punte,"    for(j=0; j<Ncolumnas; j++)\n");
        fprintf(punte,"    {\n");
        fprintf(punte,"      escribir_numero_a_fpga(Nbits, solic_in, busy_in, escribir_fpga, j);\n");        
        fprintf(punte,"\n");
        fprintf(punte,"      FSM_ready_to_out(solic, busy);\n");      
        fprintf(punte,"\n");
        fprintf(punte,"      Serial.print(leer_numero_de_fpga(Nbits, solic_out, busy_out, leer_fpga));\n");
        fprintf(punte,"      Serial.print(\" \");\n");
        fprintf(punte,"\n");
        fprintf(punte,"      FSM_out_to_ready(solic, busy);\n");
        fprintf(punte,"    }\n");
        fprintf(punte,"    Serial.println(\"\");\n");
        fprintf(punte,"  }\n");
        fprintf(punte,"    Serial.println(\"#[fin]\");\n");
        fprintf(punte,"    Serial.end();\n");
        fprintf(punte,"    while(1){}\n");
        fprintf(punte,"}\n");
        
        
        fclose(punte);
    }
    
    return 0;
}


