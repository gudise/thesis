// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <math.h>
// #include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
//#include "./gromacs/LSLJ_Funciones.h" //ambos encabezados deben unificarse en un solo archivo
//#include "./gromacs/LSVE_Funciones.h"
//#include "./gromacs/MN_Funciones.h"

#define PI 3.14159

typedef struct PORO_struct
{
    float radio;
    float long_interior;
    float long_exterior;
    float bond_fuerza;
    float bond_reposo;
    float bending_fuerza;
    float bending_ang;
    float phi;
    int n;
    int M;
    int Nb;
    int NExterior;
    char borde[256];
    char cuerpo[256];
} PORO;

typedef struct POL_struct
{
    float lpol;
    float kb;
    float b0;
    float k_theta;
    float theta0;
    char QM_borde[256];
    char QM_cuerpo[256];
    int n;
    int n_insert;
    float** pos;
    float** vel;
    float box[3];
} POL;

typedef struct MEMBRANA_struct
{
    float area;
    float lado;
    int NumeroDSPC;
} MEMBRANA;



PORO*       mallocPoro();

MEMBRANA*   mallocMembrana();

void        freePoro();

void        freeMembrana();

void        ConstruyePoro(PORO* poro, float **poro_x, float **poro_v, int *poro_residuo, int *poro_chgrp, float poro_box[3], float poro_angle[2], char verbose);

void        ConstruyeDisco(float **Rx,float **Ry,float **Rz, PORO* poro, int i);

void        MatrizDistancia(float **MD,float **Rx,float **Ry, float **Rz, PORO* poro);

float       Distancia(float **Rx,float **Ry, float **Rz,int m1, int n1,int m2, int n2);

void        MatrizVecinos(int **MVecinos, float **MDistancia, PORO* poro);

void        GeneraBond(int **MEnlaces,int **MVecinos, PORO* poro);

void        GeneraAngle(int ***MAngulos,int **MVecinos, PORO* poro);

void        ConstruyeTopol_canal(int **MEnlaces, int ***MAngulos, char* poro_topol_name, PORO* poro);

void        MoverEstructura(float **estructura,int N, float *Angle, float theta, float phi, float Rx, float Ry, float Rz);

void        Calcula_CM(float **estructura, int N, float *CentroMasa);

void        Rotacion(float *EjeRotacion, float *Vector, float angulo);

float       ProductoEscalar(float *a, float *b);

void        ProductoVectorial(float *a, float *b, float *result);

void        ConstruyeDSPC(float **dspc_x, float **dspc_v, int *dspc_residuo, int *dspc_chgrp, float dspc_box[3], float dspc_angle[2], float angulo, float area_membrana);

void        UbicarEnMembrana(float ****membrana_x, float **dspc_x, int dspc_n, int i, int j);

int         AhuecarMembrana(float ****membrana_x, int membrana_n, float radio);









PORO* mallocPoro()
{
    PORO* result;
    
    result = (PORO*)malloc(sizeof(PORO));
    
    return result;
}


MEMBRANA* mallocMembrana()
{
    MEMBRANA* result;
    
    result = (MEMBRANA*)malloc(sizeof(MEMBRANA));
    
    return result;
}


