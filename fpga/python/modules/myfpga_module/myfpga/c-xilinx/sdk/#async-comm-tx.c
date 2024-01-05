#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"


XGpio data, control;
int main()
{
	u32 numero=0;

	XGpio_Initialize(&data, XPAR_AXI_GPIO_DATA_DEVICE_ID);
	XGpio_SetDataDirection(&data, 1, 0x00000000);

	XGpio_Initialize(&control, XPAR_AXI_GPIO_CTRL_DEVICE_ID);
	XGpio_SetDataDirection(&control, 1, 1); //done
	XGpio_SetDataDirection(&control, 2, 0); //reset

	while(1)
	{
        XGpio_DiscreteWrite(&data, 1, numero); //colocamos un numero en el buffer de escritura
        
        XGpio_DiscreteWrite(&control, 2, 0); //bajamos el 'reset'->0
        
		while(XGpio_DiscreteRead(&control, 1)==0); //esperamos a que 'done'=1
        
		XGpio_DiscreteWrite(&control, 2, 1); //subimos el 'reset'->1
        
		while(XGpio_DiscreteRead(&control, 1)==1); //esperamos a que 'done'=0

		numero++;
	}

}
