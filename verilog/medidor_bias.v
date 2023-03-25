/*
 Este modulo implementa un medidor de sesgo de un oscilador GARO usando 'RESOL' ciclos del reloj de muestreo 'clock_s'. 
*/

module MEDIDOR_BIAS #(
    parameter OUT_WIDTH=32
) (
    input                       clock,
    input                       enable,
    input                       muestra,
    input[4:0]                  resol,
    output reg                  lock=0,
    output reg[OUT_WIDTH-1:0]   out
);
    reg[31:0] contador=0;
    reg[31:0] contador_b=0;
    
    always @(posedge clock) begin
        if(enable) begin
            if(contador[resol]==1'b0) begin
                contador <= contador+32'b1;
                if(muestra)
                    contador_b <= contador_b+32'b1;
            end
            else begin
                out <= contador_b;
                lock <= 1;
            end
        end
        else begin
            contador <= 0;
            contador_b <= 0;
            lock <= 0;      
        end
    end
    
endmodule
