"""
Este módulo contiene una lista de algunos recursos relacionados con la 
estructura interna de la FPGA; estas cantidades son invariantes (no se
modifican durante la ejecución de un programa).
"""

## PYNQ-Z2
pynqz2 = [[None for j in range(150)] for i in range(114)] # matriz 114x150; el elemento i,j contiene los parámetros físicos de la celda SLICE_X{i}Y{j}.
                                                          # Si la celda no existe en esta FPGA se devuelve 'None'.
# SLICE_X0
for j in range(50):
    pynqz2[0][j] = '0 M left'
    
# SLICE_X1
for j in range(50):
    pynqz2[1][j] = '1 L left'
    
# SLICE_X2
for j in range(50):
    pynqz2[2][j] = '0 M right'
    
# SLICE_X3
for j in range(50):
    pynqz2[3][j] = '1 L right'
    
# SLICE_X4
for j in range(50):
    pynqz2[4][j] = '0 M left'
    
# SLICE_X5
for j in range(50):
    pynqz2[5][j] = '1 L left'
    
# SLICE_X6
for j in range(50):
    pynqz2[6][j] = '0 M right'
    
# SLICE_X7
for j in range(50):
    pynqz2[7][j] = '1 L right'
    
# SLICE_X8
for j in range(50):
    pynqz2[8][j] = '0 M right'
    
# SLICE_X9
for j in range(50):
    pynqz2[9][j] = '1 L right'
    
# SLICE_X10
for j in range(50):
    pynqz2[10][j] = '0 M left'
    
# SLICE_X11
for j in range(50):
    pynqz2[11][j] = '1 L left'
    
# SLICE_X12
for j in range(50):
    pynqz2[12][j] = '0 M left'
    
# SLICE_X13
for j in range(50):
    pynqz2[13][j] = '1 L left'
    
# SLICE_X14
for j in range(50):
    pynqz2[14][j] = '0 M right'
    
# SLICE_X15
for j in range(50):
    pynqz2[15][j] = '1 L right'
    
# SLICE_X16
for j in range(50):
    pynqz2[16][j] = '0 M left'
    
# SLICE_X17
for j in range(50):
    pynqz2[17][j] = '1 L left'
    
# SLICE_X18
for j in range(50):
    pynqz2[18][j] = '0 M right'
    
# SLICE_X19
for j in range(50):
    pynqz2[19][j] = '1 L right'
    
# SLICE_X20
for j in range(50):
    pynqz2[20][j] = '0 M right'
    
# SLICE_X21
for j in range(50):
    pynqz2[21][j] = '1 L right'
    
# SLICE_X22
for j in range(50):
    pynqz2[22][j] = '0 M left'
    
# SLICE_X23
for j in range(50):
    pynqz2[23][j] = '1 L left'
    
# SLICE_X24
for j in range(50):
    pynqz2[24][j] = '0 L left'
    
# SLICE_X25
for j in range(50):
    pynqz2[25][j] = '1 L left'
    
# SLICE_X26
for j in range(150):
    pynqz2[26][j] = '0 M right'
    
# SLICE_X27
for j in range(150):
    pynqz2[27][j] = '1 L right'
    
# SLICE_X28
for j in range(150):
    pynqz2[28][j] = '0 M left'
    
# SLICE_X29
for j in range(150):
    pynqz2[29][j] = '1 L left'
    
# SLICE_X30
for j in range(150):
    pynqz2[30][j] = '0 M right'
    
# SLICE_X31
for j in range(150):
    pynqz2[31][j] = '1 L right'
    
# SLICE_X32
for j in range(150):
    pynqz2[32][j] = '0 M right'
    
# SLICE_X33
for j in range(150):
    pynqz2[33][j] = '1 L right'
    
# SLICE_X34
for j in range(150):
    pynqz2[34][j] = '0 M left'
    
# SLICE_X35
for j in range(150):
    pynqz2[35][j] = '1 L left'
    
# SLICE_X36
for j in range(150):
    pynqz2[36][j] = '0 M left'
    
# SLICE_X37
for j in range(150):
    pynqz2[37][j] = '1 L left'
    
# SLICE_X38
for j in range(150):
    pynqz2[38][j] = '0 M right'
    
# SLICE_X39
for j in range(150):
    pynqz2[39][j] = '1 L right'
    
# SLICE_X40
for j in range(150):
    pynqz2[40][j] = '0 L left'
    
# SLICE_X41
for j in range(150):
    pynqz2[41][j] = '1 L left'
    
# SLICE_X42
for j in range(150):
    pynqz2[42][j] = '0 M right'
    
# SLICE_X43
for j in range(150):
    pynqz2[43][j] = '1 L right'
    
# SLICE_X44
for j in range(150):
    pynqz2[44][j] = '0 L left'
    
# SLICE_X45
for j in range(150):
    pynqz2[45][j] = '1 L left'
    
# SLICE_X46
for j in range(150):
    pynqz2[46][j] = '0 M right'
    
# SLICE_X47
for j in range(150):
    pynqz2[47][j] = '1 L right'
    
# SLICE_X48
for j in range(150):
    pynqz2[48][j] = '0 L left'
    
# SLICE_X49
for j in range(150):
    pynqz2[49][j] = '1 L left'
    
# SLICE_X50
for j in range(150):
    pynqz2[50][j] = '0 M left'
    
# SLICE_X51
for j in range(150):
    pynqz2[51][j] = '1 L left'
    
# SLICE_X52
for j in range(150):
    pynqz2[52][j] = '0 L right'
    
# SLICE_X53
for j in range(150):
    pynqz2[53][j] = '1 L right'
    
# SLICE_X54
for j in range(150):
    pynqz2[54][j] = '0 M right'
    
