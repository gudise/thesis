/*
 Este módulo implementa una interfaz para conectar el sistema de procesamiento (PS/MicroBlaze) con la lógica
 programable de la FPGA (PL). La forma de comunicarse con PS es a través de una máquina de estados (FSM) que
 es controlada por PS mediante un canal 'ctrl', de acuerdo con un cierto protocolo. La comunicación con PL 
 se hace a través de un protocolo 'handshake', de modo que cuando la interfaz dispone de un dato disponible
 sube una señal 'sync'->1; una vez los cálculos en PL han terminado, la interfaz espera recibir 'ack'->1. Por
 último, para reiniciar el ciclo de lectura, la interfaz bajará 'sync'->0, y cuando reciba 'ack'->0 proseguirá
 la comunicación con PS.
 
 Hay varias alternativas de compilación en función de la relación entre DATA_WIDTH, BUFFER_IN_WIDTH, y BUFFER_OUT_WIDTH.
 Estas se implementan a través del archivo 'interfaz_pspl_define.vh' que se incluye en este módulo; en este fichero pueden
 aparecer definidos uno los sigientes tres elementos para cada estado (SCAN/PRINT), los cuales modifican el código a sintetizar:
    Estado 'SCAN' (leer datos PS->PL); se define uno o ninguno de los siguientes elementos:
         DW_GE_BIW          --> DATA_WIDTH es mayor o igual que BUFFER_IN_WIDTH 
         BIW_ALIGNED_DW     --> BUFFER_IN_WIDTH es mayor que DATA_WIDTH, pero están alineados (BUFFER_IN_WIDTH%DATA_WIDTH == 0)
         BIW_MISALIGNED_DW  --> BUFFER_IN_WIDTH es mayor que DATA_WIDTH, y no están alineados (BUFFER_IN_WIDTH%DATA_WIDTH != 0)
         
    Estado 'PRINT' (escribir datos PL->PS); se define uno o ninguno de los siguientes elementos:
         DW_GE_BOW          --> DATA_WIDTH es mayor o igual que BUFFER_OUT_WIDTH    
         BOW_ALIGNED_DW     --> BUFFER_OUT_WIDTH es mayor que DATA_WIDTH, pero están alineados (BUFFER_OUT_WIDTH%DATA_WIDTH == 0)
         BOW_MISALIGNED_DW  --> BUFFER_OUT_WIDTH es mayor que DATA_WIDTH, y no están alineados (BUFFER_OUT_WIDTH%DATA_WIDTH != 0)
         
 En caso de que no se defina ninguna de estas cantidades, el comportamiento por defecto es DW_GE_BIW y DW_GE_BOW.
*/

`include "interfaz_pspl_config.vh"


`define LOW     2'b00
`define RISING  2'b01
`define FALLING 2'b10
`define HIGH    2'b11

`define IDLE        1
`define CALC        2
`define CALC_SWAP   3
`define SCAN        4
`define SCAN_SWAP   5
`define PRINT       6
`define PRINT_SWAP  7

`define cmd_idle_sync   0
`define cmd_idle_ack    1
`define cmd_calc_sync   2
`define cmd_calc_ack    3
`define cmd_scan_sync   4
`define cmd_scan_ack    4
`define cmd_scan_wait   5
`define cmd_scan_done   5
`define cmd_print_sync  6
`define cmd_print_ack   6
`define cmd_print_wait  7
`define cmd_print_done  7

