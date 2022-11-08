`define LOW 2'b00
`define RISING 2'b01
`define FALLING 2'b10
`define HIGH 2'b11

`define IDLE        1
`define RST         2
`define RST_SYNC    3
`define CALC        4
`define CALC_SYNC   5
`define SCAN        6
`define SCAN_SYNC   7
`define PRINT       8
`define PRINT_SYNC  9

`define cmd_idle        105 //'i'
`define cmd_reset       114 //'r'
`define cmd_calc        99  //'c'
`define cmd_end	        101 //'e'
`define cmd_scan        115 //'s'
`define cmd_print       112 //'p'

`define cmd_idle_sync	 22 //SYN
`define cmd_reset_sync	 13 //'\n'
`define cmd_calc_sync	  2 //STX
`define cmd_scan_sync     3
`define cmd_print_sync    4
