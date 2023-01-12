/*
 Este módulo implementa un contador: incrementa en '1' la cantidad 'datad_in' con cada ciclo de reloj, y 
 guarda el resultado en 'data_out'.
*/

module CONTADOR #(
	parameter IN_WIDTH=32,
	parameter OUT_WIDTH=32
) (
	input						clock,
	input						sync,
	output reg					ack=1'b0,
	input[IN_WIDTH-1:0]			data_in,
	output reg[OUT_WIDTH-1:0]	data_out
);
	always @(posedge clock) begin
		case({ack,sync})
			{1'b0,1'b0}: begin
				// Esperamos a que MASTER inicie la comunicación subiendo 'sync'.
			end
			
			{1'b0,1'b1}: begin
				// Comunicación iniciada.
				data_out <= data_in+1;
				ack <= 1'b1;
			end
			
			{1'b1,1'b1}: begin
				// Esperamos a que MASTER termine la comunicación bajando 'sync'.
			end
			
			{1'b1,1'b0}: begin
				ack <= 1'b0; // Comunicación terminada.
			end
		endcase
	end

endmodule