# SLICE_X55
for j in range(150):
    pynqz2[55][j] = '1 L right'
    
# SLICE_X56
for j in range(150):
    pynqz2[56][j] = '0 L left'
    
# SLICE_X57
for j in range(150):
    pynqz2[57][j] = '1 L left'
    
# SLICE_X58
for j in range(150):
    pynqz2[58][j] = '0 M right'
    
# SLICE_X59
for j in range(150):
    pynqz2[59][j] = '1 L right'
    
# SLICE_X60
for j in range(150):
    pynqz2[60][j] = '0 L left'
    
# SLICE_X61
for j in range(150):
    pynqz2[61][j] = '1 L left'
    
# SLICE_X62
for j in range(150):
    pynqz2[62][j] = '0 M right'
    
# SLICE_X63
for j in range(150):
    pynqz2[63][j] = '1 L right'
    
# SLICE_X64
for j in range(150):
    pynqz2[64][j] = '0 L left'
    
# SLICE_X65
for j in range(150):
    pynqz2[65][j] = '1 L left'
    
# SLICE_X66
for j in range(150):
    pynqz2[66][j] = '0 M right'
    
# SLICE_X67
for j in range(150):
    pynqz2[67][j] = '1 L right'

# SLICE_X68
for j in range(50):
    pynqz2[68][j] = '0 L left'
    
# SLICE_X69
for j in range(50):
    pynqz2[69][j] = '1 L left'
    
# SLICE_X70
for j in range(50):
    pynqz2[70][j] = '0 L right'
    
# SLICE_X71
for j in range(50):
    pynqz2[71][j] = '1 L right'
    
# SLICE_X72
for j in range(50):
    pynqz2[72][j] = '0 L left'
    
# SLICE_X73
for j in range(50):
    pynqz2[73][j] = '1 L left'
    
# SLICE_X74
for j in range(50):
    pynqz2[74][j] = '0 L right'
    
# SLICE_X75
for j in range(50):
    pynqz2[75][j] = '1 L right'
    
# SLICE_X76
for j in range(50):
    pynqz2[76][j] = '0 L left'
    
# SLICE_X77
for j in range(50):
    pynqz2[77][j] = '1 L left'
    
# SLICE_X78
for j in range(50):
    pynqz2[78][j] = '0 L right'
    
# SLICE_X79
for j in range(50):
    pynqz2[79][j] = '1 L right'

# SLICE_X80
for j in range(150):
    pynqz2[80][j] = '0 L right'
    
# SLICE_X81
for j in range(150):
    pynqz2[81][j] = '1 L right'
    
# SLICE_X82
for j in range(150):
    pynqz2[82][j] = '0 M left'
    
# SLICE_X83
for j in range(150):
    pynqz2[83][j] = '1 L left'
    
# SLICE_X84
for j in range(150):
    pynqz2[84][j] = '0 L right'
    
# SLICE_X85
for j in range(150):
    pynqz2[85][j] = '1 L right'
    
# SLICE_X86
for j in range(150):
    pynqz2[86][j] = '0 M left'
    
# SLICE_X87
for j in range(150):
    pynqz2[87][j] = '1 L left'
    
# SLICE_X88
for j in range(150):
    pynqz2[88][j] = '0 L right'
    
# SLICE_X89
for j in range(150):
    pynqz2[89][j] = '1 L right'
    
# SLICE_X90
for j in range(150):
    pynqz2[90][j] = '0 M right'
    
# SLICE_X91
for j in range(150):
    pynqz2[91][j] = '1 L right'
    
# SLICE_X92
for j in range(150):
    pynqz2[92][j] = '0 M left'
    
# SLICE_X93
for j in range(150):
    pynqz2[93][j] = '1 L left'
    
# SLICE_X94
for j in range(150):
    pynqz2[94][j] = '0 M left'
    
# SLICE_X95
for j in range(150):
    pynqz2[95][j] = '1 L left'
    
# SLICE_X96
for j in range(150):
    pynqz2[96][j] = '0 M right'
    
# SLICE_X97
for j in range(150):
    pynqz2[97][j] = '1 L right'
    
# SLICE_X98
for j in range(150):
    pynqz2[98][j] = '0 M left'
    
# SLICE_X99
for j in range(150):
    pynqz2[99][j] = '1 L left'
    
# SLICE_X100
for j in range(150):
    pynqz2[100][j] = '0 M right'
    
# SLICE_X101
for j in range(150):
    pynqz2[101][j] = '1 L right'
    
# SLICE_X102
for j in range(150):
    pynqz2[102][j] = '0 M right'
    
# SLICE_X103
for j in range(150):
    pynqz2[103][j] = '1 L right'
    
# SLICE_X104
for j in range(150):
    pynqz2[104][j] = '0 M left'
    
# SLICE_X105
for j in range(150):
    pynqz2[105][j] = '1 L left'
    
# SLICE_X106
for j in range(150):
    pynqz2[106][j] = '0 L left'
    
# SLICE_X107
for j in range(150):
    pynqz2[107][j] = '1 L left'
    
# SLICE_X108
for j in range(150):
    pynqz2[108][j] = '0 M right'
    
# SLICE_X109
for j in range(150):
    pynqz2[109][j] = '1 L right'
    
# SLICE_X110
for j in range(150):
    pynqz2[110][j] = '0 L left'
    
# SLICE_X111
for j in range(150):
    pynqz2[111][j] = '1 L left'
    
# SLICE_X112
for j in range(150):
    pynqz2[112][j] = '0 M right'
    
# SLICE_X113
for j in range(150):
    pynqz2[113][j] = '1 L right'
    
    
fabric = {'pynqz2':pynqz2}
