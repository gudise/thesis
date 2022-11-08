`include "interfaz_pl_define.cp.vh"

module INTERFAZ_PL_FRONTEND #(
    //parametros
    parameter   DATA_WIDTH = 32,
    parameter   BUFFER_IN_WIDTH = 16,
    parameter   BUFFER_OUT_WIDTH = 16
    ) (
	//I/O estandar: ninguna de estas cantidades las manipula el usuario.
	input				          clock,
	input[7:0]			          ctrl_in,
	output reg[7:0]		          ctrl_out,
	input[DATA_WIDTH-1:0]         data_in, 
	output reg[DATA_WIDTH-1:0]    data_out
	
	//I/O custom
	/*
	*
	*/
	);

	//buffer de entrada/salida
    reg[BUFFER_IN_WIDTH-1:0]	buffer_in; //dato de entrada (legible por el usuario)
    reg[BUFFER_OUT_WIDTH-1:0]	buffer_out; //dato de salida (a rellenar por el usuario)
	
    //variables internas estandsr
    reg[3:0] state=`IDLE;
    reg[1:0] busy_frontend=`LOW;
    reg[1:0] busy_backend=`LOW;
    reg[7:0] ctrl_in_reg;
    reg[DATA_WIDTH-1:0] data_in_reg;
    reg[7:0] contador_std;
    reg calc_end;
    
    //variables internas custom
    /*
    *
    */
    
    //registro de input
    always @(posedge clock) begin
		ctrl_in_reg <= ctrl_in;
		
		data_in_reg <= data_in;
    end
    
    //asignaciones combinacionales/submodulos
    /*

    */
     
    //definicion de estados
    always @(posedge clock) begin
        busy_frontend[1] <= busy_frontend[0];        
        case (state)
            `IDLE: begin
                //condicion de estabilidad de 'IDLE'
                if(calc_end==0) busy_frontend[0] <= 1;
                else busy_frontend[0] <= 0;
                
                //definicion de 'IDLE'
                calc_end <= 0;
                /*
                {variable_interna_custom_i} <= 0
                */
            end
			
            `RST: begin
                //condicion de estabilidad de 'RST'
                if(calc_end==0) busy_frontend[0] <= 1;
                else busy_frontend[0] <= 0;
                
                //definicion de 'RST'
                calc_end <= 0;
                /*
                {variable_interna_custom_i} <= 0
                */
            end
            
            `CALC: begin
                //condicion de estabilidad de 'CALC'
                if(calc_end) busy_frontend[0] <= 1;
                else busy_frontend[0] <= 0;
                
                //definicion de 'CALC'
                if(!calc_end) begin
					buffer_out <= buffer_in + 1; // en este ejemplo tomamos el numero introducido y sumamos '1'
                    /*
                    * La operacion de calculo
                    * debe conducir a producir
                    * una variable 'result'
                    */

                    calc_end <=1;
                end
            end
        endcase
    end

    `include "interfaz_pl_backend.cp.vh"
    
endmodule
