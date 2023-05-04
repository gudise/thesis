#include "interfaz_pcps-pspl_config.h"

#ifndef XUARTPS_H
    #ifndef XUARTLITE_H
        #include "xuartps.h"
    #endif
#endif

#include "xgpio.h"
#include "xparameters.h"

#ifdef XUARTPS_H
    #define XUart_Send(x, y, z) XUartPs_Send(x, y, z)
    #define XUart_Recv(x, y, z) XUartPs_Recv(x, y, z)
    #define XUart   XUartPs
    #define UART_DEVICE_ID  XPAR_XUARTPS_0_DEVICE_ID
#elif defined XUARTLITE_H
    #define XUart_Send(x, y, z) XUartLite_Send(x, y, z)
    #define XUart_Recv(x, y, z) XUartLite_Recv(x, y, z)
    #define XUart   XUartLite
    #define UART_DEVICE_ID  XPAR_UARTLITE_0_DEVICE_ID
#endif

#define GPIO_CTRL_ID    XPAR_AXI_GPIO_CTRL_DEVICE_ID
#define GPIO_DATA_ID    XPAR_AXI_GPIO_DATA_DEVICE_ID


#define IDLE        1
#define CALC        2
#define SCAN        4
#define PRINT       6

#define cmd_calc        1
#define cmd_scan        2

#define cmd_idle_sync   0
#define cmd_idle_ack    1
#define cmd_calc_sync   2
#define cmd_calc_ack    3
#define cmd_scan_sync   4
#define cmd_scan_ack    4
#define cmd_scan_wait   5
#define cmd_scan_done   5
#define cmd_print_sync  6
#define cmd_print_ack   6
#define cmd_print_wait  7
#define cmd_print_done  7


#ifndef DATA_WIDTH
    #define DATA_WIDTH          32
#endif
#ifndef BUFFER_IN_WIDTH
    #define BUFFER_IN_WIDTH     32
#endif
#ifndef BUFFER_OUT_WIDTH
    #define BUFFER_OUT_WIDTH    32
#endif
#ifndef OCTETO_IN_WIDTH
    #define OCTETO_IN_WIDTH     4
#endif
#ifndef OCTETO_OUT_WIDTH
    #define OCTETO_OUT_WIDTH    4
#endif
#ifndef WORDS_IN_WIDTH
    #define WORDS_IN_WIDTH      1
#endif
#ifndef WORDS_OUT_WIDTH
    #define WORDS_OUT_WIDTH     1
#endif // Esto asegura que al menos habra unos valores por defecto para los parametros.


// Funciones (encabezados)
//// Config. perifericos
void    config_UART();
void    config_GPIO();
//// Comm. PCPS
void    send_u8(u8 data);
void    send_octeto(u8 octeto_out[]);
u8      recv_u8();
void    recv_octeto(u8 octeto_in[]);
//// Comm. PSPL
void    handshake(unsigned int sync, unsigned int ack);
//// Ciclo FSM
void    scan_cycle(u32 words_in[]);
void    calc_cycle();
void    print_cycle(u32 words_out[]);
//// E/S PCPS
void    import_words_in(u8 octeto_in[], u32 words_in[]);
void    export_octeto_out(u32 words_out[], u8 octeto_out[]);
//// Auxiliares
void    u8_to_bin(u8 entrada, char salida[8]);
void    u32_to_bin(u32 entrada, char salida[], int size);
u32     bin_to_u32(char* entrada, int size);
u8      bin_to_u8(char* entrada);


// Variables globales
XUart Uart;
XGpio gpio_ctrl;
XGpio gpio_data;

// MAIN
int main(void)
{
    int state=IDLE;
    char busy;
    u8 ctrl_in=0;
    u8 octeto_in[OCTETO_IN_WIDTH];
    u32 words_in[WORDS_IN_WIDTH];
    u32 words_out[WORDS_OUT_WIDTH];
    u8 octeto_out[OCTETO_OUT_WIDTH];

    config_UART();
    config_GPIO();
    
    while(1)
    {
        switch(state)
        {
            case IDLE:
                XUart_Recv(&Uart, &ctrl_in, 1);
                switch(ctrl_in)
                {
                    case cmd_calc:
                        state = CALC;
                        break;
                        
                    case cmd_scan:
                        state = SCAN;
                        break;
                        
                    default:
                        state = IDLE;
                        break;
                }
                break;
                
            case CALC:
                busy=1;
                while(busy)
                {
                    scan_cycle(words_in);
                    calc_cycle();
                    print_cycle(words_out);
                    busy = 0;
                }
                state = PRINT;
                break;
                    
            case SCAN:
                recv_octeto(octeto_in);
                import_words_in(octeto_in, words_in);
                state = IDLE;
                break;
                        
            case PRINT:
                export_octeto_out(words_out, octeto_out);
                send_octeto(octeto_out);
                state = IDLE;
                break;
        }
        ctrl_in=0;
    }
    return 0;
}


