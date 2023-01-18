/*
 Este modulo implementa un medidor de frecuencia para la senal 'clock_u' usando 'RESOL' ciclos de 'clock'. 
*/

module MEDIDOR_FREC #(
	parameter OUT_WIDTH=32,
	parameter RESOL=1000 // ResoluciÃƒÂ³n de la medida. En el futuro podrÃƒÂ­a ser interesante hacer de este parÃƒÂ¡metro una variable de entrada.
) (
	input						clock,
	input						enable,
	input						clock_u,
	output reg					lock=0,
	output reg[OUT_WIDTH-1:0]	out
);
	reg[31:0] contador=0;
	reg[31:0] contador_u=0;
	reg enable_u=0;
	
	always @(posedge clock) begin
		if(!enable) begin
			contador <= 0;
			enable_u <= 0;
			if(contador_u==0)
				lock <= 0;
		end
		else begin
			if(contador<=RESOL) begin
				enable_u <= 1;
				contador <= contador+1;
			end
			else begin
				enable_u <= 0;  
				out <= contador_u;
				lock <= 1;
			end
		end
	end
	
	always @(posedge clock_u) begin
		if(!enable)
			contador_u <= 0;
		else
			contador_u <= contador_u+1;
	end

endmodule