void ConstruyePoro(PORO* poro, float **poro_x, float **poro_v, int *poro_residuo, int *poro_chgrp, float poro_box[3], float poro_angle[2], char verbose)
{
    ///Definicion de variables auxiliares (para la programacion)
    int n,m,i,j;
    
    ///Definicion del contenedor de posiciones del sistema
    float **Rx;
    float **Ry;
    float **Rz;
    float **MDistancia;   //Matriz de distancias.
    int **MVecinos;
    int **MEnlaces;
    int ***MAngulos;
    
    // Reajuste de los parametros del poro
    if(verbose)
    {
        printf("\nNOTA: se han introducido los parametros:\nporo->bond_reposo=%.2f\nporo->radio=%.1f\nLongitud=%d\n\n",poro->bond_reposo,poro->radio,poro->Nb);
        printf("Estos parametros suponen:\nAmplitud_angular=%.2f\tBeads/vuelta=%d\n\n",poro->phi,poro->M);
    }
    
    poro->phi=2*PI/poro->M;
    poro->radio=poro->bond_reposo*sqrt(1/(2*(1-cos(poro->phi))));
    
    if(verbose)
        printf("Para ajustar el cilindro, se modifican los parametros:\nAmplitud angular'=%.2f\tporo->radio'=%.3f\n\n",poro->phi,poro->radio);
    
    
    ///Asigancion de memoria al sistema******************
    Rx=(float**)malloc(sizeof(float*)*poro->M);
    Ry=(float**)malloc(sizeof(float*)*poro->M);
    Rz=(float**)malloc(sizeof(float*)*poro->M);
    for(int i=0;i<poro->M;i++)
    {
        Rx[i] = (float*)malloc(sizeof(float)*poro->Nb);
        Ry[i]=(float*)malloc(sizeof(float)*poro->Nb);
        Rz[i]=(float*)malloc(sizeof(float)*poro->Nb);
    }
    
    MDistancia=(float**)malloc(sizeof(float*)*poro->M*poro->Nb);
    for(int i=0;i<(poro->M*poro->Nb);i++)
        MDistancia[i] = (float*)malloc(sizeof(float)*poro->M*poro->Nb);
    
    MVecinos=(int**)malloc(sizeof(int*)*poro->M*poro->Nb);
    for(int i=0;i<poro->M*poro->Nb;i++)
        MVecinos[i]=(int*)malloc(sizeof(int)*poro->M*poro->Nb);
    
    MEnlaces=(int**)malloc(sizeof(int*)*poro->M*poro->Nb);
    for(int i=0;i<(poro->M*poro->Nb);i++)
        MEnlaces[i]=(int*)malloc(sizeof(int)*poro->M*poro->Nb);
    
    MAngulos=(int***)malloc(sizeof(int**)*poro->M*poro->Nb);
    for(int i=0; i<(poro->M*poro->Nb); i++)
    {
        MAngulos[i]=(int**)malloc(sizeof(int*)*poro->M*poro->Nb);
        for(int j=0; j<poro->M*poro->Nb; j++)
            MAngulos[i][j]=(int*)malloc(sizeof(int)*poro->M*poro->Nb);
    }
    
    ///Construccion del sistema
    for(int i=0;i<poro->Nb;i++)
        ConstruyeDisco(Rx, Ry, Rz, poro, i); //Con este bucle rellenamos las matrices R
    
    MatrizDistancia(MDistancia, Rx, Ry, Rz, poro);
    
    MatrizVecinos(MVecinos, MDistancia, poro);
    
    GeneraBond(MEnlaces, MVecinos, poro);
    
    GeneraAngle(MAngulos, MVecinos, poro);
    
    ConstruyeTopol_canal(MEnlaces, MAngulos, "poro.itp", poro);
    
    for(int n=0;n<poro->Nb;n++){
        for(int m=0;m<poro->M;m++){
            poro_x[n*poro->M+m][0]=Rx[m][n];
            poro_x[n*poro->M+m][1]=Ry[m][n];
            poro_x[n*poro->M+m][2]=Rz[m][n];
            
            poro_v[n*poro->M+m][0]=0.0;
            poro_v[n*poro->M+m][1]=0.0;
            poro_v[n*poro->M+m][2]=0.0;
            
            poro_residuo[n*poro->M+m]=n+1;
            
            poro_chgrp[n*poro->M+m]=n*poro->M+m+1;
        }
    }
    
    poro_box[0]=fabs(Rx[poro->M-1][poro->Nb-1]*1.1);
    poro_box[1]=fabs(Ry[poro->M-1][poro->Nb-1]*1.1);
    poro_box[2]=fabs(Rz[poro->M-1][poro->Nb-1]*1.1);
    
    poro_angle[0]=0.5*PI;
    poro_angle[1]=0;
    
    
    ///Liberar memoria
    for(int i=0;i<poro->M;i++){
        free(Rx[i]);
        free(Ry[i]);
        free(Rz[i]);
    }
    free(Rx);
    free(Ry);
    free(Rz);
    
    for(int i=0;i<(poro->M*poro->Nb);i++)
        free(MDistancia[i]);
    free(MDistancia);
    
    for(int i=0;i<poro->M*poro->Nb;i++)
        free(MVecinos[i]);
    free(MVecinos);
    
    for(int i=0;i<poro->M*poro->Nb;i++)
        free(MEnlaces[i]);
    free(MEnlaces);
    
    for(int i=0;i<poro->M*poro->Nb;i++)
    {
        for(int j=0;j<poro->M*poro->Nb;j++)
            free(MAngulos[i][j]);
    }
    for(i=0;i<poro->M*poro->Nb;i++)
        free(MAngulos[i]);
    free(MAngulos);
}


