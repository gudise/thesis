
#ifndef XUARTPS_H
	#ifndef XUARTLITE_H
		#include "xuartps.h"
	#endif
#endif
#include "xparameters.h"
#include "xil_printf.h"
#include "xgpio.h"

#ifdef XUARTPS_H
	#define XUart_Send(x, y, z)	XUartPs_Send(x, y, z)
	#define XUart_Recv(x, y, z)	XUartPs_Recv(x, y, z)
	#define XUart   XUartPs

#elif defined XUARTLITE_H
	#define XUart_Send(x, y, z) XUartLite_Send(x, y, z)
	#define XUart_Recv(x, y, z)	XUartLite_Recv(x, y, z)
	#define XUart   XUartLite

#endif

#define OCT0	1		   // 256^0
#define OCT1	256		 // 256^1
#define OCT2	65536	   // 256^2
#define OCT3	16777216	// 256^3
#define OCT(x)	OCT0*(x==0)+OCT1*(x==1)+OCT2*(x==2)+OCT3*(x==3)

#define IDLE 		1
#define RST 		2
#define RST_SYNC 	3
#define CALC 		4
#define CALC_SYNC	5
#define SCAN 		6
#define SCAN_SYNC 	7
#define PRINT		8
#define PRINT_SYNC	9

#define cmd_idle 		105  //'i'
#define cmd_reset 		114  //'r'
#define cmd_calc 		99   //'c'
#define cmd_end			101  //'e'
#define cmd_scan		115	 //'s'
#define cmd_print		112  //'p'

#define cmd_idle_sync	22	 //SYN
#define cmd_reset_sync   13	 //'\n'
#define cmd_calc_sync	 2
#define cmd_scan_sync	  3
#define cmd_print_sync	  4


#include "interfaz_ps_define.h"


#define UART_DEVICE_ID	XPAR_XUARTPS_0_DEVICE_ID
#define GPIO_CTRL_ID	XPAR_AXI_GPIO_CTRL_DEVICE_ID
#define GPIO_DATA_ID	XPAR_AXI_GPIO_DATA_DEVICE_ID

//descomentar para utilizar la interfaz UART de PuTTY
//#define PUTTY


void	u32_to_4u8(u32 in, u8 out[4]);
u32		u8_to_u32(u8 in[4]);
void	send_u8(XUart* Uart, u8 data);
void	send_octeto(XUart* Uart, u8 octeto[], int octeto_size);
u8		recv_u8(XUart* Uart);
void	recv_octeto(XUart* Uart, u8 octeto_in[], int buffer_in_width);

void	config_UART(XUart* Uart, int uart_id);
void	config_GPIO(XGpio* gpio_ctrl, XGpio* gpio_data, int ctrl_id, int data_id);

void	calc_cycle();
void	reset_cycle();
void	scan_cycle();
void	print_cycle();


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
							
							reset_cycle(&gpio_ctrl);
							
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

							scan_cycle(&gpio_ctrl, &gpio_data, octeto_in, BUFFER_IN_WIDTH, DATA_WIDTH);

							calc_cycle(&gpio_ctrl);

							print_cycle(&gpio_ctrl, &gpio_data, octeto_out, BUFFER_OUT_WIDTH, DATA_WIDTH);

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



void u32_to_4u8(u32 in, u8 out[4])
{
	u32 aux;

	aux=in;

	out[3] = aux/OCT3;
	aux = aux%OCT3;
	
	out[2] = aux/OCT2;
	aux = aux%OCT2;
	
	out[1] = aux/OCT1;
	aux = aux%OCT1;
	
	out[0] = aux;
}


u32 u8_to_u32(u8 in[4])
{
	u32 out=0;

	out=OCT3*in[3]+OCT2*in[2]+OCT1*in[1]+in[0];

	return out;
}


void send_u8(XUart* Uart, u8 data)
{
	int j=0;

	while(j<1)
		j+=XUart_Send(Uart, &data, 1); //envia 1 byte (i.e. u8) al pc
}


void send_octeto(XUart* Uart, u8 octeto[], int octeto_size)
{
	int i;

	for(i=0; i<octeto_size; i++)
		send_u8(Uart, octeto[i]);
}


u8 recv_u8(XUart* Uart)
{
	u32 bytes_read=0;
	u8 result;

	while(1) {
		bytes_read = XUart_Recv(Uart, &result, 1);

		if(bytes_read>0)
			break;
	}

	return result;
}


void recv_octeto(XUart* Uart, u8 octeto_in[], int buffer_in_width)
{
	int i;

	for(i=0; i<buffer_in_width/8+(buffer_in_width%8>0); i++)
		octeto_in[i] = recv_u8(Uart);
}


