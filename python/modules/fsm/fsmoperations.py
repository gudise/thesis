#from .main import *

cmd_idle =		105 #'i'
cmd_reset =		114 #'r'
cmd_calc =		99	#'c'
cmd_end =		101 #'e'
cmd_scan =		115 #'s'
cmd_print =		112 #'p'

cmd_idle_sync =		22 #SYN
cmd_reset_sync =	13 #'\n'
cmd_calc_sync =		2  #ETX
cmd_scan_sync =		3  #ETX
cmd_print_sync =	4  #ETX


def calc(serialport, buffer_out_width):
	"""
	Esta funcion dispara un ciclo RDY->CALC->PRINT->RDY
	a una fsm conectada a traves de 'serialport'
	y devuelve un bitstr de tamaño 'buffer_out_width'
	"""
	bytestr_size = buffer_out_width//8+(buffer_out_width%8>0)
	send_u8(serialport, cmd_calc)
	bytestr = receive_bytestr(serialport, bytestr_size)
	return bytestr_to_bitstr(bytestr, buffer_out_width)
		
		
def reset(serialport):
	"""
	Esta funcion dispara un ciclo RDY->RST->RDY a una fsmuart conectada a traves de 'serialport'
	"""
	send_u8(serialport, cmd_reset)
	while 1:
		read = receive_u8(serialport)
		if read==cmd_reset_sync:
			break


def scan(serialport, bitstr_in, buffer_in_width):
	"""
	Esta funcion dispara un ciclo RDY->SCAN->RDY
	a una fsm conectada a traves de 'serialport'
	y sube un bitstr de tamaño 'buffer_in_width'
	"""
		
	send_u8(serialport, cmd_scan)
	send_bytestr(serialport, bitstr_to_bytestr(bitstr_in, buffer_in_width))
	

def end(serialport):
	send_u8(serialport, cmd_end)