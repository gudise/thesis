`define RST     0
`define IDLE 	1
`define READ 	2
`define DONE 	3

`define LOW 2'b00
`define RISING 2'b01
`define HIGH 2'b11
`define FALLING 2'b10

module HANDSHAKE_RX #(
	parameter BUFFER_WIDTH=32) (
    input clock,
    input reset,
    input ext_data,
    input ext_valid,
    output reg ext_wr_ack=0,
    output reg wr_en=0,
	output reg[BUFFER_WIDTH-1:0] data_out=0
	);
	
    reg[1:0] state=`RST;
    reg[1:0] ext_valid_reg=`LOW;
    reg[1:0] reset_reg=`LOW;
    reg[BUFFER_WIDTH-1:0] buffer_r=0;
	reg[$clog2(BUFFER_WIDTH):0] index=0;
	
	always @(posedge clock) begin   
	   ext_valid_reg[0] <= ext_valid;
	   ext_valid_reg[1] <= ext_valid_reg[0];
	   
	   reset_reg[0] <= reset;
	   reset_reg[1] <= reset_reg[0];
	end
    
    always @(posedge clock) begin
        case(state)
            `RST: begin
                index <= 0;
                data_out <= 0;
                ext_wr_ack <= 0;
                wr_en <= 0;
                
                if(reset_reg==`HIGH)
                    state <= `RST;
                else
                    state <=`IDLE;
            end
            
            `IDLE: begin
                ext_wr_ack <= 0;
                wr_en <= 0;
                
                if(reset_reg==`HIGH)
                    state <= `RST;
                else if(index==BUFFER_WIDTH)
                    state <= `DONE;
                else if(ext_valid_reg==`HIGH)
                    state <= `READ;
            end
            
            `READ: begin
                buffer_r[index] <= ext_data;
                ext_wr_ack <= 1;
                
                if(reset_reg==`HIGH)
                    state <= `RST;
                else if(ext_valid_reg==`LOW) begin
                    index <= index+1;
                    state <= `IDLE;
                end    
            end
            
            `DONE: begin
                wr_en <= 1;
                index <= 0;
                data_out <= buffer_r;
                
                if(reset_reg==`HIGH)
                    state <= `RST;
                else
                    state <= `IDLE;
            end
        endcase
    end
	
endmodule