// Funciones (definiciones)
//// Config. perifericos
void config_UART()
{
    #ifdef XUARTPS_H
    XUartPs_Config *Config;
    
    Config = XUartPs_LookupConfig(UART_DEVICE_ID);
    XUartPs_CfgInitialize(&Uart, Config, Config->BaseAddress);
    XUartPs_SetBaudRate(&Uart, 9600);
    #endif
    #ifdef XUARTLITE_H
    XUartLite_Config *Config;
    
    Config = XUartLite_LookupConfig(UART_DEVICE_ID);
    XUartLite_CfgInitialize(&Uart, Config, Config->RegBaseAddr);
    #endif
}

void config_GPIO()
{

    XGpio_Initialize(&gpio_ctrl, GPIO_CTRL_ID);
    XGpio_SetDataDirection(&gpio_ctrl, 1, 0xF); //cmd: fabric->ps
    XGpio_SetDataDirection(&gpio_ctrl, 2, 0x0); //cmd: ps->fabric

    XGpio_Initialize(&gpio_data, GPIO_DATA_ID);
    XGpio_SetDataDirection(&gpio_data, 1, 0xF); //data_out: fabric->ps
    XGpio_SetDataDirection(&gpio_data, 2, 0x0); //data_out: ps->fabric
}

//// Comm. PCPS
void send_u8(u8 data)
{
    int j=0;
    
    while(j<1)
        j+=XUart_Send(&Uart, &data, 1); //envia 1 byte (i.e. u8) al pc
}

void send_octeto(u8 octeto_out[])
{
    int i;

    for(i=0; i<OCTETO_OUT_WIDTH; i++)
        send_u8(octeto_out[i]);
}

u8 recv_u8()
{
    u32 bytes_read=0;
    u8 result;

    while(1) {
        bytes_read = XUart_Recv(&Uart, &result, 1);

        if(bytes_read>0)
            break;
    }

    return result;
}

void recv_octeto(u8 octeto_in[])
{
    int i;

    for(i=0; i<OCTETO_IN_WIDTH; i++)
        octeto_in[i] = recv_u8(Uart);
}

//// Comm. PSPL
void handshake(unsigned int sync, unsigned int ack)
{
    u32 ctrl_out;

     XGpio_DiscreteWrite(&gpio_ctrl, 2, sync);
     while(1)
     {
         ctrl_out = XGpio_DiscreteRead(&gpio_ctrl, 1);
         if(ctrl_out==ack)
             break;
     }
}

//// Ciclo FSM
void calc_cycle()
{
    handshake(cmd_calc_sync, cmd_calc_ack);
    handshake(cmd_idle_sync, cmd_idle_ack);
}

void scan_cycle(u32 words_in[])
{
    int i;
    for(i=0; i<WORDS_IN_WIDTH; i++)
    {
        handshake(cmd_scan_sync, cmd_scan_ack);
        XGpio_DiscreteWrite(&gpio_data, 2, words_in[i]);
        handshake(cmd_scan_wait, cmd_scan_done);
    }
    handshake(cmd_idle_sync, cmd_idle_ack);
}

void print_cycle(u32 words_out[])
{
    int i;

    for(i=0; i<WORDS_OUT_WIDTH; i++)
    {
        handshake(cmd_print_sync, cmd_print_ack);
        words_out[i] = XGpio_DiscreteRead(&gpio_data, 1);
        handshake(cmd_print_wait, cmd_print_done);
    }
    handshake(cmd_idle_sync, cmd_idle_ack);
}

