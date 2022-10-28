/* NOTA 1:
 En los comentarios siguientes omito grafias y signos de puntuacion
 especificos del castellano para evitar incompatibilidades.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define PI 3.1415926536
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)
#define VelocidadLuz_c 3e-2


struct CANAL_struct
{
    double radio;
    double longitudInterior;
    double longitudExterior;
    double *bond_fuerza;
    double bond_reposo;
    double *bending_fuerza;
    double bending_ang;
    int n;
    int M;
    int Nb;
    int NExterior;
    char Borde_poro[256];
    char Cuerpo_poro[256];
} CANAL;


struct VARIABLE_struct
{
    int SISTEMA_n;
    int N_simulaciones;
    int N_estadistica;
    double **ModoNormal;
} VARIABLE;


int Igual(double a, double b);

void SumaVector(double *a, double *b, double *result);

void RestaVector(double *a, double *b, double *result);

double ProductoEscalar(double *a, double *b);

double AnguloEntreVectores(double *a, double *b);

void ProductoVectorial(double *a, double *b, double *result);

void AsignaVector(double *a, double *result);

void Rotacion(double *EjeRotacion, double *Vector, double angulo);

double Distancia(double **Rx,double **Ry, double **Rz,int m1, int n1,int m2, int n2);

void MedVar(double *distribucion, int N, double* media, double* desviacion);

double ran1(long *idum);

void GenerarEstructuras(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt,int index_sim_bond,int index_sim_bending);

void ConstruyeCanal(double CANAL_radio, double CANAL_bond_reposo, double CANAL_bond_fuerza, double CANAL_bending_ang, double CANAL_bending_fuerza, int CANAL_Nb,int CANAL_NExterior, int CANAL_M, double CANAL_phi, double **CANAL_X, double **CANAL_V, int *CANAL_residuo, int *CANAL_chgrp, double *CANAL_box, double *CANAL_Angle, int index_sim_bond,int index_sim_bending,char* Borde_poro,char* Cuerpo_poro);

void ConstruyeDisco(double **Rx,double **Ry,double **Rz, double CANAL_bond_reposo,double CANAL_radio, double phi, int M, int i);

void MatrizDistancia(double **MD,double **Rx,double **Ry, double **Rz,int M, int Nb);

void MatrizVecinos(int **MV,int M, int Nb,double **MatrizDistancia,double CANAL_bond_reposo);

void GeneraBond(int **MatrizEnlaces,int **MVecinos, int M, int Nb);

void GeneraAngle(int ***MatrizAngulos,int **MVecinos, int M, int Nb);

void ConstruyeTopol_canal(double CANAL_bond_reposo,double CANAL_bond_fuerza, double CANAL_bending_ang, double CANAL_bending_fuerza,int M, int Nb,int NExterior,int **MEnlaces, int ***MAngulos, int index_sim_bond,int index_sim_bending, char* Borde_poro, char* Cuerpo_poro);

void MoverEstructura(double **estructura,int N,double theta, double phi, double Rx, double Ry, double Rz, long idum, double *Angle);

void Calcula_CM(double **estructura, int N, double *CentroMasa);

void ConstruyeIndices(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt, int index_sim_bond,int index_sim_bending);

void EscribeFicheroTopol(int index_sim_bond,int index_sim_bending);

void Genconf();


void LeerInput(struct CANAL_struct* CANAL_punt, struct VARIABLE_struct* VARIABLE_punt);

void LiberarMemoriaReservada(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt);


void GeneraMDP(struct VARIABLE_struct* VARIABLE_punt);


void MinimizaEstructura(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,int pinoffset,int ntmpi, int index_sim_bond,int index_sim_bending);

void Simula_Termalizacion(struct VARIABLE_struct* VARIABLE_punt,int pinoffset,int ntmpi,int index_sim_bond,int index_sim_bending);

void AnalisisDeModosNormales(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,int pinoffset,int ntmpi,int index_sim_bond,int index_sim_bending);

void Prepara_FicherosParaAnalisis(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,char* CarpetaSimulacion,int index_sim_bond,int index_sim_bending);

void ProcesoResultados(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt,int index_sim_bond,int index_sim_bending);

void AnalisisDeResultados(struct VARIABLE_struct* VARIABLE_punt, struct CANAL_struct* CANAL_punt);

int AnalisisDeAutovectores(struct VARIABLE_struct* VARIABLE_punt, struct CANAL_struct* CANAL_punt, int index_sim_bond,int index_sim_bending);



/**************************
 *                        *
 * FUNCIONES GENERALES    *
 *                        *
 **************************/
///FUNCIONES OPERATIVAS//////////////////////////////////////////////////////////////////
int Igual(double a, double b){  //Devuelve 1 si son iguales, 0 si son diferentes
    double epsilon=0.00000001;
    
    if (fabs(a-b)<epsilon)
        return 1;
    
    else
        return 0;
}


void SumaVector(double *a, double *b, double *result){
    int i;
    for(i=0;i<3;i++)
        result[i]=a[i]+b[i];
}

void RestaVector(double *a, double *b, double *result){
    int i;
    for(i=0;i<3;i++)
        result[i]=a[i]-b[i];
}

double ProductoEscalar(double *a, double *b){
    double result=0;
    int i;
    for(i=0;i<3;i++){
        result+=a[i]*b[i];
    }
    
    return result;
}

double AnguloEntreVectores(double *a, double *b){
    int i;
    double mod_a,mod_b,angulo;
    mod_a=sqrt(ProductoEscalar(a,a));
    mod_b=sqrt(ProductoEscalar(b,b));
    
    angulo=acos(ProductoEscalar(a,b)/(mod_a*mod_b));
    return angulo;
}

void ProductoVectorial(double *a, double *b, double *result){
    
    result[0]=a[1]*b[2]-a[2]*b[1];
    result[1]=a[2]*b[0]-a[0]*b[2];
    result[2]=a[0]*b[1]-a[1]*b[0];
    
}

void AsignaVector(double *a, double *result){
    result[0]=a[0];
    result[1]=a[1];
    result[2]=a[2];
}

void Rotacion(double *EjeRotacion, double *Vector, double angulo){
    int i,j;
    double MatrizRotacion[3][3];
    double Vector_auxiliar[3]={0,0,0};
    double mod_Eje;
    
    angulo=angulo*PI/180; ///Convertir angulos a radianes (para operar con math.h)
    mod_Eje=sqrt(ProductoEscalar(EjeRotacion, EjeRotacion)); ///Modulo del eje de rotacion. Debe ser 1, pero por si acaso lo normalizo antes de seguir.
    
    EjeRotacion[0]=EjeRotacion[0]/mod_Eje;
    EjeRotacion[1]=EjeRotacion[1]/mod_Eje;
    EjeRotacion[2]=EjeRotacion[2]/mod_Eje;
    
    MatrizRotacion[0][0]=cos(angulo)+EjeRotacion[0]*EjeRotacion[0]*(1-cos(angulo));
    MatrizRotacion[0][1]=EjeRotacion[0]*EjeRotacion[1]*(1-cos(angulo))-EjeRotacion[2]*sin(angulo);
    MatrizRotacion[0][2]=EjeRotacion[0]*EjeRotacion[2]*(1-cos(angulo))+EjeRotacion[1]*sin(angulo);
    MatrizRotacion[1][0]=EjeRotacion[0]*EjeRotacion[1]*(1-cos(angulo))+EjeRotacion[2]*sin(angulo);
    MatrizRotacion[1][1]=cos(angulo)+EjeRotacion[1]*EjeRotacion[1]*(1-cos(angulo));
    MatrizRotacion[1][2]=EjeRotacion[1]*EjeRotacion[2]*(1-cos(angulo))-EjeRotacion[0]*sin(angulo);
    MatrizRotacion[2][0]=EjeRotacion[0]*EjeRotacion[2]*(1-cos(angulo))-EjeRotacion[1]*sin(angulo);
    MatrizRotacion[2][1]=EjeRotacion[1]*EjeRotacion[2]*(1-cos(angulo))+EjeRotacion[0]*sin(angulo);
    MatrizRotacion[2][2]=cos(angulo)+EjeRotacion[2]*EjeRotacion[2]*(1-cos(angulo)); ///Matriz de rotacion construida.
    
    for(i=0;i<3;i++){
        for(j=0;j<3;j++)
            Vector_auxiliar[i]+=MatrizRotacion[i][j]*Vector[j];
    }
    
    Vector[0]=Vector_auxiliar[0];
    Vector[1]=Vector_auxiliar[1];
    Vector[2]=Vector_auxiliar[2];
    
}

double Distancia(double **Rx,double **Ry, double **Rz,int m1, int n1,int m2, int n2){
    return(sqrt((Rx[m1][n1]-Rx[m2][n2])*(Rx[m1][n1]-Rx[m2][n2])+(Ry[m1][n1]-Ry[m2][n2])*(Ry[m1][n1]-Ry[m2][n2])+(Rz[m1][n1]-Rz[m2][n2])*(Rz[m1][n1]-Rz[m2][n2])));
}

void MedVar(double *distribucion, int N, double* media, double* desviacion)
{
    int i;
    double promedio=0;
    double varianza=0;
    
    
    for(i=0;i<N;i++)
        promedio+=distribucion[i];
    
    promedio=promedio/N;
    
    for(i=0;i<N;i++)
    {
        varianza+=(distribucion[i]-promedio)*(distribucion[i]-promedio);
    }
    varianza=varianza/(N-1);
    
    *media=promedio;
    *desviacion=sqrt(varianza);
}

double ran1(long *idum){
    int j;
    long k;
    static long iy=0;
    static long iv[NTAB];
    double temp;
    if (*idum <= 0 || !iy) {
        if (-(*idum) < 1) *idum=1;
        else *idum = -(*idum);
        
        for (j=NTAB+7;j>=0;j--) {
            k=(*idum)/IQ;
            *idum=IA*(*idum-k*IQ)-IR*k;
            if (*idum < 0)
                *idum += IM;
            if (j < NTAB)
                iv[j] = *idum;
        }
        iy=iv[0];
    }
    k=(*idum)/IQ;
    *idum=IA*(*idum-k*IQ)-IR*k;
    if (*idum < 0) *idum += IM;
    j=iy/NDIV;
    iy=iv[j];
    iv[j] = *idum;
    if ((temp=AM*iy) > RNMX) return RNMX;
    else return temp;
}