void config_UART(XUart* Uart, int uart_id) {

#ifdef XUARTPS_H
	XUartPs_Config *Config;
	
	Config = XUartPs_LookupConfig(uart_id);
	XUartPs_CfgInitialize(Uart, Config, Config->BaseAddress);
	XUartPs_SetBaudRate(Uart, 9600);
#endif

#ifdef XUARTLITE_H
	XUartLite_Config *Config;
	
	Config = XUartLite_LookupConfig(uart_id);
	XUartLite_CfgInitialize(Uart, Config, Config->RegBaseAddr);
#endif
}


void config_GPIO(XGpio* gpio_ctrl, XGpio* gpio_data, int ctrl_id, int data_id) {

	XGpio_Initialize(gpio_ctrl, ctrl_id);
	XGpio_SetDataDirection(gpio_ctrl, 1, 0xF); //cmd: fabric->ps
	XGpio_SetDataDirection(gpio_ctrl, 2, 0x0); //cmd: ps->fabric

	XGpio_Initialize(gpio_data, data_id);
	XGpio_SetDataDirection(gpio_data, 1, 0xF); //data_out: fabric->ps
	XGpio_SetDataDirection(gpio_data, 2, 0x0); //data_out: ps->fabric
}


void calc_cycle(XGpio *gpio_ctrl) {

	u32 ctrl_out;

 	XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_calc);
	 while(1) {

		 ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
		 if(ctrl_out==cmd_calc_sync)
			 break;
	 }
	 
	 XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_idle);
	 while(1) {

		 ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
		 if(ctrl_out==cmd_idle_sync)
			 break;
	 }
}


void scan_cycle(XGpio *gpio_ctrl, XGpio *gpio_data, u8 octeto_in[], int buffer_in_width, int data_width) {

	u32 ctrl_out, data_in;
	int i, N=buffer_in_width/data_width+(buffer_in_width%data_width>0);

	for(i=0; i<N; i++) {
 		XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_scan);
 		while(1) {

 			ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1); //xil_printf("%d\n", ctrl_out);
 			if(ctrl_out==cmd_scan)
 				break;
 		}

 		data_in = octeto_in[i*4]+OCT1*octeto_in[i*4+1]+OCT2*octeto_in[i*4+2]+OCT3*octeto_in[i*4+3];
 		XGpio_DiscreteWrite(gpio_data, 2, data_in);

 		XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_scan_sync);
 		while(1) {

 			ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
 			if(ctrl_out==cmd_scan_sync)
 				break;
 		}
	}

	 XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_idle);
	 while(1) {

		 ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
		 if(ctrl_out==cmd_idle_sync)
			 break;
	 }
}


void print_cycle(XGpio *gpio_ctrl, XGpio *gpio_data, u8 octeto_out[], int buffer_out_width, int data_width) {

	u32 ctrl_out, data_out;
	u8 aux[4];
	int i, contador;
	int data_out_max=buffer_out_width/data_width+(buffer_out_width%data_width>0);
	int octeto_out_max=buffer_out_width/8+(buffer_out_width%8>0);

	for(i=0; i<octeto_out_max; i++)
		octeto_out[i] = 0;

	contador=0;
 	for(i=0; i<data_out_max; i++) {

 		XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_print);
 		while(1) {

 			ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
 			if(ctrl_out==cmd_print)
 				break;
 		}

 		data_out = XGpio_DiscreteRead(gpio_data, 1);

 		u32_to_4u8(data_out, aux);

 		if(contador<octeto_out_max)
 			octeto_out[contador] = aux[0];

 		if(contador+1<octeto_out_max)
 			octeto_out[contador+1] = aux[1];

 		if(contador+2<octeto_out_max)
 			octeto_out[contador+2] = aux[2];

 		if(contador+3<octeto_out_max)
 			octeto_out[contador+3] = aux[3];

 		contador+=4;

 		XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_print_sync);
 		while(1) {

 			ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
 			if(ctrl_out==cmd_print_sync)
 				break;
 		}
 	}

	 XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_idle);
	 while(1) {

		 ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
		 if(ctrl_out==cmd_idle_sync)
			 break;
	 }
}


void reset_cycle(XGpio *gpio_ctrl) {

	u32 ctrl_out;

 	XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_reset);
	 while(1) {

		 ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
		 if(ctrl_out==cmd_reset_sync)
			 break;
	 }

	 XGpio_DiscreteWrite(gpio_ctrl, 2, cmd_idle);
	 while(1) {

		 ctrl_out = XGpio_DiscreteRead(gpio_ctrl, 1);
		 if(ctrl_out==cmd_idle_sync)
			 break;
	 }
}
