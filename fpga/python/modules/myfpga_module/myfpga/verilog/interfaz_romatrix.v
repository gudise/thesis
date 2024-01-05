module INTERFAZ_ROMATRIX #(
    parameter N_OSC = 10
    ) (
    input                            clock,
    input                            enable,
    input[$clog2(N_OSC)-(N_OSC>1):0] sel_ro,
    input[N_OSC-1:0]                 out_romatrix,
    output reg[N_OSC-1:0]            enable_romatrix,
    output                           out
    );
    
    always @(posedge clock) begin
        enable_romatrix <= {N_OSC{1'b0}}; // 'Replication operator' o 'multiple concatenation'.
        if(enable) enable_romatrix[sel_ro] <= 1'b1;
    end
    
    assign out = enable? out_romatrix[sel_ro]:clock;

endmodule