void ConstruyeDisco(float **Rx,float **Ry,float **Rz, PORO* poro, int i)
{
    float phi_inicial=0;
    
    if(i%2==0)
        phi_inicial=0;
    else
        phi_inicial=0.5*poro->phi;
    
    for(int m=0; m<poro->M; m++){
        Rx[m][i] = poro->bond_reposo*i;
        Ry[m][i] = poro->radio*cos(m*poro->phi+phi_inicial);
        Rz[m][i] = poro->radio*sin(m*poro->phi+phi_inicial);
        
    }
}


void MatrizDistancia(float **MD,float **Rx,float **Ry, float **Rz, PORO* poro)
{
    int m1,n1,m2,n2;
    
    for(int i=0; i<(poro->M*poro->Nb); i++){
        for(int j=0; j<(poro->M*poro->Nb); j++){
            m1=i%poro->M;
            n1=i/poro->M;
            m2=j%poro->M;
            n2=j/poro->M; //Estas cuatro lineas convierten el indice en los valores m,n
            
            MD[i][j]=Distancia(Rx,Ry,Rz,m1,n1,m2,n2);
        }
    }
}


float Distancia(float **Rx,float **Ry, float **Rz,int m1, int n1,int m2, int n2)
{
    return(sqrt((Rx[m1][n1]-Rx[m2][n2])*(Rx[m1][n1]-Rx[m2][n2])+(Ry[m1][n1]-Ry[m2][n2])*(Ry[m1][n1]-Ry[m2][n2])+(Rz[m1][n1]-Rz[m2][n2])*(Rz[m1][n1]-Rz[m2][n2])));
}


void MatrizVecinos(int **MVecinos, float **MDistancia, PORO* poro)
{
    int m1,n1,m2,n2;
    float cota_dist = sqrt(2.0)*poro->bond_reposo;
    
    for(int i=0;i<poro->M*poro->Nb;i++)
    {
        for(int j=0;j<poro->M*poro->Nb;j++)
            MVecinos[i][j]=0;
    }
    
    for(int i=0;i<poro->M*poro->Nb;i++){
        m1=i%poro->M;
        n1=i/poro->M;
        for(int j=0;j<poro->M*poro->Nb;j++){
            m2=j%poro->M;
            n2=j/poro->M;
            if(i!=j && MDistancia[i][j]<cota_dist)
                MVecinos[i][j]=1;
            
            if(i!=j && n1==n2 && m1==0 && m2==poro->M-2)
                MVecinos[i][j]=2;
            
            if(i!=j && n1==n2 && m1==1 && m2==poro->M-1)
                MVecinos[i][j]=2;
            
            if(i!=j && n1==n2 && m1==m2+2)
                MVecinos[i][j]=2;
            
            if(i!=j && m1==m2-1 && n1==n2-2)
                MVecinos[i][j]=2;
            
            if(i!=j && m1==m2+1 && n1==n2-2)
                MVecinos[i][j]=2;
            
            if(i!=j && m1==poro->M-1 && m2==0 && n1==n2-2)
                MVecinos[i][j]=2;
            
            if(i!=j && m1==0 && m2==poro->M-1 && n1==n2-2)
                MVecinos[i][j]=2;
        }
    }
}


void GeneraBond(int **MEnlaces,int **MVecinos, PORO* poro)
{    
    for(int i=0;i<poro->M*poro->Nb;i++)
    {
        for(int j=0;j<poro->M*poro->Nb;j++)
            MEnlaces[i][j]=0;
    }
    
    for(int i=0;i<poro->M*poro->Nb;i++){
        for(int j=0;j<poro->M*poro->Nb;j++){
            if(MEnlaces[i][j]==0 && MVecinos[i][j]==1){
                MEnlaces[i][j]=1;
                MEnlaces[j][i]=-1; //De este modo marco el enlace inverso para no repetirlo despues!
            }
        }
    }
}


