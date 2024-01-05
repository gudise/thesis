/*
 * Este codigo permite leer una FIFO (implementada con la IP 'FIFO generator'). La memoria FIFO debe implementarse en modo 'First Word Falls Through' para que funcione bien!
 * PUERTOS:
 * 	fifo_dout: entrada del puerto 'dout' de la FIFO
 * 	fifo_rd_en: puerto de salida a 'rd_en' de la FIFO
 * 	data[N]: buffer de N elementos leido de la memoria. La idea es implementar este buffer con interfaz 'ap_none' de modo que sea leido por otras IP
 * 	rd_en: esta señal debe ponerse en alto para que el modulo lea N elementos de la memoria. Para reiniciar la operacion debe bajarse y volverse a subir
 * 	valid: esta señal se pone en alto cuando la operacion de leer N elementos de la memoria ha terminado
 * */

#define N 10

void my_read_fifo(int fifo_dout, bool* fifo_rd_en, int data[N], bool rd_en, bool* valid)
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE ap_none port=fifo_dout
#pragma HLS INTERFACE ap_none port=fifo_rd_en
#pragma HLS INTERFACE ap_none port=data
#pragma HLS INTERFACE ap_none port=rd_en
#pragma HLS INTERFACE ap_none port=valid

	static int flag=0;

	if(flag==0 && rd_en==1)
	{
		for(int i=0; i<N; i++)
		{
			*fifo_rd_en=1;

			data[i] = fifo_dout;
			if(i==N-1)
			{
				*fifo_rd_en=0;
				flag=1;
				*valid=1;
			}
		}
	}
	else if(flag==1 && rd_en==0){
		flag=0;
		*valid=0;
	}
}
