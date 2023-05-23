/*
 Este diseño implementa una pequeña memoria para guardar los bit
 procedentes de un TRNG formado por una matriz de osciladores de Galois.
*/

module BIT_POOL #(
    parameter POOL_WIDTH=100
    ) (
    input                       clock, //Reloj de muestreo del anillo.
    input                       enable,
    input                       muestra, //Señal muestreada.
    output reg                  full=0,
    output reg[POOL_WIDTH-1:0]  out
);
    reg[31:0] contador_pool=0;
    
    always @(negedge clock) begin
        if(enable) begin
            if(contador_pool<POOL_WIDTH) begin
                out[contador_pool] <= muestra;
                contador_pool <= contador_pool+32'b1;
            end
            else full <= 1'b1;
        end
        else begin
            full <= 1'b0;
            contador_pool <= 0;
        end
    end
    
endmodule