void GeneraAngle(int ***MAngulos,int **MVecinos, PORO* poro)
{    
    for(int i=0;i<poro->M*poro->Nb;i++)
    {
        for(int j=0;j<poro->M*poro->Nb;j++)
        {
            for(int k=0;k<poro->M*poro->Nb;k++)
                MAngulos[i][j][k]=0;
        }
    }
    
    for(int i=0;i<poro->M*poro->Nb;i++){
        for(int j=0;j<poro->M*poro->Nb;j++){
            for(int k=0;k<poro->M*poro->Nb;k++){
                if(MAngulos[i][j][k]==0 && MVecinos[i][j]==1 && MVecinos[j][k]==1 && MVecinos[i][k]==2)
                {
                    MAngulos[i][j][k]=1;
                    MAngulos[i][k][j]=-1;
                    MAngulos[j][i][k]=-1;
                    MAngulos[k][i][j]=-1;
                    MAngulos[j][k][i]=-1;
                    MAngulos[k][j][i]=-1; //De este modo marco el enlace inverso para no repetirlo despues!
                }
            }
        }
    }
}


void ConstruyeTopol_canal(int **MEnlaces, int ***MAngulos, char* poro_topol_name, PORO* poro)
{
    FILE *topol;
    
    topol=fopen(poro_topol_name,"w");
    
    fprintf(topol,";Canal proteico para simulacion de traslocacion de PEG.\n\n");
    fprintf(topol,"[moleculetype]\n;name\texclusion\nPORO\t1\n\n");
    
    fprintf(topol,"[atoms]\n;nr\tattype\tresnr\tresidue\tatom\tcgnr\tcharge\tmass\n");
    for(int i=0;i<poro->Nb;i++){
        for(int j=0;j<poro->M;j++){
            if(i<poro->NExterior || i+1>poro->Nb-poro->NExterior)
                fprintf(topol,"%d\t%s\t%d\tPORO\tQ0\t%d\t%.3f\t;\n",j+poro->M*i+1,poro->borde,i+1,j+poro->M*i+1,0.0); //OJO! Donde pone "Q0" normalmente pone "P5"!!
            else
                fprintf(topol,"%d\t%s\t%d\tPORO\tC1\t%d\t%.3f\t;\n",j+poro->M*i+1,poro->cuerpo,i+1,j+poro->M*i+1,0.0);
        }
    }
    
    fprintf(topol,"\n\n");
    fprintf(topol,"[bonds]\n;i\tj\tfunc\tb_0\tk_b\n");
    for(int i=0;i<poro->M*poro->Nb;i++){
        for(int j=0;j<poro->M*poro->Nb;j++){
            if(MEnlaces[i][j]==1)
                fprintf(topol,"%d\t%d\t%d\t%.3f\t%.1f\n",i+1,j+1,1,poro->bond_reposo,poro->bond_fuerza);
        }
    }
    
    fprintf(topol,"\n\n");
    fprintf(topol,"[angles]\n;i\tj\tk\tfunc\ttheta_0\tk_theta\n");
    for(int i=0;i<poro->M*poro->Nb;i++){
        for(int j=0;j<poro->M*poro->Nb;j++){
            for(int k=0;k<poro->M*poro->Nb;k++){
                if(MAngulos[i][j][k]==1)
                    fprintf(topol,"%d\t%d\t%d\t%d\t%.3f\t%.1f\n",i+1,j+1,k+1,1,poro->bending_ang, poro->bending_fuerza);
            }
        }
    }
    fclose(topol);
}


void MoverEstructura(float **estructura,int N, float *Angle, float theta, float phi, float Rx, float Ry, float Rz)
{ 
    int i;
    float CentroMasa[3]={0,0,0};
    float delta_theta, delta_phi;
    float Eje_rot_phi[3]={0,0,1};
    float Eje_rot_theta[3];
    float NuevaDireccion[3]={sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta)};
    float VectorDirector[3];
    float Mod=0;
    
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


void Calcula_CM(float **estructura, int N, float *CentroMasa)
{
    CentroMasa[0]=0;
    CentroMasa[1]=0;
    CentroMasa[2]=0;
    
    
    for(int i=0;i<N;i++){
        CentroMasa[0]+=estructura[i][0];
        CentroMasa[1]+=estructura[i][1];
        CentroMasa[2]+=estructura[i][2];
    }
    
    CentroMasa[0]=CentroMasa[0]/((float)N);
    CentroMasa[1]=CentroMasa[1]/((float)N);
    CentroMasa[2]=CentroMasa[2]/((float)N); ///Aqui esta calculado el centro de masas.
    
    for(int i=0;i<N;i++){
        estructura[i][0]=estructura[i][0]-CentroMasa[0];
        estructura[i][1]=estructura[i][1]-CentroMasa[1];
        estructura[i][2]=estructura[i][2]-CentroMasa[2];
    }
}


