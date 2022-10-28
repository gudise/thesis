
MICROB=0
OPCIONB=""
OPCIONC=""

i=2
for opcion
do
	if test $opcion = "-help"
	then
		echo '
Este script escribe un ejemplo de configuración y uso de varios módulos típicos de SDK (GPIO, UART) así como un módulo genérico HLS. 
--------------
	
	Opciones:
	
		-help, pinta esta ayuda.
		-mb, si esta opción está presente las funciones se escriben para un diseño con MicroBlaze (en lugar de ZynqPS, que es el estándar)
		-hls, si esta opción está presente el script escribe el código necsario para hacer funcionar un módulo HLS con interfaz AXI (nombrado de forma genérica "Hlsmodule". Para abarcar todas las posibles formas de entrada/salida, se supone que este módulo tiene 4 puertos: una entrada vectorial (que VivadoHLS implementa como una memoria), una entrada escalar, una salida vectorial (de nuevo una memoria) y una salida escalar. El código escrito por este script simplemente escribe unos valores en la entrada, y lee las salidas. Notar que si se usa VivadoHLS para sintetizar un diseño sin interfaz AXI, este deberá interactuar con el sistema de procesamiento mediante un módulo GPIO.
		-c [foo]
		
		'
			exit
	
	elif test $opcion = "-mb"
	then
		MICROB=1
	
	elif test $opcion = "-hls"
	then
		OPCIONB=${!i}
	
	elif test $opcion = "-c"
	then
		OPCIONC=${!i}
	
	fi
	
	i=$((i+1))
done

echo $OPCIONA
echo $OPCIONB
echo $OPCIONC

if test $MICROB -eq 0
then

echo '
#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xhlsmodule.h"


XGpio gpio_0;
XHlsmodule module_0;
int main()
{
	u32 number_write, number_read;

	XGpio_Initialize(&gpio_0, XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_SetDataDirection(&gpio_0, 1, 0xF); //fabric->ps
	XGpio_SetDataDirection(&gpio_0, 2, 0x0); //ps->fabric
	
	XHlsmodule_Initialize(&module_0, XPAR_XHLSMODULE_0_DEVICE_ID);

	//Enviar datos a traves de GPIO
	scanf("%ld", &number_write); //con ZynqPS es asi de facil escribir por UART
	XGpio_DiscreteWrite(&gpio_0, 2, number_write);
	number_read = XGpio_DiscreteRead(&gpio_0, 1);
	printf("number_read = %ld\n\r", number_read); //con ZynqPS es asi de facil leer por UART

	//Enviar datos a hlsmodule(entrada, salida)
	scanf("%ld", &number_write);
	XHlsmodule_Set_entrada(&module_0, number_write);
	number_read = XHlsmodule_Get_salida(&module_0);
	printf("number_read = %ld\n\r", number_read);
		
	return 0;
}
'
else

echo '
#include "xparameters.h"
#include "xgpio.h"
#include "xuartlite.h" //esto no se si esta bien
#include "xhlsmodule.h"


XGpio gpio_0;
XUartLite uart_0;
XHlsmodule module_0;
int main()
{
    u32 number_write, number_read;

	XGpio_Initialize(&gpio_0, XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_SetDataDirection(&gpio_0, 1, 0xF); //fabric->ps
	XGpio_SetDataDirection(&gpio_0, 2, 0x0); //ps->fabric
	
	XUartLite_Initialize(&uart_0, XPAR_AXI_UARTLITE_0_DEVICE_ID)

	XHlsmodule_Initialize(&module_0, XPAR_XHLSMODULE_0_DEVICE_ID);
	
	//Enviar datos a traves de GPIO
	
	//Enviar datos a hlsmodule
	
    return 0;
}
'

fi
