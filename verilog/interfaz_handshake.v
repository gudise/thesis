/*
 Estas interfaces 'handshake' están pensadas para integrarse en otros diseños, no para ser utilizadas como
 módulos 'per se'.
*/


module HANDSHAKE_MASTER #(
	parameter WIDTH=32
) (
	input clock,
	input ack,
	output reg sync
	
	input[WIDTH-1:0] data_in,
	output reg[WIDTH-1:0] data_out
);

	always @(posedge clock) begin
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

endmodule



module HANDSHAKE_SLAVE #(
	parameter WIDTH=32
) (
	input clock,
	input sync,
	output reg ack
	
	input[WIDTH-1:0] data_in,
	output reg[WIDTH-1:0] data_out
);

	always @(posedge clock) begin
		case({ack,sync})
			{1'b0,1'b0}: begin
				// Esperamos a que MASTER inicie la comunicación subiendo 'sync'.
			end
			
			{1'b0,1'b1}: begin
				// Comunicación iniciada.
				data_out <= f(data_in); // Leemos 'data_in', la procesamos para generar 'data_out'.
				
				if(data_out == f(data_in)) ack <= 1'b1; // Cuando el cálculo esté terminado subimos 'ack'.
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