void Rotacion(float *EjeRotacion, float *Vector, float angulo)
{
    float MatrizRotacion[3][3];
    float Vector_auxiliar[3]={0,0,0};
    float mod_Eje;
    
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
    
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
            Vector_auxiliar[i]+=MatrizRotacion[i][j]*Vector[j];
    }
    
    Vector[0]=Vector_auxiliar[0];
    Vector[1]=Vector_auxiliar[1];
    Vector[2]=Vector_auxiliar[2];
}


float ProductoEscalar(float *a, float *b){
    float result=0;
    int i;
    for(i=0;i<3;i++){
        result+=a[i]*b[i];
    }
    
    return result;
}


void ProductoVectorial(float *a, float *b, float *result)
{
    result[0]=a[1]*b[2]-a[2]*b[1];
    result[1]=a[2]*b[0]-a[0]*b[2];
    result[2]=a[0]*b[1]-a[1]*b[0];
}


void ConstruyeDSPC(float **dspc_x, float **dspc_v, int *dspc_residuo, int *dspc_chgrp, float dspc_box[3], float dspc_angle[2], float angulo, float lado_membrana)
{
    char dspc_atomname[14];
    FILE *dspc_topol;
    
    dspc_angle[0]=0.5*PI;
    dspc_angle[1]=0;
    
    angulo=angulo*PI/180;
    
    dspc_x[0][0]=0;
    dspc_x[0][1]=0;
    dspc_x[0][2]=0;
    dspc_atomname[0]='N';
    
    dspc_x[1][0]=dspc_x[0][0]+0.47;
    dspc_x[1][1]=dspc_x[0][1];
    dspc_x[1][2]=dspc_x[0][2];
    dspc_atomname[1]='P';
    
    dspc_x[2][0]=dspc_x[1][0]+0.47*cos(angulo);
    dspc_x[2][1]=dspc_x[1][1]+0.47*sin(angulo);
    dspc_x[2][2]=dspc_x[1][2];
    dspc_atomname[2]='G';
    
    dspc_x[3][0]=dspc_x[1][0]+0.47*cos(angulo);
    dspc_x[3][1]=dspc_x[1][1]-0.47*sin(angulo);
    dspc_x[3][2]=dspc_x[1][2];
    dspc_atomname[3]='G';
    
    
    dspc_x[4][0]=dspc_x[2][0]+0.47*cos(angulo);
    dspc_x[4][1]=dspc_x[2][1]+0.47*sin(angulo);
    dspc_x[4][2]=dspc_x[2][2];
    dspc_atomname[4]='C';
    
    for(int i=5;i<9;i++){
        dspc_x[i][0]=dspc_x[i-1][0]+0.47;
        dspc_x[i][1]=dspc_x[i-1][1];
        dspc_x[i][2]=dspc_x[i-1][2];
        dspc_atomname[i]='C';
    }
    
    dspc_x[9][0]=dspc_x[3][0]+0.47*cos(angulo);
    dspc_x[9][1]=dspc_x[3][1]-0.47*sin(angulo);
    dspc_x[9][2]=dspc_x[3][2];
    dspc_atomname[9]='C';
    
    for(int i=10;i<14;i++){
        dspc_x[i][0]=dspc_x[i-1][0]+0.47;
        dspc_x[i][1]=dspc_x[i-1][1];
        dspc_x[i][2]=dspc_x[i-1][2];
        dspc_atomname[i]='C';
    }
    
    for(int i=0;i<14;i++){
        dspc_v[i][0]=0;
        dspc_v[i][1]=0;
        dspc_v[i][2]=0;
        
        dspc_residuo[i]=1;
        
        dspc_chgrp[i]=i+1;
    }
    
    dspc_box[0]=5.0;
    dspc_box[1]=1.0;
    dspc_box[2]=1.0;
    
    // Construye topologia
    dspc_topol=fopen("dspc.itp","w");
    
    fprintf(dspc_topol,";Molecula 'DSPC'[ S.J. Marrink, A.H. de Vries, A.E. Mark] para simulacion de traslocacion de PEG\n\n");
    fprintf(dspc_topol,";Metadata:\n");
    fprintf(dspc_topol,";lado_membrana %f\n", lado_membrana);
    
    fprintf(dspc_topol,"\n[moleculetype]\n;molname\tnrexcl\nDSPC\t%d\n\n",1);
    fprintf(dspc_topol,"[atoms]\n;id\ttype\tresnr\tresidu\tatom\tcgnr\tcharge\n");
    
    fprintf(dspc_topol,"1\tQ0\t1\tDSPC\tNC\t1\t1.0\n");
    fprintf(dspc_topol,"2\tQa\t1\tDSPC\tPO\t2\t-1.0\n");
    
    fprintf(dspc_topol,"3\tNa\t1\tDSPC\tGL\t3\t0.0\n");
    fprintf(dspc_topol,"4\tNa\t1\tDSPC\tGL\t4\t0.0\n");
    fprintf(dspc_topol,"5\tC1\t1\tDSPC\tCA\t5\t0.0\n");
    fprintf(dspc_topol,"6\tC1\t1\tDSPC\tCA\t6\t0.0\n");
    fprintf(dspc_topol,"7\tC1\t1\tDSPC\tCA\t7\t0.0\n");
    fprintf(dspc_topol,"8\tC1\t1\tDSPC\tCA\t8\t0.0\n");
    fprintf(dspc_topol,"9\tC1\t1\tDSPC\tCA\t9\t0.0\n");
    fprintf(dspc_topol,"10\tC1\t1\tDSPC\tCB\t10\t0.0\n");
    fprintf(dspc_topol,"11\tC1\t1\tDSPC\tCB\t11\t0.0\n");
    fprintf(dspc_topol,"12\tC1\t1\tDSPC\tCB\t12\t0.0\n");
    fprintf(dspc_topol,"13\tC1\t1\tDSPC\tCB\t13\t0.0\n");
    fprintf(dspc_topol,"14\tC1\t1\tDSPC\tCB\t14\t0.0\n\n");
    
    fprintf(dspc_topol,"[bonds]\n;i\tj\tfunct\tlength\tforce.c.\n");
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",1,2,1,0.45,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",2,3,1,0.45,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",3,4,1,0.37,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",3,5,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",5,6,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",6,7,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",7,8,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",8,9,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",4,10,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",10,11,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",11,12,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",12,13,1,0.48,1250.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n\n",13,14,1,0.48,1250.0);
    
    fprintf(dspc_topol,"[angles]\n;i\tj\tk\tfunct\tangle\tforce.c.\n");
    
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",2,3,4,2,120.0,25.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",2,3,5,2,180.0,25.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",3,5,6,2,180.0,35.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",5,6,7,2,180.0,35.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",6,7,8,2,180.0,35.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",7,8,9,2,180.0,35.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",4,10,11,2,180.0,35.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",10,11,12,2,180.0,35.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",11,12,13,2,180.0,35.0);
    fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",12,13,14,2,180.0,35.0);
    
    fclose(dspc_topol);
}


