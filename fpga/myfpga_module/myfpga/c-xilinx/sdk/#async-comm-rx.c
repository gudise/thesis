#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"


XGpio data, control;
int main()
{
	u32 numero;

	XGpio_Initialize(&data, XPAR_AXI_GPIO_DATA_DEVICE_ID);
	XGpio_SetDataDirection(&data, 1, 0xFFFFFFFF);

	XGpio_Initialize(&control, XPAR_AXI_GPIO_CTRL_DEVICE_ID);
	XGpio_SetDataDirection(&control, 1, 1);
	XGpio_SetDataDirection(&control, 2, 0);

	while(1)
	{
		while(XGpio_DiscreteRead(&control, 1)==0); //esperamos a que 'done'=1

		numero = XGpio_DiscreteRead(&data, 1); //leemos el dato
        
		XGpio_DiscreteWrite(&control, 2, 1); //subimos el 'reset'->1
        
		while(XGpio_DiscreteRead(&control, 1)==1); //esperamos a que 'done'=0
        
		XGpio_DiscreteWrite(&control, 2, 0); //bajamos el reset->0 para que la maquina pueda continuar

		printf("%lu\n\r", numero);
	}

}
