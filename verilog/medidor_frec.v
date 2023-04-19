/*
 Este modulo implementa un medidor de frecuencia para la senal 'clock_u' usando 'RESOL' ciclos de 'clock'.
 El tiempo de medida utilizado para resolver la frecuencia de la señal de llegada se puede modificar mediante
 la variable de entrada 'resol'. Este es un número de 5 bits que contiene el exponente del valor máximo
 del contador, i.e. el reloj de referencia girará 2^resol vueltas antes de terminar la medida.  
*/

module MEDIDOR_FREC #(
    parameter OUT_WIDTH=32
) (
    input                       clock,
    input                       enable,
    input                       clock_u,
    input[4:0]                  resol, // Exponente base 2 de la variable de resolucion.
    output reg                  lock=0,
    output reg[OUT_WIDTH-1:0]   out
);
    reg[31:0] contador=0;
    reg[31:0] contador_u=0;
    
    always @(posedge clock) begin
        if(enable) begin
            if(contador[resol]==1'b0) begin
                contador <= contador+32'b1;
            end
            else begin
                out <= contador_u;
                lock <= 1;
            end
        end
        else begin
            contador <= 0;
            if(contador_u==0)
                lock <= 0;
        end
    end
    
    always @(posedge clock_u) begin
        if(enable && contador>0) begin // merece la pena estudiar la diferencia entre poner o no la condición 'contador>0'
            contador_u <= contador_u+32'b1;
        end
        else begin
            contador_u <= 0;
        end
    end

endmodule