//// E/S PCPS
void import_words_in(u8 octeto_in[], u32 words_in[])
{
    /*
     * Esta funcion toma BUFFER_IN_WIDTH bits repartidos en OCTETO_IN_WIDTH octetos tal y como
     * son leidos desde UART en el array octeto_in, y lo convierte en WORDS_IN_WIDTH palabras
     * de 32 bits que se almacenan en el array words_in. Este array puede ser enviado tal cual
     * a PL.
     * */

    int i, j, contador_bits;
    char buffer_in[BUFFER_IN_WIDTH], aux_octeto[8], aux_words[DATA_WIDTH];

    contador_bits=0;
    for(i=0; i<OCTETO_IN_WIDTH; i++)
    {
        u8_to_bin(octeto_in[i], aux_octeto);
        for(j=0; j<8; j++)
        {
            if(contador_bits==BUFFER_IN_WIDTH)
                break;
            else
            {
                buffer_in[contador_bits]=aux_octeto[j];
                contador_bits++;
            }
        }
    } // Aqui recuperamos 'buffer_in' desde 'octeto_in'.


    for(i=0; i<WORDS_IN_WIDTH; i++)
        words_in[i] = 0;

    contador_bits=0;
    for(i=0; i<WORDS_IN_WIDTH; i++)
    {
        for(j=0; j<DATA_WIDTH;j++)
        {
            if(contador_bits<BUFFER_IN_WIDTH)
            {
                aux_words[j]=buffer_in[contador_bits];
                contador_bits++;
            }
            else
                aux_words[j]=0; // Este if-else permite paddear las palabras cuando a buffer_in le sobran un numero de bits insuficiente para completar una palabra completa.
        }
        words_in[i]=bin_to_u32(aux_words, DATA_WIDTH);
    }
}

void export_octeto_out(u32 words_out[], u8 octeto_out[])
{
    /*
     * Esta funcion toma BUFFER_OUT_WIDTH bits repartidos en WORDS_OUT_WIDTH numeros de tipo

     * u32, cada uno de los cuales almacena DATA_WIDTH bits, y lo convierte a OCTETO_OUT_WIDTH bytes.
     * */

    int i, j, contador_bits;
    char buffer_out[BUFFER_OUT_WIDTH], aux_words[DATA_WIDTH], aux_octeto[8];

    contador_bits=0;
    for(i=0; i<WORDS_OUT_WIDTH; i++)
    {
        u32_to_bin(words_out[i], aux_words, DATA_WIDTH);
        for(j=0; j<DATA_WIDTH; j++)
        {
            if(contador_bits==BUFFER_OUT_WIDTH)
                break;
            else
            {
                buffer_out[contador_bits]=aux_words[j];
                contador_bits++;
            }
        }
    } // Aqui recuperamos 'buffer_out' desde 'words_in'.

    for(i=0; i<OCTETO_OUT_WIDTH; i++)
        octeto_out[i]=0;

    contador_bits=0;
    for(i=0; i<OCTETO_OUT_WIDTH; i++)
    {
        for(j=0; j<8; j++)
        {
            if(contador_bits<BUFFER_OUT_WIDTH)
            {
                aux_octeto[j] = buffer_out[contador_bits];
                contador_bits++;
            }
            else
                aux_octeto[j] = 0;
        }
        octeto_out[i]=bin_to_u8(aux_octeto);
    }
}

//// Auxiliares
void u8_to_bin(u8 entrada, char salida[8])
{
    /*
     * Esta funcion convierte un numero u8 (de 0 a 255) a formato binario, y guarda el
     * resultado en un string 'salida' en formato 'little endian' (salida[0] es el bit menos significativo).
     * */

    int i;
    for(i=0; i<8; i++)
    {
        salida[i] = entrada%2;
        entrada = entrada/2;
    }
}

void u32_to_bin(u32 entrada, char salida[], int size)
{
    /*
     * Esta funcion convierte un numero u32 a formato binario, y guarda el
     * resultado en un string 'salida' en formato 'little endian' (salida[0]
     * es el bit menos significativo), truncado a 'size' bits (de modo que
     * 'salida' debe ser un array de tamano 'size').
     * */

    int i;
    for(i=0; i<size; i++)
    {
        salida[i] = entrada%2;
        entrada = entrada/2;
    }
}

u32 bin_to_u32(char* entrada, int size)
{
    /*
     * Esta funcion toma una palabra binaria de 'size' bits, y lo convierte a u32.
     * Obviamente size<=32.
     * */

    int i;
    u32 result, factor;
    
    factor=1;
    result=0;
    for(i=0; i<size; i++)
    {
        if(entrada[i])
            result+=factor;
        factor<<=1;
    }
    return result;
}

u8 bin_to_u8(char* entrada)
{
    /*
     * Esta funcion toma una palabra binaria de 8 bits, y lo convierte a u8.
     * */

    int i;
    u8 result, factor;
    
    factor=1;
    result=0;
    for(i=0; i<8; i++)
    {
        if(entrada[i])
            result+=factor;
        factor<<=1;
    }
    return result;
}

