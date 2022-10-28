`include "def_ctes.local.vh"

module RO_FSM2_PL #(
    //parametros
    parameter   DATA_WIDTH = 32,
    parameter   BUFFER_IN_WIDTH = 16,
    parameter   BUFFER_OUT_WIDTH = 16
    ) (
	//I/O estandar
	input				          clock,
	input[7:0]			          ctrl_in,
	output reg[7:0]		          ctrl_out,
	input[DATA_WIDTH-1:0]         data_in,
	output reg[DATA_WIDTH-1:0]    data_out
	);  
    
    //variables internas estandsr
    reg[3:0] state=`IDLE;
    reg[1:0] estable=`LOW;
    reg[1:0] estacionario=`LOW;
    reg[7:0] ctrl_in_reg;
    reg[DATA_WIDTH-1:0] data_in_reg;
    reg[7:0] contador_std;
    reg[BUFFER_IN_WIDTH-1:0] buffer_in;
    reg[BUFFER_OUT_WIDTH-1:0] buffer_out;
    reg calc_end;
    
    //variables internas custom
    reg[31:0] contador=0;
    reg[31:0] contador_ro;
    reg enable_ro=0;
    wire out_ro;
    wire out_ro_slow;
    wire out_ro_medium;
    wire out_ro_fast;
    
    
    //registro de input
    always @(posedge clock) begin
		ctrl_in_reg <= ctrl_in;
		
		data_in_reg <= data_in;
    end
    
    
    //asignaciones combinacionales/submodulos
    slow_ro (
        .enable( 1'b1 ),
        .out(out_ro_slow)
	);
	
    medium_ro (
        .enable( 1'b1 ),
        .out(out_ro_medium)
	);
	
    fast_ro (
        .enable( 1'b1 ),
        .out(out_ro_fast)
	);
	
	multiplex_4_1 (
	   .selector(buffer_in[1:0]),
	   .entrada0(clock),
	   .entrada1(out_ro_slow),
	   .entrada2(out_ro_medium),
	   .entrada3(out_ro_fast),
	   
	   .salida(out_ro)
	);
	
    always @(posedge out_ro) begin
        if(enable_ro==0)
            contador_ro<=0;
        else
            contador_ro <= contador_ro+1;
    end	
     
    //definicion de estados
    always @(posedge clock) begin
        estable[1] <= estable[0];        
        case (state)
            `IDLE: begin
                //condicion de estabilidad de 'IDLE'
                if(calc_end==0 && contador==0 && contador_ro==0) estable[0] <= 1;
                else estable[0] <= 0;
                
                //definicion de 'IDLE'
                calc_end <= 0;
                contador<=0;
                enable_ro <= 0;
            end
			
            `RST: begin
                //condicion de estabilidad de 'RST'
                if(calc_end==0 && contador==0 && contador_ro==0) estable[0] <= 1;
                else estable[0] <= 0;
                
                //definicion de 'RST'
                calc_end <= 0;
                contador<=0;
                enable_ro <= 0;
            end
            
            `CALC: begin
                //condicion de estabilidad de 'CALC'
                if(calc_end) estable[0] <= 1;
                else estable[0] <= 0;
                
                //definicion de 'CALC'
                if(!calc_end) begin
                    if(contador==0)
                        enable_ro <= 1;
                        
                    if(contador<=100000*buffer_in[BUFFER_IN_WIDTH-1:2])
                        contador <= contador+1;
                    else begin
                        calc_end <= 1;
                        buffer_out <= contador_ro;
                    end
                end
            end
        endcase
    end

    `include "fsm2_pl_stateloop.local.vh"
    
endmodule
