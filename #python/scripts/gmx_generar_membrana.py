#!/usr/bin/python3.8

import sys
import numpy as np


class PORO:
	def __init__(self, radio, long_interior, long_exterior, kb, b0, ktheta, theta0, borde, cuerpo):
		self.radio = radio
		self.long_interior = long_interior
		self.long_exterior = long_exterior
		self.kb = kb
		self.b0 = b0
		self.ktheta = ktheta
		self.theta0 = theta0
		self.borde = borde
		self.cuerpo = cuerpo

		self.n_exterior = int(long_exterior/b0)
		self.n = int((long_interior+2*long_exterior)/b0)

		self.phi = np.acos(1-b0**2/(2*radio**2))
		self.m = int(2*PI/self.phi)

		self.N = self.n*self.m

		self.phi_corregido = 2*PI/poro.m
		self.radio_corregido = poro.b0*np.sqrt(1/(2*(1-np.cos(phi_corregido))))

		self.Rx = [[poro.b0*i for i in range(poro.n)] for j in range(poro.m)]
		self.Ry = [[radio_corregido*np.cos(j*phi_corregido+0.5*phi_corregido*(i%2) for i in range(poro.n)] for j in range(poro.m)]
		self.Rz = [[radio_corregido*np.sin(j*phi_corregido+0.5*phi_corregido*(i%2) for i in range(poro.n)] for j in range(poro.m)]

		self.x = [Rx[j][i] for i,j in range(poro.n),range(poro.m)]
		self.y = [Ry[j][i] for i,j in range(poro.n),range(poro.m)]
		self.z = [Rz[j][i] for i,j in range(poro.n),range(poro.m)]
		self.residuo = [i+1 for i in range(poro.n) for j in range(poro.m)]
		self.chgrp = [i*poro.m+j for i in range(poro.n) for j in range(poro.m)]

		self.box = [abs(Rx[poro.m-1][poro.n-1]*1.1), abs(Ry[poro.m-1][poro.n-1]*1.1), abs(Rz[poro.m-1][poro.n-1]*1.1)]
		self.angle = [0.5*PI, 0]


	def centro_masas(self):
		return centro_masas(self)


	def centrar(self):
		centrar_estructura(self)


    def move(self):
        mover_estructura(self, theta, phi, Rx, Ry, Rz)


	def topol(self):
		matriz_distancia = [[np.sqrt((self.Rx[i%self.m][i//self.m]-self.Rx[j%self.m][j//self.m])**2+(self.Ry[i%self.m][i//self.m]-self.Ry[j%self.m][j//self.m])*2+(self.Rz[i%self.m][i//self.m]-self.Rz[j%self.m][j//self.m])**2) for j in range(self.n*self.m)] for i in range(self.n*self.m)]

		matriz_vecinos = [[1 if i!=j and matriz_distancia[i][j]<np.sqrt(2.0)*self.b0 else 2 if i!=j and ( (i//self.m==j//self.m and i%self.m==0 and j%self.m==self.m-2) or (i//self.m==j//self.m and i%self.m==1 and j%self.m==self.m-1) or (i//self.m==j//self.m and i%self.m==j%self.m+2) or (i%self.m==j%self.m-1 and i//self.m==j//self.m-2) or (i%self.m==j%self.m+1 and i//self.m==j//self.m-2) or (i%self.m==self.m-1 and j%self.m==0 and i//self.m==j//self.m-2) or (i%self.m==0 and j%self.m==self.m-1 and i//self.m==j//self.m-2) ) else 0  for j in range(self.n*self.m)] for i in range(self.n*self.m)]

		matriz_enlaces = [[1 if matriz_vecinos[i][j]==1 else 0 for j in range(i+1, self.n*self.m, 1)] for i in range(self.n*self.m)]
		matriz_angulos = [[[1 if matriz_vecinos[i][j]==1 and matriz_vecinos[j][k]==1 and matriz_vecinos[i][k]==1 else 0 for k in range(j+1, self.n*self.m, 1)] for j in range(i+1, self.n*self.m, 1)] for i in range(self.n*self.m)]

		with open("poro.itp", "w") as f:
			print(";Poro para la simulacion de traslocacion de un polimero")
			print("\n[moleculetype]\n;name\texclusion\nPORO\t1")

			print("\n[atoms]\n;nr\tattype\tresnr\tresidue\tatom\tcgnr\tcharge\tmass")
			for i in range(self.n):
				for j in range(self.m):
					if i<self.n_exterior or i+1>self.n-self.n_exterior:
						print("%d\t%s\t%d\tPORO\tQ0\t%d\t%.3f" %(j+self.m*i+1, self.borde, i+1, j+self.m*i+1, 0.0))
					else
						print("%d\t%s\t%d\tPORO\tC1\t%d\t%.3f" %(j+self.m*i+1, self.cuerpo, i+1, j+self.m*i+1, 0.0))

			print("\n[bonds]\n;i\tj\tfunc\tb_0\tk_b");
			for i in range(self.m*self.n):
				for j in range(self.m*self.n):
					if matriz_enlaces[i][j]==1:
						print("%d\t%d\t%d\t%.3f\t%.1f" %(i+1,j+1,1,self.b0, self.kb))

			print("\n[angles]\n;i\tj\tk\tfunc\ttheta_0\tk_theta");
			for i in range(self.m*self.n):
				for j in range(self.m*self.n):
					for k in range(self.m*self.n):
						if matriz_angulos[i][j][k]==1:
							print("%d\t%d\t%d\t%d\t%.3f\t%.1f" %(i+1, j+1, k+1, 1, self.theta0, self.ktheta))


class DSPC:
    N=14
	angle = [0.5*PI, 0]

	x = [0 for i in range(14)]
	y = [0 for i in range(14)]
	z = [0 for i in range(14)]

	x[0] = 0
	y[0] = 0
	z[0] = 0
	atomname.append('N')

	x[1] = 0.47
	y[1] = 0
	z[1] = 0
	atomname.append('P')

	x[2] = 0.47*(1+np.cos(PI/9))
	y[2] = 0.47*np.sin(PI/9)
	z[2] = 0
	atomname.append('G')

	x[3] = 0.47*(1+np.cos(PI/9))
	y[3] = -0.47*np.sin(PI/9)
	z[3] = 0
	atomname.append('G')

	x[4] =  0.47*(1+2*np.cos(PI/9))
	y[4] = 2*0.47*np.sin(PI/9)
	z[4] = 0
	atomname.append('C')

	for i in range(5, 9, 1):
		x[i]=x[i-1]+0.47
		y[i]=y[i-1]
		z[i]=z[i-1]
		atomname.append('C')

	x[9]=x[3]+0.47*np.cos(PI/9)
	y[9]=y[3]-0.47*np.sin(PI/9)
	z[9]=z[3]
	atomname.append('C')

	for i in range(10, 14, 1):
		x[i]=x[i-1]+0.47
		y[i]=y[i-1]
		z[i]=z[i-1]
		atomname.append('C')

	residuo = [1 for i in range(14)]
	chgrp = [i+1 for i in range(14)]

	box = [5.0, 1.0, 1.0]


	def centro_masas(self):
		return centro_masas(self)


	def centrar(self):
		centrar_estructura(self)


    def move(self):
        mover_estructura(self, theta, phi, Rx, Ry, Rz)


	def topol(self):
		with open("dspc.itp","w") as f:
			print(";Molecula 'DSPC'[ S.J. Marrink, A.H. de Vries, A.E. Mark] para simulacion de traslocacion de PEG");

			print("\n[moleculetype]\n;molname\tnrexcl\nDSPC\t1")

			print("\n[atoms]\n;id\ttype\tresnr\tresidu\tatom\tcgnr\tcharge")
			print("1\tQ0\t1\tDSPC\tNC\t1\t1.0")
			print("2\tQa\t1\tDSPC\tPO\t2\t-1.0")
			print("3\tNa\t1\tDSPC\tGL\t3\t0.0")
			print("4\tNa\t1\tDSPC\tGL\t4\t0.0")
			print("5\tC1\t1\tDSPC\tCA\t5\t0.0")
			print("6\tC1\t1\tDSPC\tCA\t6\t0.0")
			print("7\tC1\t1\tDSPC\tCA\t7\t0.0")
			print("8\tC1\t1\tDSPC\tCA\t8\t0.0")
			print("9\tC1\t1\tDSPC\tCA\t9\t0.0")
			print("10\tC1\t1\tDSPC\tCB\t10\t0.0")
			print("11\tC1\t1\tDSPC\tCB\t11\t0.0")
			print("12\tC1\t1\tDSPC\tCB\t12\t0.0")
			print("13\tC1\t1\tDSPC\tCB\t13\t0.0")
			print("14\tC1\t1\tDSPC\tCB\t14\t0.0")

			print("\n[bonds]\n;i\tj\tfunct\tlength\tforce.c.")
			print("1\t2\t1\t0.450\t1250.000")
			print("2\t3\t1\t0.450\t1250.000")
			print("3\t4\t1\t0.370\t1250.000")
			print("3\t5\t1\t0.480\t1250.000")
			print("5\t6\t1\t0.480\t1250.000")
			print("6\t7\t1\t0.480\t1250.000")
			print("7\t8\t1\t0.480\t1250.000")
			print("8\t9\t1\t0.480\t1250.000")
			print("4\t10\t1\t0.480\t1250.000")
			print("10\t11\t1\t0.480\t1250.000")
			print("11\t12\t1\t0.480\t1250.000")
			print("12\t13\t1\t0.480\t1250.000")
			print("13\t14\t1\t0.480\t1250.000")

			print("\n[angles]\n;i\tj\tk\tfunct\tangle\tforce.c")
			print("2\t3\t4\t2\t120.000\t25.000")
			print("2\t3\t5\t2\t180.000\t25.000")
			print("3\t5\t6\t2\t180.000\t35.000")
			print("5\t6\t7\t2\t180.000\t35.000")
			print("6\t7\t8\t2\t180.000\t35.000")
			print("7\t8\t9\t2\t180.000\t35.000")
			print("4\t10\t11\t2\t180.000\t35.000")
			print("10\t11\t12\t2\t180.000\t35.000")
			print("11\t12\t13\t2\t180.000\t35.000")
			print("12\t13\t14\t2\t180.000\t35.000")


class MEMBRANA:
	def __init__(self, area):
		self.area = area
		self.lado = np.sqrt(area)
		self.n = int(self.lado)

		self.n_dspc =2*(membrana_n*membrana_n-eliminados);

	float area;
    float lado;
    int NumeroDSPC;


def centro_masas(estructura):
    """
    Esta funcion devuelve el CM de la estructura.
    """
    cm_x=0
    cm_y=0
    cm_z=0
    for i in range(estructura.n):
        cm_x+=estructura.x[i]
        cm_y+=estructura.y[i]
        cm_z+=estructura.z[i]
    cm_x/=estructura.N
    cm_y/=estructura.N
    cm_z/=estructura.N

    return cm_x, cm_y, cm_z


def centrar_estructura(estructura):
    """
    Esta funcion centra la estructura en torno al origen
    """
    cm_x, cm_y, cm_z = estructura.centro_masas(estructura)
    for i in range(estructura.N):
        estructura.x[i]-=cm_x
        estructura.y[i]-=cm_y
        estructura.z[i]-=cm_z


def rotar_vector(vector, eje_rotacion, angulo):
    matriz_rotacion=[[0 for i in range(3)] for j in range(3)]
    vector_auxiliar=[0, 0, 0]

    angulo_rad=angulo*PI/180; #Convertir angulos a radianes
    mod_eje=np.sqrt(np.inner(eje_rotacion, eje_rotacion)) # Modulo del eje de rotacion. Debe ser 1, pero por si acaso lo normalizo antes de seguir.

    eje_rotacion[0]=eje_rotacion[0]/mod_eje
    eje_rotacion[1]=eje_rotacion[1]/mod_eje
    eje_rotacion[2]=eje_rotacion[2]/mod_eje

    matriz_rotacion[0][0]=np.cos(angulo_rad)+eje_rotacion[0]*eje_rotacion[0]*(1-np.cos(angulo_rad))
    matriz_rotacion[0][1]=eje_rotacion[0]*eje_rotacion[1]*(1-np.cos(angulo_rad))-eje_rotacion[2]*np.sin(angulo_rad)
    matriz_rotacion[0][2]=eje_rotacion[0]*eje_rotacion[2]*(1-np.cos(angulo_rad))+eje_rotacion[1]*np.sin(angulo_rad)
    matriz_rotacion[1][0]=eje_rotacion[0]*eje_rotacion[1]*(1-np.cos(angulo_rad))+eje_rotacion[2]*np.sin(angulo_rad)
    matriz_rotacion[1][1]=np.cos(angulo_rad)+eje_rotacion[1]*eje_rotacion[1]*(1-np.cos(angulo_rad))
    matriz_rotacion[1][2]=eje_rotacion[1]*eje_rotacion[2]*(1-np.cos(angulo_rad))-eje_rotacion[0]*np.sin(angulo_rad)
    matriz_rotacion[2][0]=eje_rotacion[0]*eje_rotacion[2]*(1-np.cos(angulo_rad))-eje_rotacion[1]*np.sin(angulo_rad)
    matriz_rotacion[2][1]=eje_rotacion[1]*eje_rotacion[2]*(1-np.cos(angulo_rad))+eje_rotacion[0]*np.sin(angulo_rad)
    matriz_rotacion[2][2]=np.cos(angulo_rad)+eje_rotacion[2]*eje_rotacion[2]*(1-np.cos(angulo_rad)) # Matriz de rotacion construida.

    for i in range(3):
        for j in range(3):
            vector_auxiliar[i]+=matriz_rotacion[i][j]*vector[j] # producto matriz_rotacion·vector
    vector = vector_auxiliar


def mover_estructura(estructura, theta, phi, Rx, Ry, Rz):
    """
    Coloca el CM de "estructura" en Rx,Ry,Rz; y la orienta en theta, phi.
    """
    estructura.centrar_estructura()

    vector_director = [np.sin(estructura.angle[0])*np.cos(estructura.angle[1]), np.sin(estructura.angle[0])*np.sin(estructura.angle[1]), np.cos(estructura.angle[0])]

    delta_theta=theta-180*estructura.angle[0]/PI;
    delta_phi=phi-180*estructura.angle[1]/PI;

    for i in range(estructura.N):
        rotar_vector([0, 0, 1], [estructura.x[i], estructura.y[i], estructura.z[i]], delta_phi)

    nueva_direccion = [np.sin(theta)*np.cos(phi), np.sin(theta)*np.sin(phi), np.cos(theta)]
    eje_rot_theta = np.cross(vector_director, nueva_direccion)

    mod = np.sqrt(np.inner(eje_rot_theta, eje_rot_theta))

    if mod<0.00001 and mod >-0.00001:
        mod=1.0
        eje_rot_theta = nueva_direccion

    eje_rot_theta[0]=eje_rot_theta[0]/mod
    eje_rot_theta[1]=eje_rot_theta[1]/mod
    eje_rot_theta[2]=eje_rot_theta[2]/mod


    rotar_vector(eje_rot_theta, vector_director, delta_theta)
    for i in range(estructura.N):
        rotar_vector(eje_rot_theta, [estructura.x[i], estructura.y[i], estructura.z[i]], delta_theta)

    for i in range(estructura.N):
        estructura.x[i]+=Rx
        estructura.y[i]+=Ry
        estructura.z[i]+=Rz

    estructura.angle[0]=PI/180*theta
    estructura.angle[1]=PI/180*phi





borde = "P2"
cuerpo = "N0"
only_top = 0
lpol = 30.0
kb = 17000.0
b0 = 0.33
ktheta = 50.0
theta0 = 180.0
radio = 0.5

for i, opt in enumerate(sys.argv):
	if opt == "-help":
            printf("\n Programa para generar un sistema membrana-poro (no hace ninguna simulacion, de modo que la estructura no esta minimizada). Debe utilizarse junto con un polimero termalizado(utilizando 'gmx_integrarSistema') para realizar una simulacion completa.\n-------------------------------------------\n\nOpciones:\n\n")
            printf("-area 200.0,\n\tsuperficie de la membrana\n\n\n")
            printf("-radio 0.5 ,\n\tradio del poro\n\n\n")
            printf("-long_interior_poro(lip) 6.4 ,\n\tEsta opcion permite cambiar la longitud del poro (en nanometros). Se recomienda no modificar esta opcion, ya que depende de la anchura de la membrana (que no puede modificarse)\n\n\n");
            printf("-long_exterior_poro(lep) 1.0 ,\n\tEsta opcion permite cambiar el numero de anillos que conforman el poro (por defecto = 3).\n\n\n");
            printf("-b0 0.33,\n\tdistancia de reposo entre dos particulas bond del poro\n\n\n");
            printf("-kb 7000.0,\n\tcte de acoplo para la interaccion bond de las particulas del poro\n\n\n");
            printf("-theta0 180.0,\n\tangulo de reposo formado por tres particulas bending del poro\n\n\n");
            printf("-ktheta 50.0,\n\tcte de acoplo para la interaccion bending de las particulas del poro\n\n\n");
            printf("-borde P3 ,\n\testa opcion permite definir el tipo de atomo (de Martini) que utilizamos en los anillos exteriores del poro.\n\n\n");
            printf("-cuerpo C1 ,\n\testa opcion permite definir el tipo de atomo (de Martini) que utilizamos en los anillos interiores del poro.\n\n\n");
            printf("-only_top,\n\tTODAVIA NO FUNCIONA: HAY QUE SEPARAR LA GENERACION DE .ITP DE .GRO.si esta opcion esta presente, el programa escribe unicamente los ficheros 'membrana.itp' y 'poro.itp' con los valores introducidos. Esto permite modificar los valores para una nueva simulacion si ntener que lanzar de nuevo todo el proceso de termalizacion (no obstante cuidado, ya que los archivos .gro 'antiguos' que queremos usar podrian quedarse obsoletos con los nuevos parametros).\n\n\n");
		
		exit()
		
		if opt=="-area":
			area = float(opt[i+1])

		if opt=="-radio":
			radio = float(opt[i+1])

		if opt=="-long_interior_poro" or opt=="-lip":
			long_interior=float(opt[i+1])

		if opt=="-long_exterior_poro" or opt=="-lep":
			long_exterior=float(opt[i+1])

		if opt=="-b0":
			b0=float(opt[i+1])

		if opt=="-kb":
			kb=float(opt[i+1])

		if opt=="-theta0":
			theta0=float(opt[i+1])

		if opt=="-ktheta":
			ktheta=float(opt[i+1])

		if opt=="-borde":
			borde=opt[i+1]

		if opt=="-cuerpo":
			cuerpo=opt[i+1]

        if opt=="-only_top":
			only_top=1;
		


    poro = PORO(radio, long_interior, long_exterior, kb, b0, ktheta, theta0, borde, cuerpo)
    poro.move(0, 0, -0.4, -0.4, 0)

    atom_number = 0
    poro_index = []
    for i in range(poro.N):
        atom_number+=1
        poro_index.append(atom_number)




    dspc_residuo=(int*)malloc(sizeof(int)*14);

    dspc_chgrp=(int*)malloc(sizeof(int)*14);

    dspc_x=(float**)malloc(sizeof(float*)*14);
    for(int i=0;i<14;i++)
        dspc_x[i]=(float*)malloc(sizeof(float)*3);

    dspc_v=(float**)malloc(sizeof(float*)*14);
    for(int i=0;i<14;i++)
        dspc_v[i]=(float*)malloc(sizeof(float)*3);

    membrana_index_a=(int***)malloc(sizeof(int**)*membrana_n);
    for(int i=0;i<membrana_n;i++)
        membrana_index_a[i]=(int**)malloc(sizeof(int*)*membrana_n);
    for(int i=0;i<membrana_n;i++)
    {
        for(int j=0;j<membrana_n;j++)
            membrana_index_a[i][j]=(int*)malloc(sizeof(int)*14);
    }

    membrana_index_b=(int***)malloc(sizeof(int**)*membrana_n);
    for(int i=0;i<membrana_n;i++)
        membrana_index_b[i]=(int**)malloc(sizeof(int*)*membrana_n);
    for(int i=0;i<membrana_n;i++)
    {
        for(int j=0;j<membrana_n;j++)
            membrana_index_b[i][j]=(int*)malloc(sizeof(int)*14);
    }

    membrana_x_a=(float****)malloc(sizeof(float***)*membrana_n);

    for(int i=0;i<membrana_n;i++)
        membrana_x_a[i]=(float***)malloc(sizeof(float**)*membrana_n);

    for(int i=0;i<membrana_n;i++){
        for(int j=0;j<membrana_n;j++)
            membrana_x_a[i][j]=(float**)malloc(sizeof(float*)*14);
    }
    for(int i=0;i<membrana_n;i++){
        for(int j=0;j<membrana_n;j++){
            for(int k=0;k<14;k++)
                membrana_x_a[i][j][k]=(float*)malloc(sizeof(float)*3);
        }
    }

    membrana_x_b=(float****)malloc(sizeof(float***)*membrana_n);

    for(int i=0;i<membrana_n;i++)
        membrana_x_b[i]=(float***)malloc(sizeof(float**)*membrana_n);

    for(int i=0;i<membrana_n;i++){
        for(int j=0;j<membrana_n;j++)
            membrana_x_b[i][j]=(float**)malloc(sizeof(float*)*14);
    }
    for(int i=0;i<membrana_n;i++){
        for(int j=0;j<membrana_n;j++){
            for(int k=0;k<14;k++)
                membrana_x_b[i][j][k]=(float*)malloc(sizeof(float)*3);
        }
    }

    ConstruyeDSPC(dspc_x, dspc_v, dspc_residuo, dspc_chgrp, dspc_box, dspc_angle, 20.0, membrana->lado);

    ///RELLENAR membrana_a:
    for(int i=0;i<membrana_n;i++)
    {
        for(int j=0;j<membrana_n;j++)
        {
            MoverEstructura(dspc_x,14, dspc_angle,0,180*ran1()-90,0.7*(i-0.5*membrana_n),0.7*(j-0.5*membrana_n),1.6);
            UbicarEnMembrana(membrana_x_a,dspc_x,14,i,j);
        }
    }

    ///RELLENAR membrana_b:
    for(int i=0;i<membrana_n;i++){
        for(int j=0;j<membrana_n;j++){
            MoverEstructura(dspc_x,14, dspc_angle, 180,180*ran1()-90,0.7*(i-0.5*membrana_n),0.7*(j-0.5*membrana_n),-1.6);
            UbicarEnMembrana(membrana_x_b,dspc_x,14,i,j);
        }
    }

    AhuecarMembrana(membrana_x_a,membrana_n,poro->radio*1.6);
    eliminados=AhuecarMembrana(membrana_x_b,membrana_n,poro->radio*1.6);

    for(int i=0;i<membrana_n;i++){
        for(int j=0;j<membrana_n;j++){
            for(int k=0;k<14;k++){
                if(membrana_x_a[i][j][0]!=NULL){
                    atom_number++;
                    membrana_index_a[i][j][k]=atom_number;
                }

            }
        }
    }

    for(int i=0;i<membrana_n;i++){
        for(int j=0;j<membrana_n;j++){
            for(int k=0;k<14;k++){
                if(membrana_x_b[i][j][0]!=NULL){
                    atom_number++;
                    membrana_index_b[i][j][k]=atom_number;
                }
            }
        }
    }

    ///ESCRITURA DEL SISTEMA:
    membrana->NumeroDSPC=2*(membrana_n*membrana_n-eliminados);
    sistema_n=poro->n+membrana->NumeroDSPC*14;

    punte = fopen(out_name, "w");
    fprintf(punte,"%s\n","Sistema");

    fprintf(punte,"%d\n",sistema_n);

    for(int i=0;i<poro->n;i++)
    {
        if(i/poro->M<poro->NExterior || i/poro->M+1>poro->Nb-poro->NExterior)
            fprintf(punte,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",poro_residuo[i],"PORO",'G',poro_index[i],poro_x[i][0],poro_x[i][1],poro_x[i][2],poro_v[i][0],poro_v[i][1],poro_v[i][2]);
        else
            fprintf(punte,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",poro_residuo[i],"PORO",'C',poro_index[i],poro_x[i][0],poro_x[i][1],poro_x[i][2],poro_v[i][0],poro_v[i][1],poro_v[i][2]);
    }

    contador=1;
    for(int j=0;j<membrana_n;j++)
    {
        for(int k=0;k<membrana_n;k++)
        {
            if(membrana_x_a[j][k][0]!=NULL)
            {
                for(int i=0;i<14;i++)
                    fprintf(punte,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",contador+poro_residuo[poro->n-1],"DSPC",dspc_atomname[i],membrana_index_a[j][k][i],membrana_x_a[j][k][i][0],membrana_x_a[j][k][i][1],membrana_x_a[j][k][i][2],dspc_v[i][0],dspc_v[i][1],dspc_v[i][2]);

                contador++;
            }
        }
    }

    for(int j=0;j<membrana_n;j++)
    {
        for(int k=0;k<membrana_n;k++)
        {
            if(membrana_x_b[j][k][0]!=NULL)
            {
                for(int i=0;i<14;i++)
                    fprintf(punte,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",contador+poro_residuo[poro->n-1],"DSPC",dspc_atomname[i],membrana_index_b[j][k][i],membrana_x_b[j][k][i][0],membrana_x_b[j][k][i][1],membrana_x_b[j][k][i][2],dspc_v[i][0],dspc_v[i][1],dspc_v[i][2]);

                contador++;
            }
        }
    }

    fprintf(punte,"%f\t%f\t%f\n",poro_box[0]+membrana_n*dspc_box[0],+poro_box[1]+membrana_n*dspc_box[1],poro_box[2]+membrana_n*dspc_box[2]);

    fclose(punte);

    // escribir topol.top
    punte = fopen("membrana_topol.top", "w");
    fprintf(punte,"#include \"dry_martini_v2.1.itp\"\n");
    fprintf(punte,"#include \"poro.itp\"\n");
    fprintf(punte,"#include \"dspc.itp\"\n");
    fprintf(punte, "\n");
    fprintf(punte,"[system]\nMembrana\n\n[molecules]\n");
    fprintf(punte,"PORO\t1\n");
    fprintf(punte,"DSPC\t%d",contador-1);
    fclose(punte);

	return 0;
}

