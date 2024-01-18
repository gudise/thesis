module multiplex_4_1 (
	input[1:0] selector,
	input entrada0,
	input entrada1,
	input entrada2,
	input entrada3,
	
	output reg salida
);

    always @(*) begin
        case(selector)
            2'b00: salida = entrada0;
            2'b01: salida = entrada1;
            2'b10: salida = entrada2;
            2'b11: salida = entrada3;
        endcase
    end

endmodule
