`timescale 1ns / 1ps

`define IDLE		1
`define RST		 2
`define RST_SYNC	3
`define CALC		4
`define CALC_SYNC   5
`define SCAN		6
`define SCAN_SYNC   7
`define PRINT	   8
`define PRINT_SYNC  9

`define cmd_idle		0
`define cmd_reset	   1
`define cmd_calc		2
`define cmd_scan		3
`define cmd_print	   4
`define cmd_end			5

`define cmd_idle_sync	 6
`define cmd_reset_sync	 7
`define cmd_calc_sync	 8
`define cmd_scan_sync	9
`define cmd_print_sync   10


module top_tb;
    reg        clock=0;
	reg[7:0]   ctrl_in;
	wire[7:0]  ctrl_out;
	reg[31:0]  data_in;
	wire[31:0] data_out;
	wire  out_ro;
	wire[3:0] interfaz_state;
	
    always #1clock = ~clock;
    
    task comm_cycle (
        input[3:0] data
    );
       begin
            ctrl_in <= `cmd_scan;
            #10 
            while(ctrl_out != `cmd_scan) #1 ;
            #10
            data_in <= data;
            #10
            ctrl_in <= `cmd_scan_sync;
            #10
            while(ctrl_out != `cmd_scan_sync) #1 ;
            #10
            ctrl_in <= `cmd_idle;
            #10
            while(ctrl_out != `cmd_idle_sync) #1 ;
            #10
            ctrl_in <= `cmd_calc;
            #10
            while(ctrl_out != `cmd_calc_sync)    #1 ;
            #10
            ctrl_in <= `cmd_idle;
            #10
            while(ctrl_out != `cmd_idle_sync) #1 ;
            #10
            ctrl_in <= `cmd_print;
            #10
            while(ctrl_out != `cmd_print) #1 ;
            #10
            ctrl_in <= `cmd_print_sync;
            #10
            while(ctrl_out != `cmd_print_sync) #1 ;
            #10
            ctrl_in <= `cmd_idle;
            #10
            while(ctrl_out != `cmd_idle_sync) #1 ;
        end
    endtask
    
    initial begin
    #10
    comm_cycle(0);
    $display("%d\n", data_out);
    
     comm_cycle(1);
    $display("%d\n", data_out);
    
     comm_cycle(2);
    $display("%d\n", data_out);
    
     comm_cycle(3);
    $display("%d\n", data_out);
    
     comm_cycle(4);
    $display("%d\n", data_out);
    
     comm_cycle(5);
    $display("%d\n", data_out);
    
     comm_cycle(6);
    $display("%d\n", data_out);
    
     comm_cycle(7);
    $display("%d\n", data_out);
    
     comm_cycle(8);
    $display("%d\n", data_out);
    
     comm_cycle(9);
    $display("%d\n\n", data_out);
    
    $display("Sim. ejecutada correctamente");
    $finish;
    
        
    end

    TOP top (
	   .clock(clock),
	   .ctrl_in(ctrl_in),
	   .ctrl_out(ctrl_out),
	   .data_in(data_in),
	   .data_out(data_out)
    );

endmodule
