    //bucle dinamico estandar (NO TOCAR!)
    always @(posedge clock) begin
        estacionario[1] <= estacionario[0];  
        case(state)

            `IDLE: begin
                ctrl_out <= `cmd_idle_sync;
                
                contador_std <= 0;
                
                case(ctrl_in_reg)
                    `cmd_reset: begin
                        state <= `RST;
                        estacionario[0] <= 0;
                    end
                    `cmd_calc: begin
                        state <= `CALC;
                        estacionario[0] <= 0;
                    end
                    `cmd_scan: begin
                        state <= `SCAN;
                        estacionario[0] <= 0;
                    end
                    `cmd_print: begin
                        state <= `PRINT;
                        estacionario[0] <= 0;
                    end
                    default: begin
                        state <= `IDLE;
                        estacionario[0] <= 1;
                    end
                endcase
            end
        
            `RST: begin
                contador_std <= 0;
                if(estable==`HIGH && estacionario==`HIGH) begin
                    state <= `RST_SYNC;
                    estacionario[0] <= 0;
                end
                else begin
                    state <= `RST;
                    estacionario[0] <= 1;
                end
            end
            
            `RST_SYNC: begin
                ctrl_out <= `cmd_reset_sync;
                
                if(ctrl_in_reg==`cmd_idle) begin
                    state <= `IDLE;
                    estacionario[0] <= 0;
                end
                else begin
                    state <= `RST_SYNC;
                    estacionario[0] <= 1;
                end
            end
            
            `CALC: begin
                if(ctrl_in_reg==`cmd_reset) begin
                    state <= `RST;
                    estacionario[0] <= 0;
                end
                else if(estable==`HIGH && estacionario==`HIGH) begin
                    state <= `CALC_SYNC;
                    estacionario[0] <= 0;
                end
                else begin
                    state <= `CALC;
                    estacionario[0] <= 1;
                end                
            end
            
            `CALC_SYNC: begin
                ctrl_out <= `cmd_calc_sync;
                
                if(ctrl_in_reg==`cmd_idle) begin
                    state <= `IDLE;
                    estacionario[0] <= 0;
                end
                else begin
                    state <= `CALC_SYNC;
                    estacionario[0] <= 1;
                end
            end
            
            `SCAN: begin
                ctrl_out <= `cmd_scan;
                
                if(ctrl_in_reg==`cmd_reset) begin
                    state <= `RST;
                    estacionario[0] <= 0;
                end
                else if(ctrl_in_reg==`cmd_scan_sync) begin
                
                    if(BUFFER_IN_WIDTH >= DATA_WIDTH*(contador_std+1))
                        buffer_in[DATA_WIDTH*(contador_std+1)-1 -: DATA_WIDTH] <= data_in_reg[DATA_WIDTH-1:0]; 
                    else begin
                        if(BUFFER_IN_WIDTH<=DATA_WIDTH)
                            buffer_in[BUFFER_IN_WIDTH-1 -: BUFFER_IN_WIDTH] <= data_in_reg[BUFFER_IN_WIDTH-1 -: BUFFER_IN_WIDTH];
                        else
                            buffer_in[BUFFER_IN_WIDTH-1 -: BUFFER_IN_WIDTH%DATA_WIDTH] <= data_in_reg[BUFFER_IN_WIDTH%DATA_WIDTH-1 -: BUFFER_IN_WIDTH%DATA_WIDTH];    
                    end
                    
					state <= `SCAN_SYNC;
                    estacionario[0] <= 0;
                end
                else begin
                    state <= `SCAN;
                    estacionario[0] <= 1;
                end
            end
            
            `SCAN_SYNC: begin
                ctrl_out <= `cmd_scan_sync;
                
                if(ctrl_in_reg==`cmd_idle) begin
                    state <= `IDLE;
                    estacionario[0] <= 0;
                end
                else if(ctrl_in_reg==`cmd_scan) begin
                    state <= `SCAN;
                    contador_std <= contador_std + 1;
                    estacionario[0] <= 0;
                end
                else begin
                    state <= `SCAN_SYNC;
                    estacionario[0] <= 1;
                end
            end
            
            `PRINT: begin
                ctrl_out <= `cmd_print;
                                
                if(BUFFER_OUT_WIDTH >= DATA_WIDTH*(contador_std+1))
                    data_out <= buffer_out[DATA_WIDTH*(contador_std+1)-1 -: DATA_WIDTH];
                else begin
                    data_out <= 0;
                    if(BUFFER_OUT_WIDTH<=DATA_WIDTH)
                        data_out <= buffer_out[BUFFER_OUT_WIDTH-1 -: BUFFER_OUT_WIDTH];
                    else
                        data_out <= buffer_out[BUFFER_OUT_WIDTH-1 -: BUFFER_OUT_WIDTH%DATA_WIDTH];
                end
                                      
                if(ctrl_in_reg==`cmd_reset) begin
                    state <= `RST;
                    estacionario[0] <= 0;
                end
                else if(ctrl_in_reg==`cmd_print_sync) begin
                    state <= `PRINT_SYNC;
                    estacionario[0] <= 0;
                end
                else begin
                    state <= `PRINT;
                    estacionario[0] <= 1;
                end
            end
            
            `PRINT_SYNC: begin
                ctrl_out <= `cmd_print_sync;
                                
                if(ctrl_in_reg==`cmd_idle) begin
                    state <= `IDLE;
                    estacionario[0] <= 0;
                end
                else if(ctrl_in_reg==`cmd_print) begin
                    state <= `PRINT;
                    contador_std <= contador_std + 1;
                    estacionario[0] <= 0;
                end
                else begin
                    state <= `PRINT_SYNC;
                    estacionario[0] <= 1;
                end
            end
        endcase
    end
