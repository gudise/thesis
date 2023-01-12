`define LOW 2'b00
`define RISING 2'b01
`define FALLING 2'b10
`define HIGH 2'b11

`define IDLE		1
`define RST		 2
`define RST_SYNC	3
`define CALC		4
`define CALC_SYNC   5
`define SCAN		6
`define SCAN_SYNC   7
`define PRINT	   8
`define PRINT_SYNC  9

`define cmd_idle		0
`define cmd_reset	   1
`define cmd_calc		2
`define cmd_scan		3
`define cmd_print	   4
`define cmd_end			5

`define cmd_idle_sync	 6
`define cmd_reset_sync	 7
`define cmd_calc_sync	 8
`define cmd_scan_sync	9
`define cmd_print_sync   10

module INTERFAZ_PL_FRONTEND #(
	//parametros
	parameter   DATA_WIDTH=32,
	parameter   BUFFER_IN_WIDTH=16,
	parameter   BUFFER_OUT_WIDTH=16
	) (
	//I/O estandar: ninguna de estas cantidades las manipula el usuario.
	input							clock,
	input[7:0]						ctrl_in,	// instrucción de PS.
	output reg[7:0]					ctrl_out,	// instrucción a PS.
	input[DATA_WIDTH-1:0]			data_in,	// dato de PS.
	output reg[DATA_WIDTH-1:0]		data_out,	// dato a PS.
	output reg						sync=1'b0,	//
	input							ack,		// intefaz 'handshake' con PL.
	output reg[BUFFER_IN_WIDTH-1:0]	buffer_in,	// 'buffer' enviado por PS.
	input[BUFFER_OUT_WIDTH-1:0]		buffer_out	// 'buffer' devuelto a PS.
	
	//I/O custom
	/*
	*
	*/
	);

	//variables internas estandar
	reg[3:0] state=`IDLE;
	reg[1:0] busy_frontend=`LOW;
	reg[1:0] busy_backend=`LOW;
	reg[7:0] ctrl_in_reg;
	reg[DATA_WIDTH-1:0] data_in_reg;
	reg[7:0] contador_std;
	
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
				if(ack==0) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//definicion de 'IDLE'
				/*
				{variable_interna_custom_i} <= 0
				*/
			end
			
			`RST: begin
				//condicion de estabilidad de 'RST'
				if(ack==0) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//definicion de 'RST'
				/*
				sync <= 0; // Por completitud (en realidad no hace falta).
				{variable_interna_custom_i} <= 0
				*/
			end
			
			`CALC: begin
				//condicion de estabilidad de 'CALC'
				if(ack) busy_frontend[0] <= 1;
				else busy_frontend[0] <= 0;
				
				//comunicación 'handshake' con PL
				case({ack,sync})
					{1'b0,1'b0}: begin
						sync <= 1'b1; // MASTER inicia la comunicación.
					end
					
					{1'b0,1'b1}: begin
						// Esperamos a que SLAVE conteste subiendo 'ack'.
					end
					
					{1'b1,1'b1}: begin
						sync <= 1'b0; // MASTER termina el ciclo de comunicación.
					end
					
					{1'b1,1'b0}: begin
						// SLAVE bajará 'ack'.
					end
				endcase
			end
		endcase
	end

	//bucle dinamico estandar (NO TOCAR!)
	always @(posedge clock) begin
		busy_backend[1] <= busy_backend[0];  
		case(state)

			`IDLE: begin
				ctrl_out <= `cmd_idle_sync;
				
				contador_std <= 0;
				
				case(ctrl_in_reg)
					`cmd_reset: begin
						state <= `RST;
						busy_backend[0] <= 0;
					end
					`cmd_calc: begin
						state <= `CALC;
						busy_backend[0] <= 0;
					end
					`cmd_scan: begin
						state <= `SCAN;
						busy_backend[0] <= 0;
					end
					`cmd_print: begin
						state <= `PRINT;
						busy_backend[0] <= 0;
					end
					default: begin
						state <= `IDLE;
						busy_backend[0] <= 1;
					end
				endcase
			end
		
			`RST: begin
				contador_std <= 0;
				if(busy_frontend==`HIGH && busy_backend==`HIGH) begin
					state <= `RST_SYNC;
					busy_backend[0] <= 0;
				end
				else begin
					state <= `RST;
					busy_backend[0] <= 1;
				end
			end
			
			`RST_SYNC: begin
				ctrl_out <= `cmd_reset_sync;
				
				if(ctrl_in_reg==`cmd_idle) begin
					state <= `IDLE;
					busy_backend[0] <= 0;
				end
				else begin
					state <= `RST_SYNC;
					busy_backend[0] <= 1;
				end
			end
			
			`CALC: begin
				if(ctrl_in_reg==`cmd_reset) begin
					state <= `RST;
					busy_backend[0] <= 0;
				end
				else if(busy_frontend==`HIGH && busy_backend==`HIGH) begin
					state <= `CALC_SYNC;
					busy_backend[0] <= 0;
				end
				else begin
					state <= `CALC;
					busy_backend[0] <= 1;
				end				
			end
			
			`CALC_SYNC: begin
				ctrl_out <= `cmd_calc_sync;
				
				if(ctrl_in_reg==`cmd_idle) begin
					state <= `IDLE;
					busy_backend[0] <= 0;
				end
				else begin
					state <= `CALC_SYNC;
					busy_backend[0] <= 1;
				end
			end
			
			`SCAN: begin
				ctrl_out <= `cmd_scan;
				
				if(ctrl_in_reg==`cmd_reset) begin
					state <= `RST;
					busy_backend[0] <= 0;
				end
				else if(ctrl_in_reg==`cmd_scan_sync) begin
				
					if(BUFFER_IN_WIDTH >= DATA_WIDTH*(contador_std+1))
						buffer_in[DATA_WIDTH*(contador_std+1)-1 -: DATA_WIDTH] <= data_in_reg[DATA_WIDTH-1:0]; 
					else begin
						if(BUFFER_IN_WIDTH<=DATA_WIDTH)
							buffer_in[BUFFER_IN_WIDTH-1 -: BUFFER_IN_WIDTH] <= data_in_reg[BUFFER_IN_WIDTH-1 -: BUFFER_IN_WIDTH];
						else
							buffer_in[BUFFER_IN_WIDTH-1 -: BUFFER_IN_WIDTH%DATA_WIDTH] <= data_in_reg[BUFFER_IN_WIDTH%DATA_WIDTH-1 -: BUFFER_IN_WIDTH%DATA_WIDTH];	
					end
					
					state <= `SCAN_SYNC;
					busy_backend[0] <= 0;
				end
				else begin
					state <= `SCAN;
					busy_backend[0] <= 1;
				end
			end
			
			`SCAN_SYNC: begin
				ctrl_out <= `cmd_scan_sync;
				
				if(ctrl_in_reg==`cmd_idle) begin
					state <= `IDLE;
					busy_backend[0] <= 0;
				end
				else if(ctrl_in_reg==`cmd_scan) begin
					state <= `SCAN;
					contador_std <= contador_std + 1;
					busy_backend[0] <= 0;
				end
				else begin
					state <= `SCAN_SYNC;
					busy_backend[0] <= 1;
				end
			end
			
			`PRINT: begin
				ctrl_out <= `cmd_print;
								
				if(BUFFER_OUT_WIDTH >= DATA_WIDTH*(contador_std+1))
					data_out <= buffer_out[DATA_WIDTH*(contador_std+1)-1 -: DATA_WIDTH];
				else begin
					data_out <= 0;
					if(BUFFER_OUT_WIDTH<=DATA_WIDTH)
						data_out <= buffer_out[BUFFER_OUT_WIDTH-1 -: BUFFER_OUT_WIDTH];
					else
						data_out <= buffer_out[BUFFER_OUT_WIDTH-1 -: BUFFER_OUT_WIDTH%DATA_WIDTH];
				end
									  
				if(ctrl_in_reg==`cmd_reset) begin
					state <= `RST;
					busy_backend[0] <= 0;
				end
				else if(ctrl_in_reg==`cmd_print_sync) begin
					state <= `PRINT_SYNC;
					busy_backend[0] <= 0;
				end
				else begin
					state <= `PRINT;
					busy_backend[0] <= 1;
				end
			end
			
			`PRINT_SYNC: begin
				ctrl_out <= `cmd_print_sync;
								
				if(ctrl_in_reg==`cmd_idle) begin
					state <= `IDLE;
					busy_backend[0] <= 0;
				end
				else if(ctrl_in_reg==`cmd_print) begin
					state <= `PRINT;
					contador_std <= contador_std + 1;
					busy_backend[0] <= 0;
				end
				else begin
					state <= `PRINT_SYNC;
					busy_backend[0] <= 1;
				end
			end
		endcase
	end


endmodule
