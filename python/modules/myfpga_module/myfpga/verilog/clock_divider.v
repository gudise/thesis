module CLOCK_DIVIDER (
	input clock_in,
	input[4:0] fdiv, // Factor de division: frec_out = frec_in/(2^(1+fdiv))
	output reg clock_out=0
);
	reg[31:0] contador=1;
	
	always @(posedge clock_in) begin
		if(contador<(32'b1<<fdiv))
			contador <= contador+1;
		else begin
			clock_out <= ~clock_out;
			contador <= 1;
		end
	end

endmodule