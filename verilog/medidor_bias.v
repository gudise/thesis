/*
 Este modulo implementa un medidor de sesgo de un oscilador GARO usando 'RESOL' ciclos del reloj de muestreo 'clock_s'. 
*/

module MEDIDOR_BIAS #(
	parameter OUT_WIDTH=32,
	parameter RESOL=1000 // ResoluciÃ³n de la medida. En el futuro podrÃ­a ser interesante hacer de este parÃ¡metro una variable de entrada.
) (
	input						clock,
	input						enable,
	input						muestra,
	output reg					lock=0,
	output reg[OUT_WIDTH-1:0]	out
);
	reg[31:0] contador=0;
	reg[31:0] contador_b=0;
	
	always @(posedge clock) begin
		if(!enable) begin
			contador <= 0;
			contador_b <= 0;
			lock <= 0;
		end
		else begin
			if(contador<=RESOL) begin
				contador <= contador+1;
				if(muestra)
				    contador_b <= contador_b+1;
			end
			else begin
				out <= contador_b;
				lock <= 1;
			end
		end
	end
	
endmodule