module INTERFAZ_PSPL #(
    parameter   DATA_WIDTH=32,
    parameter   BUFFER_IN_WIDTH=16,
    parameter   BUFFER_OUT_WIDTH=16
    ) (
    input                           clock,
    input[7:0]                      ctrl_in,    // instrucción de PS.
    output reg[7:0]                 ctrl_out,   // instrucción a PS.
    input[DATA_WIDTH-1:0]           data_in,    // dato de PS.
    output reg[DATA_WIDTH-1:0]      data_out,   // dato a PS.
    output reg                      sync=0,     //
    input                           ack,        // intefaz 'handshake' con PL.
    output reg[BUFFER_IN_WIDTH-1:0] buffer_in,  // 'buffer' enviado por PS.
    input[BUFFER_OUT_WIDTH-1:0]     buffer_out  // 'buffer' devuelto a PS.
    );

    reg[3:0] state=`IDLE;
    reg[1:0] busy_frontend=`LOW;
    reg[1:0] busy_backend=`LOW;
    reg[7:0] ctrl_in_reg;
    reg[DATA_WIDTH-1:0] data_in_reg;
    reg[7:0] contador_std;
    
    //registro de input
    always @(posedge clock) begin
        ctrl_in_reg <= ctrl_in;
        data_in_reg <= data_in;
    end
     
    //definicion de estados
    always @(posedge clock) begin
        busy_frontend[1] <= busy_frontend[0];
        case (state)
            `IDLE: begin
                //condicion de estabilidad de 'IDLE'
                if(!sync && !ack) busy_frontend[0] <= 1;
                else busy_frontend[0] <= 0;
                
                sync <= 0;
            end
            
            `CALC: begin
                //condicion de estabilidad de 'CALC'
                if(ack) busy_frontend[0] <= 1;
                else busy_frontend[0] <= 0;
                
                sync <= 1;
            end
        endcase
    end

    //bucle dinamico estandar
    always @(posedge clock) begin
        busy_backend[1] <= busy_backend[0];  
        case(state)

            `IDLE: begin
                ctrl_out <= `cmd_idle_ack;
                
                contador_std <= 0;
                
                case(ctrl_in_reg)
                    `cmd_calc_sync: begin
                        state <= `CALC;
                        busy_backend[0] <= 0;
                    end
                    `cmd_scan_sync: begin
                        state <= `SCAN;
                        busy_backend[0] <= 0;
                    end
                    `cmd_print_sync: begin
                        state <= `PRINT;
                        busy_backend[0] <= 0;
                    end
                    default: begin
                        state <= `IDLE;
                        busy_backend[0] <= 1;
                    end
                endcase
            end
            
            `CALC: begin
                if(busy_frontend==`HIGH && busy_backend==`HIGH) begin
                    state <= `CALC_SWAP;
                    busy_backend[0] <= 0;
                end
                else begin
                    state <= `CALC;
                    busy_backend[0] <= 1;
                end             
            end
            
            `CALC_SWAP: begin
                ctrl_out <= `cmd_calc_ack;
                
                if(ctrl_in_reg==`cmd_idle_sync) begin
                    state <= `IDLE;
                    busy_backend[0] <= 0;
                end
                else begin
                    state <= `CALC_SWAP;
                    busy_backend[0] <= 1;
                end
            end
            
            `SCAN: begin
                ctrl_out <= `cmd_scan_ack;
                
                if(ctrl_in_reg==`cmd_scan_wait) begin
                    `ifdef DW_GE_BIW
                        buffer_in <= data_in;
                    
                    `elsif BIW_ALIGNED_DW
                        buffer_in[DATA_WIDTH*(contador_std+1)-1 -: DATA_WIDTH] <= data_in;
                        
                    `elsif BIW_MISALIGNED_DW
                        if(contador_std<BUFFER_IN_WIDTH/DATA_WIDTH)
                            buffer_in[DATA_WIDTH*(contador_std+1)-1 -: DATA_WIDTH] <= data_in;
                        else if(contador_std==BUFFER_IN_WIDTH/DATA_WIDTH)
                            buffer_in[BUFFER_IN_WIDTH-1 : BUFFER_IN_WIDTH-BUFFER_IN_WIDTH%DATA_WIDTH] <= data_in;
                    `else
                        buffer_in <= data_in;
                        
                    `endif
                    
                    state <= `SCAN_SWAP;
                    busy_backend[0] <= 0;
                end
                else begin
                    state <= `SCAN;
                    busy_backend[0] <= 1;
                end
            end
            
            `SCAN_SWAP: begin
                ctrl_out <= `cmd_scan_done;
                
                if(ctrl_in_reg==`cmd_idle_sync) begin
                    state <= `IDLE;
                    busy_backend[0] <= 0;
                end
                else if(ctrl_in_reg==`cmd_scan_sync) begin
                    state <= `SCAN;
                    contador_std <= contador_std + 1;
                    busy_backend[0] <= 0;
                end
                else begin
                    state <= `SCAN_SWAP;
                    busy_backend[0] <= 1;
                end
            end
            
            `PRINT: begin
                ctrl_out <= `cmd_print_ack;
                                
                `ifdef DW_GE_BOW
                    data_out <= buffer_out;
                    
                `elsif BOW_ALIGNED_DW
                    data_out <= buffer_out[DATA_WIDTH*(contador_std+1)-1 -: DATA_WIDTH];
                
                `elsif BOW_MISALIGNED_DW
                    if(contador_std<BUFFER_OUT_WIDTH/DATA_WIDTH)
                        data_out <= buffer_out[DATA_WIDTH*(contador_std+1)-1 -: DATA_WIDTH];
                    else if(contador_std==BUFFER_OUT_WIDTH/DATA_WIDTH)
                        data_out <= buffer_out[BUFFER_OUT_WIDTH-1 : BUFFER_OUT_WIDTH-BUFFER_OUT_WIDTH%DATA_WIDTH];
                `else
                    data_out <= buffer_out;
                
                `endif
                
                if(ctrl_in_reg==`cmd_print_wait) begin
                    state <= `PRINT_SWAP;
                    busy_backend[0] <= 0;
                end
                else begin
                    state <= `PRINT;
                    busy_backend[0] <= 1;
                end
            end
            
            `PRINT_SWAP: begin
                ctrl_out <= `cmd_print_done;
                                
                if(ctrl_in_reg==`cmd_idle_sync) begin
                    state <= `IDLE;
                    busy_backend[0] <= 0;
                end
                else if(ctrl_in_reg==`cmd_print_sync) begin
                    state <= `PRINT;
                    contador_std <= contador_std + 1;
                    busy_backend[0] <= 0;
                end
                else begin
                    state <= `PRINT_SWAP;
                    busy_backend[0] <= 1;
                end
            end
        endcase
    end

endmodule
