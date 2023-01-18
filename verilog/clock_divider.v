module CLOCK_DIVIDER #(
	parameter FDIV=0 // Factor de division: frec_out = frec_in/(2*(FDIV+1))
) (
	input clock_in,
	output reg clock_out=0
);
	reg[31:0] contador=0;
	
	always @(posedge clock_in) begin
		if(contador<FDIV)
			contador <= contador+1;
		else begin
			clock_out <= !clock_out;
			contador <= 0;
		end
	end

endmodule