#!/usr/bin/python3.8

import sys

borde = "P2"
cuerpo = "N0"
only_top = 0
lpol = 30.0
kb = 17000.0
b0 = 0.33
ktheta = 50.0
theta0 = 180.0

for i, opt in enumerate(sys.argv): # "i" es el indice que recorre la lista de opciones, "opt" es la opcion propiamente dicha
	if opt == "-help":
		print("\nInterfaz Gromacs-Martini: termalizacion del polimero\n----------------------------------------------------\n\n");
		print("-lpol [30.0] {>0}, longitud del polimero (nanometros)\n\n");
		print("-kb [17000.0] {>0}, constante elastica de las interacciones bond\n\n");
		print("-b0 [0.33] {>0}, distancia de equilibrio de la interaccion bond\n\n");
		print("-ktheta [50.0] {>0}, constante elastica de la interaccion bending\n\n");
		print("-theta0 [180.0] {0 : 180}, angulo de equilibrio para interaccion bending\n\n");
		print("-borde [P2] {atomos MARTINI}, Atomo de los extremos del polimero\n\n");
		print("-cuerpo [N0] {atomos MARTINI}, Atomos del cuerpo del polimero\n\n");
		print("-only_top,\n\tsi esta opcion esta presente, el programa escribe unicamente los ficheros 'membrana.itp' y 'poro.itp' con los valores introducidos. Esto permite modificar los valores para una nueva simulacion si ntener que lanzar de nuevo todo el proceso de termalizacion (no obstante cuidado, ya que los archivos .gro 'antiguos' que queremos usar podrian quedarse obsoletos con los nuevos parametros).\n\n\n");
		
		exit()
		
	if opt == "-lpol":
		lpol=float(sys.argv[i+1])
	
	if opt == "-kb":
		kb=float(sys.argv[i+1])
		
	if opt == "-b0":
		b0=float(sys.argv[i+1])

	if opt == "-ktheta":
		ktheta=float(sys.argv[i+1])

	if opt == "-theta0":
		theta0=float(sys.argv[i+1])

	if opt == "-borde":
		borde=sys.argv[i+1]

	if opt == "-cuerpo":
		cuerpo=sys.argv[i+1]

	if opt == "-only_top":
		only_top=1
		
N = int(lpol/b0)

x=[]
y=[]
z=[]
for i in range(N):
	x.append(0.0)
	y.append(0.0)
	z.append(i*b0)

box=[2*lpol for i in range(3)]

# topologia:
with open("polimero.itp", "w") as f:
	print(";cg PEG_modificado\n", file=f)
	print("[moleculetype]", file=f)
	print(";name\texclusiones", file=f)
	print("PEG\t1", file=f)

	print("\n[atoms]", file=f)
	print(";nr\tattype\tresnr\tresidue\tatom\tcgnr\tcharge\tmass", file=f)
	print("%d\t%s\t%d\t%s\t%s\t%d\t%.3f\t" %(1, borde, 1, "POL", borde, 1, 0.0), file=f)
	for i in range(1, N-1, 1):
		print("%d\t%s\t%d\t%s\t%s\t%d\t%.3f\t" %(i+1, cuerpo, 1, "POL", cuerpo, i+1, 0.0), file=f)
	print("%d\t%s\t%d\t%s\t%s\t%d\t%.3f\t" %(N, borde, 1, "POL", borde, N, 0.0), file=f)

	print("\n[bonds]", file=f)
	print(";ai\taj\tfunc\tb_0\tk_b", file=f)
	for i in range(N-1):
		print("%d\t%d\t%d\t%.3f\t%.3f" %(i+1, i+2, 1, b0, kb), file=f)

	print("\n[angles]", file=f)
	print(";ai\taj\tak\tfunc\ttheta_0\tk_theta", file=f)
	for i in range(N-2):
		print("%d\t%d\t%d\t%d\t%.2f\t%.2f" %(i+1, i+2, i+3, 1, theta0, ktheta), file=f)

	print("\n[dihedrals]", file=f)
	for i in range(N-3):
		print("%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d" %(i+1, i+2, i+3, i+4, 1, 180.00, 1.96, 1), file=f)
		print("%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d" %(i+1, i+2, i+3, i+4, 1, 0.0, 0.18, 2), file=f)
		print("%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d" %(i+1, i+2, i+3, i+4, 1, 0.0, 0.33, 3), file=f)
		print("%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d" %(i+1, i+2, i+3, i+4, 1, 0.0, 0.12, 4), file=f)

with open("polimero_topol.top", "w") as f:
	print("#include \"dry_martini_v2.1.itp\"", file=f)
	print("#include \"polimero.itp\"", file=f)
	print("\n[system]\npolimero\n\n[molecules]\nPOL\t1", file=f)

if not only_top:
	# archivo de indices
	with open("polimero.ndx", "w") as f:
		print( "[POL]", file=f)
		for i in range(N):
			print(f"{i+1}", file=f)
		print( "\n[DENTRO]\n1", file=f)

	# escritura del sistema
	with open("polimero.gro", "w") as f:
		print( f"polimero\n{N}", file=f)
		print("%5d%-5s%5s%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f" %(1,"POL",borde,1,x[0], y[0], z[0], 0.0, 0.0, 0.0), file=f)
		for i in range(1, N-1, 1):
			print("%5d%-5s%5s%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f" %(1,"POL",cuerpo,i+1,x[i], y[i], z[i], 0.0, 0.0, 0.0), file=f)
		print("%5d%-5s%5s%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f" %(1,"POL",borde,1,x[N-1], y[N-1], z[N-1], 0.0, 0.0, 0.0), file=f)
		print("%f\t%f\t%f" %(box[0], box[1], box[2]), file=f)

