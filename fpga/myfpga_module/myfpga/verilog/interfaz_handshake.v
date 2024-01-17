/*
 Estas interfaces 'handshake' están pensadas para integrarse en un módulo, no para ser utilizadas como
 módulos 'per se'. Notar que un mismo módulo también puede implementar ambas interfaces, actuando como slave y master
 de un segundo y tercer módulo.
*/


module HANDSHAKE_MASTER #(
	parameter WIDTH=32
) (
	input					clock,
	output reg				sync=1'b0,
	input					ack,
	input[WIDTH-1:0]		data_in,
	output reg[WIDTH-1:0]	data_out
);

	always @(posedge clock) begin
		case({ack,sync})
			2'b00: begin
				sync <= 1'b1; // MASTER inicia la comunicación.
			end
			
			2'b01: begin
				// Esperamos a que SLAVE conteste subiendo 'ack'.
			end
			
			2'b11: begin
				sync <= 1'b0; // MASTER termina el ciclo de comunicación.
			end
			
			2'b10: begin
				// SLAVE bajará 'ack'.
			end
		endcase
	end

endmodule



module HANDSHAKE_SLAVE #(
	parameter WIDTH=32
) (
	input					clock,
	input					sync,
	output reg				ack=1'b0,
	input[WIDTH-1:0]		data_in,
	output reg[WIDTH-1:0]	data_out
);

	always @(posedge clock) begin
		case({ack,sync})
			2'b00: begin
				// Esperamos a que MASTER inicie la comunicación subiendo 'sync'.
			end
			
			2'b01: begin
				// Comunicación iniciada.
				data_out <= f(data_in); // Leemos 'data_in', la procesamos para generar 'data_out'.
				
				if(data_out == f(data_in)) ack <= 1'b1; // Cuando el cálculo esté terminado subimos 'ack'.
			end
			
			2'b11: begin
				// Esperamos a que MASTER termine la comunicación bajando 'sync'.
			end
			
			2'b10: begin
				ack <= 1'b0; // Comunicación terminada.
			end
		endcase
	end

endmodule
