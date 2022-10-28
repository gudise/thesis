#ifndef XUARTPS_H
	#ifndef XUARTLITE_H
		#include "xuartps.h"
	#endif
#endif
#include "xparameters.h"
#include "xil_printf.h"
#include "xgpio.h"

#include "fsm.cp.h"
#include "fsm_parameters.h"


#define UART_DEVICE_ID	XPAR_XUARTPS_0_DEVICE_ID
#define GPIO_CTRL_ID	XPAR_AXI_GPIO_CTRL_DEVICE_ID
#define GPIO_DATA_ID	XPAR_AXI_GPIO_DATA_DEVICE_ID

//descomentar para utilizar la interfaz UART de PuTTY
//#define PUTTY


XUart Uart;
XGpio gpio_ctrl;
XGpio gpio_data;
int main(void)
{
    int state=IDLE, i;
    char program_end=0;
    char busy;
    u8 ctrl_in=0;
    u8 octeto_in[BUFFER_IN_WIDTH/8+(BUFFER_IN_WIDTH%8>0)];
    u8 octeto_out[BUFFER_OUT_WIDTH/8+(BUFFER_OUT_WIDTH%8>0)];

    config_UART(&Uart, UART_DEVICE_ID);
    
    config_GPIO(&gpio_ctrl, &gpio_data, GPIO_CTRL_ID, GPIO_DATA_ID);
    
    while(1) {
        
        switch(state) {
            case IDLE:

                XUart_Recv(&Uart, &ctrl_in, 1);
                
                switch(ctrl_in) {
                    
                    case cmd_reset:
                        state = RST;
                        break;
                        
                    case cmd_calc:
                        state = CALC;
                        break;
                        
                    case cmd_scan:
                    	state = SCAN;
                    	break;

                    case cmd_end:
                        program_end = 1;
                        break;

                    default:
                        state = IDLE;
                        break;
                }
                break;
                
                    case RST:
                        busy=1;
                        while(busy) {
                            
                            fsm_reset_cycle(&gpio_ctrl);
                            
                            busy = 0;
                        }
                        #ifdef PUTTY
                        xil_printf("\n");
                        #endif
                        
                        #ifndef PUTTY
                        send_u8(&Uart, cmd_reset_sync);
                        #endif
                        
                        state = IDLE;
                        
                        break;
                        
                    case CALC:
                        busy=1;
                        while(busy) {

                        	fsm_scan_cycle(&gpio_ctrl, &gpio_data, octeto_in, BUFFER_IN_WIDTH, DATA_WIDTH);

                        	fsm_calc_cycle(&gpio_ctrl);

                            fsm_print_cycle(&gpio_ctrl, &gpio_data, octeto_out, BUFFER_OUT_WIDTH, DATA_WIDTH);

                            busy = 0;
                        }

                        state = PRINT;

                        break;

                    case SCAN:

                    	recv_octeto(&Uart, octeto_in, BUFFER_IN_WIDTH);

                    	state = IDLE;

                    	break;

                    case PRINT:

                        #ifdef PUTTY
                        for(i=0; i<BUFFER_OUT_WIDTH/8+(BUFFER_OUT_WIDTH%8>0); i++)
                        	xil_printf("%d\t", octeto_out[i]);
                        #endif
                        
                        #ifndef PUTTY
                        send_octeto(&Uart, octeto_out, BUFFER_OUT_WIDTH/8+(BUFFER_OUT_WIDTH%8>0));
                        #endif
                        
                        state = IDLE;
                        
                    	break;
        }
        ctrl_in=0;
        
        if(program_end)
            break;
    }
    
    return 0;
}