///CONSTRUCCION DE ESTRUCTURAS////////////////////////////////////////
void GenerarEstructuras(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt,int index_sim_bond,int index_sim_bending)
{
    int i,j,k;
    char comando[512];
    
    double CANAL_phi;
    char CANAL_name[256];
    int *CANAL_residuo;
    int *CANAL_chgrp;
    int *CANAL_index;
    double **CANAL_X;
    double **CANAL_V;
    double CANAL_box[3];
    double CANAL_Angle[2];
    
    int AtomNumber=0;
    char Configuracion_name[256];
    long idum=-(long)time(NULL);
    FILE* SISTEMA_nuevo;
    FILE *pipe;
    
    
    
    sprintf(Configuracion_name,"SISTEMA_conf_%d_%d.gro",index_sim_bond,index_sim_bending);
    SISTEMA_nuevo=fopen(Configuracion_name,"w");
    
    
    ///-----------------CANAL---------------------------------------------
    CANAL_punt->Nb=(int)((CANAL_punt->longitudInterior+2*CANAL_punt->longitudExterior)/CANAL_punt->bond_reposo);
    CANAL_punt->NExterior=(int)(CANAL_punt->longitudExterior/CANAL_punt->bond_reposo);
    CANAL_phi=acos(1-CANAL_punt->bond_reposo*CANAL_punt->bond_reposo/(2*CANAL_punt->radio*CANAL_punt->radio));
    CANAL_punt->M=(int)(2*PI/CANAL_phi);
    CANAL_punt->n=CANAL_punt->Nb*CANAL_punt->M;
    
    
    
    
    
    CANAL_residuo=(int*)malloc(sizeof(int)*CANAL_punt->n);
    
    
    CANAL_chgrp=(int*)malloc(sizeof(int)*CANAL_punt->n);
    CANAL_index=(int*)malloc(sizeof(int)*CANAL_punt->n);
    
    CANAL_X=(double**)malloc(sizeof(double*)*CANAL_punt->n);
    for(i=0;i<CANAL_punt->n;i++)
        *(CANAL_X+i)=(double*)malloc(sizeof(double)*3);
    
    CANAL_V=(double**)malloc(sizeof(double*)*CANAL_punt->n);
    for(i=0;i<CANAL_punt->n;i++)
        *(CANAL_V+i)=(double*)malloc(sizeof(double)*3);
    
    
    printf("\n\nPORO\n-----------------\n");
    
    ConstruyeCanal(CANAL_punt->radio,CANAL_punt->bond_reposo,CANAL_punt->bond_fuerza[index_sim_bond],CANAL_punt->bending_ang,CANAL_punt->bending_fuerza[index_sim_bending],CANAL_punt->Nb,CANAL_punt->NExterior,CANAL_punt->M,CANAL_phi,CANAL_X,CANAL_V,CANAL_residuo,CANAL_chgrp,CANAL_box,CANAL_Angle,index_sim_bond,index_sim_bending,CANAL_punt->Borde_poro,CANAL_punt->Cuerpo_poro);
    
    MoverEstructura(CANAL_X,CANAL_punt->n,0, 0, -0.4, -0.4, 0, idum,CANAL_Angle);
    
    
    for(i=0;i<CANAL_punt->n;i++){
        AtomNumber++;
        CANAL_index[i]=AtomNumber;
    }
    
    
    
    ///ESCRITURA DEL SISTEMA:
    VARIABLE_punt->SISTEMA_n=CANAL_punt->n;
    
    fprintf(SISTEMA_nuevo,"%s\n","Sistema");
    
    fprintf(SISTEMA_nuevo,"%d\n",VARIABLE_punt->SISTEMA_n);
    
    for(i=0;i<CANAL_punt->n;i++){
        if(i/CANAL_punt->M<CANAL_punt->NExterior || i/CANAL_punt->M+1>CANAL_punt->Nb-CANAL_punt->NExterior)
            fprintf(SISTEMA_nuevo,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",CANAL_residuo[i],"CHNNL",'G',CANAL_index[i],CANAL_X[i][0],CANAL_X[i][1],CANAL_X[i][2],CANAL_V[i][0],CANAL_V[i][1],CANAL_V[i][2]);
        else
            fprintf(SISTEMA_nuevo,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",CANAL_residuo[i],"CHNNL",'C',CANAL_index[i],CANAL_X[i][0],CANAL_X[i][1],CANAL_X[i][2],CANAL_V[i][0],CANAL_V[i][1],CANAL_V[i][2]);
    }
    
    
    
    
    fprintf(SISTEMA_nuevo,"%lf\t%lf\t%lf\n",CANAL_box[0],CANAL_box[1],CANAL_box[2]);
    
    
    
    ///ESCRITURA DE INDICES
    ConstruyeIndices(CANAL_punt,VARIABLE_punt,index_sim_bond,index_sim_bending);
    
    
    
    ///LIBERACION DE MEMORIA
    free(CANAL_residuo);
    
    free(CANAL_chgrp);
    free(CANAL_index);
    
    for(i=0;i<CANAL_punt->n;i++)
        free(*(CANAL_X+i));
    free(CANAL_X);
    
    for(i=0;i<CANAL_punt->n;i++)
        free(*(CANAL_V+i));
    free(CANAL_V);
    
    
    fflush(SISTEMA_nuevo);
    fclose(SISTEMA_nuevo);
    
}


///Construccion del Canal

void ConstruyeCanal(double CANAL_radio, double CANAL_bond_reposo, double CANAL_bond_fuerza, double CANAL_bending_ang, double CANAL_bending_fuerza, int CANAL_Nb,int CANAL_NExterior, int CANAL_M, double CANAL_phi, double **CANAL_X, double **CANAL_V, int *CANAL_residuo, int *CANAL_chgrp, double *CANAL_box, double *CANAL_Angle, int index_sim_bond,int index_sim_bending,char* Borde_poro,char* Cuerpo_poro){
    
    ///Definicion de variables auxiliares (para la programacion)
    int n,m,i,j;
    
    ///Definicion del contenedor de posiciones del sistema
    double **Rx;
    double **Ry;
    double **Rz;
    double **MDistancia;   //Matriz de distancias.
    int **MVecinos;
    int **MEnlaces;
    int ***MAngulos;
    
    
    ///Definicion de parametros del canal******************
    printf("\nNOTA: se han introducido los parametros:\nCANAL_bond_reposo=%.2f\nCANAL_radio=%.1f\nLongitud=%d\n\n",CANAL_bond_reposo,CANAL_radio,CANAL_Nb);
    printf("Estos parametros suponen:\nAmplitud_angular=%.2f\tBeads/vuelta=%d\n\n",CANAL_phi,CANAL_M);
    CANAL_phi=2*PI/CANAL_M;
    CANAL_radio=CANAL_bond_reposo*sqrt(1/(2*(1-cos(CANAL_phi))));
    printf("Para ajustar el cilindro, se modifican los parametros:\nAmplitud angular'=%.2f\tCANAL_radio'=%.3f\n\n",CANAL_phi,CANAL_radio);
    
    
    
    ///Asigancion de memoria al sistema******************
    Rx=(double**)malloc(sizeof(double*)*CANAL_M);
    Ry=(double**)malloc(sizeof(double*)*CANAL_M);
    Rz=(double**)malloc(sizeof(double*)*CANAL_M);
    for(i=0;i<CANAL_M;i++){
        *(Rx+i)=(double*)malloc(sizeof(double)*CANAL_Nb);
        *(Ry+i)=(double*)malloc(sizeof(double)*CANAL_Nb);
        *(Rz+i)=(double*)malloc(sizeof(double)*CANAL_Nb);
    }
    
    MDistancia=(double**)malloc(sizeof(double*)*CANAL_M*CANAL_Nb);
    for(i=0;i<(CANAL_M*CANAL_Nb);i++)
        *(MDistancia+i)=(double*)malloc(sizeof(double)*CANAL_M*CANAL_Nb);
    
    MVecinos=(int**)malloc(sizeof(int*)*CANAL_M*CANAL_Nb);
    for(i=0;i<CANAL_M*CANAL_Nb;i++)
        *(MVecinos+i)=(int*)malloc(sizeof(int)*CANAL_M*CANAL_Nb);
    
    MEnlaces=(int**)malloc(sizeof(int*)*CANAL_M*CANAL_Nb);
    for(i=0;i<(CANAL_M*CANAL_Nb);i++)
        *(MEnlaces+i)=(int*)malloc(sizeof(int)*CANAL_M*CANAL_Nb);
    
    
    MAngulos=(int***)malloc(sizeof(int**)*CANAL_M*CANAL_Nb);
    for(i=0;i<(CANAL_M*CANAL_Nb);i++)
        *(MAngulos+i)=(int**)malloc(sizeof(int*)*CANAL_M*CANAL_Nb);
    
    for(i=0;i<CANAL_M*CANAL_Nb;i++){
        for(j=0;j<CANAL_M*CANAL_Nb;j++)
            *(*(MAngulos+i)+j)=(int*)malloc(sizeof(int)*CANAL_M*CANAL_Nb);
    }
    
    
    
    ///Construccion del sistema
    for(i=0;i<CANAL_Nb;i++)
        ConstruyeDisco(Rx,Ry,Rz,CANAL_bond_reposo,CANAL_radio,CANAL_phi,CANAL_M,i); //Con este bucle rellenamos las matrices R
    
    MatrizDistancia(MDistancia,Rx,Ry,Rz,CANAL_M,CANAL_Nb);
    
    MatrizVecinos(MVecinos,CANAL_M,CANAL_Nb,MDistancia,CANAL_bond_reposo);
    
    GeneraBond(MEnlaces,MVecinos,CANAL_M,CANAL_Nb);
    GeneraAngle(MAngulos,MVecinos,CANAL_M,CANAL_Nb);
    ConstruyeTopol_canal(CANAL_bond_reposo,CANAL_bond_fuerza,CANAL_bending_ang,CANAL_bending_fuerza,CANAL_M,CANAL_Nb,CANAL_NExterior,MEnlaces,MAngulos,index_sim_bond,index_sim_bending,Borde_poro,Cuerpo_poro);
    
    
    for(n=0;n<CANAL_Nb;n++){
        for(m=0;m<CANAL_M;m++){
            CANAL_X[n*CANAL_M+m][0]=Rx[m][n];
            CANAL_X[n*CANAL_M+m][1]=Ry[m][n];
            CANAL_X[n*CANAL_M+m][2]=Rz[m][n];
            
            CANAL_V[n*CANAL_M+m][0]=0.0;
            CANAL_V[n*CANAL_M+m][1]=0.0;
            CANAL_V[n*CANAL_M+m][2]=0.0;
            
            CANAL_residuo[n*CANAL_M+m]=n+1;
            
            CANAL_chgrp[n*CANAL_M+m]=n*CANAL_M+m+1;
        }
    }
    
    CANAL_box[0]=fabs(Rx[CANAL_M-1][CANAL_Nb-1]*1.1);
    CANAL_box[1]=fabs(Ry[CANAL_M-1][CANAL_Nb-1]*1.1);
    CANAL_box[2]=fabs(Rz[CANAL_M-1][CANAL_Nb-1]*1.1);
    
    CANAL_Angle[0]=0.5*PI;
    CANAL_Angle[1]=0;
    
    
    ///Liberar memoria
    for(i=0;i<CANAL_M;i++){
        free(*(Rx+i));
        free(*(Ry+i));
        free(*(Rz+i));
    }
    
    free(Rx);
    free(Ry);
    free(Rz);
    
    
    for(i=0;i<(CANAL_M*CANAL_Nb);i++)
        free(*(MDistancia+i));
    
    free(MDistancia);
    
    for(i=0;i<CANAL_M*CANAL_Nb;i++)
        free(*(MVecinos+i));
    free(MVecinos);
    
    for(i=0;i<CANAL_M*CANAL_Nb;i++)
        free(*(MEnlaces+i));
    free(MEnlaces);
    
    
    for(i=0;i<CANAL_M*CANAL_Nb;i++)
        for(j=0;j<CANAL_M*CANAL_Nb;j++)
            free(*(*(MAngulos+j)+i));
    
    for(i=0;i<CANAL_M*CANAL_Nb;i++)
        free(*(MAngulos+i));
    
    free(MAngulos);
    
}

void ConstruyeDisco(double **Rx,double **Ry,double **Rz, double CANAL_bond_reposo,double CANAL_radio, double phi, int M, int i){
    int m;
    double phi_inicial=0;
    
    if(i%2==0)
        phi_inicial=0;
    else
        phi_inicial=0.5*phi;
    
    
    for(m=0;m<M;m++){
        Rx[m][i]=CANAL_bond_reposo*i;
        Ry[m][i]=CANAL_radio*cos(m*phi+phi_inicial);
        Rz[m][i]=CANAL_radio*sin(m*phi+phi_inicial);
        
    }
    
}


void MatrizDistancia(double **MD,double **Rx,double **Ry, double **Rz,int M, int Nb){
    int i,j,m1,n1,m2,n2;
    
    for(i=0;i<(M*Nb);i++){
        for(j=0;j<(M*Nb);j++){
            m1=i%M;
            n1=i/M;
            m2=j%M;
            n2=j/M; //Estas cuatro lineas convierten el indice en los valores m,n
            
            MD[i][j]=Distancia(Rx,Ry,Rz,m1,n1,m2,n2);
        }
    }
}


void MatrizVecinos(int **MV,int M, int Nb,double **MatrizDistancia,double CANAL_bond_reposo){
    
    int i,j,m1,n1,m2,n2;
    float cota_dist=sqrt(2.0)*CANAL_bond_reposo;
    
    for(i=0;i<M*Nb;i++){
        for(j=0;j<M*Nb;j++)
            MV[i][j]=0;
    }
    
    for(i=0;i<M*Nb;i++){
        m1=i%M;
        n1=i/M;
        for(j=0;j<M*Nb;j++){
            m2=j%M;
            n2=j/M;
            if(i!=j && MatrizDistancia[i][j]<cota_dist)
                MV[i][j]=1;
            
            
            if(i!=j && n1==n2 && m1==0 && m2==M-2)
                MV[i][j]=2;
            
            if(i!=j && n1==n2 && m1==1 && m2==M-1)
                MV[i][j]=2;
            
            if(i!=j && n1==n2 && m1==m2+2)
                MV[i][j]=2;
            
            if(i!=j && m1==m2-1 && n1==n2-2)
                MV[i][j]=2;
            
            if(i!=j && m1==m2+1 && n1==n2-2)
                MV[i][j]=2;
            
            if(i!=j && m1==M-1 && m2==0 && n1==n2-2)
                MV[i][j]=2;
            
            if(i!=j && m1==0 && m2==M-1 && n1==n2-2)
                MV[i][j]=2;
            
            
        }
    }
    
}

void GeneraBond(int **MatrizEnlaces,int **MVecinos, int M, int Nb){
    int i,j;
    
    for(i=0;i<M*Nb;i++){
        for(j=0;j<M*Nb;j++)
            MatrizEnlaces[i][j]=0;
    }
    
    for(i=0;i<M*Nb;i++){
        for(j=0;j<M*Nb;j++){
            if(MatrizEnlaces[i][j]==0 && MVecinos[i][j]==1){
                MatrizEnlaces[i][j]=1;
                MatrizEnlaces[j][i]=-1; //De este modo marco el enlace inverso para no repetirlo despues!
            }
        }
    }
    
}

void GeneraAngle(int ***MatrizAngulos,int **MVecinos, int M, int Nb){
    int i,j,k;
    
    for(i=0;i<M*Nb;i++){
        for(j=0;j<M*Nb;j++){
            for(k=0;k<M*Nb;k++)
                MatrizAngulos[i][j][k]=0;
        }
    }
    
    for(i=0;i<M*Nb;i++){
        for(j=0;j<M*Nb;j++){
            for(k=0;k<M*Nb;k++){
                if(MatrizAngulos[i][j][k]==0 && MVecinos[i][j]==1 && MVecinos[j][k]==1 && MVecinos[i][k]==2){
                    MatrizAngulos[i][j][k]=1;
                    MatrizAngulos[i][k][j]=-1;
                    MatrizAngulos[j][i][k]=-1;
                    MatrizAngulos[k][i][j]=-1;
                    MatrizAngulos[j][k][i]=-1;
                    MatrizAngulos[k][j][i]=-1; //De este modo marco el enlace inverso para no repetirlo despues!
                }
            }
        }
    }
    
}

void ConstruyeTopol_canal(double CANAL_bond_reposo,double CANAL_bond_fuerza, double CANAL_bending_ang, double CANAL_bending_fuerza,int M, int Nb,int NExterior,int **MEnlaces, int ***MAngulos, int index_sim_bond,int index_sim_bending, char* Borde_poro, char* Cuerpo_poro){
    
    int i,j,k;
    FILE *topol;
    char CanalTopol_name[256];
    
    sprintf(CanalTopol_name,"SISTEMA_canal_%d_%d.itp",index_sim_bond,index_sim_bending);
    topol=fopen(CanalTopol_name,"w");
    
    fprintf(topol,";Canal proteico para simulacion de traslocacion de DNA. TFM Guillermo Diez.\n\n");
    fprintf(topol,"[moleculetype]\n;name\texclusion\nCANAL\t1\n\n");
    
    fprintf(topol,"[atoms]\n;nr\tattype\tresnr\tresidue\tatom\tcgnr\tcharge\tmass\n");
    for(i=0;i<Nb;i++){
        for(j=0;j<M;j++){
            if(i<NExterior || i+1>Nb-NExterior)
                fprintf(topol,"%d\t%s\t%d\tCHNNL\tQ0\t%d\t%.3f\t;\n",j+M*i+1,Borde_poro,i+1,j+M*i+1,0.0); //OJO! Donde pone "Q0" normalmente pone "P5"!!
            else
                fprintf(topol,"%d\t%s\t%d\tCHNNL\tC1\t%d\t%.3f\t;\n",j+M*i+1,Cuerpo_poro,i+1,j+M*i+1,0.0);
        }
    }
    
    
    fprintf(topol,"\n\n");
    fprintf(topol,"[bonds]\n;i\tj\tfunc\tb_0\tk_b\n");
    for(i=0;i<M*Nb;i++){
        for(j=0;j<M*Nb;j++){
            if(MEnlaces[i][j]==1)
                fprintf(topol,"%d\t%d\t%d\t%.3f\t%.1f\n",i+1,j+1,1,CANAL_bond_reposo,CANAL_bond_fuerza);
        }
    }
    
    fprintf(topol,"\n\n");
    fprintf(topol,"[angles]\n;i\tj\tk\tfunc\ttheta_0\tk_theta\n");
    for(i=0;i<M*Nb;i++){
        for(j=0;j<M*Nb;j++){
            for(k=0;k<M*Nb;k++){
                if(MAngulos[i][j][k]==1)
                    fprintf(topol,"%d\t%d\t%d\t%d\t%.3f\t%.1f\n",i+1,j+1,k+1,1,CANAL_bending_ang,CANAL_bending_fuerza);
            }
        }
    }
    
    fclose(topol);
}


///Desplazamiento de estructuras
void MoverEstructura(double **estructura,int N,double theta, double phi, double Rx, double Ry, double Rz, long idum, double *Angle){ ///Mueve una estructura a la orientacion (theta,phi), posicion R
    int i;
    double CentroMasa[3]={0,0,0};
    double delta_theta, delta_phi;
    double Eje_rot_phi[3]={0,0,1};
    double Eje_rot_theta[3];
    double NuevaDireccion[3]={sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta)};
    double VectorDirector[3];
    double Mod=0;
    
    Calcula_CM(estructura,N,CentroMasa);
    
    VectorDirector[0]=sin(Angle[0])*cos(Angle[1]);
    VectorDirector[1]=sin(Angle[0])*sin(Angle[1]);
    VectorDirector[2]=cos(Angle[0]);
    
    
    delta_theta=theta-180*Angle[0]/PI;
    delta_phi=phi-180*Angle[1]/PI;
    
    
    Rotacion(Eje_rot_phi,VectorDirector,delta_phi);
    for(i=0;i<N;i++)
        Rotacion(Eje_rot_phi,*(estructura+i),delta_phi);
    
    ProductoVectorial(VectorDirector,NuevaDireccion,Eje_rot_theta);
    Mod=sqrt(ProductoEscalar(Eje_rot_theta,Eje_rot_theta));
    if(Mod<0.00001 && Mod >-0.00001){
        Mod=1.0;
        Eje_rot_theta[0]=NuevaDireccion[0];
        Eje_rot_theta[1]=NuevaDireccion[1];
        Eje_rot_theta[2]=NuevaDireccion[2];
    }
    
    Eje_rot_theta[0]=Eje_rot_theta[0]/Mod;
    Eje_rot_theta[1]=Eje_rot_theta[1]/Mod;
    Eje_rot_theta[2]=Eje_rot_theta[2]/Mod;
    
    
    Rotacion(Eje_rot_theta,VectorDirector,delta_theta);
    for(i=0;i<N;i++)
        Rotacion(Eje_rot_theta,*(estructura+i),delta_theta);
    
    for(i=0;i<N;i++){
        estructura[i][0]=estructura[i][0]+Rx;
        estructura[i][1]=estructura[i][1]+Ry;
        estructura[i][2]=estructura[i][2]+Rz;
    }
    
    
    Angle[0]=PI/180*theta;
    Angle[1]=PI/180*phi;
    
} ///Coloca el CM de "estructura" en Rx,Ry,Rz; y la orienta en thtea, phi.

void Calcula_CM(double **estructura, int N, double *CentroMasa){
    int i;
    
    CentroMasa[0]=0;
    CentroMasa[1]=0;
    CentroMasa[2]=0;
    
    
    for(i=0;i<N;i++){
        CentroMasa[0]+=estructura[i][0];
        CentroMasa[1]+=estructura[i][1];
        CentroMasa[2]+=estructura[i][2];
    }
    
    CentroMasa[0]=CentroMasa[0]/((double)N);
    CentroMasa[1]=CentroMasa[1]/((double)N);
    CentroMasa[2]=CentroMasa[2]/((double)N); ///Aqui esta calculado el centro de masas.
    
    for(i=0;i<N;i++){
        estructura[i][0]=estructura[i][0]-CentroMasa[0];
        estructura[i][1]=estructura[i][1]-CentroMasa[1];
        estructura[i][2]=estructura[i][2]-CentroMasa[2];
    }
}


///Construccion de indices y topologia
void ConstruyeIndices(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt, int index_sim_bond,int index_sim_bending){
    int i;
    FILE *punte;
    char Indices_name[256];
    
    sprintf(Indices_name,"SISTEMA_index_%d_%d.ndx",index_sim_bond,index_sim_bending);
    punte=fopen(Indices_name,"w");
    
    
    fprintf(punte, "[CHNNL]\n");
    for(i=0;i<CANAL_punt->n;i++)
        fprintf(punte,"%d\n",i+1);
    
    
    fprintf(punte, "\n[SECO]\n");
    for(i=0;i<VARIABLE_punt->SISTEMA_n;i++)
        fprintf(punte,"%d\n",i+1);
    
    
    fclose(punte);
}

void EscribeFicheroTopol(int index_sim_bond,int index_sim_bending)
{
    FILE *topol;
    char Topol_name[256];
    
    sprintf(Topol_name,"topol_%d_%d.top",index_sim_bond,index_sim_bending);
    
    topol=fopen(Topol_name,"w");
    
    
    fprintf(topol,"#include %cdry_martini_v2.1.itp%c\n",34,34);
    
    fprintf(topol,"#include %cSISTEMA_canal_%d_%d.itp%c\n",34,index_sim_bond,index_sim_bending,34);
    
    fprintf(topol,"[system]\nCanalProteico\n\n[molecules]\n");
    
    fprintf(topol,"CANAL\t1\n");
    
    fclose(topol);
    
}


///CONFIGURACION DEL EXPERIMENTO//////////////////////////////
void Genconf()
{
    int i;
    FILE* genconf;
    FILE* Experimento;
    FILE* pipe;
    
    int N_simulaciones;
    int N_term;
    
    char Borde_poro[256];
    char Cuerpo_poro[256];
    char Acoplo_presion[256];
    char basura[256];
    
    double CANAL_bond_fuerza_ini;
    double CANAL_bond_fuerza_fin;
    double d_CANAL_bond_fuerza;
    
    double CANAL_bond_reposo;
    double CANAL_bending_ang;
    
    double CANAL_bending_fuerza_ini;
    double CANAL_bending_fuerza_fin;
    double d_CANAL_bending_fuerza;
    
    double CANAL_radio;
    double temperatura,tau_t;
    
    char flag;
    
    
    pipe=popen("bash","w");
    fprintf(pipe,"mkdir -p Input\n");
    fflush(pipe);
    pclose(pipe);
    
    
    
    Experimento=fopen("ModosNormales.txt","r");
    
    fscanf(Experimento,"%s%s%d",basura,basura,&N_simulaciones);
    
    fscanf(Experimento,"%s%s%lf",basura,basura,&CANAL_radio);
    
    fscanf(Experimento,"%s%s%s",basura,basura,Borde_poro);
    
    fscanf(Experimento,"%s%s%s",basura,basura,Cuerpo_poro);
    
    fscanf(Experimento,"%s%s%lf",basura,basura,&CANAL_bond_reposo);
    
    fscanf(Experimento,"%s%s%lf",basura,basura,&CANAL_bending_ang);
    
    fscanf(Experimento,"%s%s%lf",basura,basura,&CANAL_bond_fuerza_ini);
    
    fscanf(Experimento,"%s%s%lf",basura,basura,&CANAL_bond_fuerza_fin);
    
    fscanf(Experimento,"%s%s%lf",basura,basura,&CANAL_bending_fuerza_ini);
    
    fscanf(Experimento,"%s%s%lf",basura,basura,&CANAL_bending_fuerza_fin);
    
    
    
    
    fclose(Experimento);
    
    
    
    genconf=fopen("./Input/genconf.txt","w");
    
    
    if(N_simulaciones>1)
    {
        d_CANAL_bond_fuerza=exp(log(CANAL_bond_fuerza_fin/CANAL_bond_fuerza_ini)/(N_simulaciones-1));
        d_CANAL_bending_fuerza=exp(log(CANAL_bending_fuerza_fin/CANAL_bending_fuerza_ini)/(N_simulaciones-1));
    }
    else
    {
        d_CANAL_bond_fuerza=1;
        d_CANAL_bending_fuerza=1;
    }
    
    
    fprintf(genconf,"N_simulaciones\t=\t%d\n",N_simulaciones);
    
    fprintf(genconf,"BordePoro\t=\t%s\n",Borde_poro);
    fprintf(genconf,"CuerpoPoro\t=\t%s\n",Cuerpo_poro);
    
    
    fprintf(genconf,"CANAL_radio\t=\t%.2f\n",CANAL_radio);
    
    fprintf(genconf,"CANAL_longitud_Interior\t=\t5.40\n");
    
    fprintf(genconf,"CANAL_longitudExterior\t=\t%f\n",CANAL_bond_reposo*2);
    
    fprintf(genconf,"CANAL_bond_fuerza\t=");
    for(i=0;i<N_simulaciones;i++)
        fprintf(genconf,"\t%.1f",CANAL_bond_fuerza_ini*pow(d_CANAL_bond_fuerza,i));
    fprintf(genconf,"\n");
    
    fprintf(genconf,"CANAL_bond_reposo\t=\t%.2f\n",CANAL_bond_reposo);
    
    fprintf(genconf,"CANAL_bending\t=");
    for(i=0;i<N_simulaciones;i++)
        fprintf(genconf,"\t%.1f",CANAL_bending_fuerza_ini*pow(d_CANAL_bending_fuerza,i));
    fprintf(genconf,"\n");
    
    fprintf(genconf,"CANAL_bending_ang\t=\t%.1f\n",CANAL_bending_ang);
    
    
    fclose(genconf);
}


///ANALISIS/////////////////////////////////////////////////////////////////////////
double CalculaRadioGiro(int index_sim_bond,int index_sim_bending, char* Carpeta, char* GROfile)
{
    int i,j;
    int N_particulas=0;
    double** Coordenadas;
    double RadioVector_medio[3]={0,0,0};
    double Delta_RadioVector[3];
    double radio_giro=0;
    char coord_data_name[256];
    char index_name[256];
    char seeker[256];
    char basura[256];
    char flag0='a', flag1='b';
    int indice_index;
    int indice_particula;
    FILE *coord_data;
    FILE * index;
    
    sprintf(index_name,"SISTEMA_index_%d_%d.ndx",index_sim_bond,index_sim_bending);
    index=fopen(index_name,"r");
    
    for(i=0;i<1;i++)
    {
        fscanf(index,"%s",seeker);
        
        if(strcmp(seeker,"[FUERA]")==0)
        {
            fseek(index,1,SEEK_CUR);
            for(j=0;j<1;j++)
            {
                if(N_particulas==0)
                    fscanf(index,"%d",&indice_index);
                else
                    fscanf(index,"%s",seeker);
                
                N_particulas++;
                flag0=fgetc(index);
                flag1=fgetc(index);
                fseek(index,-2,SEEK_CUR);
                if(flag1=='\n' || (flag1==EOF))
                    break;
                j=-1;
            }
            break;
        }
        i=-1;
    } //Aqui ya he contado cuantas particulas contribuyen para calcular el radio de giro
    
    Coordenadas=(double**)malloc(sizeof(double*)*N_particulas);
    for(i=0;i<N_particulas;i++)
        Coordenadas[i]=(double*)malloc(sizeof(double)*3);
    
    fclose(index);
    
    sprintf(coord_data_name,"%s/%s_%d_%d.gro",Carpeta,GROfile,index_sim_bond,index_sim_bending);
    coord_data=fopen(coord_data_name,"r");
    fscanf(coord_data,"%s%s",basura,basura);
    
    for(i=0;i<1;i++)
    {
        fscanf(coord_data,"%s%s%d%s%s%s%s%s%s",basura,basura,&indice_particula,basura,basura,basura,basura,basura,basura);
        if(indice_particula==indice_index-1)
        {
            for(j=0;j<N_particulas;j++)
                fscanf(coord_data,"%s%s%s%lf%lf%lf%s%s%s",basura,basura,basura,&Coordenadas[j][0],&Coordenadas[j][1],&Coordenadas[j][2],basura,basura,basura);
            break;
        }
        
        i=-1;
    } //Aqui ya estan almacenadas las coordenas para calcular el radio de giro
    
    
    for(i=0;i<N_particulas;i++)
    {
        RadioVector_medio[0]+=Coordenadas[i][0];
        RadioVector_medio[1]+=Coordenadas[i][1];
        RadioVector_medio[2]+=Coordenadas[i][2];
    }
    RadioVector_medio[0]=RadioVector_medio[0]/N_particulas;
    RadioVector_medio[1]=RadioVector_medio[1]/N_particulas;
    RadioVector_medio[2]=RadioVector_medio[2]/N_particulas;
    
    for(i=0;i<N_particulas;i++)
    {
        Delta_RadioVector[0]=Coordenadas[i][0]-RadioVector_medio[0];
        Delta_RadioVector[1]=Coordenadas[i][1]-RadioVector_medio[1];
        Delta_RadioVector[2]=Coordenadas[i][2]-RadioVector_medio[2];
        
        radio_giro+=ProductoEscalar(Delta_RadioVector,Delta_RadioVector);
    }
    radio_giro=sqrt(radio_giro/N_particulas);
    
    
    for(i=0;i<N_particulas;i++)
        free(Coordenadas[i]);
    free(Coordenadas);
    fclose(coord_data);
    
    return radio_giro;
}



void AnalisisDeModosNormales(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,int pinoffset,int ntmpi,int index_sim_bond,int index_sim_bending)
{
    int i;
    
    char comando[512];
    char CarpetaObjetivo[256];
    FILE *pipe;
    
    
    
    
    sprintf(CarpetaObjetivo,"Kb=%.2f_b0=%.2f_Ktheta=%.2f_theta0=%.2f__%d_%d",CANAL_punt->bond_fuerza[index_sim_bond],CANAL_punt->bond_reposo,CANAL_punt->bending_fuerza[index_sim_bending],CANAL_punt->bending_ang,index_sim_bond,index_sim_bending);
    
    
    if(fork()==0)
    {
        sprintf(comando,"mkdir %s\n",CarpetaObjetivo);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    
    wait(NULL);
    
    
    if(fork()==0)
    {
        sprintf(comando,"cp ./Input/martini_mn.mdp ./Input/dry_martini_v2.1.itp SISTEMA_canal_%d_%d.itp SISTEMA_index_%d_%d.ndx topol_%d_%d.top AnalisisMN_min_%d_%d.gro traj_min_%d_%d.trr %s\n",index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,CarpetaObjetivo);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    
    wait(NULL);
    
    
    
    if(fork()==0)
    {
        sprintf(comando,"cd ./%s\ngmx_d grompp -f martini_mn.mdp -c AnalisisMN_min_%d_%d.gro -t traj_min_%d_%d.trr -p topol_%d_%d.top -o topol_%d_%d.tpr -n SISTEMA_index_%d_%d.ndx -maxwarn 3\n",CarpetaObjetivo,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    
    wait(NULL);
    
    
    if(fork()==0)
    {
        sprintf(comando,"cd ./%s\ngmx_d mdrun -s topol_%d_%d.tpr -c AnalisisMN_%d_%d.gro -mtx hess_%d_%d.mtx -rdd 2 -v -ntmpi %d -pin on -pinoffset %d -pinstride 1\n",CarpetaObjetivo,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,ntmpi,pinoffset);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    
    wait(NULL);
    
    
    if(fork()==0)
    {
        sprintf(comando,"cd ./%s\ngmx_d nmeig -f hess_%d_%d.mtx -s topol_%d_%d.tpr -of eigenfreq_%d_%d.xvg -ol eigenval_%d_%d.xvg -os spectrum_%d_%d.xvg -v eigenvec_%d_%d.trr -first 1 -last %d\n",CarpetaObjetivo,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,3*CANAL_punt->n);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    
    wait(NULL);
    
    /*if(fork()==0)
    {
        sprintf(comando,"cd ./%s\ngmx_d anaeig -v eigenvec_%d_%d.trr -s topol_%d_%d.tpr -n SISTEMA_index_%d_%d.ndx -eig eigenval_%d_%d.xvg -comp eigcomp_%d_%d.xvg -first 1 -last -1 -xvg none\n",CarpetaObjetivo,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);*/
    
    

    
    
    for(i=7;i<=3*VARIABLE_punt->SISTEMA_n;i++)
    {
        if(fork()==0)
        {
            sprintf(comando,"cd ./%s\ngmx_d nmtraj -s topol_%d_%d.tpr -v eigenvec_%d_%d.trr -o autovector_%d__%d_%d.gro -eignr %d -temp 50000 -phases 90.0 -nframes 1 \n",CarpetaObjetivo,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,i,index_sim_bond,index_sim_bending,i);
            
            pipe=popen(comando,"w");
            pclose(pipe);
            
            exit(0);
        }
        wait(NULL);
        
        if(fork()==0)
        {
            sprintf(comando,"cp ./%s/autovector_%d__%d_%d.gro ./\n",CarpetaObjetivo,i,index_sim_bond,index_sim_bending);
            
            pipe=popen(comando,"w");
            pclose(pipe);
            
            exit(0);
        }
        wait(NULL);
    }
    
    if(fork()==0)
    {
        sprintf(comando,"cp ./%s/eigenfreq_%d_%d.xvg ./\n",CarpetaObjetivo,index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    
    
    

    
    
        
        if(fork()==0)
        {
            sprintf(comando,"rm -r Kb=%.2f_b0=%.2f_Ktheta=%.2f_theta0=%.2f__%d_%d\n",CANAL_punt->bond_fuerza[index_sim_bond],CANAL_punt->bond_reposo,CANAL_punt->bending_fuerza[index_sim_bending],CANAL_punt->bending_ang,index_sim_bond,index_sim_bending);
            
            pipe=popen(comando,"w");
            pclose(pipe);
            
            exit(0);
        }
        
        wait(NULL);
        
    
    
    
    
    if(fork()==0)
    {
        sprintf(comando,"rm SISTEMA_canal_%d_%d.itp SISTEMA_index_%d_%d.ndx topol_%d_%d.top AnalisisMN_min_%d_%d.gro traj_min_%d_%d.trr\n",index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    
    wait(NULL);
    
    
}




///LEER INPUT//////////////////////////////////////////////////////////////////////////////////////////
void LeerInput(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt)
{
    int i,j;
    FILE *input;
    char basura[256];
    
    input=fopen("./Input/genconf.txt","r");
    
    fscanf(input,"%s%s%d\n",basura,basura,&VARIABLE_punt->N_simulaciones);
    
    CANAL_punt->bond_fuerza=(double*)malloc(sizeof(double)*(VARIABLE_punt->N_simulaciones));
    
    CANAL_punt->bending_fuerza=(double*)malloc(sizeof(double)*(VARIABLE_punt->N_simulaciones));
    
    fscanf(input,"%s\t%s\t%s\n",basura,basura,CANAL_punt->Borde_poro);
    fscanf(input,"%s\t%s\t%s\n",basura,basura,CANAL_punt->Cuerpo_poro);
    
    
    fscanf(input,"%s%s%lf",basura,basura,&CANAL_punt->radio);
    
    fscanf(input,"%s%s%lf",basura,basura,&CANAL_punt->longitudInterior);
    
    
    fscanf(input,"%s%s%lf",basura,basura,&CANAL_punt->longitudExterior);
    
    
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        fscanf(input,"%lf",&CANAL_punt->bond_fuerza[i]);
    
    fscanf(input,"%s%s%lf",basura,basura,&CANAL_punt->bond_reposo);
    
    
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        fscanf(input,"%lf",&CANAL_punt->bending_fuerza[i]);
    
    
    fscanf(input,"%s%s%lf",basura,basura,&CANAL_punt->bending_ang);
    
    
    fclose(input);
    
}




///LIBERACION DE MEMORIA Y LIMPIEZA/////////////////////////////////////
void LiberarMemoriaReservada(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt)
{
    int i;
    free(CANAL_punt->bond_fuerza);
    
    free(CANAL_punt->bending_fuerza);
}





/**************************
 *                        *
 * FUNCIONES ESPECIFICAS  *
 *                        *
 **************************/

void GeneraMDP(struct VARIABLE_struct* VARIABLE_punt)
{
    FILE *min,*sim,*dry_martini;
    
    
    
    min=fopen("Input/minim.mdp","w");
    sim=fopen("Input/martini_mn.mdp","w");
    dry_martini=fopen("Input/dry_martini_v2.1.itp","w");
    
    fprintf(min,"integrator\t= cg\n\nemstep\t\t = 0.001\nemtol\t\t= 0.01\nnsteps\t\t= 500000\ncomm-mode\t= Linear\ncomm-grps\t= \nnstcgsteep\t\t= 1000\n\nnstlist\t\t = 1\nnstxout\t\t = 0\nnstvout\t\t = 0\nnstfout\t\t = 0\nnstlog\t\t = 0\nnstenergy\t\t = 0\nnstxout-compressed\t\t = 0\ncutoff-scheme\t\t = verlet \nverlet-buffer-drift\t= 0.005\nns_type\t\t = grid\n\ncoulombtype\t\t = reaction-field\n\nrcoulomb\t\t = 1.2\n\nvdw-type\t\t = cutoff\nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t = 1.2\n\npbc\t\t = xyz\n");
    
    
    
    fprintf(sim,"\ntitle\t\t = Analisis_Modos_Normales\n\n\n\n\nintegrator\t\t = nm\n\ncutoff-scheme\t\t = Verlet\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = cutoff \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t = 1.1\t\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\n");
    
    
    
    fprintf(dry_martini,"[ defaults ]\n  1      2         no        1.0     1.0\n\n#define dm_rrII        0.4700 4.5000   # regular/regular\n#define dm_rrIII       0.4700 4.0000\n#define dm_rrIV        0.4700 3.5000\n#define dm_rrV         0.4700 3.1000\n#define dm_rrVI        0.4700 2.7000\n#define dm_rrVII       0.4700 2.3000\n#define dm_rrVIII      0.4700 2.0000\n#define dm_rrIX        0.6200 2.0000\n#define dm_rrX         0.4700 1.5000\n#define dm_rrXI        0.4700 1.0000\n#define dm_rrXII       0.4700 0.5000\n#define dm_rrVI_qq     0.6000 2.7000   # regular/regular, charge/charge interactions\n#define dm_rrVII_qq    0.6000 2.3000\n#define dm_rrVIII_qq   0.6000 2.0000\n#define dm_ssII        0.4300 3.4000   # small/small\n#define dm_ssIII       0.4300 3.0000\n#define dm_ssIV        0.4300 2.6000\n#define dm_ssV         0.4300 2.3000\n#define dm_ssVI        0.4300 2.0000\n#define dm_ssVII       0.4300 1.7000\n#define dm_ssVIII      0.4300 1.5000\n#define dm_ssIX        0.6200 2.0000\n#define dm_ssX         0.4300 1.1250\n#define dm_ssXI        0.4300 0.7500\n#define dm_ssXII       0.4300 0.3750\n#define dm_ssVI_qq     0.5500 2.0000   # small/small, charge/charge interactions\n#define dm_ssVII_qq    0.5500 1.7000\n#define dm_ssVIII_qq   0.5500 1.5000\n#define dm_rsII        0.4700 4.5000   # regular/small\n#define dm_rsIII       0.4700 4.0000\n#define dm_rsIV        0.4700 3.5000\n#define dm_rsV         0.4700 3.1000\n#define dm_rsVI        0.4700 2.7000\n#define dm_rsVII       0.4700 2.3000\n#define dm_rsVIII      0.4700 2.0000\n#define dm_rsIX        0.6200 2.0000\n#define dm_rsX         0.4700 1.5000\n#define dm_rsXI        0.4700 1.0000\n#define dm_rsXII       0.4700 0.5000\n#define dm_rsVI_qq     0.6000 2.7000   # regular/small, charge/charge interactions\n#define dm_rsVII_qq    0.6000 2.3000\n#define dm_rsVIII_qq   0.6000 2.0000\n\n[ atomtypes ]\n  Qda     72.0 0.0  A     0.0 0.0   # regular beads\n  Qd      72.0 0.0  A     0.0 0.0\n  Qa      72.0 0.0  A     0.0 0.0\n  Q0      72.0 0.0  A     0.0 0.0\n  P5      72.0 0.0  A     0.0 0.0\n  P4      72.0 0.0  A     0.0 0.0\n  P3      72.0 0.0  A     0.0 0.0\n  P2      72.0 0.0  A     0.0 0.0\n  P1      72.0 0.0  A     0.0 0.0\n  Nda     72.0 0.0  A     0.0 0.0\n  Nd      72.0 0.0  A     0.0 0.0\n  Na      72.0 0.0  A     0.0 0.0\n  N0      72.0 0.0  A     0.0 0.0\n  C5      72.0 0.0  A     0.0 0.0\n  C4      72.0 0.0  A     0.0 0.0\n  C3      72.0 0.0  A     0.0 0.0\n  C2      72.0 0.0  A     0.0 0.0\n  C1      72.0 0.0  A     0.0 0.0\n  SQda    45.0 0.0  A     0.0 0.0   # small beads\n  SQd     45.0 0.0  A     0.0 0.0\n  SQa     45.0 0.0  A     0.0 0.0\n  SQ0     45.0 0.0  A     0.0 0.0\n  SP5     45.0 0.0  A     0.0 0.0\n  SP4     45.0 0.0  A     0.0 0.0\n  SP3     45.0 0.0  A     0.0 0.0\n  SP2     45.0 0.0  A     0.0 0.0\n  SP1     45.0 0.0  A     0.0 0.0\n  SNda    45.0 0.0  A     0.0 0.0\n  SNd     45.0 0.0  A     0.0 0.0\n  SNa     45.0 0.0  A     0.0 0.0\n  SN0     45.0 0.0  A     0.0 0.0\n  SC5     45.0 0.0  A     0.0 0.0\n  SC4     45.0 0.0  A     0.0 0.0\n  SC3     45.0 0.0  A     0.0 0.0\n  SC2     45.0 0.0  A     0.0 0.0\n  SC1     45.0 0.0  A     0.0 0.0\n  SP1c    45.0 0.0  A     0.0 0.0   # fix for dry cholesterol\n\n\n[ nonbond_params ]\n    Qda Qda    1    dm_rrVI_qq\n    Qda Qd     1    dm_rrVI_qq\n    Qda Qa     1    dm_rrVI_qq\n    Qda Q0     1    dm_rrVIII_qq\n    Qda P5     1    dm_rrXII\n    Qda P4     1    dm_rrXII\n    Qda P3     1    dm_rrXII\n    Qda P2     1    dm_rrXII\n    Qda P1     1    dm_rrXII\n    Qda Nda    1    dm_rrXII\n    Qda Nd     1    dm_rrXII\n    Qda Na     1    dm_rrXII\n    Qda N0     1    dm_rrXII\n    Qda C5     1    dm_rrXII\n    Qda C4     1    dm_rrXII\n    Qda C3     1    dm_rrXII\n    Qda C2     1    dm_rrIX\n    Qda C1     1    dm_rrIX\n    Qda SQda   1    dm_rsVI_qq\n    Qda SQd    1    dm_rsVI_qq\n    Qda SQa    1    dm_rsVI_qq\n    Qda SQ0    1    dm_rsVIII_qq\n    Qda SP5    1    dm_rsXII\n    Qda SP4    1    dm_rsXII\n    Qda SP3    1    dm_rsXII\n    Qda SP2    1    dm_rsXII\n    Qda SP1    1    dm_rsXII\n    Qda SNda   1    dm_rsXII\n    Qda SNd    1    dm_rsXII\n    Qda SNa    1    dm_rsXII\n    Qda SN0    1    dm_rsXII\n    Qda SC5    1    dm_rsXII\n    Qda SC4    1    dm_rsXII\n    Qda SC3    1    dm_rsXII\n    Qda SC2    1    dm_rsIX\n    Qda SC1    1    dm_rsIX\n    Qda SP1c   1    dm_rsVI\n     Qd Qd     1    dm_rrVII_qq\n     Qd Qa     1    dm_rrVI_qq\n     Qd Q0     1    dm_rrVIII_qq\n     Qd P5     1    dm_rrXII\n     Qd P4     1    dm_rrXII\n     Qd P3     1    dm_rrXII\n     Qd P2     1    dm_rrXII\n     Qd P1     1    dm_rrXII\n     Qd Nda    1    dm_rrXII\n     Qd Nd     1    dm_rrXII\n     Qd Na     1    dm_rrXII\n     Qd N0     1    dm_rrXII\n     Qd C5     1    dm_rrXII\n     Qd C4     1    dm_rrXII\n     Qd C3     1    dm_rrXII\n     Qd C2     1    dm_rrIX\n     Qd C1     1    dm_rrIX\n     Qd SQda   1    dm_rsVI_qq\n     Qd SQd    1    dm_rsVII_qq\n     Qd SQa    1    dm_rsVI_qq\n     Qd SQ0    1    dm_rsVIII_qq\n     Qd SP5    1    dm_rsXII\n     Qd SP4    1    dm_rsXII\n     Qd SP3    1    dm_rsXII\n     Qd SP2    1    dm_rsXII\n     Qd SP1    1    dm_rsXII\n     Qd SNda   1    dm_rsXII\n     Qd SNd    1    dm_rsXII\n     Qd SNa    1    dm_rsXII\n     Qd SN0    1    dm_rsXII\n     Qd SC5    1    dm_rsXII\n     Qd SC4    1    dm_rsXII\n     Qd SC3    1    dm_rsXII\n     Qd SC2    1    dm_rsIX\n     Qd SC1    1    dm_rsIX\n     Qd SP1c   1    dm_rsVI\n     Qa Qa     1    dm_rrVII_qq\n     Qa Q0     1    dm_rrVIII_qq\n     Qa P5     1    dm_rrXII\n     Qa P4     1    dm_rrXII\n     Qa P3     1    dm_rrXII\n     Qa P2     1    dm_rrXII\n     Qa P1     1    dm_rrXII\n     Qa Nda    1    dm_rrXII\n     Qa Nd     1    dm_rrXII\n     Qa Na     1    dm_rrXII\n     Qa N0     1    dm_rrXII\n     Qa C5     1    dm_rrXII\n     Qa C4     1    dm_rrXII\n     Qa C3     1    dm_rrXII\n     Qa C2     1    dm_rrIX\n     Qa C1     1    dm_rrIX\n     Qa SQda   1    dm_rsVI_qq\n     Qa SQd    1    dm_rsVI_qq\n     Qa SQa    1    dm_rsVII_qq\n     Qa SQ0    1    dm_rsVIII_qq\n     Qa SP5    1    dm_rsXII\n     Qa SP4    1    dm_rsXII\n     Qa SP3    1    dm_rsXII\n     Qa SP2    1    dm_rsXII\n     Qa SP1    1    dm_rsXII\n     Qa SNda   1    dm_rsXII\n     Qa SNd    1    dm_rsXII\n     Qa SNa    1    dm_rsXII\n     Qa SN0    1    dm_rsXII\n     Qa SC5    1    dm_rsXII\n     Qa SC4    1    dm_rsXII\n     Qa SC3    1    dm_rsXII\n     Qa SC2    1    dm_rsIX\n     Qa SC1    1    dm_rsIX\n     Qa SP1c   1    dm_rsVI\n     Q0 Q0     1    dm_rrVIII_qq\n     Q0 P5     1    dm_rrXII\n     Q0 P4     1    dm_rrXII\n     Q0 P3     1    dm_rrXII\n     Q0 P2     1    dm_rrXII\n     Q0 P1     1    dm_rrXII\n     Q0 Nda    1    dm_rrXII\n     Q0 Nd     1    dm_rrXII\n     Q0 Na     1    dm_rrXII\n     Q0 N0     1    dm_rrXII\n     Q0 C5     1    dm_rrXII\n     Q0 C4     1    dm_rrXII\n     Q0 C3     1    dm_rrXII\n     Q0 C2     1    dm_rrIX\n     Q0 C1     1    dm_rrIX\n     Q0 SQda   1    dm_rsVIII_qq\n     Q0 SQd    1    dm_rsVIII_qq\n     Q0 SQa    1    dm_rsVIII_qq\n     Q0 SQ0    1    dm_rsVIII_qq\n     Q0 SP5    1    dm_rsXII\n     Q0 SP4    1    dm_rsXII\n     Q0 SP3    1    dm_rsXII\n     Q0 SP2    1    dm_rsXII\n     Q0 SP1    1    dm_rsXII\n     Q0 SNda   1    dm_rsXII\n     Q0 SNd    1    dm_rsXII\n     Q0 SNa    1    dm_rsXII\n     Q0 SN0    1    dm_rsXII\n     Q0 SC5    1    dm_rsXII\n     Q0 SC4    1    dm_rsXII\n     Q0 SC3    1    dm_rsXII\n     Q0 SC2    1    dm_rsIX\n     Q0 SC1    1    dm_rsIX\n     Q0 SP1c   1    dm_rsVI\n     P5 P5     1    dm_rrV\n     P5 P4     1    dm_rrV\n     P5 P3     1    dm_rrV\n     P5 P2     1    dm_rrV\n     P5 P1     1    dm_rrX\n     P5 Nda    1    dm_rrXII\n     P5 Nd     1    dm_rrXII\n     P5 Na     1    dm_rrXII\n     P5 N0     1    dm_rrXII\n     P5 C5     1    dm_rrXII\n     P5 C4     1    dm_rrXII\n     P5 C3     1    dm_rrXII\n     P5 C2     1    dm_rrXII\n     P5 C1     1    dm_rrXII\n     P5 SQda   1    dm_rsXII\n     P5 SQd    1    dm_rsXII\n     P5 SQa    1    dm_rsXII\n     P5 SQ0    1    dm_rsXII\n     P5 SP5    1    dm_rsV\n     P5 SP4    1    dm_rsV\n     P5 SP3    1    dm_rsV\n     P5 SP2    1    dm_rsV\n     P5 SP1    1    dm_rsX\n     P5 SNda   1    dm_rsXII\n     P5 SNd    1    dm_rsXII\n     P5 SNa    1    dm_rsXII\n     P5 SN0    1    dm_rsXII\n     P5 SC5    1    dm_rsXII\n     P5 SC4    1    dm_rsXII\n     P5 SC3    1    dm_rsXII\n     P5 SC2    1    dm_rsXII\n     P5 SC1    1    dm_rsXII\n     P5 SP1c   1    dm_rsX\n     P4 P4     1    dm_rrVI\n     P4 P3     1    dm_rrVI\n     P4 P2     1    dm_rrVII\n     P4 P1     1    dm_rrX\n     P4 Nda    1    dm_rrXI\n     P4 Nd     1    dm_rrXI\n     P4 Na     1    dm_rrXI\n     P4 N0     1    dm_rrXI\n     P4 C5     1    dm_rrXI\n     P4 C4     1    dm_rrXI\n     P4 C3     1    dm_rrXI\n     P4 C2     1    dm_rrXII\n     P4 C1     1    dm_rrXII\n     P4 SQda   1    dm_rsXII\n     P4 SQd    1    dm_rsXII\n     P4 SQa    1    dm_rsXII\n     P4 SQ0    1    dm_rsXII\n     P4 SP5    1    dm_rsV\n     P4 SP4    1    dm_rsVI\n     P4 SP3    1    dm_rsVI\n     P4 SP2    1    dm_rsVII\n     P4 SP1    1    dm_rsX\n     P4 SNda   1    dm_rsXI\n     P4 SNd    1    dm_rsXI\n     P4 SNa    1    dm_rsXI\n     P4 SN0    1    dm_rsXI\n     P4 SC5    1    dm_rsXI\n     P4 SC4    1    dm_rsXI\n     P4 SC3    1    dm_rsXI\n     P4 SC2    1    dm_rsXII\n     P4 SC1    1    dm_rsXII\n     P4 SP1c   1    dm_rsX\n     P3 P3     1    dm_rrVI\n     P3 P2     1    dm_rrVII\n     P3 P1     1    dm_rrX\n     P3 Nda    1    dm_rrXI\n     P3 Nd     1    dm_rrXI\n     P3 Na     1    dm_rrXI\n     P3 N0     1    dm_rrX\n     P3 C5     1    dm_rrX\n     P3 C4     1    dm_rrX\n     P3 C3     1    dm_rrX\n     P3 C2     1    dm_rrX\n     P3 C1     1    dm_rrXI\n     P3 SQda   1    dm_rsXII\n     P3 SQd    1    dm_rsXII\n     P3 SQa    1    dm_rsXII\n     P3 SQ0    1    dm_rsXII\n     P3 SP5    1    dm_rsV\n     P3 SP4    1    dm_rsVI\n     P3 SP3    1    dm_rsVI\n     P3 SP2    1    dm_rsVII\n     P3 SP1    1    dm_rsX\n     P3 SNda   1    dm_rsXI\n     P3 SNd    1    dm_rsXI\n     P3 SNa    1    dm_rsXI\n     P3 SN0    1    dm_rsX\n     P3 SC5    1    dm_rsX\n     P3 SC4    1    dm_rsX\n     P3 SC3    1    dm_rsX\n     P3 SC2    1    dm_rsX\n     P3 SC1    1    dm_rsXI\n     P3 SP1c   1    dm_rsX\n     P2 P2     1    dm_rrVII\n     P2 P1     1    dm_rrVIII\n     P2 Nda    1    dm_rrXI\n     P2 Nd     1    dm_rrXI\n     P2 Na     1    dm_rrXI\n     P2 N0     1    dm_rrIX\n     P2 C5     1    dm_rrVI\n     P2 C4     1    dm_rrVI\n     P2 C3     1    dm_rrVII\n     P2 C2     1    dm_rrVIII\n     P2 C1     1    dm_rrX\n     P2 SQda   1    dm_rsXII\n     P2 SQd    1    dm_rsXII\n     P2 SQa    1    dm_rsXII\n     P2 SQ0    1    dm_rsXII\n     P2 SP5    1    dm_rsV\n     P2 SP4    1    dm_rsVII\n     P2 SP3    1    dm_rsVII\n     P2 SP2    1    dm_rsVII\n     P2 SP1    1    dm_rsVIII\n     P2 SNda   1    dm_rsXI\n     P2 SNd    1    dm_rsXI\n     P2 SNa    1    dm_rsXI\n     P2 SN0    1    dm_rsIX\n     P2 SC5    1    dm_rsVI\n     P2 SC4    1    dm_rsVI\n     P2 SC3    1    dm_rsVII\n     P2 SC2    1    dm_rsVIII\n     P2 SC1    1    dm_rsX\n     P2 SP1c   1    dm_rsVIII\n     P1 P1     1    dm_rrVII\n     P1 Nda    1    dm_rrVII\n     P1 Nd     1    dm_rrVII\n     P1 Na     1    dm_rrVII\n     P1 N0     1    dm_rrVII\n     P1 C5     1    dm_rrVI\n     P1 C4     1    dm_rrVI\n     P1 C3     1    dm_rrVI\n     P1 C2     1    dm_rrVI\n     P1 C1     1    dm_rrVII\n     P1 SQda   1    dm_rsXII\n     P1 SQd    1    dm_rsXII\n     P1 SQa    1    dm_rsXII\n     P1 SQ0    1    dm_rsXII\n     P1 SP5    1    dm_rsX\n     P1 SP4    1    dm_rsX\n     P1 SP3    1    dm_rsX\n     P1 SP2    1    dm_rsVIII\n     P1 SP1    1    dm_rsVII\n     P1 SNda   1    dm_rsVII\n     P1 SNd    1    dm_rsVII\n     P1 SNa    1    dm_rsVII\n     P1 SN0    1    dm_rsVII\n     P1 SC5    1    dm_rsVI\n     P1 SC4    1    dm_rsVI\n     P1 SC3    1    dm_rsVI\n     P1 SC2    1    dm_rsVI\n     P1 SC1    1    dm_rsVII\n     P1 SP1c   1    dm_rsVII\n    Nda Nda    1    dm_rrVI\n    Nda Nd     1    dm_rrVI\n    Nda Na     1    dm_rrVI\n    Nda N0     1    dm_rrVII\n    Nda C5     1    dm_rrVI\n    Nda C4     1    dm_rrVI\n    Nda C3     1    dm_rrVI\n    Nda C2     1    dm_rrVI\n    Nda C1     1    dm_rrVI\n    Nda SQda   1    dm_rsXII\n    Nda SQd    1    dm_rsXII\n    Nda SQa    1    dm_rsXII\n    Nda SQ0    1    dm_rsXII\n    Nda SP5    1    dm_rsXII\n    Nda SP4    1    dm_rsXI\n    Nda SP3    1    dm_rsXI\n    Nda SP2    1    dm_rsXI\n    Nda SP1    1    dm_rsVII\n    Nda SNda   1    dm_rsVI\n    Nda SNd    1    dm_rsVI\n    Nda SNa    1    dm_rsVI\n    Nda SN0    1    dm_rsVII\n    Nda SC5    1    dm_rsVI\n    Nda SC4    1    dm_rsVI\n    Nda SC3    1    dm_rsVI\n    Nda SC2    1    dm_rsVI\n    Nda SC1    1    dm_rsVI\n    Nda SP1c   1    dm_rsVII\n     Nd Nd     1    dm_rrVII\n     Nd Na     1    dm_rrVI\n     Nd N0     1    dm_rrVII\n     Nd C5     1    dm_rrVI\n     Nd C4     1    dm_rrVI\n     Nd C3     1    dm_rrVI\n     Nd C2     1    dm_rrVI\n     Nd C1     1    dm_rrVI\n     Nd SQda   1    dm_rsXII\n     Nd SQd    1    dm_rsXII\n     Nd SQa    1    dm_rsXII\n     Nd SQ0    1    dm_rsXII\n     Nd SP5    1    dm_rsXII\n     Nd SP4    1    dm_rsXI\n     Nd SP3    1    dm_rsXI\n     Nd SP2    1    dm_rsXI\n     Nd SP1    1    dm_rsVII\n     Nd SNda   1    dm_rsVI\n     Nd SNd    1    dm_rsVII\n     Nd SNa    1    dm_rsVI\n     Nd SN0    1    dm_rsVII\n     Nd SC5    1    dm_rsVI\n     Nd SC4    1    dm_rsVI\n     Nd SC3    1    dm_rsVI\n     Nd SC2    1    dm_rsVI\n     Nd SC1    1    dm_rsVI\n     Nd SP1c   1    dm_rsVII\n     Na Na     1    dm_rrVII\n     Na N0     1    dm_rrVII\n     Na C5     1    dm_rrVI\n     Na C4     1    dm_rrVI\n     Na C3     1    dm_rrVI\n     Na C2     1    dm_rrVI\n     Na C1     1    dm_rrVI\n     Na SQda   1    dm_rsXII\n     Na SQd    1    dm_rsXII\n     Na SQa    1    dm_rsXII\n     Na SQ0    1    dm_rsXII\n     Na SP5    1    dm_rsXII\n     Na SP4    1    dm_rsXI\n     Na SP3    1    dm_rsXI\n     Na SP2    1    dm_rsXI\n     Na SP1    1    dm_rsVII\n     Na SNda   1    dm_rsVI\n     Na SNd    1    dm_rsVI\n     Na SNa    1    dm_rsVII\n     Na SN0    1    dm_rsVII\n     Na SC5    1    dm_rsVI\n     Na SC4    1    dm_rsVI\n     Na SC3    1    dm_rsVI\n     Na SC2    1    dm_rsVI\n     Na SC1    1    dm_rsVI\n     Na SP1c   1    dm_rsVII\n     N0 N0     1    dm_rrV\n     N0 C5     1    dm_rrV\n     N0 C4     1    dm_rrIV\n     N0 C3     1    dm_rrIV\n     N0 C2     1    dm_rrIV\n     N0 C1     1    dm_rrIV\n     N0 SQda   1    dm_rsXII\n     N0 SQd    1    dm_rsXII\n     N0 SQa    1    dm_rsXII\n     N0 SQ0    1    dm_rsXII\n     N0 SP5    1    dm_rsXII\n     N0 SP4    1    dm_rsXI\n     N0 SP3    1    dm_rsX\n     N0 SP2    1    dm_rsIX\n     N0 SP1    1    dm_rsVII\n     N0 SNda   1    dm_rsVII\n     N0 SNd    1    dm_rsVII\n     N0 SNa    1    dm_rsVII\n     N0 SN0    1    dm_rsV\n     N0 SC5    1    dm_rsV\n     N0 SC4    1    dm_rsIV\n     N0 SC3    1    dm_rsIV\n     N0 SC2    1    dm_rsIV\n     N0 SC1    1    dm_rsIV\n     N0 SP1c   1    dm_rsVII\n     C5 C5     1    dm_rrIII\n     C5 C4     1    dm_rrIII\n     C5 C3     1    dm_rrIII\n     C5 C2     1    dm_rrIII\n     C5 C1     1    dm_rrIII\n     C5 SQda   1    dm_rsXII\n     C5 SQd    1    dm_rsXII\n     C5 SQa    1    dm_rsXII\n     C5 SQ0    1    dm_rsXII\n     C5 SP5    1    dm_rsXII\n     C5 SP4    1    dm_rsXI\n     C5 SP3    1    dm_rsX\n     C5 SP2    1    dm_rsVI\n     C5 SP1    1    dm_rsVI\n     C5 SNda   1    dm_rsVI\n     C5 SNd    1    dm_rsVI\n     C5 SNa    1    dm_rsVI\n     C5 SN0    1    dm_rsV\n     C5 SC5    1    dm_rsIII\n     C5 SC4    1    dm_rsIII\n     C5 SC3    1    dm_rsIII\n     C5 SC2    1    dm_rsIII\n     C5 SC1    1    dm_rsIII\n     C5 SP1c   1    dm_rsVI\n     C4 C4     1    dm_rrII\n     C4 C3     1    dm_rrII\n     C4 C2     1    dm_rrIII\n     C4 C1     1    dm_rrIII\n     C4 SQda   1    dm_rsXII\n     C4 SQd    1    dm_rsXII\n     C4 SQa    1    dm_rsXII\n     C4 SQ0    1    dm_rsXII\n     C4 SP5    1    dm_rsXII\n     C4 SP4    1    dm_rsXI\n     C4 SP3    1    dm_rsX\n     C4 SP2    1    dm_rsVI\n     C4 SP1    1    dm_rsVI\n     C4 SNda   1    dm_rsVI\n     C4 SNd    1    dm_rsVI\n     C4 SNa    1    dm_rsVI\n     C4 SN0    1    dm_rsIV\n     C4 SC5    1    dm_rsIII\n     C4 SC4    1    dm_rsII\n     C4 SC3    1    dm_rsII\n     C4 SC2    1    dm_rsIII\n     C4 SC1    1    dm_rsIII\n     C4 SP1c   1    dm_rsVI\n     C3 C3     1    dm_rrII\n     C3 C2     1    dm_rrII\n     C3 C1     1    dm_rrII\n     C3 SQda   1    dm_rsXII\n     C3 SQd    1    dm_rsXII\n     C3 SQa    1    dm_rsXII\n     C3 SQ0    1    dm_rsXII\n     C3 SP5    1    dm_rsXII\n     C3 SP4    1    dm_rsXI\n     C3 SP3    1    dm_rsX\n     C3 SP2    1    dm_rsVII\n     C3 SP1    1    dm_rsVI\n     C3 SNda   1    dm_rsVI\n     C3 SNd    1    dm_rsVI\n     C3 SNa    1    dm_rsVI\n     C3 SN0    1    dm_rsIV\n     C3 SC5    1    dm_rsIII\n     C3 SC4    1    dm_rsII\n     C3 SC3    1    dm_rsII\n     C3 SC2    1    dm_rsII\n     C3 SC1    1    dm_rsII\n     C3 SP1c   1    dm_rsVI\n     C2 C2     1    dm_rrII\n     C2 C1     1    dm_rrII\n     C2 SQda   1    dm_rsIX\n     C2 SQd    1    dm_rsIX\n     C2 SQa    1    dm_rsIX\n     C2 SQ0    1    dm_rsIX\n     C2 SP5    1    dm_rsXII\n     C2 SP4    1    dm_rsXII\n     C2 SP3    1    dm_rsX\n     C2 SP2    1    dm_rsVIII\n     C2 SP1    1    dm_rsVI\n     C2 SNda   1    dm_rsVI\n     C2 SNd    1    dm_rsVI\n     C2 SNa    1    dm_rsVI\n     C2 SN0    1    dm_rsIV\n     C2 SC5    1    dm_rsIII\n     C2 SC4    1    dm_rsIII\n     C2 SC3    1    dm_rsII\n     C2 SC2    1    dm_rsII\n     C2 SC1    1    dm_rsII\n     C2 SP1c   1    dm_rsVI\n     C1 C1     1    dm_rrII\n     C1 SQda   1    dm_rsIX\n     C1 SQd    1    dm_rsIX\n     C1 SQa    1    dm_rsIX\n     C1 SQ0    1    dm_rsIX\n     C1 SP5    1    dm_rsXII\n     C1 SP4    1    dm_rsXII\n     C1 SP3    1    dm_rsXI\n     C1 SP2    1    dm_rsX\n     C1 SP1    1    dm_rsVII\n     C1 SNda   1    dm_rsVI\n     C1 SNd    1    dm_rsVI\n     C1 SNa    1    dm_rsVI\n     C1 SN0    1    dm_rsIV\n     C1 SC5    1    dm_rsIII\n     C1 SC4    1    dm_rsIII\n     C1 SC3    1    dm_rsII\n     C1 SC2    1    dm_rsII\n     C1 SC1    1    dm_rsII\n     C1 SP1c   1    dm_rsVII\n   SQda SQda   1    dm_ssVI_qq\n   SQda SQd    1    dm_ssVI_qq\n   SQda SQa    1    dm_ssVI_qq\n   SQda SQ0    1    dm_ssVIII_qq\n   SQda SP5    1    dm_ssXII\n   SQda SP4    1    dm_ssXII\n   SQda SP3    1    dm_ssXII\n   SQda SP2    1    dm_ssXII\n   SQda SP1    1    dm_ssXII\n   SQda SNda   1    dm_ssXII\n   SQda SNd    1    dm_ssXII\n   SQda SNa    1    dm_ssXII\n   SQda SN0    1    dm_ssXII\n   SQda SC5    1    dm_ssXII\n   SQda SC4    1    dm_ssXII\n   SQda SC3    1    dm_ssXII\n   SQda SC2    1    dm_ssIX\n   SQda SC1    1    dm_ssIX\n   SQda SP1c   1    dm_ssXII\n    SQd SQd    1    dm_ssVII_qq\n    SQd SQa    1    dm_ssVI_qq\n    SQd SQ0    1    dm_ssVIII_qq\n    SQd SP5    1    dm_ssXII\n    SQd SP4    1    dm_ssXII\n    SQd SP3    1    dm_ssXII\n    SQd SP2    1    dm_ssXII\n    SQd SP1    1    dm_ssXII\n    SQd SNda   1    dm_ssXII\n    SQd SNd    1    dm_ssXII\n    SQd SNa    1    dm_ssXII\n    SQd SN0    1    dm_ssXII\n    SQd SC5    1    dm_ssXII\n    SQd SC4    1    dm_ssXII\n    SQd SC3    1    dm_ssXII\n    SQd SC2    1    dm_ssIX\n    SQd SC1    1    dm_ssIX\n    SQd SP1c   1    dm_ssXII\n    SQa SQa    1    dm_ssVII_qq\n    SQa SQ0    1    dm_ssVIII_qq\n    SQa SP5    1    dm_ssXII\n    SQa SP4    1    dm_ssXII\n    SQa SP3    1    dm_ssXII\n    SQa SP2    1    dm_ssXII\n    SQa SP1    1    dm_ssXII\n    SQa SNda   1    dm_ssXII\n    SQa SNd    1    dm_ssXII\n    SQa SNa    1    dm_ssXII\n    SQa SN0    1    dm_ssXII\n    SQa SC5    1    dm_ssXII\n    SQa SC4    1    dm_ssXII\n    SQa SC3    1    dm_ssXII\n    SQa SC2    1    dm_ssIX\n    SQa SC1    1    dm_ssIX\n    SQa SP1c   1    dm_ssXII\n    SQ0 SQ0    1    dm_ssVIII_qq\n    SQ0 SP5    1    dm_ssXII\n    SQ0 SP4    1    dm_ssXII\n    SQ0 SP3    1    dm_ssXII\n    SQ0 SP2    1    dm_ssXII\n    SQ0 SP1    1    dm_ssXII\n    SQ0 SNda   1    dm_ssXII\n    SQ0 SNd    1    dm_ssXII\n    SQ0 SNa    1    dm_ssXII\n    SQ0 SN0    1    dm_ssXII\n    SQ0 SC5    1    dm_ssXII\n    SQ0 SC4    1    dm_ssXII\n    SQ0 SC3    1    dm_ssXII\n    SQ0 SC2    1    dm_ssIX\n    SQ0 SC1    1    dm_ssIX\n    SQ0 SP1c   1    dm_ssXII\n    SP5 SP5    1    dm_ssV\n    SP5 SP4    1    dm_ssV\n    SP5 SP3    1    dm_ssV\n    SP5 SP2    1    dm_ssV\n    SP5 SP1    1    dm_ssX\n    SP5 SNda   1    dm_ssXII\n    SP5 SNd    1    dm_ssXII\n    SP5 SNa    1    dm_ssXII\n    SP5 SN0    1    dm_ssXII\n    SP5 SC5    1    dm_ssXII\n    SP5 SC4    1    dm_ssXII\n    SP5 SC3    1    dm_ssXII\n    SP5 SC2    1    dm_ssXII\n    SP5 SC1    1    dm_ssXII\n    SP5 SP1c   1    dm_rsX\n    SP4 SP4    1    dm_ssVI\n    SP4 SP3    1    dm_ssVI\n    SP4 SP2    1    dm_ssVII\n    SP4 SP1    1    dm_ssX\n    SP4 SNda   1    dm_ssXI\n    SP4 SNd    1    dm_ssXI\n    SP4 SNa    1    dm_ssXI\n    SP4 SN0    1    dm_ssXI\n    SP4 SC5    1    dm_ssXI\n    SP4 SC4    1    dm_ssXI\n    SP4 SC3    1    dm_ssXI\n    SP4 SC2    1    dm_ssXII\n    SP4 SC1    1    dm_ssXII\n    SP4 SP1c   1    dm_rsX\n    SP3 SP3    1    dm_ssVI\n    SP3 SP2    1    dm_ssVII\n    SP3 SP1    1    dm_ssX\n    SP3 SNda   1    dm_ssXI\n    SP3 SNd    1    dm_ssXI\n    SP3 SNa    1    dm_ssXI\n    SP3 SN0    1    dm_ssX\n    SP3 SC5    1    dm_ssX\n    SP3 SC4    1    dm_ssX\n    SP3 SC3    1    dm_ssX\n    SP3 SC2    1    dm_ssX\n    SP3 SC1    1    dm_ssXI\n    SP3 SP1c   1    dm_rsX\n    SP2 SP2    1    dm_ssVII\n    SP2 SP1    1    dm_ssVIII\n    SP2 SNda   1    dm_ssXI\n    SP2 SNd    1    dm_ssXI\n    SP2 SNa    1    dm_ssXI\n    SP2 SN0    1    dm_ssIX\n    SP2 SC5    1    dm_ssVI\n    SP2 SC4    1    dm_ssVI\n    SP2 SC3    1    dm_ssVII\n    SP2 SC2    1    dm_ssVIII\n    SP2 SC1    1    dm_ssX\n    SP2 SP1c   1    dm_rsVIII\n    SP1 SP1    1    dm_ssVII\n    SP1 SNda   1    dm_ssVII\n    SP1 SNd    1    dm_ssVII\n    SP1 SNa    1    dm_ssVII\n    SP1 SN0    1    dm_ssVII\n    SP1 SC5    1    dm_ssVI\n    SP1 SC4    1    dm_ssVI\n    SP1 SC3    1    dm_ssVI\n    SP1 SC2    1    dm_ssVI\n    SP1 SC1    1    dm_ssVII\n    SP1 SP1c   1    dm_rsVII\n   SNda SNda   1    dm_ssVI\n   SNda SNd    1    dm_ssVI\n   SNda SNa    1    dm_ssVI\n   SNda SN0    1    dm_ssVII\n   SNda SC5    1    dm_ssVI\n   SNda SC4    1    dm_ssVI\n   SNda SC3    1    dm_ssVI\n   SNda SC2    1    dm_ssVI\n   SNda SC1    1    dm_ssVI\n   SNda SP1c   1    dm_rsVII\n    SNd SNd    1    dm_ssVII\n    SNd SNa    1    dm_ssVI\n    SNd SN0    1    dm_ssVII\n    SNd SC5    1    dm_ssVI\n    SNd SC4    1    dm_ssVI\n    SNd SC3    1    dm_ssVI\n    SNd SC2    1    dm_ssVI\n    SNd SC1    1    dm_ssVI\n    SNd SP1c   1    dm_rsVII\n    SNa SNa    1    dm_ssVII\n    SNa SN0    1    dm_ssVII\n    SNa SC5    1    dm_ssVI\n    SNa SC4    1    dm_ssVI\n    SNa SC3    1    dm_ssVI\n    SNa SC2    1    dm_ssVI\n    SNa SC1    1    dm_ssVI\n    SNa SP1c   1    dm_rsVII\n    SN0 SN0    1    dm_ssV\n    SN0 SC5    1    dm_ssV\n    SN0 SC4    1    dm_ssIV\n    SN0 SC3    1    dm_ssIV\n    SN0 SC2    1    dm_ssIV\n    SN0 SC1    1    dm_ssIV\n    SN0 SP1c   1    dm_rsVII\n    SC5 SC5    1    dm_ssIII\n    SC5 SC4    1    dm_ssIII\n    SC5 SC3    1    dm_ssIII\n    SC5 SC2    1    dm_ssIII\n    SC5 SC1    1    dm_ssIII\n    SC5 SP1c   1    dm_rsVI\n    SC4 SC4    1    dm_ssII\n    SC4 SC3    1    dm_ssII\n    SC4 SC2    1    dm_ssIII\n    SC4 SC1    1    dm_ssIII\n    SC4 SP1c   1    dm_rsVI\n    SC3 SC3    1    dm_ssII\n    SC3 SC2    1    dm_ssII\n    SC3 SC1    1    dm_ssII\n    SC3 SP1c   1    dm_rsVI\n    SC2 SC2    1    dm_ssII\n    SC2 SC1    1    dm_ssII\n    SC2 SP1c   1    dm_rsVI\n    SC1 SC1    1    dm_ssII\n    SC1 SP1c   1    dm_rsVII\n   SP1c SP1c   1    dm_rsVII\n\n");
    
    
    fclose(min);
    fclose(sim);
    fclose(dry_martini);
}




void MinimizaEstructura(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,int pinoffset,int ntmpi, int index_sim_bond,int index_sim_bending)
{
    double box_x;
    double box_y;
    double box_z;
    char comando[512];
    FILE *pipe;
    
    if(fork()==0)
    {
        sprintf(comando,"mkdir CarpetaPrincipal_%d_%d\n",index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    
    box_x=3+CANAL_punt->longitudInterior+CANAL_punt->longitudExterior;
    box_y=box_x;
    box_z=box_x;
    
    if(fork()==0)
    {
        sprintf(comando,"gmx_d editconf -f SISTEMA_conf_%d_%d.gro -o Sistema_%d_%d.gro -box %.2f %.2f %.2f -c\n",index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,box_x,box_y,box_z);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    
    
    EscribeFicheroTopol(index_sim_bond,index_sim_bending);
    
    
    if(fork()==0)
    {
        sprintf(comando,"cp ./Input/minim.mdp ./Input/term.mdp ./Input/dry_martini_v2.1.itp Sistema_%d_%d.gro SISTEMA_canal_%d_%d.itp SISTEMA_index_%d_%d.ndx topol_%d_%d.top CarpetaPrincipal_%d_%d\n",index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    
    if(fork()==0)
    {
        sprintf(comando,"cd CarpetaPrincipal_%d_%d\ngmx_d grompp -f minim.mdp -c Sistema_%d_%d.gro -p topol_%d_%d.top -o topol_%d_%d.tpr -n SISTEMA_index_%d_%d.ndx -maxwarn 3\n",index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    
    
    if(fork()==0)
    {
        sprintf(comando,"cd CarpetaPrincipal_%d_%d\ngmx_d mdrun -s topol_%d_%d.tpr -c AnalisisMN_min_%d_%d.gro -o traj_min_%d_%d.trr -rdd 2 -v -ntmpi %d -pin on -pinoffset %d -pinstride 1\n",index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,ntmpi,pinoffset);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    
    
    
    
    if(fork()==0)
    {
        sprintf(comando,"cp CarpetaPrincipal_%d_%d/AnalisisMN_min_%d_%d.gro CarpetaPrincipal_%d_%d/SISTEMA_index_%d_%d.ndx CarpetaPrincipal_%d_%d/traj_min_%d_%d.trr ./\n",index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    
    
    if(fork()==0)
    {
        sprintf(comando,"rm Sistema_%d_%d.gro SISTEMA_conf_%d_%d.gro\nrm -r CarpetaPrincipal_%d_%d\n",index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending,index_sim_bond,index_sim_bending);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    

}




void ProcesoResultados(struct CANAL_struct* CANAL_punt,struct VARIABLE_struct* VARIABLE_punt,int index_sim_bond,int index_sim_bending)
{
    int i,j,k,x,x_bueno;;
    double y;
    char flag;
    char comando[512];
    FILE *freq,*Nfreq;
    FILE *pipe;
    char freq_name[256];
    char Nfreq_name[256];
    
    sprintf(freq_name,"eigenfreq_%d_%d.xvg",index_sim_bond,index_sim_bending);
    sprintf(Nfreq_name,"eigenfreq_%d_%d.txt",index_sim_bond,index_sim_bending);
    
    x_bueno=AnalisisDeAutovectores(VARIABLE_punt,CANAL_punt,index_sim_bond,index_sim_bending);
    
    freq=fopen(freq_name,"r");
    Nfreq=fopen(Nfreq_name,"w");
    
    for(i=0;i<1;i++)
    {
        flag=fgetc(freq);
        if(flag=='@' || flag=='#' || flag=='&')
        {
            for(k=0;k<1;k++)
            {
                if(fgetc(freq)=='\n')
                    break;
                k=-1;
            }
        }
        else
        {
            for(j=0;j<1;j++)
            {
                fscanf(freq,"%d%lf",&x,&y);
                if(x==x_bueno)
                    fprintf(Nfreq,"%d\t%lf\n",x,y*VelocidadLuz_c);
                if(feof(freq)!=0)
                    break;
                j=-1;
            }
            break;
        }
        i=-1;
    }
    fclose(freq);
    fclose(Nfreq);
    
    
    if(fork()==0)
    {
        sprintf(comando,"rm %s\n",freq_name);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        exit(0);
    }
    wait(NULL);
    
}


void AnalisisDeResultados(struct VARIABLE_struct* VARIABLE_punt, struct CANAL_struct* CANAL_punt)
{
    int i,index_sim_bond,index_sim_bending,x;
    double y;
    char eigenfreq_name[256];
    char comando[512];
    char flag;
    FILE *pipe;
    FILE *eigenfreq;
    FILE *MapaFrec;
    
    
    MapaFrec=fopen("Mapa_Frecuencias.txt","w");
    fprintf(MapaFrec,"K_b\tK_theta\tmodo\tFreq[ps^-1]\n");

    
    for(index_sim_bond=0;index_sim_bond<VARIABLE_punt->N_simulaciones;index_sim_bond++)
    {
        for(index_sim_bending=0;index_sim_bending<VARIABLE_punt->N_simulaciones;index_sim_bending++)
        {
            sprintf(eigenfreq_name,"eigenfreq_%d_%d.txt",index_sim_bond,index_sim_bending);
            
            eigenfreq=fopen(eigenfreq_name,"r");
            
            fscanf(eigenfreq,"%d%lf",&x,&y);
            
            fprintf(MapaFrec,"%lf\t%lf\t%d\t%lf\n",CANAL_punt->bond_fuerza[index_sim_bond],CANAL_punt->bending_fuerza[index_sim_bending],x,y);
            fflush(MapaFrec);
                
            
            fclose(eigenfreq);
        }
    }
    
    fclose(MapaFrec);
    
    
    
    for(index_sim_bond=0;index_sim_bond<VARIABLE_punt->N_simulaciones;index_sim_bond++)
    {
        for(index_sim_bending=0;index_sim_bending<VARIABLE_punt->N_simulaciones;index_sim_bending++)
        {
            if(fork()==0)
            {
                sprintf(comando,"rm eigenfreq_%d_%d.txt\n",index_sim_bond,index_sim_bending);
                
                pipe=popen(comando,"w");
                pclose(pipe);
                
                exit(0);
            }
            wait(NULL);
        }
        
    }
}


int AnalisisDeAutovectores(struct VARIABLE_struct* VARIABLE_punt, struct CANAL_struct* CANAL_punt, int index_sim_bond,int index_sim_bending)
{
    int i;
    int index_modo;
    int resultado=0;
    char basura[256];
    char inst_name[256];
    char comando[1024];
    char flag;
    double x,y,z;
    double producto=0,producto_previo=0,norma_ref=0,norma_inst=0;
    FILE *referencia,*instantaneo,*pipe;
    
    VARIABLE_punt->ModoNormal=(double**)malloc(sizeof(double*)*(3*VARIABLE_punt->SISTEMA_n+1));
    for(i=0;i<(3*VARIABLE_punt->SISTEMA_n+1);i++)
        VARIABLE_punt->ModoNormal[i]=(double*)malloc(sizeof(double)*3*VARIABLE_punt->SISTEMA_n);
    
    CANAL_punt->Nb=(int)((CANAL_punt->longitudInterior+2*CANAL_punt->longitudExterior)/CANAL_punt->bond_reposo);
    CANAL_punt->M=(int)(2*PI/(acos(1-CANAL_punt->bond_reposo*CANAL_punt->bond_reposo/(2*CANAL_punt->radio*CANAL_punt->radio))));
    VARIABLE_punt->SISTEMA_n=CANAL_punt->Nb*CANAL_punt->M;
    
    
    referencia=fopen("ModoReferencia.txt","r");
    for(i=0;i<1;i++)
    {
        flag=fgetc(referencia);
        if(flag=='\n')
            break;
        i=-1;
    }
    fscanf(referencia,"%s",basura);
    
    for(i=0;i<3*VARIABLE_punt->SISTEMA_n;i+=3)
    {
        fscanf(referencia,"%s%s%s",basura,basura,basura);
        fscanf(referencia,"%lf%lf%lf",&x,&y,&z);
        
        VARIABLE_punt->ModoNormal[0][i]=x;
        VARIABLE_punt->ModoNormal[0][i+1]=y;
        VARIABLE_punt->ModoNormal[0][i+2]=z;
    }
    fclose(referencia);
    
    
    for(index_modo=7;index_modo<=3*VARIABLE_punt->SISTEMA_n;index_modo++)
    {
        sprintf(inst_name,"autovector_%d__%d_%d.gro",index_modo,index_sim_bond,index_sim_bending);
        instantaneo=fopen(inst_name,"r");
        for(i=0;i<1;i++)
        {
            flag=fgetc(instantaneo);
            if(flag=='\n')
                break;
            i=-1;
        }
        fscanf(instantaneo,"%s",basura);
        
        for(i=0;i<3*VARIABLE_punt->SISTEMA_n;i+=3)
        {
            fscanf(instantaneo,"%s%s%s",basura,basura,basura);
            fscanf(instantaneo,"%lf%lf%lf",&x,&y,&z);
            
            VARIABLE_punt->ModoNormal[index_modo][i]=x;
            VARIABLE_punt->ModoNormal[index_modo][i+1]=y;
            VARIABLE_punt->ModoNormal[index_modo][i+2]=z;
        }
        fclose(instantaneo);
        
        if(fork()==0)
        {
            sprintf(comando,"rm %s\n",inst_name);
            pipe=popen(comando,"w");
            pclose(pipe);
            
            exit(0);
        }
        wait(NULL);
    }
    
    
    
    
    norma_ref=0;
    for(i=0;i<3*VARIABLE_punt->SISTEMA_n;i++)
        norma_ref+=VARIABLE_punt->ModoNormal[0][i]*VARIABLE_punt->ModoNormal[0][i];
    norma_ref=sqrt(norma_ref);
    
    for(index_modo=7;index_modo<=3*VARIABLE_punt->SISTEMA_n;index_modo++)
    {
        producto=0;
        norma_inst=0;
        for(i=0;i<3*VARIABLE_punt->SISTEMA_n;i++)
        {
            producto+=VARIABLE_punt->ModoNormal[0][i]*VARIABLE_punt->ModoNormal[index_modo][i];
            norma_inst+=VARIABLE_punt->ModoNormal[index_modo][i]*VARIABLE_punt->ModoNormal[index_modo][i];
        }
        norma_inst=sqrt(norma_inst);
        producto=producto/(norma_ref*norma_inst);
        
        
        if(fabs(producto)>fabs(producto_previo))
        {
            producto_previo=producto;
            resultado=index_modo;
        }
    }
    
    for(i=0;i<(3*VARIABLE_punt->SISTEMA_n+1);i++)
        free(VARIABLE_punt->ModoNormal[i]);
    free(VARIABLE_punt->ModoNormal);
    
    return resultado;
}

