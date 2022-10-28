#include "main.h"
#include "hardware.h"


int main(int N_opcion, char** opcion)
{
    FILE* punte;
    int i, Nbits=32, Ncount=100;
    char out_name[256]={"wrapper"}, puertos=1, design_name[300], contraints_name[300], arduino_name[300], env[256], *string;
    
    //lectura de opciones
    for(i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            printf("\nEste programa crea todos los ficheros necesarios (.v .xdc .ino) para implementar un diseno 'debug' de una maquina de estados finitos (FSM) junto con una interfaz de lectura/escritura (TRANSN)\n----------------------------------\n\n");
            printf("-out [wrapper], todas las salidas del programa tendran este nombre seguido de la extension correspondiente\n\n");
            printf("-nbits [32], esta opcion permite especificar el numero de bits que utilizara el modulo de transferencia numerica (TRANSN)\n\n");
            printf("-ncount [100], esta opcion permite cambiar el numero de ciclos contados por la FPGA y transferidos al arduino a traves de TRANSN\n\n");
            //printf("-puertos [pynq] {pynq, zybo, spartan}, si esta opcion esta presente el programa creara un fichero de restricciones (.xdc o .ucf) con la relación de puertos E/S de la placa \n\n"); //por ahora solo estan disponibles los puertos de pynq-z2
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-out")==0 )
            sscanf(opcion[i+1], "%s", out_name);
        
        if( strcmp(opcion[i], "-nbits")==0 )
            sscanf(opcion[i+1], "%d", &Nbits);
        
        if( strcmp(opcion[i], "-ncount")==0 )
            sscanf(opcion[i+1], "%d", &Ncount);
        
        /*if( strcmp(opcion[i], "-puertos")==0 )
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
        }*/
    }
    
    sprintf(design_name,"%s.v", out_name);
    
    punte=fopen(design_name,"w");
    
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
    fprintf(punte,"module wrapper(clock, reset, solicitud, busy, solicitud_transn, busy_transn, leer_fpga, escribir_fpga, modo, modo_led, led);\n");
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
    fprintf(punte,"    input solicitud_transn;\n");
    fprintf(punte,"    input modo;\n");
    fprintf(punte,"    input escribir_fpga;\n");
    fprintf(punte,"    output modo_led;\n");
    fprintf(punte,"    output leer_fpga;\n");
    fprintf(punte,"    output busy_transn;\n");
    fprintf(punte,"    output[0:3] led;\n");
    fprintf(punte,"    \n");
    fprintf(punte,"    //variables internas custom\n");
    fprintf(punte,"    reg[0:%d] count=0;\n", Nbits-1);
    fprintf(punte,"    wire pin_comunicacion;\n");
    fprintf(punte,"    wire[0:%d] numero;\n", Nbits-1);
    fprintf(punte,"    reg calc_end=0;\n");
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
    fprintf(punte,"    assign modo_led = modo;\n");
    fprintf(punte,"    assign led[0] = count[0];\n");
    fprintf(punte,"    assign led[1] = count[1];\n");
    fprintf(punte,"    assign led[2] = count[2];\n");
    fprintf(punte,"    assign led[3] = count[3];\n");
    fprintf(punte,"    assign numero = modo? %d'bz : count;\n", Nbits);
    fprintf(punte,"    assign pin_comunicacion = modo? escribir_fpga : 1'bz;\n");
    fprintf(punte,"    assign leer_fpga = modo? 1'bz : pin_comunicacion;\n");
    fprintf(punte,"    TRANSN transfer(clock, reset, solicitud_transn, busy_transn, numero, pin_comunicacion, modo);\n");
    fprintf(punte,"\n");
    fprintf(punte,"    //definicion de estados\n");
    fprintf(punte,"    always @(posedge clock) begin\n");
    fprintf(punte,"        estable[1] <= estable[0];\n");
    fprintf(punte,"        \n");
    fprintf(punte,"        case (state) \n");
    fprintf(punte,"            `RST: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'RST'\n");
    fprintf(punte,"                if(count==0) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'RST'\n");
    fprintf(punte,"                count <= 0;\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `RDY: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'RDY'\n");
    fprintf(punte,"                if(calc_end==0) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'RDY'\n");
    fprintf(punte,"                calc_end <= 0;\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `CALC: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'CALC'\n");
    fprintf(punte,"                if(calc_end==1) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'CALC'\n");
    fprintf(punte,"                if(calc_end==0) begin\n");
    fprintf(punte,"                    count <= numero;\n");
    fprintf(punte,"                    calc_end <= 1;\n");
    fprintf(punte,"                end\n");
    fprintf(punte,"            end\n");
    fprintf(punte,"            \n");
    fprintf(punte,"            `OUT: begin\n");
    fprintf(punte,"                //condicion de estabilidad de 'OUT'\n");
    fprintf(punte,"                if(1) estable[0] <= 1;\n");
    fprintf(punte,"                else estable[0] <= 0;\n");
    fprintf(punte,"                \n");
    fprintf(punte,"                //definicion de 'OUT'\n");
    fprintf(punte,"                /*\n");
    fprintf(punte,"                Conseguir condicion de output\n");
    fprintf(punte,"                */\n");
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
    
    transfiereNumero(punte, Nbits);
    fprintf(punte," \n");
    fprintf(punte," \n");
    
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
            fprintf(punte,"set_property -dict { PACKAGE_PIN T15   IOSTANDARD LVCMOS33 } [get_ports { solicitud_transn }]; #ar[5]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN R16   IOSTANDARD LVCMOS33 } [get_ports { busy_transn }]; #ar[6]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN U17   IOSTANDARD LVCMOS33 } [get_ports { leer_fpga }]; #ar[7]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN V17   IOSTANDARD LVCMOS33 } [get_ports { escribir_fpga }]; #ar[8]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN V18   IOSTANDARD LVCMOS33 } [get_ports { modo }]; #ar[9]\n");
            fprintf(punte,"\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN N15   IOSTANDARD LVCMOS33 } [get_ports { modo_led }]; #led_r\n");
            fprintf(punte,"\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN R14   IOSTANDARD LVCMOS33 } [get_ports { led[0] }]; #led[0]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN P14   IOSTANDARD LVCMOS33 } [get_ports { led[1] }]; #led[1]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN N16   IOSTANDARD LVCMOS33 } [get_ports { led[2] }]; #led[2]\n");
            fprintf(punte,"set_property -dict { PACKAGE_PIN M14   IOSTANDARD LVCMOS33 } [get_ports { led[3] }]; #led[3]\n");

            fclose(punte);
            break;
            
//         case 2:
//             sprintf(contraints_name,"%s.xdc", out_name);
//             punte=fopen(contraints_name,"w");
//             fprintf(punte,"#Xilinx part: xc7z010clg400-1\n\n##Clock signal\n");
//             fprintf(punte,"set_property -dict {PACKAGE_PIN L16 IOSTANDARD LVCMOS33} [get_ports clock]\n");
//             fprintf(punte,"create_clock -period 8.000 -name clock -waveform {0.000 4.000} -add [get_ports clock]\n");
//             fprintf(punte,"\n");
//             fprintf(punte,"##Pmod Header JB\n");
//             fprintf(punte,"set_property -dict {PACKAGE_PIN T20 IOSTANDARD LVCMOS33} [get_ports reset]\n");
//             fprintf(punte,"set_property -dict {PACKAGE_PIN U20 IOSTANDARD LVCMOS33} [get_ports solicitud]\n");
//             fprintf(punte,"set_property -dict {PACKAGE_PIN V20 IOSTANDARD LVCMOS33} [get_ports busy]\n");
//             fprintf(punte,"set_property -dict {PACKAGE_PIN W20 IOSTANDARD LVCMOS33} [get_ports solicitud_transn]\n");
//             fprintf(punte,"\n");
//             fclose(punte);
//             break;
//             
//         case 3:
//             sprintf(contraints_name,"%s.ucf", out_name);
//             punte=fopen(contraints_name,"w");
//             fprintf(punte,"#Xilinx part: spartan\n\n##Clock signal\n");
//             fprintf(punte,"NET %cclock%c LOC = A10;\n",34,34);
//             fprintf(punte,"\n");
//             fprintf(punte,"##IO signals\n");
//             fprintf(punte,"NET %creset%c LOC = A14; #u8-7, arduino 2\n",34,34);
//             fprintf(punte,"NET %csolicitud%c LOC = C13; #u8-9, arduino 3\n",34,34);
//             fprintf(punte,"NET %creadout%c LOC = B12; #u8-11, arduino 4\n",34,34);
//             fprintf(punte,"NET %cbusy%c LOC = C11; #u8-13, arduino 5\n",34,34);
//             fprintf(punte,"NET %ccomienza_medida%c LOC = B10; #u8-15, arduino 6\n",34,34);
//             fprintf(punte,"NET %cacaba_medida%c LOC = C9;  #u8-17, arduino 7\n",34,34);
//             fprintf(punte,"NET %cfin%c LOC = B8;  #u8-19, arduino 8\n",34,34);
//             fclose(punte);
//             break;
    }
    
    
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
    fprintf(punte,"\n");
    fprintf(punte,"#define Nfilas %d\n", Ncount);
	fprintf(punte,"#define Ncolumnas 1\n");
	fprintf(punte,"#define Nbits %d\n", Nbits);
	fprintf(punte,"\n");
    fprintf(punte,"const int reset=2,\n");
    fprintf(punte,"          solic=3,\n");
    fprintf(punte,"          busy=4,\n");
    fprintf(punte,"          solic_transn=5,\n");
    fprintf(punte,"          busy_transn=6,\n");
    fprintf(punte,"          leer_fpga=7,\n");
    fprintf(punte,"          escr_fpga=8,\n");
    fprintf(punte,"          modo=9; //LOW-->sentido lectura de la fpga, HIGH-->sentido escritura en la fpga\n");
    fprintf(punte,"int i, j, n;\n");
    fprintf(punte,"\n");
    fprintf(punte,"void setup() {\n");
    fprintf(punte,"  //put your setup code here, to run once:\n");
    fprintf(punte,"  pinMode(reset, OUTPUT);\n");
    fprintf(punte,"  pinMode(solic, OUTPUT);\n");
    fprintf(punte,"  pinMode(busy, INPUT);\n");
    fprintf(punte,"  pinMode(solic_transn, OUTPUT);\n");
    fprintf(punte,"  pinMode(busy_transn, INPUT);\n");
    fprintf(punte,"  pinMode(leer_fpga, INPUT);\n");
    fprintf(punte,"  pinMode(escr_fpga, OUTPUT);\n");
    fprintf(punte,"  pinMode(modo, OUTPUT);\n");
    fprintf(punte,"  \n");
    fprintf(punte,"  Serial.begin(9600);\n");
    fprintf(punte,"}\n");
    fprintf(punte,"\n");
    fprintf(punte,"void loop() {\n");
    fprintf(punte,"    // put your main code here, to run repeatedly:\n");
    fprintf(punte,"\n");
	fprintf(punte,"  Serial.println(\"# [N_filas] %d\");\n", Ncount);
	fprintf(punte,"  Serial.println(\"# [N_columnas] %d\");\n", 1);
	fprintf(punte,"  Serial.println(\"# [matriz]\");\n");
	fprintf(punte,"\n");
    fprintf(punte,"  FSM_reset_to_ready(reset, busy);\n");
    fprintf(punte,"\n");
    fprintf(punte,"  for(i=0; i<Nfilas; i++)\n");
    fprintf(punte,"  {\n");
    fprintf(punte,"    delay(500);\n");
    fprintf(punte,"\n");
    fprintf(punte,"    escribir_numero_a_fpga(Nbits, solic_transn, busy_transn, escr_fpga, modo, i);\n");
    fprintf(punte,"    FSM_ready_to_out(solic, busy);\n");      
    fprintf(punte,"    Serial.print(leer_numero_de_fpga(Nbits, solic_transn, busy_transn, leer_fpga, modo)); Serial.println(\"\");\n");
    fprintf(punte,"    FSM_out_to_ready(solic, busy);\n");
    fprintf(punte,"  }\n");
    fprintf(punte,"  Serial.println(\"\\n# [fin]\");\n");
    fprintf(punte,"  Serial.end();\n");
    fprintf(punte,"  while(1){}\n");
    fprintf(punte,"}\n");
    
    
    fclose(punte);
    
    
    return 0;
}