void UbicarEnMembrana(float ****membrana_x, float **dspc_x, int dspc_n, int i, int j)
{    
    for(int k=0;k<dspc_n;k++)
    {
        membrana_x[i][j][k][0]=dspc_x[k][0];
        membrana_x[i][j][k][1]=dspc_x[k][1];
        membrana_x[i][j][k][2]=dspc_x[k][2];
    }
}


int AhuecarMembrana(float ****membrana_x, int membrana_n, float radio)
{
    float distancia;
    int eliminados=0;
    float Centro[2]={0,0};
    
    for(int i=0;i<membrana_n;i++)
    {
        for(int j=0;j<membrana_n;j++)
        {
            Centro[0]+=membrana_x[i][j][0][0];
            Centro[1]+=membrana_x[i][j][0][1];
        }
    }
    
    Centro[0]=Centro[0]/(membrana_n*membrana_n);
    Centro[1]=Centro[1]/(membrana_n*membrana_n);
    
    for(int i=0;i<membrana_n;i++)
    {
        for(int j=0;j<membrana_n;j++)
        {
            distancia=sqrt((Centro[0]-membrana_x[i][j][0][0])*(Centro[0]-membrana_x[i][j][0][0])+(Centro[1]-membrana_x[i][j][0][1])*(Centro[1]-membrana_x[i][j][0][1]));
            if(distancia<radio)
            {
                membrana_x[i][j][0]=NULL;
                eliminados++;
            }
        }
    }
    return eliminados;
}


