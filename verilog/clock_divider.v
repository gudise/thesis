module CLOCK_DIVIDER #(
	parameter val_ini=1'b0,		// valor inicial del reloj de salida
	parameter factor=5	// frec. salida = entrada/(2*factor)
) (
	input clock,
	output reg out=val_ini
);
	integer contador=0;
	
	always @(posedge clock) begin
		contador <= contador+1;
		if(contador == factor) begin
			out <= ~out;
			contador=0;
		end
	end

endmodule