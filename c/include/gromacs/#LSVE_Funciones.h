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

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

#define PI 3.1415926536
#define kB 1.3806488e-26 //kJ/K
#define Navogadro 6.02214179e+23 //mol^-1

long idum;

typedef struct CANAL_struct
{
    int sel;
    float radio;
    float longitudInterior;
    float longitudExterior;
    float *bond_fuerza;
    float bond_reposo;
    float *bending_fuerza;
    float bending_ang;
    float phi;
    int n;
    int M;
    int Nb;
    int NExterior;
    char Borde_poro[256];
    char Cuerpo_poro[256];
} CANAL;

typedef struct POL_struct
{
    int sel;
    float bond_fuerza;
    float bond_reposo;
    float bending_fuerza;
    float bending_ang;
    float posicion;
    float *longitud;
    int n;
    char Borde_polimero[256];
    char Cuerpo_polimero[256];
} POL;

struct MEMBRANA_struct
{
    int sel;
    float area;
    float lado;
    int NumeroDSPC;
} MEMBRANA;

typedef struct VARIABLE_struct
{
    int SISTEMA_n;
    int N_simulaciones;
    int N_bond_fuerza;
    int N_bending_fuerza;
    int N_pull_rate;
    int N_pull_fuerza;
    int N_E_frecuencia;
    int N_pol_longitud;
    int N_estadistica;
    int AGUA_sel;
    float term_tolerancia;
    float term_tol_derivada;
    int N_term;
    int N_Lterm;
    int N_sim;
    float N_sim_cte;
    float dt;
    float tau_t;
    float ref_t;
    float *pull_rate;
    float *pull_fuerza;
    float pull_k;
    float radio_giro;
    float tiempo_extra;
    char pulling[256];
    char PintaTrayectoria[256];
    char PintaPoro[256];
    char Acoplo_presion[256];
    char conf_externa[512];
    char pulling_modo[256];
    int N_CPU;
    int N_nodo;
    int nodo;
    int index_nodo;
    int *lista_nodos;
    int cpu;
    int *pid_array_nodo;
    int *pid_array_cpu;
    int **MatrizSeguridad;
    int *ArraySeguridad;
    char campo_E[256];
    float *Ezfrecuencia;
    float Ezintensidad;
    float Exintensidad;
    float Eyintensidad;
    int primercand;
    int ultimocand;
	int every_out;

} VARIABLE;


int Igual(float a, float b);

void SumaVector(float *a, float *b, float *result);

void RestaVector(float *a, float *b, float *result);

float ProductoEscalar(float *a, float *b);

float AnguloEntreVectores(float *a, float *b);

void ProductoVectorial(float *a, float *b, float *result);

void AsignaVector(float *a, float *result);

void Rotacion(float *EjeRotacion, float *Vector, float angulo);

float Distancia(float **Rx,float **Ry, float **Rz,int m1, int n1,int m2, int n2);

void MedVar(float *distribucion, int N, float* media, float* desviacion);

float ran1(long *idum);

int Dado(int Ncaras);

void GenerarEstructuras(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int index_simulacion);

void ConstruyeCanal(float CANAL_radio, float CANAL_bond_reposo, float CANAL_bond_fuerza, float CANAL_bending_ang, float CANAL_bending_fuerza, int CANAL_Nb,int CANAL_NExterior, int CANAL_M, float CANAL_phi, float **CANAL_X, float **CANAL_V, int *CANAL_residuo, int *CANAL_chgrp, float *CANAL_box, float *CANAL_Angle, int index_simulacion,char* Borde_poro,char* Cuerpo_poro);

void ConstruyeDisco(float **Rx,float **Ry,float **Rz, float CANAL_bond_reposo,float CANAL_radio, float phi, int M, int i);

void MatrizDistancia(float **MD,float **Rx,float **Ry, float **Rz,int M, int Nb);

void MatrizVecinos(int **MV,int M, int Nb,float **MatrizDistancia,float CANAL_bond_reposo);

void GeneraBond(int **MatrizEnlaces,int **MVecinos, int M, int Nb);

void GeneraAngle(int ***MatrizAngulos,int **MVecinos, int M, int Nb);

void ConstruyeTopol_canal(float CANAL_bond_reposo,float CANAL_bond_fuerza, float CANAL_bending_ang, float CANAL_bending_fuerza,int M, int Nb,int NExterior,int **MEnlaces, int ***MAngulos, int index_simulacion, char* Borde_poro, char* Cuerpo_poro);

void ConstruyePolimero(int POL_n,float POL_bond_reposo, float POL_bond_fuerza,float POL_bending_ang,float POL_bending_fuerza,float **POL_X, float **POL_V, int *POL_residuo, int *POL_chgrp, float *POL_box, float *POL_Angle, int index_simulacion ,char* Borde_polimero, char* Cuerpo_polimero);

void ConstruyeTopol_polimero(int POL_n,float POL_bond_reposo,float POL_bond_fuerza,float POL_bending_ang,float POL_bending_fuerza,int index_simulacion,char* Borde_polimero,char* Cuerpo_polimero);

void ConstruyeDSPC(float **DSPC_X, float **DSPC_V, int *DSPC_residuo, int *DSPC_chgrp, float *DSPC_box, float *DSPC_Angle, char *DSPC_atomname, float angulo,int AGUA_sel ,int index_simulacion, char* campoE);

void UbicarEnMembrana(float ****MEMBRANA_X, float **DSPC_X, int DSPC_n, int i, int j);

int AsignaOrientacion_Membrana(int i, int j, int MEMBRANA_n);

int AhuecarMembrana(float ****MEMBRANA_X,int MEMBRANA_n,float radio);

void ConstruyeTopol_DSPC(int index_simulacion, int AGUA_sel,char* campoE);

void MoverEstructura(float **estructura,int N,float theta, float phi, float Rx, float Ry, float Rz, float *Angle);

void Calcula_CM(float **estructura, int N, float *CentroMasa);

void ConstruyeIndices(int SISTEMA_n, int CANAL_n,int CANAL_M,int CANAL_Nb,float CANAL_bond_reposo, int POL_n,int DSPC_n ,int NumeroDSPC, int index_simulacion,int CANAL_sel,int POL_sel, float POL_posicion,float POL_bond_reposo, int MEMBRANA_sel);

void EscribeFicheroTopol(int NumeroDSPC, int NumeroW, int index_simulacion, int CANAL_sel, int POL_sel, int MEMBRANA_sel);

void Genconf();

float CalculaRadioGiro(int index_simulacion, char* Carpeta, char* GROfile);

float TiempoTraslocacion(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int index_simulacion, int index_estad);

void AnalisisDeResultados(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt);

void LeerInput(struct CANAL_struct* CANAL_punt, struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt, struct VARIABLE_struct* VARIABLE_punt);

void LiberarMemoriaReservada(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt);
void LimpiaEstructurasUsadas(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int index_simulacion);


void GeneraMDP(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt,struct VARIABLE_struct* VARIABLE_punt, int index_simulacion,int index_estad);

void Prepara_FicherosParaTermalizacion(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt,struct MEMBRANA_struct* MEMBRANA_punt,int pinoffset,int ntmpi, int ntomp, int index_simulacion);

void Simula_Termalizacion(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int pinoffset,int ntmpi, int ntomp,int index_simulacion);

void Prepara_FicherosParaTraslocacion(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt,struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int index_simulacion, int index_estad);

void Simula_Traslocacion(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int pinoffset,int ntmpi, int ntomp,int index_simulacion,int index_estad);

void Prepara_FicherosParaAnalisis(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt,struct MEMBRANA_struct* MEMBRANA_punt,int index_simulacion,int index_estad);

void UtilizaEstructuraPredefinida(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt, int index_simulacion);

int GarantizoQueTodoEstaBien(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt);




/**************************
 *                        *
 * FUNCIONES GENERALES    *
 *                        *
 **************************/
///FUNCIONES OPERATIVAS//////////////////////////////////////////////////////////////////
int Igual(float a, float b){  //Devuelve 1 si son iguales, 0 si son diferentes
    float epsilon=0.0000001;
    
    if (fabs(a-b)<epsilon)
        return 1;
    
    else
        return 0;
}


void SumaVector(float *a, float *b, float *result){
    int i;
    for(i=0;i<3;i++)
        result[i]=a[i]+b[i];
}

void RestaVector(float *a, float *b, float *result){
    int i;
    for(i=0;i<3;i++)
        result[i]=a[i]-b[i];
}

float ProductoEscalar(float *a, float *b){
    float result=0;
    int i;
    for(i=0;i<3;i++){
        result+=a[i]*b[i];
    }
    
    return result;
}

float AnguloEntreVectores(float *a, float *b){
    int i;
    float mod_a,mod_b,angulo;
    mod_a=sqrt(ProductoEscalar(a,a));
    mod_b=sqrt(ProductoEscalar(b,b));
    
    angulo=acos(ProductoEscalar(a,b)/(mod_a*mod_b));
    return angulo;
}

void ProductoVectorial(float *a, float *b, float *result){
    
    result[0]=a[1]*b[2]-a[2]*b[1];
    result[1]=a[2]*b[0]-a[0]*b[2];
    result[2]=a[0]*b[1]-a[1]*b[0];
    
}

void AsignaVector(float *a, float *result){
    result[0]=a[0];
    result[1]=a[1];
    result[2]=a[2];
}

void Rotacion(float *EjeRotacion, float *Vector, float angulo){
    int i,j;
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
    
    for(i=0;i<3;i++){
        for(j=0;j<3;j++)
            Vector_auxiliar[i]+=MatrizRotacion[i][j]*Vector[j];
    }
    
    Vector[0]=Vector_auxiliar[0];
    Vector[1]=Vector_auxiliar[1];
    Vector[2]=Vector_auxiliar[2];
    
}

float Distancia(float **Rx,float **Ry, float **Rz,int m1, int n1,int m2, int n2){
    return(sqrt((Rx[m1][n1]-Rx[m2][n2])*(Rx[m1][n1]-Rx[m2][n2])+(Ry[m1][n1]-Ry[m2][n2])*(Ry[m1][n1]-Ry[m2][n2])+(Rz[m1][n1]-Rz[m2][n2])*(Rz[m1][n1]-Rz[m2][n2])));
}

void MedVar(float *distribucion, int N, float* media, float* desviacion)
{
    int i;
    float promedio=0;
    float varianza=0;
    
    
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

float ran1(long *idum){
    int j;
    long k;
    static long iy=0;
    static long iv[NTAB];
    float temp;
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

int Dado(int Ncaras)
{
    return (int)(ran1(&idum)*Ncaras);
}


///CONSTRUCCION DE ESTRUCTURAS////////////////////////////////////////
void GenerarEstructuras(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int index_simulacion)
{
    int i,j,k;
    char comando[8192];
    
    char CANAL_name[256];
    int *CANAL_residuo;
    int *CANAL_chgrp;
    int *CANAL_index;
    float **CANAL_X;
    float **CANAL_V;
    float CANAL_box[3];
    float CANAL_Angle[2];
    
    char POL_name[256];
    int *POL_residuo;
    int *POL_chgrp;
    int *POL_index;
    float **POL_X;
    float **POL_V;
    float POL_box[3];
    float POL_Angle[2];
    
    int DSPC_n=14;
    int *DSPC_residuo;
    char *DSPC_atomname;
    int *DSPC_chgrp;
    float **DSPC_X;
    float **DSPC_V;
    float DSPC_box[3];
    float DSPC_Angle[2];
    int MEMBRANA_n;
    int eliminados;
    int ***MEMBRANA_index_a;
    int ***MEMBRANA_index_b;
    float ****MEMBRANA_X_a;
    float ****MEMBRANA_X_b;
    
    int AtomNumber=0;
    char Configuracion_name[256];

    FILE* SISTEMA_nuevo;
    FILE *pipe;
    
    
    sprintf(Configuracion_name,"SISTEMA_conf_%d.gro",index_simulacion);
    SISTEMA_nuevo=fopen(Configuracion_name,"w");
    
    
    ///-----------------CANAL---------------------------------------------
    
    if(CANAL_punt->sel==0)
        CANAL_punt->n=1;
    
    
    CANAL_residuo=(int*)malloc(sizeof(int)*CANAL_punt->n);
    
    
    CANAL_chgrp=(int*)malloc(sizeof(int)*CANAL_punt->n);
    CANAL_index=(int*)malloc(sizeof(int)*CANAL_punt->n);
    
    CANAL_X=(float**)malloc(sizeof(float*)*CANAL_punt->n);
    for(i=0;i<CANAL_punt->n;i++)
        *(CANAL_X+i)=(float*)malloc(sizeof(float)*3);
    
    CANAL_V=(float**)malloc(sizeof(float*)*CANAL_punt->n);
    for(i=0;i<CANAL_punt->n;i++)
        *(CANAL_V+i)=(float*)malloc(sizeof(float)*3);
    
    
    if(CANAL_punt->sel==1){
        printf("\n\nPORO\n-----------------\n");
        
        ConstruyeCanal(CANAL_punt->radio,CANAL_punt->bond_reposo,CANAL_punt->bond_fuerza[index_simulacion],CANAL_punt->bending_ang,CANAL_punt->bending_fuerza[index_simulacion],CANAL_punt->Nb,CANAL_punt->NExterior,CANAL_punt->M,CANAL_punt->phi,CANAL_X,CANAL_V,CANAL_residuo,CANAL_chgrp,CANAL_box,CANAL_Angle,index_simulacion,CANAL_punt->Borde_poro,CANAL_punt->Cuerpo_poro);
        
        MoverEstructura(CANAL_X,CANAL_punt->n,0, 0, -0.4, -0.4, 0,CANAL_Angle);
    }
    
    for(i=0;i<CANAL_punt->n;i++){
        AtomNumber++;
        CANAL_index[i]=AtomNumber;
    }
    
    
    
    
    ///-----------------POLY---------------------------------------------
    POL_punt->n=(int)(POL_punt->longitud[index_simulacion]/POL_punt->bond_reposo);
    
    if(POL_punt->sel==0)
        POL_punt->n=1;
    
    POL_residuo=(int*)malloc(sizeof(int)*POL_punt->n);
    
    
    POL_chgrp=(int*)malloc(sizeof(int)*POL_punt->n);
    POL_index=(int*)malloc(sizeof(int)*POL_punt->n);
    
    POL_X=(float**)malloc(sizeof(float*)*(POL_punt->n));
    for(i=0;i<(POL_punt->n);i++)
        *(POL_X+i)=(float*)malloc(sizeof(float)*3);
    
    POL_V=(float**)malloc(sizeof(float*)*(POL_punt->n));
    for(i=0;i<(POL_punt->n);i++)
        *(POL_V+i)=(float*)malloc(sizeof(float)*3);
    
    
    
    if(POL_punt->sel==1){
        printf("\n\nPOLIMERO\n-----------------\n");
        
        ConstruyePolimero(POL_punt->n,POL_punt->bond_reposo,POL_punt->bond_fuerza,POL_punt->bending_ang,POL_punt->bending_fuerza,POL_X,POL_V,POL_residuo,POL_chgrp,POL_box,POL_Angle,index_simulacion,POL_punt->Borde_polimero,POL_punt->Cuerpo_polimero);
        
        MoverEstructura(POL_X,POL_punt->n,0, 0, -0.5, -0.5,POL_punt->posicion*POL_punt->bond_reposo*POL_punt->n*0.5,POL_Angle);
    }
    
    for(i=0;i<POL_punt->n;i++){
        AtomNumber++;
        POL_index[i]=AtomNumber;
        
    }
    
    
    
    
    ///-----------------MEMBRANA---------------------------------------------
    if(MEMBRANA_punt->sel==0)
        DSPC_n=1;
    
    
    MEMBRANA_punt->lado=sqrt(MEMBRANA_punt->area);
    MEMBRANA_n=(int)MEMBRANA_punt->lado;
    
    DSPC_residuo=(int*)malloc(sizeof(int)*DSPC_n);
    
    DSPC_atomname=(char*)malloc(sizeof(char)*DSPC_n);
    
    DSPC_chgrp=(int*)malloc(sizeof(int)*DSPC_n);
    
    DSPC_X=(float**)malloc(sizeof(float*)*DSPC_n);
    for(i=0;i<DSPC_n;i++)
        *(DSPC_X+i)=(float*)malloc(sizeof(float)*3);
    
    DSPC_V=(float**)malloc(sizeof(float*)*DSPC_n);
    for(i=0;i<DSPC_n;i++)
        *(DSPC_V+i)=(float*)malloc(sizeof(float)*3);
    
    MEMBRANA_index_a=(int***)malloc(sizeof(int**)*MEMBRANA_n);
    for(i=0;i<MEMBRANA_n;i++)
        MEMBRANA_index_a[i]=(int**)malloc(sizeof(int*)*MEMBRANA_n);
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            MEMBRANA_index_a[i][j]=(int*)malloc(sizeof(int)*14);
        }
    }
    
    MEMBRANA_index_b=(int***)malloc(sizeof(int**)*MEMBRANA_n);
    for(i=0;i<MEMBRANA_n;i++)
        MEMBRANA_index_b[i]=(int**)malloc(sizeof(int*)*MEMBRANA_n);
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            MEMBRANA_index_b[i][j]=(int*)malloc(sizeof(int)*14);
        }
    }
    
    
    
    MEMBRANA_X_a=(float****)malloc(sizeof(float***)*MEMBRANA_n);
    
    for(i=0;i<MEMBRANA_n;i++)
        MEMBRANA_X_a[i]=(float***)malloc(sizeof(float**)*MEMBRANA_n);
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++)
            MEMBRANA_X_a[i][j]=(float**)malloc(sizeof(float*)*DSPC_n);
    }
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            for(k=0;k<DSPC_n;k++)
                MEMBRANA_X_a[i][j][k]=(float*)malloc(sizeof(float)*3);
        }
    }
    
    MEMBRANA_X_b=(float****)malloc(sizeof(float***)*MEMBRANA_n);
    
    for(i=0;i<MEMBRANA_n;i++)
        MEMBRANA_X_b[i]=(float***)malloc(sizeof(float**)*MEMBRANA_n);
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++)
            MEMBRANA_X_b[i][j]=(float**)malloc(sizeof(float*)*DSPC_n);
    }
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            for(k=0;k<DSPC_n;k++)
                MEMBRANA_X_b[i][j][k]=(float*)malloc(sizeof(float)*3);
        }
    }
    
    
    
    if(MEMBRANA_punt->sel==1){
        printf("\n\nMEMBRANA\n-----------------\n");
        
        ConstruyeDSPC(DSPC_X,DSPC_V,DSPC_residuo,DSPC_chgrp,DSPC_box,DSPC_Angle,DSPC_atomname, 20,VARIABLE_punt->AGUA_sel,index_simulacion,VARIABLE_punt->campo_E);
        
        ///RELLENAR MEMBRANA_a:
        for(i=0;i<MEMBRANA_n;i++){
            for(j=0;j<MEMBRANA_n;j++){
                MoverEstructura(DSPC_X,DSPC_n,0,180*ran1(&idum)-90,0.7*(i-0.5*MEMBRANA_n),0.7*(j-0.5*MEMBRANA_n),1.6,DSPC_Angle);
                UbicarEnMembrana(MEMBRANA_X_a,DSPC_X,DSPC_n,i,j);
            }
        }
        
        
        ///RELLENAR MEMBRANA_b:
        for(i=0;i<MEMBRANA_n;i++){
            for(j=0;j<MEMBRANA_n;j++){
                MoverEstructura(DSPC_X,DSPC_n,180,180*ran1(&idum)-90,0.7*(i-0.5*MEMBRANA_n),0.7*(j-0.5*MEMBRANA_n),-1.6,DSPC_Angle);
                UbicarEnMembrana(MEMBRANA_X_b,DSPC_X,DSPC_n,i,j);
            }
        }
        
        
        AhuecarMembrana(MEMBRANA_X_a,MEMBRANA_n,CANAL_punt->radio*1.6);
        eliminados=AhuecarMembrana(MEMBRANA_X_b,MEMBRANA_n,CANAL_punt->radio*1.6);
        
        for(i=0;i<MEMBRANA_n;i++){
            for(j=0;j<MEMBRANA_n;j++){
                for(k=0;k<DSPC_n;k++){
                    if(MEMBRANA_X_a[i][j][0]!=NULL){
                        AtomNumber++;
                        MEMBRANA_index_a[i][j][k]=AtomNumber;
                    }
                    
                }
            }
        }
        
        
        
        for(i=0;i<MEMBRANA_n;i++){
            for(j=0;j<MEMBRANA_n;j++){
                for(k=0;k<DSPC_n;k++){
                    if(MEMBRANA_X_b[i][j][0]!=NULL){
                        AtomNumber++;
                        MEMBRANA_index_b[i][j][k]=AtomNumber;
                    }
                }
            }
        }
        
    }
    
    
    
    
    ///ESCRITURA DEL SISTEMA:
    MEMBRANA_punt->NumeroDSPC=2*MEMBRANA_punt->sel*(MEMBRANA_n*MEMBRANA_n-eliminados);
    VARIABLE_punt->SISTEMA_n=CANAL_punt->sel*CANAL_punt->n+POL_punt->sel*POL_punt->n+MEMBRANA_punt->NumeroDSPC*DSPC_n;
    
    fprintf(SISTEMA_nuevo,"%s\n","Sistema");
    
    fprintf(SISTEMA_nuevo,"%d\n",VARIABLE_punt->SISTEMA_n);
    
    if(CANAL_punt->sel==1){
        for(i=0;i<CANAL_punt->n;i++){
            if(i/CANAL_punt->M<CANAL_punt->NExterior || i/CANAL_punt->M+1>CANAL_punt->Nb-CANAL_punt->NExterior)
                fprintf(SISTEMA_nuevo,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",CANAL_residuo[i],"CHNNL",'G',CANAL_index[i],CANAL_X[i][0],CANAL_X[i][1],CANAL_X[i][2],CANAL_V[i][0],CANAL_V[i][1],CANAL_V[i][2]);
            else
                fprintf(SISTEMA_nuevo,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",CANAL_residuo[i],"CHNNL",'C',CANAL_index[i],CANAL_X[i][0],CANAL_X[i][1],CANAL_X[i][2],CANAL_V[i][0],CANAL_V[i][1],CANAL_V[i][2]);
        }
    }
    
    if(POL_punt->sel==1){
        for(i=0;i<POL_punt->n;i++){
            if(i==0)
                fprintf(SISTEMA_nuevo,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",1+CANAL_punt->sel*CANAL_residuo[CANAL_punt->n-1],"PEG",'H',POL_index[i],POL_X[i][0],POL_X[i][1],POL_X[i][2],POL_V[i][0],POL_V[i][1],POL_V[i][2]);
            else
                fprintf(SISTEMA_nuevo,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",1+CANAL_punt->sel*CANAL_residuo[CANAL_punt->n-1],"PEG",'C',POL_index[i],POL_X[i][0],POL_X[i][1],POL_X[i][2],POL_V[i][0],POL_V[i][1],POL_V[i][2]);
        }
    }
    
    if(MEMBRANA_punt->sel==1){
        for(j=0;j<MEMBRANA_n;j++){
            for(k=0;k<MEMBRANA_n;k++){
                if(MEMBRANA_X_a[j][k][0]!=NULL){
                    for(i=0;i<DSPC_n;i++)
                        fprintf(SISTEMA_nuevo,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",j*MEMBRANA_n+k+1+POL_punt->sel*POL_residuo[POL_punt->n-1]+CANAL_punt->sel*CANAL_residuo[CANAL_punt->n-1],"DSPC",DSPC_atomname[i],MEMBRANA_index_a[j][k][i],MEMBRANA_X_a[j][k][i][0],MEMBRANA_X_a[j][k][i][1],MEMBRANA_X_a[j][k][i][2],DSPC_V[i][0],DSPC_V[i][1],DSPC_V[i][2]);
                }
            }
        }
        
        for(j=0;j<MEMBRANA_n;j++){
            for(k=0;k<MEMBRANA_n;k++){
                if(MEMBRANA_X_b[j][k][0]!=NULL){
                    for(i=0;i<DSPC_n;i++)
                        fprintf(SISTEMA_nuevo,"%5d%-5s%5c%5d%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f\n",MEMBRANA_n*MEMBRANA_n+j*MEMBRANA_n+k+1+POL_punt->sel*POL_residuo[POL_punt->n-1]+CANAL_punt->sel*CANAL_residuo[CANAL_punt->n-1],"DSPC",DSPC_atomname[i],MEMBRANA_index_b[j][k][i],MEMBRANA_X_b[j][k][i][0],MEMBRANA_X_b[j][k][i][1],MEMBRANA_X_b[j][k][i][2],DSPC_V[i][0],DSPC_V[i][1],DSPC_V[i][2]);
                }
            }
        }
    }
    
    fprintf(SISTEMA_nuevo,"%f\t%f\t%f\n",POL_box[0]+CANAL_box[0]+MEMBRANA_n*DSPC_box[0],POL_box[1]+CANAL_box[1]+MEMBRANA_n*DSPC_box[1],POL_box[2]+CANAL_box[2]+MEMBRANA_n*DSPC_box[2]);
    
    
    
    ///ESCRITURA DE INDICES
    ConstruyeIndices(VARIABLE_punt->SISTEMA_n,CANAL_punt->n,CANAL_punt->M,CANAL_punt->Nb,CANAL_punt->bond_reposo, POL_punt->n,DSPC_n,MEMBRANA_punt->NumeroDSPC,index_simulacion,CANAL_punt->sel,POL_punt->sel,POL_punt->posicion,POL_punt->bond_reposo,MEMBRANA_punt->sel);
    printf("\nNumero de DSPC total= %d\n",MEMBRANA_punt->NumeroDSPC);
    
    
    
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
    
    
    
    free(POL_residuo);
    
    free(POL_chgrp);
    free(POL_index);
    
    for(i=0;i<(POL_punt->n);i++)
        free(*(POL_X+i));
    free(POL_X);
    
    for(i=0;i<(POL_punt->n);i++)
        free(*(POL_V+i));
    free(POL_V);
    
    
    
    free(DSPC_residuo);
    
    free(DSPC_atomname);
    
    free(DSPC_chgrp);
    
    for(i=0;i<DSPC_n;i++)
        free(*(DSPC_X+i));
    free(DSPC_X);
    
    for(i=0;i<DSPC_n;i++)
        free(*(DSPC_V+i));
    free(DSPC_V);
    
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            free(MEMBRANA_index_a[i][j]);
        }
    }
    for(i=0;i<MEMBRANA_n;i++)
        free(MEMBRANA_index_a[i]);
    free(MEMBRANA_index_a);
    
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            free(MEMBRANA_index_b[i][j]);
        }
    }
    for(i=0;i<MEMBRANA_n;i++)
        free(MEMBRANA_index_b[i]);
    free(MEMBRANA_index_b);
    
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            for(k=0;k<DSPC_n;k++)
                free(MEMBRANA_X_a[i][j][k]);
        }
    }
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++)
            free(MEMBRANA_X_a[i][j]);
    }
    
    
    for(i=0;i<MEMBRANA_n;i++)
        free(MEMBRANA_X_a[i]);
    
    free(MEMBRANA_X_a);
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            for(k=0;k<DSPC_n;k++)
                free(MEMBRANA_X_b[i][j][k]);
        }
    }
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++)
            free(MEMBRANA_X_b[i][j]);
    }
    
    
    for(i=0;i<MEMBRANA_n;i++)
        free(MEMBRANA_X_b[i]);
    
    free(MEMBRANA_X_b);
    
    
    fflush(SISTEMA_nuevo);
    fclose(SISTEMA_nuevo);
    
}


///Construccion del Canal

void ConstruyeCanal(float CANAL_radio, float CANAL_bond_reposo, float CANAL_bond_fuerza, float CANAL_bending_ang, float CANAL_bending_fuerza, int CANAL_Nb,int CANAL_NExterior, int CANAL_M, float CANAL_phi, float **CANAL_X, float **CANAL_V, int *CANAL_residuo, int *CANAL_chgrp, float *CANAL_box, float *CANAL_Angle, int index_simulacion,char* Borde_poro,char* Cuerpo_poro){
    
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
    
    
    ///Definicion de parametros del canal******************
    printf("\nNOTA: se han introducido los parametros:\nCANAL_bond_reposo=%.2f\nCANAL_radio=%.1f\nLongitud=%d\n\n",CANAL_bond_reposo,CANAL_radio,CANAL_Nb);
    printf("Estos parametros suponen:\nAmplitud_angular=%.2f\tBeads/vuelta=%d\n\n",CANAL_phi,CANAL_M);
    CANAL_phi=2*PI/CANAL_M;
    CANAL_radio=CANAL_bond_reposo*sqrt(1/(2*(1-cos(CANAL_phi))));
    printf("Para ajustar el cilindro, se modifican los parametros:\nAmplitud angular'=%.2f\tCANAL_radio'=%.3f\n\n",CANAL_phi,CANAL_radio);
    
    
    
    ///Asigancion de memoria al sistema******************
    Rx=(float**)malloc(sizeof(float*)*CANAL_M);
    Ry=(float**)malloc(sizeof(float*)*CANAL_M);
    Rz=(float**)malloc(sizeof(float*)*CANAL_M);
    for(i=0;i<CANAL_M;i++){
        *(Rx+i)=(float*)malloc(sizeof(float)*CANAL_Nb);
        *(Ry+i)=(float*)malloc(sizeof(float)*CANAL_Nb);
        *(Rz+i)=(float*)malloc(sizeof(float)*CANAL_Nb);
    }
    
    MDistancia=(float**)malloc(sizeof(float*)*CANAL_M*CANAL_Nb);
    for(i=0;i<(CANAL_M*CANAL_Nb);i++)
        *(MDistancia+i)=(float*)malloc(sizeof(float)*CANAL_M*CANAL_Nb);
    
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
    ConstruyeTopol_canal(CANAL_bond_reposo,CANAL_bond_fuerza,CANAL_bending_ang,CANAL_bending_fuerza,CANAL_M,CANAL_Nb,CANAL_NExterior,MEnlaces,MAngulos,index_simulacion,Borde_poro,Cuerpo_poro);
    
    
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

void ConstruyeDisco(float **Rx,float **Ry,float **Rz, float CANAL_bond_reposo,float CANAL_radio, float phi, int M, int i){
    int m;
    float phi_inicial=0;
    
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


void MatrizDistancia(float **MD,float **Rx,float **Ry, float **Rz,int M, int Nb){
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


void MatrizVecinos(int **MV,int M, int Nb,float **MatrizDistancia,float CANAL_bond_reposo){
    
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

void ConstruyeTopol_canal(float CANAL_bond_reposo,float CANAL_bond_fuerza, float CANAL_bending_ang, float CANAL_bending_fuerza,int M, int Nb,int NExterior,int **MEnlaces, int ***MAngulos, int index_simulacion, char* Borde_poro, char* Cuerpo_poro){
    
    int i,j,k;
    FILE *topol;
    char CanalTopol_name[256];
    
    sprintf(CanalTopol_name,"SISTEMA_canal_%d.itp",index_simulacion);
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


///Construccion del polimero
void ConstruyePolimero(int POL_n,float POL_bond_reposo, float POL_bond_fuerza,float POL_bending_ang,float POL_bending_fuerza,float **POL_X, float **POL_V, int *POL_residuo, int *POL_chgrp, float *POL_box, float *POL_Angle, int index_simulacion ,char* Borde_polimero, char* Cuerpo_polimero){
    int i;
    float VectorDirector[3];
    
    POL_Angle[0]=0.5*PI;
    POL_Angle[1]=0;
    
    
    for(i=0;i<POL_n;i++){
        POL_X[i][0]=i*POL_bond_reposo;
        POL_X[i][1]=0;
        POL_X[i][2]=0;
        
        POL_V[i][0]=0;
        POL_V[i][1]=0;
        POL_V[i][2]=0;
        
        POL_residuo[i]=1;
        
        POL_chgrp[i]=i+1;
    }
    
    POL_box[0]=1.1*POL_n;
    POL_box[1]=1;
    POL_box[2]=1;
    
    ConstruyeTopol_polimero(POL_n,POL_bond_reposo,POL_bond_fuerza,POL_bending_ang,POL_bending_fuerza,index_simulacion,Borde_polimero,Cuerpo_polimero);
}

void ConstruyeTopol_polimero(int POL_n,float POL_bond_reposo,float POL_bond_fuerza,float POL_bending_ang,float POL_bending_fuerza,int index_simulacion,char* Borde_polimero,char* Cuerpo_polimero){
    int i;
    FILE *POL_nuevo_topol;
    char PolTopol_name[256];
    
    sprintf(PolTopol_name,"SISTEMA_pol_%d.itp",index_simulacion);
    POL_nuevo_topol=fopen(PolTopol_name,"w");
    
    fprintf(POL_nuevo_topol,";cg PEG9_MOD\n");
    fprintf(POL_nuevo_topol,"[moleculetype]\n");
    fprintf(POL_nuevo_topol,";name\texclusiones\n");
    fprintf(POL_nuevo_topol,"PEG\t1\n");
    fprintf(POL_nuevo_topol,"[atoms]\n");
    fprintf(POL_nuevo_topol,";nr\tattype\tresnr\tresidue\tatom\tcgnr\tcharge\tmass\n");
    
    for(i=0;i<POL_n;i++){
        if(i==0)
            fprintf(POL_nuevo_topol,"%d\t%s\t%d\t%s\t%s\t%d\t%.3f\t;\n",i+1,Borde_polimero,1,"PEG","C",i+1,0.0);
        if(i>0 && i<POL_n-1)
            fprintf(POL_nuevo_topol,"%d\t%s\t%d\t%s\t%s\t%d\t%.3f\t;\n",i+1,Cuerpo_polimero,1,"PEG","C",i+1,0.0);
        if(i==POL_n-1)
            fprintf(POL_nuevo_topol,"%d\t%s\t%d\t%s\t%s\t%d\t%.3f\t;\n",i+1,Borde_polimero,1,"PEG","C",i+1,0.0);
    }
    
    fprintf(POL_nuevo_topol,"[bonds]\n");
    fprintf(POL_nuevo_topol,";ai\taj\tfunc\tb_0\tk_b\n");
    
    for(i=0;i<POL_n-1;i++)
        fprintf(POL_nuevo_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",i+1,i+2,1,POL_bond_reposo,POL_bond_fuerza);
    
    fprintf(POL_nuevo_topol,"[angles]\n");
    fprintf(POL_nuevo_topol,";ai\taj\tak\tfunc\ttheta_0\tk_theta\n");
    
    for(i=0;i<POL_n-2;i++)
        fprintf(POL_nuevo_topol,"%d\t%d\t%d\t%d\t%.2f\t%.2f\n",i+1,i+2,i+3,1,POL_bending_ang,POL_bending_fuerza);
    
    
    fprintf(POL_nuevo_topol,"[dihedrals]\n");
    fprintf(POL_nuevo_topol,";\n");
    
    for(i=0;i<POL_n-3;i++){
        fprintf(POL_nuevo_topol,"%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d\n",i+1,i+2,i+3,i+4,1,180.00,1.96,1);
        fprintf(POL_nuevo_topol,"%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d\n",i+1,i+2,i+3,i+4,1,0.0,0.18,2);
        fprintf(POL_nuevo_topol,"%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d\n",i+1,i+2,i+3,i+4,1,0.0,0.33,3);
        fprintf(POL_nuevo_topol,"%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d\n",i+1,i+2,i+3,i+4,1,0.0,0.12,4);
    }
    
    
    fclose(POL_nuevo_topol);
}



///Construccion de la membrana
void ConstruyeDSPC(float **DSPC_X, float **DSPC_V, int *DSPC_residuo, int *DSPC_chgrp, float *DSPC_box, float *DSPC_Angle, char *DSPC_atomname, float angulo,int AGUA_sel ,int index_simulacion, char* campoE){
    
    int i,DSPC_n=14;
    
    DSPC_Angle[0]=0.5*PI;
    DSPC_Angle[1]=0;
    
    angulo=angulo*PI/180;
    
    DSPC_X[0][0]=0;
    DSPC_X[0][1]=0;
    DSPC_X[0][2]=0;
    DSPC_atomname[0]='N';
    
    DSPC_X[1][0]=DSPC_X[0][0]+0.47;
    DSPC_X[1][1]=DSPC_X[0][1];
    DSPC_X[1][2]=DSPC_X[0][2];
    DSPC_atomname[1]='P';
    
    DSPC_X[2][0]=DSPC_X[1][0]+0.47*cos(angulo);
    DSPC_X[2][1]=DSPC_X[1][1]+0.47*sin(angulo);
    DSPC_X[2][2]=DSPC_X[1][2];
    DSPC_atomname[2]='G';
    
    DSPC_X[3][0]=DSPC_X[1][0]+0.47*cos(angulo);
    DSPC_X[3][1]=DSPC_X[1][1]-0.47*sin(angulo);
    DSPC_X[3][2]=DSPC_X[1][2];
    DSPC_atomname[3]='G';
    
    
    DSPC_X[4][0]=DSPC_X[2][0]+0.47*cos(angulo);
    DSPC_X[4][1]=DSPC_X[2][1]+0.47*sin(angulo);
    DSPC_X[4][2]=DSPC_X[2][2];
    DSPC_atomname[4]='C';
    
    for(i=5;i<9;i++){
        DSPC_X[i][0]=DSPC_X[i-1][0]+0.47;
        DSPC_X[i][1]=DSPC_X[i-1][1];
        DSPC_X[i][2]=DSPC_X[i-1][2];
        DSPC_atomname[i]='C';
    }
    
    DSPC_X[9][0]=DSPC_X[3][0]+0.47*cos(angulo);
    DSPC_X[9][1]=DSPC_X[3][1]-0.47*sin(angulo);
    DSPC_X[9][2]=DSPC_X[3][2];
    DSPC_atomname[9]='C';
    
    for(i=10;i<14;i++){
        DSPC_X[i][0]=DSPC_X[i-1][0]+0.47;
        DSPC_X[i][1]=DSPC_X[i-1][1];
        DSPC_X[i][2]=DSPC_X[i-1][2];
        DSPC_atomname[i]='C';
    }
    
    
    
    for(i=0;i<14;i++){
        DSPC_V[i][0]=0;
        DSPC_V[i][1]=0;
        DSPC_V[i][2]=0;
        
        DSPC_residuo[i]=1;
        
        DSPC_chgrp[i]=i+1;
    }
    
    DSPC_box[0]=5.0;
    DSPC_box[1]=1.0;
    DSPC_box[2]=1.0;
    
    //Construye topologia:
    ConstruyeTopol_DSPC(index_simulacion,AGUA_sel,campoE);
}


void UbicarEnMembrana(float ****MEMBRANA_X, float **DSPC_X, int DSPC_n, int i, int j){
    int k;
    
    for(k=0;k<DSPC_n;k++){
        MEMBRANA_X[i][j][k][0]=DSPC_X[k][0];
        MEMBRANA_X[i][j][k][1]=DSPC_X[k][1];
        MEMBRANA_X[i][j][k][2]=DSPC_X[k][2];
    }
}

int AsignaOrientacion_Membrana(int i, int j, int MEMBRANA_n){
    int k,l;
    int **Asignacion;
    int retorno;
    
    Asignacion=(int**)malloc(sizeof(int*)*MEMBRANA_n);
    for(k=0;k<MEMBRANA_n;k++)
        Asignacion[k]=(int*)malloc(sizeof(int)*MEMBRANA_n);
    
    
    Asignacion[0][0]=0;
    for(k=1;k<MEMBRANA_n;k++){
        if(Asignacion[0][k-1]==0)
            Asignacion[0][k]=1;
        
        if(Asignacion[0][k-1]==1)
            Asignacion[0][k]=0;
    }
    
    for(k=1;k<MEMBRANA_n;k++){
        if(Asignacion[k-1][0]==0)
            Asignacion[k][0]=1;
        
        if(Asignacion[k-1][0]==1)
            Asignacion[k][0]=0;
    }
    
    
    for(k=1;k<MEMBRANA_n;k++){
        for(l=1;l<MEMBRANA_n;l++){
            if(Asignacion[k-1][l]==0 && Asignacion[k][l-1]==0)
                Asignacion[k][l]=1;
            
            if(Asignacion[k-1][l]==1 && Asignacion[k][l-1]==1)
                Asignacion[k][l]=0;
        }
    }
    
    
    
    retorno=Asignacion[i][j];
    
    for(k=0;k<MEMBRANA_n;k++)
        free(Asignacion[k]);
    free(Asignacion);
    
    return retorno;
    
} ///Esta funcion devuelve 1 si el acido graso debe estar orientado en sentido +, y 0 si el sentido debe ser - (utilizar en el bucle de construccion de la membrana, "rellenar membrana")

int AhuecarMembrana(float ****MEMBRANA_X,int MEMBRANA_n,float radio){
    int i,j,k,l;
    float distancia;
    int eliminados=0;
    float Centro[2]={0,0};
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            Centro[0]+=MEMBRANA_X[i][j][0][0];
            Centro[1]+=MEMBRANA_X[i][j][0][1];
        }
    }
    
    Centro[0]=Centro[0]/(MEMBRANA_n*MEMBRANA_n);
    Centro[1]=Centro[1]/(MEMBRANA_n*MEMBRANA_n);
    
    for(i=0;i<MEMBRANA_n;i++){
        for(j=0;j<MEMBRANA_n;j++){
            distancia=sqrt((Centro[0]-MEMBRANA_X[i][j][0][0])*(Centro[0]-MEMBRANA_X[i][j][0][0])+(Centro[1]-MEMBRANA_X[i][j][0][1])*(Centro[1]-MEMBRANA_X[i][j][0][1]));
            if(distancia<radio){
                MEMBRANA_X[i][j][0]=NULL;
                eliminados++;
            }
        }
    }
    return eliminados;
}

void ConstruyeTopol_DSPC(int index_simulacion, int AGUA_sel,char* campoE){
    int i;
    FILE *dspc_topol;
    char DSPCTopol_name[256];
    
    sprintf(DSPCTopol_name,"SISTEMA_dspc_%d.itp",index_simulacion);
    dspc_topol=fopen(DSPCTopol_name,"w");
    
    fprintf(dspc_topol,";Molecula 'DSPC'[ S.J. Marrink, A.H. de Vries, A.E. Mark] para simulacion de traslocacion de DNA. TFM Guillermo Diez.\n\n");
    
    fprintf(dspc_topol,"[moleculetype]\n;molname\tnrexcl\nDSPC\t%d\n\n",1);
    fprintf(dspc_topol,"[atoms]\n;id\ttype\tresnr\tresidu\tatom\tcgnr\tcharge\n");
    
    if(strcmp(campoE,"si")==0)
    {
        fprintf(dspc_topol,"1\tQ0\t1\tDSPC\tNC3\t1\t0.0\n");
        fprintf(dspc_topol,"2\tQa\t1\tDSPC\tPO4\t2\t0.0\n");
    }
    else
    {
        fprintf(dspc_topol,"1\tQ0\t1\tDSPC\tNC3\t1\t1.0\n");
        fprintf(dspc_topol,"2\tQa\t1\tDSPC\tPO4\t2\t-1.0\n");
    }
    
    fprintf(dspc_topol,"3\tNa\t1\tDSPC\tGL1\t3\t0.0\n");
    fprintf(dspc_topol,"4\tNa\t1\tDSPC\tGL2\t4\t0.0\n");
    fprintf(dspc_topol,"5\tC1\t1\tDSPC\tC1A\t5\t0.0\n");
    fprintf(dspc_topol,"6\tC1\t1\tDSPC\tC2A\t6\t0.0\n");
    fprintf(dspc_topol,"7\tC1\t1\tDSPC\tC3A\t7\t0.0\n");
    fprintf(dspc_topol,"8\tC1\t1\tDSPC\tC4A\t8\t0.0\n");
    fprintf(dspc_topol,"9\tC1\t1\tDSPC\tC5A\t9\t0.0\n");
    fprintf(dspc_topol,"10\tC1\t1\tDSPC\tC1B\t10\t0.0\n");
    fprintf(dspc_topol,"11\tC1\t1\tDSPC\tC2B\t11\t0.0\n");
    fprintf(dspc_topol,"12\tC1\t1\tDSPC\tC3B\t12\t0.0\n");
    fprintf(dspc_topol,"13\tC1\t1\tDSPC\tC4B\t13\t0.0\n");
    fprintf(dspc_topol,"14\tC1\t1\tDSPC\tC5B\t14\t0.0\n\n");
    
    if(AGUA_sel==1)
    {
        fprintf(dspc_topol,"[bonds]\n;i\tj\tfunct\tlength\tforce.c.\n");
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",1,2,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",2,3,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",3,4,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",3,5,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",5,6,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",6,7,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",7,8,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",8,9,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",4,10,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",10,11,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",11,12,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n",12,13,1,0.47,1250.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%.3f\t%.3f\n\n",13,14,1,0.47,1250.0);
        
        fprintf(dspc_topol,"[angles]\n;i\tj\tk\tfunct\tangle\tforce.c.\n");
        
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",2,3,4,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",2,3,5,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",3,5,6,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",5,6,7,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",6,7,8,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",7,8,9,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",4,10,11,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",10,11,12,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",11,12,13,2,120.0,25.0);
        fprintf(dspc_topol,"%d\t%d\t%d\t%d\t%.3f\t%.3f\n",12,13,14,2,120.0,25.0);
    }
    
    if(AGUA_sel==0)
    {
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
        
    }
    
    
    fclose(dspc_topol);
    
}


///Desplazamiento de estructuras
void MoverEstructura(float **estructura,int N,float theta, float phi, float Rx, float Ry, float Rz, float *Angle){ ///Mueve una estructura a la orientacion (theta,phi), posicion R
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

void Calcula_CM(float **estructura, int N, float *CentroMasa){
    int i;
    
    CentroMasa[0]=0;
    CentroMasa[1]=0;
    CentroMasa[2]=0;
    
    
    for(i=0;i<N;i++){
        CentroMasa[0]+=estructura[i][0];
        CentroMasa[1]+=estructura[i][1];
        CentroMasa[2]+=estructura[i][2];
    }
    
    CentroMasa[0]=CentroMasa[0]/((float)N);
    CentroMasa[1]=CentroMasa[1]/((float)N);
    CentroMasa[2]=CentroMasa[2]/((float)N); ///Aqui esta calculado el centro de masas.
    
    for(i=0;i<N;i++){
        estructura[i][0]=estructura[i][0]-CentroMasa[0];
        estructura[i][1]=estructura[i][1]-CentroMasa[1];
        estructura[i][2]=estructura[i][2]-CentroMasa[2];
    }
}


///Construccion de indices y topologia
void ConstruyeIndices(int SISTEMA_n, int CANAL_n,int CANAL_M,int CANAL_Nb,float CANAL_bond_reposo, int POL_n,int DSPC_n ,int NumeroDSPC, int index_simulacion,int CANAL_sel,int POL_sel, float POL_posicion,float POL_bond_reposo, int MEMBRANA_sel){
    int i;
    int *N_pol_dentro;
    FILE *punte;
    char Indices_name[256];
    
    N_pol_dentro=(int*)malloc(sizeof(int)*POL_n);
    for(i=0;i<POL_n;i++)
    {
        if(fabs((POL_n-i)*POL_bond_reposo-0.5*(POL_n+1)*POL_bond_reposo+0.5*POL_posicion*POL_bond_reposo*POL_n)<CANAL_bond_reposo*0.5*CANAL_Nb)
            N_pol_dentro[i]=1;
        else
            N_pol_dentro[i]=-1;
    }
    
    sprintf(Indices_name,"SISTEMA_index_%d.ndx",index_simulacion);
    punte=fopen(Indices_name,"w");
    
    if(CANAL_sel!=0)
    {
        fprintf(punte, "[CHNNL]\n");
        for(i=0;i<CANAL_n;i++)
            fprintf(punte,"%d\n",i+1);
    }
    
    if(POL_sel!=0)
    {
        fprintf(punte, "\n[PEG]\n");
        for(i=0;i<POL_n;i++)
            fprintf(punte,"%d\n",CANAL_sel*CANAL_n+i+1);
    }
    
    if(MEMBRANA_sel!=0)
    {
        fprintf(punte, "\n[DSPC]\n");
        for(i=0;i<NumeroDSPC*DSPC_n;i++)
            fprintf(punte,"%d\n",CANAL_sel*CANAL_n+POL_sel*POL_n+i+1);
    }
    
    if(POL_sel!=0)
    {
        fprintf(punte, "\n[PULLING]\n");
        fprintf(punte,"%d\n",CANAL_sel*CANAL_n+1);
    }
    
    
    fprintf(punte, "\n[SECO]\n");
    for(i=0;i<SISTEMA_n;i++)
        fprintf(punte,"%d\n",i+1);
    
    if(CANAL_sel!=0 || POL_sel!=0)
    {
        fprintf(punte, "\n[ANALISIS]\n");
        for(i=0;i<CANAL_M;i++)
            fprintf(punte,"%d\n",i+1);
        fprintf(punte,"%d\n",CANAL_sel*CANAL_n+POL_sel*POL_n);
    }
    
    if(CANAL_sel!=0 || POL_sel!=0)
    {
        fprintf(punte, "\n[CONTRAPESO]\n");
        for(i=0;i<CANAL_n;i++)
            fprintf(punte,"%d\n",i+1);
        for(i=0;i<NumeroDSPC*DSPC_n;i++)
            fprintf(punte,"%d\n",CANAL_sel*CANAL_n+POL_sel*POL_n+i+1);
    }
    
    if(POL_sel!=0)
    {
        fprintf(punte, "\n[DENTRO]\n");
        for(i=0;i<POL_n;i++)
        {
            if(N_pol_dentro[i]==1)
                fprintf(punte,"%d\n",CANAL_sel*CANAL_n+i+1);
        }
        
    }
    
    if(POL_sel!=0)
    {
        fprintf(punte, "\n[FUERA]\n");
        for(i=0;i<POL_n;i++)
        {
            if(N_pol_dentro[i]==-1)
                fprintf(punte,"%d\n",CANAL_sel*CANAL_n+i+1);
        }
    }
    
    fclose(punte);
    free(N_pol_dentro);
}

void EscribeFicheroTopol(int NumeroDSPC, int NumeroW, int index_simulacion, int CANAL_sel, int POL_sel, int MEMBRANA_sel){
    FILE *topol;
    char Topol_name[256];
    
    sprintf(Topol_name,"topol_%d.top",index_simulacion);
    
    topol=fopen(Topol_name,"w");
    
    if(NumeroW>0)
        fprintf(topol,"#include %cmartini_v2.2.itp%c\n",34,34);
    if(NumeroW==0)
        fprintf(topol,"#include %cdry_martini_v2.1.itp%c\n",34,34);
    if(CANAL_sel!=0)
        fprintf(topol,"#include %cSISTEMA_canal_%d.itp%c\n",34,index_simulacion,34);
    if(POL_sel!=0)
        fprintf(topol,"#include %cSISTEMA_pol_%d.itp%c\n",34,index_simulacion,34);
    if(MEMBRANA_sel!=0)
        fprintf(topol,"#include %cSISTEMA_dspc_%d.itp%c\n\n",34,index_simulacion,34);
    
    fprintf(topol,"[system]\nCanalProteico\n\n[molecules]\n");
    if(CANAL_sel!=0)
        fprintf(topol,"CANAL\t1\n");
    if(POL_sel!=0)
        fprintf(topol,"PEG\t1\n");
    if(MEMBRANA_sel!=0)
        fprintf(topol,"DSPC\t%d",NumeroDSPC);
    
    if(NumeroW>0)
        fprintf(topol,"\nW\t%d",NumeroW);
    
    
    fclose(topol);
    
}


///CONFIGURACION DEL EXPERIMENTO//////////////////////////////
void Genconf()
{
    int i,j, k, l, m, n;
    FILE* genconf;
    FILE* Experimento;
    FILE* pipe;
    
    int N_pull_rate, N_pull_fuerza, N_E_frecuencia, N_bond_fuerza, N_bending_fuerza, N_pol_longitud, every_out;
    int N_estadistica;
    float term_tol_derivada;
    float term_tolerancia;
    int N_term;
    int N_Lterm;
    int N_CPU;
    int N_nodo=0;
    int *nodo;
    float N_sim_cte;
    float dt;
    
    char AGUA[256];
    char CANAL[256];
    char POLIMERO[256];
    char MEMBRANA[256];
    char pulling[256];
    char PintaTrayectoria[256];
    char PintaPoro[256];
    char Borde_poro[256];
    char Cuerpo_poro[256];
    char Cuerpo_polimero[256];
    char Borde_polimero[256];
    char Acoplo_presion[256];
    char pulling_modo[256];
    char conf_externa[256];
    int primercand, ultimocand;
    char campo_E[256];
    char Auto_genconf[256];
    char basura[256];
    
    float CANAL_bond_fuerza_ini;
    float CANAL_bond_fuerza_fin;
    float d_CANAL_bond_fuerza;
    
    float CANAL_bond_reposo;
    
    float CANAL_bending_fuerza_ini;
    float CANAL_bending_fuerza_fin;
    float d_CANAL_bending_fuerza;
    
    float CANAL_bending_ang;
    
    float POL_longitud_ini;
    float POL_longitud_fin;
    float d_POL_longitud;
    
    float pull_rate_ini;
    float pull_rate_fin;
    float d_pull_rate;
    
    float pull_fuerza_ini;
    float pull_fuerza_fin;
    float d_pull_fuerza;
    
    float Ezfrecuencia_ini;
    float Ezfrecuencia_fin;
    float d_Ezfrecuencia;
    
    float Ezintensidad;
    float Exintensidad;
    float Eyintensidad;
    
    float CANAL_radio;
    float POL_posicion;
    float POL_bond_fuerza;
    float POL_bond_reposo;
    float POL_bending_fuerza;
    float POL_bending_ang;
    float MEMBRANA_area;
    float pull_k,temperatura,tau_t;
    float CANAL_long_int;
    int CANAL_n_ext;
    
    
    pipe=popen("bash","w");
    fprintf(pipe,"mkdir -p ./Input\n");
    fflush(pipe);
    pclose(pipe);
    
    
    Experimento=fopen("ExperimentoTraslocacionVE.txt","r");
    
    fscanf(Experimento,"%s%s",basura,basura);
    
    for(i=0;i<1;i++)
    {
        basura[0]=fgetc(Experimento);
        if(basura[0]==' ')
            N_nodo++;
        if(basura[0]=='\n')
            break;
        i=-1;
        
    }
    
    fseek(Experimento,0,SEEK_SET);
    
    nodo=(int*)malloc(sizeof(int)*N_nodo);
    
    fscanf(Experimento,"%s%s",basura,basura);
    for(i=0;i<N_nodo;i++)
        fscanf(Experimento,"%d",&nodo[i]);
    
    fscanf(Experimento,"%s%s%d",basura,basura,&N_CPU);
    
    fscanf(Experimento,"%s%s%d",basura,basura,&N_estadistica);
    
    fscanf(Experimento,"%s%s%s",basura,basura,PintaTrayectoria);
    fscanf(Experimento,"%s%s%s",basura,basura,PintaPoro);
    fscanf(Experimento, "%s%s%d", basura, basura, &every_out);
    fscanf(Experimento,"%s%s%s",basura,basura,conf_externa);
    fscanf(Experimento,"%s%s%d%d",basura,basura,&primercand,&ultimocand);
    
    
    fscanf(Experimento,"%s%s%s",basura,basura,CANAL);
    fscanf(Experimento,"%s%s%s",basura,basura,POLIMERO);
    fscanf(Experimento,"%s%s%s",basura,basura,MEMBRANA);
    fscanf(Experimento,"%s%s%s",basura,basura,AGUA);
    
    fscanf(Experimento,"%s%s%f",basura,basura,&CANAL_radio);
    fscanf(Experimento,"%s%s%f",basura,basura,&CANAL_long_int);
    fscanf(Experimento,"%s%s%d",basura,basura,&CANAL_n_ext);
    fscanf(Experimento,"%s%s%f",basura,basura,&POL_posicion);
    fscanf(Experimento,"%s%s%f",basura,basura,&POL_bond_fuerza);
    fscanf(Experimento,"%s%s%f",basura,basura,&POL_bond_reposo);
    fscanf(Experimento,"%s%s%f",basura,basura,&POL_bending_fuerza);
    fscanf(Experimento,"%s%s%f",basura,basura,&POL_bending_ang);
    fscanf(Experimento,"%s%s%f",basura,basura,&MEMBRANA_area);
    
    fscanf(Experimento,"%s%s%s",basura,basura,Borde_poro);
    fscanf(Experimento,"%s%s%s",basura,basura,Cuerpo_poro);
    fscanf(Experimento,"%s%s%s",basura,basura,Borde_polimero);
    fscanf(Experimento,"%s%s%s",basura,basura,Cuerpo_polimero);
    
    fscanf(Experimento,"%s%s%f",basura,basura,&temperatura);
    fscanf(Experimento,"%s%s%f",basura,basura,&tau_t);
    fscanf(Experimento,"%s%s%s",basura,basura,Acoplo_presion);
    
    //fscanf(Experimento,"%s%s%f",basura,basura,&term_tol_derivada);
	term_tol_derivada=0;
    //fscanf(Experimento,"%s%s%f",basura,basura,&term_tolerancia);
	term_tolerancia=0;
    fscanf(Experimento,"%s%s%d",basura,basura,&N_term);
    //fscanf(Experimento,"%s%s%d",basura,basura,&N_Lterm);
	N_Lterm=0;
    fscanf(Experimento,"%s%s%f",basura,basura,&N_sim_cte);
    fscanf(Experimento,"%s%s%f",basura,basura,&dt);
    
    fscanf(Experimento,"%s%s%s",basura,basura,pulling);
    fscanf(Experimento,"%s%s%s",basura,basura,pulling_modo);
    fscanf(Experimento,"%s%s%f",basura,basura,&pull_k);
    fscanf(Experimento,"%s%s%f",basura,basura,&pull_rate_ini);
    fscanf(Experimento,"%s%s%f",basura,basura,&pull_rate_fin);
    fscanf(Experimento,"%s%s%d",basura,basura,&N_pull_rate);
    fscanf(Experimento,"%s%s%f",basura,basura,&pull_fuerza_ini);
    fscanf(Experimento,"%s%s%f",basura,basura,&pull_fuerza_fin);
    fscanf(Experimento,"%s%s%d",basura,basura,&N_pull_fuerza);
    
    fscanf(Experimento,"%s%s%s",basura,basura,campo_E);
    fscanf(Experimento,"%s%s%f",basura,basura,&Ezintensidad);
    fscanf(Experimento,"%s%s%f",basura,basura,&Exintensidad);
    fscanf(Experimento,"%s%s%f",basura,basura,&Eyintensidad);
    fscanf(Experimento,"%s%s%f",basura,basura,&Ezfrecuencia_ini);
    fscanf(Experimento,"%s%s%f",basura,basura,&Ezfrecuencia_fin);
    fscanf(Experimento,"%s%s%d",basura,basura,&N_E_frecuencia);
    
    fscanf(Experimento,"%s%s%f",basura,basura,&CANAL_bond_fuerza_ini);
    fscanf(Experimento,"%s%s%f",basura,basura,&CANAL_bond_fuerza_fin);
    fscanf(Experimento,"%s%s%d",basura,basura,&N_bond_fuerza);
    
    fscanf(Experimento,"%s%s%f",basura,basura,&CANAL_bond_reposo);
    
    fscanf(Experimento,"%s%s%f",basura,basura,&CANAL_bending_fuerza_ini);
    fscanf(Experimento,"%s%s%f",basura,basura,&CANAL_bending_fuerza_fin);
    fscanf(Experimento,"%s%s%d",basura,basura,&N_bending_fuerza);
    
    fscanf(Experimento,"%s%s%f",basura,basura,&CANAL_bending_ang);
    
    fscanf(Experimento,"%s%s%f",basura,basura,&POL_longitud_ini);
    POL_longitud_ini=(POL_longitud_ini+0.001)*POL_bond_reposo;
    
    fscanf(Experimento,"%s%s%f",basura,basura,&POL_longitud_fin);
    POL_longitud_fin=(POL_longitud_fin+0.001)*POL_bond_reposo;
    
    fscanf(Experimento,"%s%s%d",basura,basura,&N_pol_longitud);
    
    fscanf(Experimento,"%s%s%s",basura,basura,Auto_genconf);
    
    fclose(Experimento);
    
    
    if(strcmp(Auto_genconf,"no")==0)
        return;
    
    
    pull_rate_ini=pull_rate_ini*POL_bond_reposo;
    pull_rate_fin=pull_rate_fin*POL_bond_reposo;
    
    genconf=fopen("./Input/genconf.txt","w");
    
    
    
    if(N_bond_fuerza>1) d_CANAL_bond_fuerza=(CANAL_bond_fuerza_fin-CANAL_bond_fuerza_ini)/(N_bond_fuerza-1);
    else d_CANAL_bond_fuerza=1;

    if(N_bending_fuerza>1) d_CANAL_bending_fuerza=(CANAL_bending_fuerza_fin-CANAL_bending_fuerza_ini)/(N_bending_fuerza-1);
    else d_CANAL_bending_fuerza=1;

    if(N_pull_rate>1) d_pull_rate=(pull_rate_fin-pull_rate_ini)/(N_pull_rate-1);
    else d_pull_rate=1;

    if(N_pull_fuerza>1) d_pull_fuerza=(pull_fuerza_fin-pull_fuerza_ini)/(N_pull_fuerza-1);
    else d_pull_fuerza=1;

    if(N_E_frecuencia>1) d_Ezfrecuencia=(Ezfrecuencia_fin-Ezfrecuencia_ini)/(N_E_frecuencia-1);
    else d_Ezfrecuencia=1;

    if(N_pol_longitud>1) d_POL_longitud=(POL_longitud_fin-POL_longitud_ini)/(N_pol_longitud-1);
    else d_POL_longitud=1;
    
    
    fprintf(genconf,"N_nodo\t=\t%d\n",N_nodo);
    fprintf(genconf,"Nodo\t=");
    for(i=0;i<N_nodo;i++)
        fprintf(genconf,"\t%d",nodo[i]);
    fprintf(genconf,"\n");
    fprintf(genconf,"N_CPU\t=\t%d\n",N_CPU);
    
    fprintf(genconf, "N_simulacion = %d\n",N_bond_fuerza*N_bending_fuerza*N_pull_rate*N_pull_fuerza*N_E_frecuencia*N_pol_longitud);
    
    fprintf(genconf,"N_estadistica\t=\t%d\n",N_estadistica);
    
    fprintf(genconf,"term_tol_derivada\t=\t%f\n",term_tol_derivada);
    fprintf(genconf,"term_tolerancia\t=\t%f\n",term_tolerancia);
    fprintf(genconf,"N_term\t=\t%d\n",N_term);
    fprintf(genconf,"N_Lterm\t=\t%d\n",N_Lterm);
    fprintf(genconf,"N_sim_cte\t=\t%f\n",N_sim_cte);
    fprintf(genconf,"dt\t=\t%f\n",dt);
    
    fprintf(genconf,"PintarTrayectoria\t=\t%s\n",PintaTrayectoria);
    fprintf(genconf,"PintarPoro\t=\t%s\n",PintaPoro);
	fprintf(genconf,"every_out\t=\t%d\n", every_out);
    fprintf(genconf,"ConfigExterna\t=\t%s\n",conf_externa);
    fprintf(genconf,"candidatos\t=\t%d\t%d\n", primercand, ultimocand);
    
    fprintf(genconf,"AcoploPresion\t=\t%s\n",Acoplo_presion);
    
    fprintf(genconf,"BordePoro\t=\t%s\n",Borde_poro);
    fprintf(genconf,"CuerpoPoro\t=\t%s\n",Cuerpo_poro);
    fprintf(genconf,"BordePolimero\t=\t%s\n",Borde_polimero);
    fprintf(genconf,"CuerpoPolimero\t=\t%s\n",Cuerpo_polimero);
    
    fprintf(genconf,"Pulling\t=\t%s\n",pulling);
    fprintf(genconf,"Pulling_modo\t=\t%s\n",pulling_modo);
    
    fprintf(genconf,"Pull_rate\t=");
    
    for(i=0; i<N_bond_fuerza; i++)
    {
	for(j=0; j<N_bending_fuerza; j++)
	{
	    for(k=0; k<N_pull_rate; k++)
	    {
		for(l=0; l<N_pull_fuerza; l++)
		{
		    for(m=0; m<N_E_frecuencia; m++)
		    {
			for(n=0; n<N_pol_longitud; n++)
			    fprintf(genconf,"\t%f",pull_rate_ini+d_pull_rate*k);
		    }
		}
	    }
	}
    }
    
    fprintf(genconf,"\n");
    
    fprintf(genconf,"Pull_fuerza\t=");
    
    for(i=0; i<N_bond_fuerza; i++)
    {
        for(j=0; j<N_bending_fuerza; j++)
        {
            for(k=0; k<N_pull_rate; k++)
            {
                for(l=0; l<N_pull_fuerza; l++)
                {
                    for(m=0; m<N_E_frecuencia; m++)
                    {
                        for(n=0; n<N_pol_longitud; n++)
                            fprintf(genconf,"\t%f",pull_fuerza_ini+d_pull_fuerza*l);
                    }
                }
            }
        }
    }
    
    fprintf(genconf,"\n");
    
    fprintf(genconf,"Pull_k\t=\t%f\n",pull_k);
    fprintf(genconf,"Temperatura\t=\t%f\n",temperatura);
    fprintf(genconf,"tau_T\t=\t%f\n",tau_t);
    
    fprintf(genconf,"AGUA\t=\t%s\n",AGUA);
    
    fprintf(genconf,"CANAL\t=\t%s\n",CANAL);
    
    fprintf(genconf,"CANAL_radio\t=\t%f\n",CANAL_radio);
    
    fprintf(genconf,"CANAL_longitud_Interior\t=\t%f\n",CANAL_long_int);
    
    fprintf(genconf,"CANAL_longitudExterior\t=\t%f\n",CANAL_bond_reposo*CANAL_n_ext);
    
    fprintf(genconf,"CANAL_bond_fuerza\t=");
    
    for(i=0; i<N_bond_fuerza; i++)
    {
        for(j=0; j<N_bending_fuerza; j++)
        {
            for(k=0; k<N_pull_rate; k++)
            {
                for(l=0; l<N_pull_fuerza; l++)
                {
                    for(m=0; m<N_E_frecuencia; m++)
                    {
                        for(n=0; n<N_pol_longitud; n++)
                            fprintf(genconf,"\t%f",CANAL_bond_fuerza_ini+d_CANAL_bond_fuerza*i);
                    }

                }

            }

        }

    }
    
    fprintf(genconf,"\n");
    
    fprintf(genconf,"CANAL_bond_reposo\t=\t%f\n",CANAL_bond_reposo);
    
    fprintf(genconf,"CANAL_bending\t=");
    
    for(i=0; i<N_bond_fuerza; i++)
    {
        for(j=0; j<N_bending_fuerza; j++)
        {
            for(k=0; k<N_pull_rate; k++)
            {
                for(l=0; l<N_pull_fuerza; l++)
                {
                    for(m=0; m<N_E_frecuencia; m++)
                    {
                        for(n=0; n<N_pol_longitud; n++)
                            fprintf(genconf,"\t%f",CANAL_bending_fuerza_ini+d_CANAL_bending_fuerza*j);
                    }
                }
            }
        }
    }
    
    fprintf(genconf,"\n");
    
    fprintf(genconf,"CANAL_bending_ang\t=\t%f\n",CANAL_bending_ang);
    
    fprintf(genconf,"POLIMERO\t=\t%s\n",POLIMERO);
    
    fprintf(genconf,"POL_bond_fuerza\t=\t%f\n",POL_bond_fuerza);
    
    fprintf(genconf,"POL_bond_reposo\t=\t%f\n",POL_bond_reposo);
    
    
    fprintf(genconf,"POL_bending\t=\t%f\n",POL_bending_fuerza);
    
    fprintf(genconf,"POL_bending_ang\t=\t%f\n",POL_bending_ang);
    
    
    fprintf(genconf,"POL_longitud\t=");
    
    for(i=0; i<N_bond_fuerza; i++)
    {
        for(j=0; j<N_bending_fuerza; j++)
        {
            for(k=0; k<N_pull_rate; k++)
            {
                for(l=0; l<N_pull_fuerza; l++)
                {
                    for(m=0; m<N_E_frecuencia; m++)
                    {
                        for(n=0; n<N_pol_longitud; n++)
                            fprintf(genconf,"\t%f",POL_longitud_ini+d_POL_longitud*n);
                    }
                }
            }
        }
    }

    fprintf(genconf,"\n");
    
    
    fprintf(genconf,"POL_posicion\t=\t%f\n",POL_posicion);
    
    
    fprintf(genconf,"MEMBRANA\t=\t%s\n",MEMBRANA);
    
    fprintf(genconf,"MEMBRANA_area\t=\t%f\n",MEMBRANA_area);
    
    fprintf(genconf,"Campo_E\t=\t%s\n",campo_E);
    fprintf(genconf,"Ezfrecuencia\t=");

    for(i=0; i<N_bond_fuerza; i++)
    {
        for(j=0; j<N_bending_fuerza; j++)
        {
            for(k=0; k<N_pull_rate; k++)
            {
                for(l=0; l<N_pull_fuerza; l++)
                {
                    for(m=0; m<N_E_frecuencia; m++)
                    {
                        for(n=0; n<N_pol_longitud; n++)
                            fprintf(genconf,"\t%f",Ezfrecuencia_ini+d_Ezfrecuencia*m);
                    }
                }
            }
        }
    }

    fprintf(genconf,"\n");
    
    fprintf(genconf,"Ezintensidad_z\t=\t%f\n",Ezintensidad);
    fprintf(genconf,"Ezintensidad_x\t=\t%f\n",Exintensidad);
    fprintf(genconf,"Ezintensidad_y\t=\t%f\n",Eyintensidad);
    
    
    fclose(genconf);
    
    
    return;
    
    
}


///ANALISIS/////////////////////////////////////////////////////////////////////////
float CalculaRadioGiro(int index_simulacion, char* Carpeta, char* GROfile)
{
    int i,j;
    int N_particulas=0;
    float** Coordenadas;
    float RadioVector_medio[3]={0,0,0};
    float Delta_RadioVector[3];
    float radio_giro=0;
    char coord_data_name[256];
    char index_name[256];
    char seeker[256];
    char basura[256];
    char flag0='a', flag1='b';
    int indice_index;
    int indice_particula;
    FILE *coord_data;
    FILE * index;
    
    sprintf(index_name,"SISTEMA_index_%d.ndx",index_simulacion);
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
    
    Coordenadas=(float**)malloc(sizeof(float*)*N_particulas);
    for(i=0;i<N_particulas;i++)
        Coordenadas[i]=(float*)malloc(sizeof(float)*3);
    
    fclose(index);
    
    sprintf(coord_data_name,"%s/%s_%d.gro",Carpeta,GROfile,index_simulacion);
    coord_data=fopen(coord_data_name,"r");
    fscanf(coord_data,"%s%s",basura,basura);
    
    for(i=0;i<1;i++)
    {
        fscanf(coord_data,"%s%s%d%s%s%s%s%s%s",basura,basura,&indice_particula,basura,basura,basura,basura,basura,basura);
        if(indice_particula==indice_index-1)
        {
            for(j=0;j<N_particulas;j++)
                fscanf(coord_data,"%s%s%s%f%f%f%s%s%s",basura,basura,basura,&Coordenadas[j][0],&Coordenadas[j][1],&Coordenadas[j][2],basura,basura,basura);
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

float TiempoTraslocacion(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int index_simulacion, int index_estad)
{
    int i,j;
    int frames;
    char tiempo_extra_name[256];
    char fichero_analisis[256];
    char basura[256];
    char comando[8192];
    FILE *pipe;
    FILE* read_analisis,*tiempo_extra_file;
    float CM[3];
    int CANAL_M;
    float X_marcador,Y_marcador,Z_marcador;
    float X,Y,Z;
    float tiempo;
    float tiempo_previo;
    float tiempo_sim=0;
    float tiempo_extra;
    float CANAL_radio;
    
    tiempo_extra=VARIABLE_punt->tiempo_extra;
    CANAL_radio=CANAL_punt->radio;
    
    
    frames=0;
    sprintf(fichero_analisis,"traj_analisis_%d_%d.gro",index_simulacion,index_estad);
    read_analisis=fopen(fichero_analisis,"r");
    
    for(i=0;i<1;i++)
    {
        if(fgetc(read_analisis)=='=')
            frames++;
        if(feof(read_analisis)!=0)
            break;
        i=-1;
    }
    
    fseek(read_analisis,0,SEEK_SET);
    for(j=0;j<frames;j++)
    {
        CM[0]=0;
        CM[1]=0;
        CM[2]=0;
        tiempo_previo=tiempo;
        
        //Leo el tiempo:
        for(i=0;i<1;i++)
        {
            if(fgetc(read_analisis)=='=')
            {
                fscanf(read_analisis,"%f",&tiempo);
                break;
            }
            i=-1;
        }
        
        //Posicion del CM del disco:
        fscanf(read_analisis,"%d",&CANAL_M);
        CANAL_M=CANAL_M-1;
        for(i=0;i<CANAL_M;i++)
        {
            fscanf(read_analisis,"%s%s%s",basura,basura,basura);
            fscanf(read_analisis,"%f%f%f",&X,&Y,&Z);
            if(j==0 || j==frames-1)
                fscanf(read_analisis,"%s%s%s",basura,basura,basura);
            
            CM[0]+=X;
            CM[1]+=Y;
            CM[2]+=Z;
        }
        
        CM[0]=CM[0]/CANAL_M;
        CM[1]=CM[1]/CANAL_M;
        CM[2]=CM[2]/CANAL_M;
        
        //Posicion del marcador:
        fscanf(read_analisis,"%s%s%s",basura,basura,basura);
        fscanf(read_analisis,"%f%f%f",&X_marcador,&Y_marcador,&Z_marcador);
        
        if(tiempo>0 && Z_marcador>CM[2] && X_marcador<CM[0]+CANAL_radio && X_marcador>CM[0]-CANAL_radio && Y_marcador<CM[1]+CANAL_radio && Y_marcador>CM[1]-CANAL_radio) //Ha traslocado
        {
            tiempo_sim=0.5*(tiempo+tiempo_previo);
            break;
        }
    }
    fclose(read_analisis);
    
    if(fork()==0)
    {
        sprintf(comando,"rm traj_analisis_%d_%d.gro\n",index_simulacion,index_estad);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    if(Igual(tiempo_sim,0)==1)
        return 0;
    else
        return tiempo_sim+tiempo_extra;
}


void AnalisisDeResultados(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt)
{
    int i,j;
    int index_simulacion,index_estad;
    int N_lineas=0;
    int **puntero_pullx_basura;
    int **puntero_pullf_basura;
    float* estadistica_vel,*estadistica_tiempo,*estadistica_pos;
    float tiempo_trans;
    float vel_medio,tiempo_medio,fuerza_medio;
    float error_vel,error_tiempo,error_fuerza;
    float POL_long_trans;
    float x,y,xf,xp,yf,yp,fuerza_inst=0;
    float *array_x;
    float *array_W;
    float **array_fuerza;
    float **array_fuerza_media;
    float **array_fuerza_error;
    float **array_pos;
    float **array_pos_media;
    float **array_pos_error;
    int fuerza_contador=0;
    char flag;
    char comando[8192];
    char write_analisis_name[256];
    char traj_analisis_name[256];
    char read_analisis_name[256];
    char read_fuerza_name[256];
    char read_pos_name[256];
    FILE *write_analisis,*read_analisis,*pipe,*read_fuerza,*write_trayectoria,*read_pos;
    
    puntero_pullf_basura=(int**)malloc(sizeof(int*)*VARIABLE_punt->N_simulaciones);
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        puntero_pullf_basura[index_simulacion]=(int*)malloc(sizeof(int)*VARIABLE_punt->N_estadistica);
    
    puntero_pullx_basura=(int**)malloc(sizeof(int*)*VARIABLE_punt->N_simulaciones);
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        puntero_pullx_basura[index_simulacion]=(int*)malloc(sizeof(int)*VARIABLE_punt->N_estadistica);
    
    
    printf("\n");
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
    {
        for(index_estad=0;index_estad<VARIABLE_punt->N_estadistica;index_estad++)
        {
            sprintf(read_fuerza_name,"pullf_%d_%d.xvg",index_simulacion,index_estad);
            read_fuerza=fopen(read_fuerza_name,"r");
            for(i=0;i<1;i++)
            {
                flag=fgetc(read_fuerza);
                if(flag=='#' || flag=='@')
                {
                    for(j=0;j<1;j++)
                    {
                        flag=fgetc(read_fuerza);
                        if(flag=='\n')
                            break;
                        j=-1;
                    }
                }
                else
                {
                    puntero_pullf_basura[index_simulacion][index_estad]=ftell(read_fuerza)-1;
                    for(j=0;j<1;j++)
                    {
                        flag=fgetc(read_fuerza);
                        if(flag=='\n' && index_simulacion==0 && index_estad==0)
                            N_lineas++;
                        if(feof(read_fuerza)!=0)
                            break;
                        j=-1;
                    }
                    break;
                }
                i=-1;
            }
            fclose(read_fuerza);
            
            sprintf(read_pos_name,"pullx_%d_%d.xvg",index_simulacion,index_estad);
            read_pos=fopen(read_pos_name,"r");
            for(i=0;i<1;i++)
            {
                flag=fgetc(read_pos);
                if(flag=='#' || flag=='@')
                {
                    for(j=0;j<1;j++)
                    {
                        flag=fgetc(read_pos);
                        if(flag=='\n')
                            break;
                        j=-1;
                    }
                }
                else
                {
                    puntero_pullx_basura[index_simulacion][index_estad]=ftell(read_pos)-1;
                    break;
                }
                i=-1;
            }
            fclose(read_pos);
            
            printf("Analisis trayectoria: %.3f %%\n",25.0*(index_simulacion*VARIABLE_punt->N_estadistica+index_estad+1.0)/(1.0*VARIABLE_punt->N_simulaciones*VARIABLE_punt->N_estadistica));
        }
    }
    
    array_x=(float*)malloc(sizeof(float)*N_lineas);
    
    array_W=(float*)malloc(sizeof(float)*VARIABLE_punt->N_simulaciones);
    
    array_fuerza=(float**)malloc(sizeof(float*)*N_lineas);
    for(i=0;i<N_lineas;i++)
        array_fuerza[i]=(float*)malloc(sizeof(float)*VARIABLE_punt->N_estadistica);
    
    array_fuerza_media=(float**)malloc(sizeof(float*)*VARIABLE_punt->N_simulaciones);
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        array_fuerza_media[index_simulacion]=(float*)malloc(sizeof(float)*N_lineas);
    
    array_fuerza_error=(float**)malloc(sizeof(float*)*VARIABLE_punt->N_simulaciones);
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        array_fuerza_error[index_simulacion]=(float*)malloc(sizeof(float)*N_lineas);
    
    array_pos=(float**)malloc(sizeof(float*)*N_lineas);
    for(i=0;i<N_lineas;i++)
        array_pos[i]=(float*)malloc(sizeof(float)*VARIABLE_punt->N_estadistica);
    
    array_pos_media=(float**)malloc(sizeof(float*)*VARIABLE_punt->N_simulaciones);
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        array_pos_media[index_simulacion]=(float*)malloc(sizeof(float)*N_lineas);
    
    array_pos_error=(float**)malloc(sizeof(float*)*VARIABLE_punt->N_simulaciones);
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        array_pos_error[index_simulacion]=(float*)malloc(sizeof(float)*N_lineas);
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
    {
        for(index_estad=0;index_estad<VARIABLE_punt->N_estadistica;index_estad++)
        {
            sprintf(read_fuerza_name,"pullf_%d_%d.xvg",index_simulacion,index_estad);
            read_fuerza=fopen(read_fuerza_name,"r");
            fseek(read_fuerza,puntero_pullf_basura[index_simulacion][index_estad],SEEK_SET);
            
            for(i=0;i<N_lineas;i++)
            {
                fscanf(read_fuerza,"%f%f",&xf,&yf);
                
                if(index_estad==0 && index_simulacion==0)
                    array_x[i]=xf;
                
                array_fuerza[i][index_estad]=yf;
            }
            fclose(read_fuerza);
            
            printf("Analisis trayectoria: %.3f %%\n",25.0+25.0*(index_simulacion*VARIABLE_punt->N_estadistica+index_estad+1.0)/(1.0*VARIABLE_punt->N_simulaciones*VARIABLE_punt->N_estadistica));
        }
        for(i=0;i<N_lineas;i++)
            MedVar(array_fuerza[i],VARIABLE_punt->N_estadistica,&array_fuerza_media[index_simulacion][i],&array_fuerza_error[index_simulacion][i]);
    }
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
    {
        for(index_estad=0;index_estad<VARIABLE_punt->N_estadistica;index_estad++)
        {
            sprintf(read_pos_name,"pullx_%d_%d.xvg",index_simulacion,index_estad);
            read_pos=fopen(read_pos_name,"r");
            fseek(read_pos,puntero_pullx_basura[index_simulacion][index_estad],SEEK_SET);
            
            for(i=0;i<N_lineas;i++)
            {
                fscanf(read_pos,"%f%f",&xp,&yp);
                
                if(index_estad==0 && index_simulacion==0)
                    array_x[i]=xp;
                
                array_pos[i][index_estad]=yp;
            }
            fclose(read_pos);
            
            printf("Analisis trayectoria: %.3f %%\n",50.0+25.0*(index_simulacion*VARIABLE_punt->N_estadistica+index_estad+1.0)/(1.0*VARIABLE_punt->N_simulaciones*VARIABLE_punt->N_estadistica));
        }
        for(i=0;i<N_lineas;i++)
            MedVar(array_pos[i],VARIABLE_punt->N_estadistica,&array_pos_media[index_simulacion][i],&array_pos_error[index_simulacion][i]);
    }
    
    
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
    {
        array_W[index_simulacion]=0;
        
        sprintf(traj_analisis_name,"Trayectoria_%.1f_%.1f__Analisis.txt",CANAL_punt->bond_fuerza[index_simulacion],CANAL_punt->bending_fuerza[index_simulacion]);
        write_trayectoria=fopen(traj_analisis_name,"w");
        fprintf(write_trayectoria,"tiempo\tfuerza_media\tposicion\tW");
        fprintf(write_trayectoria,"\n");
        
        for(i=0;i<N_lineas;i++)
        {
            fprintf(write_trayectoria,"%f",array_x[i]);
            
            if(i==0)
                fprintf(write_trayectoria,"\t%f\t%f\t%f",array_fuerza_media[index_simulacion][i],array_pos_media[index_simulacion][i],0.0);
            if(i>0)
            {
                array_W[index_simulacion]+=(array_pos_media[index_simulacion][i]-array_pos_media[index_simulacion][i-1])*array_fuerza_media[index_simulacion][i];
                
                fprintf(write_trayectoria,"\t%f\t%f\t%f",array_fuerza_media[index_simulacion][i],array_pos_media[index_simulacion][i],array_W[index_simulacion]);
            }
            
            if(i%1000==0)
                printf("Analisis trayectoria: %.3f %%\n",75.0+25.0*(i*VARIABLE_punt->N_simulaciones+index_simulacion+1.0)/(1.0*N_lineas*VARIABLE_punt->N_simulaciones));
            
            fprintf(write_trayectoria,"\n");
        }
        fclose(write_trayectoria);
    }
    
    
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
    {
        for(index_estad=0;index_estad<VARIABLE_punt->N_estadistica;index_estad++)
        {
            if(fork()==0)
            {
                sprintf(comando,"rm pullf_%d_%d.xvg pullx_%d_%d.xvg\n",index_simulacion,index_estad,index_simulacion,index_estad);
                
                pipe=popen(comando,"w");
                pclose(pipe);
                
                LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
            }
            wait(NULL);
        }
    }
    
    
    
    
    
    
    
    
    
    estadistica_vel=(float*)malloc(sizeof(float)*VARIABLE_punt->N_estadistica);
    for(i=0;i<VARIABLE_punt->N_estadistica;i++)
        estadistica_vel[i]=0;
    
    estadistica_tiempo=(float*)malloc(sizeof(float)*VARIABLE_punt->N_estadistica);
    for(i=0;i<VARIABLE_punt->N_estadistica;i++)
        estadistica_tiempo[i]=0;
    
    
    
    sprintf(write_analisis_name,"PolPos=%.1f_CanalRadio=%.3f_tau=%.1f_pullMode=%s_E=%s__Analisis.txt",POL_punt->posicion,CANAL_punt->radio,VARIABLE_punt->tau_t,VARIABLE_punt->pulling_modo,VARIABLE_punt->campo_E);
    
    write_analisis=fopen(write_analisis_name,"w");
    fprintf(write_analisis,"#SIM\tCANAL_bending_cte\tCANAL_bending_ang0\tCANAL_bond_k\tCANAL_bond_l0\tPOL_long_trans\tpull_rate[ps^-1]\tFrecuencia[ps^-1]\ttiempo_trans\terror_tiempo\tvelocidad_trans[ps^-1]\terror_vel\tfuerza_pull\tW_trasloc\n");
    fclose(write_analisis);
    
    printf("\n\n");
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
    {
        array_W[index_simulacion]=0;
        
        POL_long_trans=POL_punt->longitud[index_simulacion]-fabs((POL_punt->posicion+1)*0.5*POL_punt->longitud[index_simulacion]+CANAL_punt->bond_reposo*0.5*CANAL_punt->Nb);
        
        for(index_estad=0;index_estad<VARIABLE_punt->N_estadistica;index_estad++)
        {
            fuerza_inst=0;
            fuerza_contador=0;
            
            sprintf(read_analisis_name,"TiempoTrans_%d_%d.txt",index_simulacion,index_estad);
            read_analisis=fopen(read_analisis_name,"r");
            fscanf(read_analisis,"%f",&tiempo_trans);
            fclose(read_analisis);
            
            
            estadistica_vel[index_estad]=POL_long_trans/(tiempo_trans*POL_punt->bond_reposo);
            estadistica_tiempo[index_estad]=tiempo_trans;
            
            
            sprintf(comando,"rm TiempoTrans_%d_%d.txt\n",index_simulacion,index_estad);
            
            pipe=popen(comando,"w");
            pclose(pipe);
            
            
            
            printf("Analisis global: %.3f %%\n",100.0*(index_simulacion*VARIABLE_punt->N_estadistica+index_estad+1.0)/(1.0*VARIABLE_punt->N_simulaciones*VARIABLE_punt->N_estadistica));
            
        }
        
        MedVar(estadistica_vel,VARIABLE_punt->N_estadistica,&vel_medio,&error_vel);
        MedVar(estadistica_tiempo,VARIABLE_punt->N_estadistica,&tiempo_medio,&error_tiempo);
        MedVar(array_fuerza_media[index_simulacion],N_lineas,&fuerza_medio,&error_fuerza);
        
        for(i=0;i<N_lineas;i++)
        {
            if(i>0 && array_x[i]<tiempo_medio)
                array_W[index_simulacion]+=(array_pos_media[index_simulacion][i]-array_pos_media[index_simulacion][i-1])*array_fuerza_media[index_simulacion][i];
        }
        
        write_analisis=fopen(write_analisis_name,"a");
        if(Igual(tiempo_trans,0)==1)
            fprintf(write_analisis,"%d\t%f\t%f\t%f\t%f\t%f\t%s %s\n",index_simulacion, CANAL_punt->bending_fuerza[index_simulacion],CANAL_punt->bending_ang,CANAL_punt->bond_fuerza[index_simulacion],CANAL_punt->bond_reposo,POL_punt->longitud[index_simulacion],"NO","TRASLOCACION");
        else
            fprintf(write_analisis,"%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",index_simulacion, CANAL_punt->bending_fuerza[index_simulacion],CANAL_punt->bending_ang,CANAL_punt->bond_fuerza[index_simulacion],CANAL_punt->bond_reposo,POL_long_trans,VARIABLE_punt->pull_rate[index_simulacion]/POL_punt->bond_reposo,VARIABLE_punt->Ezfrecuencia[index_simulacion],tiempo_medio,error_tiempo,vel_medio,error_vel,fuerza_medio,array_W[index_simulacion]);
        fclose(write_analisis);
        
    }
    
    free(array_x);
    
    free(array_W);
    
    for(i=0;i<N_lineas;i++)
        free(array_fuerza[i]);
    
    free(array_fuerza);
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        free(array_fuerza_error[index_simulacion]);
    
    free(array_fuerza_error);
    
    
    for(i=0;i<N_lineas;i++)
        free(array_pos[i]);
    
    free(array_pos);
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        free(array_pos_media[index_simulacion]);
    
    free(array_pos_media);
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        free(array_pos_error[index_simulacion]);
    
    free(array_pos_error);
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        free(puntero_pullf_basura[index_simulacion]);
    
    free(puntero_pullf_basura);
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        free(puntero_pullx_basura[index_simulacion]);
    
    free(puntero_pullx_basura);
    
    
    free(estadistica_vel);
    
    free(estadistica_tiempo);
    
    
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        free(array_fuerza_media[index_simulacion]);
    
    free(array_fuerza_media);
    
}





///LEER INPUT//////////////////////////////////////////////////////////////////////////////////////////
void LeerInput(struct CANAL_struct* CANAL_punt, struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt, struct VARIABLE_struct* VARIABLE_punt)
{
    int i;
    FILE *input;
    FILE *min,*term,*water,*martini,*dry_martini, *Lterm;
    char Pcoupl[256];
    float ref_p;
    char input_string[256];
    char basura[256];
    
    input=fopen("./Input/genconf.txt","r");
    
    fscanf(input,"%s%s%d\n",basura,basura,&VARIABLE_punt->N_nodo);
    VARIABLE_punt->lista_nodos=(int*)malloc(sizeof(int)*VARIABLE_punt->N_nodo);
    
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_nodo;i++)
        fscanf(input,"%d",VARIABLE_punt->lista_nodos+i);
    
    fscanf(input,"%s%s%d\n",basura,basura,&VARIABLE_punt->N_CPU);
    
    fscanf(input, "%s%s%d\n",basura, basura, &VARIABLE_punt->N_simulaciones);

    CANAL_punt->bond_fuerza=(float*)malloc(sizeof(float)*(VARIABLE_punt->N_simulaciones));
    CANAL_punt->bending_fuerza=(float*)malloc(sizeof(float)*(VARIABLE_punt->N_simulaciones));
    VARIABLE_punt->pull_rate=(float*)malloc(sizeof(float)*(VARIABLE_punt->N_simulaciones));
    VARIABLE_punt->pull_fuerza=(float*)malloc(sizeof(float)*(VARIABLE_punt->N_simulaciones));
    VARIABLE_punt->Ezfrecuencia=(float*)malloc(sizeof(float)*(VARIABLE_punt->N_simulaciones));
    POL_punt->longitud=(float*)malloc(sizeof(float)*(VARIABLE_punt->N_simulaciones));
    
    fscanf(input,"%s%s%d\n",basura,basura,&VARIABLE_punt->N_estadistica);
    
    fscanf(input,"%s%s%f\n",basura,basura,&VARIABLE_punt->term_tol_derivada);
    fscanf(input,"%s%s%f\n",basura,basura,&VARIABLE_punt->term_tolerancia);
    fscanf(input,"%s%s%d\n",basura,basura,&VARIABLE_punt->N_term);
    fscanf(input,"%s%s%d\n",basura,basura,&VARIABLE_punt->N_Lterm);
    fscanf(input,"%s%s%f\n",basura,basura,&VARIABLE_punt->N_sim_cte);
    fscanf(input,"%s%s%f\n",basura,basura,&VARIABLE_punt->dt);
    
    fscanf(input,"%s\t%s\t%s\n",basura,basura,VARIABLE_punt->PintaTrayectoria);
    fscanf(input,"%s\t%s\t%s\n",basura,basura,VARIABLE_punt->PintaPoro);
	fscanf(input,"%s\t%s\t%d\n",basura,basura,&VARIABLE_punt->every_out);
    fscanf(input,"%s\t%s\t%s\n",basura,basura,VARIABLE_punt->conf_externa);
    fscanf(input,"%s\t%s\t%d\t%d\n", basura, basura, &VARIABLE_punt->primercand, &VARIABLE_punt->ultimocand);
    
    fscanf(input,"%s\t%s\t%s\n",basura,basura,VARIABLE_punt->Acoplo_presion);
    
    fscanf(input,"%s\t%s\t%s\n",basura,basura,CANAL_punt->Borde_poro);
    fscanf(input,"%s\t%s\t%s\n",basura,basura,CANAL_punt->Cuerpo_poro);
    fscanf(input,"%s\t%s\t%s\n",basura,basura,POL_punt->Borde_polimero);
    fscanf(input,"%s\t%s\t%s\n",basura,basura,POL_punt->Cuerpo_polimero);
    
    fscanf(input,"%s\t%s\t%s\n",basura,basura,VARIABLE_punt->pulling);
    fscanf(input,"%s\t%s\t%s\n",basura,basura,VARIABLE_punt->pulling_modo);
    
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        fscanf(input,"%f",VARIABLE_punt->pull_rate+i);
    
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        fscanf(input,"%f",VARIABLE_punt->pull_fuerza+i);
    
    fscanf(input,"%s%s%f",basura,basura,&VARIABLE_punt->pull_k);
    fscanf(input,"%s%s%f",basura,basura,&VARIABLE_punt->ref_t);
    fscanf(input,"%s%s%f",basura,basura,&VARIABLE_punt->tau_t);
    
    fscanf(input,"%s\t%s\t%s\n",basura,basura,input_string);
    if(strcmp(input_string,"si")==0)
        VARIABLE_punt->AGUA_sel=1;
    else
        VARIABLE_punt->AGUA_sel=0;
    
    
    fscanf(input,"%s%s%s",basura,basura,input_string);
    if(strcmp(input_string,"si")==0)
        CANAL_punt->sel=1;
    else
        CANAL_punt->sel=0;
    
    fscanf(input,"%s%s%f",basura,basura,&CANAL_punt->radio);
    
    fscanf(input,"%s%s%f",basura,basura,&CANAL_punt->longitudInterior);
    
    
    fscanf(input,"%s%s%f",basura,basura,&CANAL_punt->longitudExterior);
    
    
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        fscanf(input,"%f",CANAL_punt->bond_fuerza+i);
    
    
    fscanf(input,"%s%s%f",basura,basura,&CANAL_punt->bond_reposo);
    
    
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        fscanf(input,"%f",CANAL_punt->bending_fuerza+i);
    
    
    fscanf(input,"%s%s%f",basura,basura,&CANAL_punt->bending_ang);
    
    
    fscanf(input,"%s%s%s",basura,basura,input_string);
    if(strcmp(input_string,"si")==0)
        POL_punt->sel=1;
    else
        POL_punt->sel=0;
    
    fscanf(input,"%s%s%f",basura,basura,&POL_punt->bond_fuerza);
    
    
    fscanf(input,"%s%s%f",basura,basura,&POL_punt->bond_reposo);
    
    
    fscanf(input,"%s%s%f",basura,basura,&POL_punt->bending_fuerza);
    
    
    fscanf(input,"%s%s%f",basura,basura,&POL_punt->bending_ang);
    
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        fscanf(input,"%f",POL_punt->longitud+i);
    
    
    fscanf(input,"%s%s%f",basura,basura,&POL_punt->posicion);
    
    
    fscanf(input,"%s%s%s",basura,basura,input_string);
    if(strcmp(input_string,"si")==0)
        MEMBRANA_punt->sel=1;
    else
        MEMBRANA_punt->sel=0;
    
    
    fscanf(input,"%s%s%f",basura,basura,&MEMBRANA_punt->area);
    
    fscanf(input,"%s\t%s\t%s\n",basura,basura,VARIABLE_punt->campo_E);
    fscanf(input,"%s%s",basura,basura);
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        fscanf(input,"%f",VARIABLE_punt->Ezfrecuencia+i);
    
    fscanf(input,"%s%s%f\n",basura,basura,&VARIABLE_punt->Ezintensidad);
    fscanf(input,"%s%s%f\n",basura,basura,&VARIABLE_punt->Exintensidad);
    fscanf(input,"%s%s%f\n",basura,basura,&VARIABLE_punt->Eyintensidad);
    
    
    
    fclose(input);
    
    
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
    {
        CANAL_punt->Nb=(int)((CANAL_punt->longitudInterior+2*CANAL_punt->longitudExterior)/CANAL_punt->bond_reposo);
        CANAL_punt->NExterior=(int)(CANAL_punt->longitudExterior/CANAL_punt->bond_reposo);
        CANAL_punt->phi=acos(1-CANAL_punt->bond_reposo*CANAL_punt->bond_reposo/(2*CANAL_punt->radio*CANAL_punt->radio));
        CANAL_punt->M=(int)(2*PI/CANAL_punt->phi);
        CANAL_punt->n=CANAL_punt->Nb*CANAL_punt->M;
    }
    
    
    min=fopen("Input/minim.mdp","w");
    term=fopen("Input/term.mdp","w");
    Lterm=fopen("Input/Lterm.mdp","w");
    water=fopen("Input/water.gro","w");
    martini=fopen("Input/martini_v2.2.itp","w");
    dry_martini=fopen("Input/dry_martini_v2.1.itp","w");
    
    if(strcmp(VARIABLE_punt->Acoplo_presion,"si")==0)
    {
        sprintf(Pcoupl,"parrinello-rahman");
        
        if(strcmp(VARIABLE_punt->pulling,"si")==0)
            ref_p=0.5;
        
        if(strcmp(VARIABLE_punt->pulling,"no")==0)
            ref_p=1.0;
    }
    else
    {
        sprintf(Pcoupl,"no");
        ref_p=1.0;
    }
    
    fprintf(min,"integrator\t= steep\n\nemstep\t\t = 0.0001\nemtol\t\t= 100.0\nnsteps\t\t= 500000\ncomm-mode\t= Linear\ncomm-grps\t= \n\nnstlist\t\t = 1\nnstxout\t\t = 0\nnstvout\t\t = 0\nnstfout\t\t = 0\nnstlog\t\t = 0\nnstenergy\t\t = 0\nnstxout-compressed\t\t = -1\ncutoff-scheme\t\t = group \nverlet-buffer-drift\t= 0.005\nns_type\t\t = grid\n\ncoulombtype\t\t = reaction-field\n\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\npbc\t\t = xyz\n");
    
    
    fprintf(term,"\ntitle\t\t = Termalizacion\n\n\n\n\nintegrator\t\t = sd\ndt\t\t = %f\nnsteps\t\t = %d\nnstcomm\t\t = 100\ncomm-mode\t\t = Linear\ncomm-grps\t\t = \n\n\n\nnstxout\t\t = %d\nnstvout\t\t = %d\nnstfout\t\t = %d\nnstlog\t\t = 1000\nnstenergy\t\t = 1000\nnstxout-compressed\t\t = -1\ncompressed-x-grps\t\t = SECO\n\nfreezegrps\t\t = DENTRO CHNNL\nfreezedim\t\t = Y Y Y N N Y\n\ncutoff-scheme\t\t = group\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\n\n\ntcoupl\t\t = v-rescale\ntc-grps\t\t = SECO\ntau_t\t\t = %.1f\nref_t\t\t = %.2f\nPcoupl\t\t = %s\nPcoupltype\t\t = semiisotropic\ntau_p\t\t = 12.0\ncompressibility\t\t = 3e-4 3e-4\nref_p\t\t = %.2f %.2f\n\ngen_vel\t\t = no \ngen_temp\t\t = 300 \ngen_seed\t\t = -1\n\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\nperiodic-molecules = yes\n",VARIABLE_punt->dt,VARIABLE_punt->N_term,VARIABLE_punt->N_term,VARIABLE_punt->N_term,VARIABLE_punt->N_term,0.1,300.0,Pcoupl,ref_p,ref_p);
    
    fprintf(Lterm,"\ntitle\t\t = LTermalizacion\n\n\n\n\nintegrator\t\t = sd\ndt\t\t = %f\nnsteps\t\t = %d\nnstcomm\t\t = 100\ncomm-mode\t\t = Linear\ncomm-grps\t\t = \n\n\n\nnstxout\t\t = %d\nnstvout\t\t = %d\nnstfout\t\t = %d\nnstlog\t\t = 1000\nnstenergy\t\t = 1000\nnstxout-compressed\t\t = -1\ncompressed-x-grps\t\t = SECO\n\nfreezegrps\t\t = DENTRO CHNNL\nfreezedim\t\t = Y Y Y N N Y\n\ncutoff-scheme\t\t = group\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\n\n\ntcoupl\t\t = v-rescale\ntc-grps\t\t = SISTEMA\ntau_t\t\t = %.1f\nref_t\t\t = %.2f\nPcoupl\t\t = %s\nPcoupltype\t\t = semiisotropic\ntau_p\t\t = 12.0\ncompressibility\t\t = 3e-4 3e-4\nref_p\t\t = %.2f %.2f\n\ngen_vel\t\t = no \ngen_temp\t\t = 300 \ngen_seed\t\t = -1\n\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\nperiodic-molecules = yes\n",VARIABLE_punt->dt,VARIABLE_punt->N_Lterm,VARIABLE_punt->N_Lterm,VARIABLE_punt->N_Lterm,VARIABLE_punt->N_Lterm,0.1,300.0,Pcoupl,ref_p,ref_p);
    
    
    
    fprintf(water,"WATER\n  400\n    1W        W    1   0.084   3.595   3.359  0.0663  0.0148  0.2032\n    2W        W    2   0.460   2.488   2.882  0.0999 -0.0648 -0.1661\n    3W        W    3   3.165   2.218   0.652 -0.0881 -0.1237 -0.0896\n    4W        W    4   3.295   3.303   2.679 -0.1486 -0.3229  0.1120\n    5W        W    5   1.213   3.294   0.205 -0.1053 -0.1062  0.1014\n    6W        W    6   1.296   1.446   1.188  0.0877 -0.0566 -0.1282\n    7W        W    7   0.811   1.294   1.352  0.1967  0.2199  0.3289\n    8W        W    8   1.802   2.109   0.786 -0.1596 -0.1576 -0.0010\n    9W        W    9   1.477   3.201   3.141 -0.1225  0.0135  0.1267\n   10W        W   10   0.126   0.762   3.104  0.0776 -0.1781 -0.1549\n   11W        W   11   0.888   0.042   2.906  0.1622 -0.2895 -0.1539\n   12W        W   12   2.582   0.430   1.793  0.0293  0.1399  0.1850\n   13W        W   13   2.231   1.301   0.363  0.0119  0.0262 -0.0578\n   14W        W   14   2.801   0.735   3.454  0.2122 -0.0713 -0.1525\n   15W        W   15   0.167   0.527   2.579 -0.2237 -0.0155 -0.0333\n   16W        W   16   0.266   3.146   3.600  0.0972 -0.2452  0.1849\n   17W        W   17   1.788   2.536   0.332  0.0552  0.0717  0.1324\n   18W        W   18   0.220   1.319   2.025  0.1433  0.0166 -0.2722\n   19W        W   19   1.113   1.210   3.262  0.4727  0.0172 -0.1323\n   20W        W   20   3.572   1.069   1.761 -0.2059  0.1490 -0.2361\n   21W        W   21   3.336   2.112   2.620  0.0164  0.2724 -0.2412\n   22W        W   22   1.644   1.445   1.647  0.1858 -0.0232 -0.1210\n   23W        W   23   2.190   2.784   2.016 -0.0310 -0.0346 -0.0709\n   24W        W   24   0.690   2.218   2.248  0.2468  0.3122  0.0688\n   25W        W   25   0.049   1.746   1.819  0.1395 -0.2593  0.1148\n   26W        W   26   1.085   1.718   3.011  0.1122 -0.2451 -0.0872\n   27W        W   27   3.356   1.501   3.265  0.1578  0.0056  0.1978\n   28W        W   28   0.972   2.945   1.710  0.1559 -0.3802  0.1129\n   29W        W   29   2.093   2.634   2.844 -0.0502 -0.3423 -0.3093\n   30W        W   30   1.313   1.274   2.069  0.2198  0.0937  0.0394\n   31W        W   31   1.605   2.058   1.712  0.1814  0.5446  0.0734\n   32W        W   32   2.602   2.831   3.560 -0.2065  0.0601 -0.0493\n   33W        W   33   1.681   2.151   3.570 -0.2224 -0.0375 -0.1153\n   34W        W   34   1.662   1.867   3.048  0.1370  0.0775 -0.0222\n   35W        W   35   0.071   1.258   3.570  0.2232  0.1052 -0.2036\n   36W        W   36   2.903   2.793   3.169 -0.2625 -0.0170 -0.1553\n   37W        W   37   0.484   0.375   1.167  0.0519 -0.1542  0.1676\n   38W        W   38   1.011   2.633   2.166 -0.0727  0.1966 -0.1345\n   39W        W   39   3.474   2.416   1.456 -0.1824 -0.2669  0.1078\n   40W        W   40   2.887   0.816   2.023 -0.1587  0.0574 -0.0220\n   41W        W   41   3.172   0.769   0.118 -0.4070  0.0521 -0.1165\n   42W        W   42   3.637   1.570   2.424 -0.2147 -0.2476  0.1190\n   43W        W   43   1.604   3.277   2.177  0.1467 -0.1670 -0.0368\n   44W        W   44   3.378   2.743   0.615  0.0989  0.0259  0.0111\n   45W        W   45   0.507   3.529   0.369  0.1744 -0.3678 -0.4485\n   46W        W   46   0.900   3.575   1.045  0.0173  0.0027 -0.0388\n   47W        W   47   0.697   0.329   2.471  0.0408  0.2341  0.2612\n   48W        W   48   3.320   1.347   2.746  0.2014 -0.1504 -0.1893\n   49W        W   49   2.725   1.769   1.205 -0.1672 -0.0662  0.2345\n   50W        W   50   2.691   1.457   0.232  0.0327 -0.0493  0.0328\n   51W        W   51   1.899   2.601   2.372  0.2344  0.1809 -0.0296\n   52W        W   52   1.705   0.931   1.571  0.0388  0.0230 -0.1394\n   53W        W   53   2.305   3.018   2.565 -0.0847  0.2589 -0.0957\n   54W        W   54   3.176   1.809   0.987  0.0442  0.0113 -0.1133\n   55W        W   55   2.078   0.467   1.003  0.1398  0.2946 -0.0153\n   56W        W   56   0.735   3.161   0.686  0.0416  0.2541 -0.0365\n   57W        W   57   0.106   0.660   1.285 -0.0736  0.1442  0.4423\n   58W        W   58   2.892   1.267   1.759  0.2127  0.0276  0.1395\n   59W        W   59   0.350   2.884   1.674 -0.3019 -0.1338 -0.0189\n   60W        W   60   1.691   3.234   0.544  0.0983  0.0110 -0.0495\n   61W        W   61   3.083   1.260   0.008  0.1221 -0.0937 -0.2201\n   62W        W   62   0.106   3.018   2.172  0.0705  0.3027  0.0096\n   63W        W   63   0.897   2.590   2.652  0.1734  0.1356  0.3042\n   64W        W   64   1.270   1.950   1.271 -0.0550 -0.1659  0.3565\n   65W        W   65   1.292   0.355   2.364 -0.0203 -0.2554  0.1799\n   66W        W   66   3.521   0.117   1.661  0.3071  0.3798  0.1215\n   67W        W   67   0.740   1.989   0.129 -0.1525 -0.0925  0.3060\n   68W        W   68   0.395   1.022   0.308 -0.2947 -0.0132 -0.1144\n   69W        W   69   0.457   2.978   1.102 -0.0204  0.1903  0.0988\n   70W        W   70   1.588   2.461   1.350  0.2348  0.0808  0.2965\n   71W        W   71   2.920   3.320   3.064  0.2283  0.0820 -0.3299\n   72W        W   72   1.846   0.962   0.748 -0.2551 -0.0416  0.0514\n   73W        W   73   3.229   3.360   1.520  0.3613  0.0067  0.1580\n   74W        W   74   2.906   1.213   3.118  0.0527  0.2079 -0.1073\n   75W        W   75   3.619   2.869   1.252 -0.0990  0.0203 -0.2161\n   76W        W   76   2.246   2.161   2.736 -0.2291  0.1045  0.0531\n   77W        W   77   0.012   1.712   0.942 -0.0223 -0.2216 -0.0878\n   78W        W   78   1.123   2.127   2.515 -0.2846 -0.2787 -0.2638\n   79W        W   79   2.327   1.391   0.893  0.2179  0.1295 -0.3747\n   80W        W   80   2.204   1.881   0.997 -0.0062 -0.2331  0.1179\n   81W        W   81   0.547   1.301   2.471 -0.1377 -0.1221  0.0593\n   82W        W   82   0.532   1.177   3.393  0.2577 -0.2536 -0.3249\n   83W        W   83   0.422   0.094   1.552  0.1453 -0.1852 -0.0740\n   84W        W   84   2.190   0.441   2.113  0.0589 -0.2857  0.1295\n   85W        W   85   1.838   0.626   3.276 -0.0173 -0.1918 -0.1173\n   86W        W   86   2.193   0.775   0.454  0.1000  0.1049  0.1765\n   87W        W   87   3.376   3.168   3.208  0.0081 -0.1364 -0.2274\n   88W        W   88   2.343   0.249   3.247  0.3649  0.3062  0.3534\n   89W        W   89   3.362   2.550   3.369 -0.2024  0.1635 -0.0631\n   90W        W   90   2.792   0.717   2.965  0.0561 -0.0918 -0.1598\n   91W        W   91   2.592   0.655   1.235  0.0626 -0.0639 -0.1071\n   92W        W   92   2.985   1.789   3.639  0.0417 -0.3997  0.1843\n   93W        W   93   0.725   3.365   1.625  0.0385 -0.2352 -0.0069\n   94W        W   94   1.383   1.082   0.611 -0.1681  0.2478  0.1267\n   95W        W   95   0.952   0.366   3.275  0.0231  0.3955 -0.1296\n   96W        W   96   3.004   0.850   0.956 -0.0966  0.3260 -0.1595\n   97W        W   97   3.371   1.332   0.901 -0.1633  0.3427  0.0260\n   98W        W   98   1.114   3.549   3.372 -0.0179 -0.0041  0.1525\n   99W        W   99   1.040   0.085   0.607  0.1413 -0.3096  0.4443\n  100W        W  100   3.361   0.619   1.642  0.4842 -0.0207  0.5468\n  101W        W  101   3.511   2.868   0.087 -0.0575  0.0049  0.1629\n  102W        W  102   0.418   2.071   1.789  0.1713  0.1921  0.0892\n  103W        W  103   2.335   0.658   0.019  0.0637 -0.0252  0.2308\n  104W        W  104   3.469   2.990   1.739 -0.1340  0.3007  0.2742\n  105W        W  105   3.508   0.184   2.489 -0.1665 -0.1548 -0.1240\n  106W        W  106   1.953   1.396   3.043 -0.0415 -0.2723 -0.5601\n  107W        W  107   2.046   2.735   0.746  0.1221 -0.0753 -0.1382\n  108W        W  108   2.446   2.029   3.486  0.3129  0.1316  0.1030\n  109W        W  109   2.120   0.006   1.795  0.3725  0.2272 -0.1770\n  110W        W  110   0.732   0.909   1.858 -0.1675 -0.2741 -0.1735\n  111W        W  111   0.286   2.109   2.632 -0.0931 -0.0053  0.2849\n  112W        W  112   2.145   3.302   1.338 -0.2220 -0.1848 -0.0470\n  113W        W  113   3.339   0.899   2.210  0.2879  0.2254  0.3853\n  114W        W  114   1.976   3.246   2.977  0.0149  0.0732 -0.0611\n  115W        W  115   3.319   2.305   0.182 -0.2453  0.0580 -0.0697\n  116W        W  116   1.754   1.788   1.243 -0.1141 -0.1139  0.1031\n  117W        W  117   3.606   0.299   2.984 -0.1646  0.0040  0.1883\n  118W        W  118   1.755   2.134   2.578 -0.3099 -0.0147 -0.1300\n  119W        W  119   1.972   1.020   2.023  0.0888 -0.0025  0.2680\n  120W        W  120   2.824   1.318   0.861 -0.0616  0.2395 -0.1256\n  121W        W  121   3.111   1.051   0.503  0.0113 -0.2479 -0.3118\n  122W        W  122   2.920   3.091   0.892 -0.4971  0.1746  0.1995\n  123W        W  123   2.754   2.694   1.688  0.1224 -0.3850 -0.0041\n  124W        W  124   2.650   1.773   0.666 -0.0440  0.0541 -0.0076\n  125W        W  125   2.083   1.983   1.652  0.2052  0.1291  0.0301\n  126W        W  126   2.964   1.771   1.719  0.5041 -0.0469 -0.2100\n  127W        W  127   0.339   0.786   2.143 -0.4300  0.1992 -0.0765\n  128W        W  128   3.117   1.838   2.970  0.1363  0.0447  0.1601\n  129W        W  129   3.468   3.456   2.116 -0.0331 -0.2801 -0.1871\n  130W        W  130   1.398   2.900   1.518 -0.2972  0.0182  0.3602\n  131W        W  131   2.405   3.433   2.842  0.2105  0.0327 -0.1871\n  132W        W  132   2.399   0.763   2.606 -0.0434 -0.0644 -0.0216\n  133W        W  133   1.606   3.471   2.667 -0.1580  0.3306 -0.1256\n  134W        W  134   3.303   0.425   1.133 -0.2677 -0.1144 -0.4424\n  135W        W  135   3.094   1.744   2.434 -0.4656 -0.4140 -0.2546\n  136W        W  136   0.077   1.065   2.472  0.3436 -0.1402 -0.0194\n  137W        W  137   2.937   0.860   1.490 -0.0676  0.1756  0.1849\n  138W        W  138   3.348   1.011   3.267 -0.1402 -0.0983 -0.0192\n  139W        W  139   2.233   2.710   1.485 -0.1288 -0.0196 -0.0012\n  140W        W  140   1.489   3.301   1.098  0.0791 -0.0550  0.3065\n  141W        W  141   0.604   0.481   0.356  0.0588  0.2845 -0.0519\n  142W        W  142   1.708   0.135   2.166  0.1174  0.1707  0.3807\n  143W        W  143   0.736   3.255   0.014 -0.0360 -0.1523 -0.1054\n  144W        W  144   1.110   1.721   0.765 -0.0796  0.1643 -0.0430\n  145W        W  145   1.764   2.350   3.078  0.2583  0.2456 -0.1121\n  146W        W  146   1.594   0.708   1.068  0.0322 -0.1418 -0.2712\n  147W        W  147   2.950   0.305   1.496 -0.0766  0.0108  0.2121\n  148W        W  148   2.670   3.587   1.824 -0.1680  0.1137  0.1200\n  149W        W  149   2.082   3.235   2.164  0.1653 -0.0435 -0.1009\n  150W        W  150   1.478   2.272   2.125 -0.0664 -0.1343  0.0275\n  151W        W  151   3.248   2.995   2.251  0.0890  0.1395  0.1766\n  152W        W  152   0.942   0.498   1.020  0.1151 -0.3380 -0.3329\n  153W        W  153   1.464   2.795   1.980 -0.0305 -0.2229  0.1110\n  154W        W  154   0.947   0.726   0.045  0.2246  0.0444  0.0731\n  155W        W  155   1.748   1.335   2.439 -0.1723  0.1179  0.1753\n  156W        W  156   2.596   0.201   2.309  0.1431  0.0340 -0.1891\n  157W        W  157   0.016   2.044   2.194  0.4073  0.1302 -0.0354\n  158W        W  158   1.720   1.862   0.352  0.1678  0.0940 -0.0840\n  159W        W  159   0.988   0.177   0.122 -0.1394  0.2426  0.1875\n  160W        W  160   1.726   1.258   1.173 -0.0379  0.1111 -0.0388\n  161W        W  161   2.696   0.234   2.872  0.2600  0.0706 -0.0201\n  162W        W  162   1.284   2.246   2.981  0.0077  0.2093  0.0046\n  163W        W  163   2.088   2.028   3.198 -0.2407  0.0407 -0.2144\n  164W        W  164   2.705   2.058   3.091 -0.0530  0.2797 -0.0156\n  165W        W  165   1.989   0.198   3.596  0.0360 -0.4815 -0.0698\n  166W        W  166   1.211   0.814   1.989  0.4084  0.1852  0.1640\n  167W        W  167   0.637   0.770   1.387  0.3736  0.2223  0.0702\n  168W        W  168   0.423   3.482   1.063  0.2890  0.1103  0.1199\n  169W        W  169   1.755   2.812   3.188  0.0278  0.0020 -0.1507\n  170W        W  170   0.790   2.297   0.547 -0.3842  0.1452  0.1170\n  171W        W  171   2.470   1.682   1.768 -0.0804  0.2562 -0.1608\n  172W        W  172   2.750   1.481   2.167 -0.3347  0.0076 -0.4763\n  173W        W  173   1.955   2.155   2.101  0.0182 -0.0897 -0.1092\n  174W        W  174   2.425   2.529   0.233 -0.1305  0.2575  0.0974\n  175W        W  175   2.097   0.550   1.561  0.0675 -0.0870  0.0549\n  176W        W  176   3.576   0.828   0.829  0.1053 -0.0199 -0.0299\n  177W        W  177   0.070   3.417   2.860 -0.0807 -0.3244  0.3322\n  178W        W  178   0.487   2.643   2.392  0.0028 -0.0326 -0.1743\n  179W        W  179   0.054   3.489   0.624 -0.2507 -0.0384  0.0724\n  180W        W  180   1.953   0.759   2.430  0.0338  0.1251  0.1175\n  181W        W  181   1.316   2.782   0.279  0.3409 -0.0158 -0.1541\n  182W        W  182   3.221   3.164   0.465 -0.0302 -0.0250 -0.0045\n  183W        W  183   2.196   1.817   0.479 -0.0132 -0.0264  0.0454\n  184W        W  184   1.039   1.008   0.996  0.1147 -0.3307 -0.0826\n  185W        W  185   1.871   0.918   0.048 -0.3365  0.1020 -0.2199\n  186W        W  186   2.455   2.483   3.226  0.2101 -0.1662  0.1260\n  187W        W  187   2.808   0.389   0.843  0.1810 -0.2480  0.0898\n  188W        W  188   0.138   0.237   0.291 -0.1467  0.1265 -0.1051\n  189W        W  189   1.424   2.556   2.581 -0.1913  0.0962  0.1389\n  190W        W  190   1.223   2.472   1.673 -0.1754  0.0102  0.0344\n  191W        W  191   0.965   2.424   0.071 -0.0457 -0.1995  0.2202\n  192W        W  192   0.206   2.665   0.839  0.5416 -0.0341  0.2301\n  193W        W  193   0.710   1.735   3.281  0.0856 -0.0158  0.2660\n  194W        W  194   1.414   0.295   3.467 -0.0168  0.0182  0.0587\n  195W        W  195   2.123   0.437   2.823  0.0142  0.0285  0.2569\n  196W        W  196   1.318   0.205   1.104 -0.0480  0.1082 -0.1413\n  197W        W  197   2.377   0.282   0.609  0.1518 -0.2812  0.2467\n  198W        W  198   0.322   0.214   2.186  0.3305 -0.2422  0.1640\n  199W        W  199   0.370   1.177   1.556 -0.2230  0.0771  0.3354\n  200W        W  200   3.059   0.285   1.962 -0.5292  0.0727 -0.0062\n  201W        W  201   0.481   0.783   0.776 -0.0682 -0.0407 -0.2144\n  202W        W  202   3.052   0.471   2.392  0.1390 -0.1475 -0.1754\n  203W        W  203   3.448   1.674   0.077  0.3015  0.0346 -0.3725\n  204W        W  204   0.740   0.432   1.896  0.0606  0.1171  0.4440\n  205W        W  205   1.854   1.004   2.836  0.3616  0.1726  0.2310\n  206W        W  206   0.002   0.694   0.314 -0.2970 -0.0481 -0.1576\n  207W        W  207   0.788   2.083   2.875  0.2148 -0.1223  0.0966\n  208W        W  208   2.146   1.481   2.054 -0.1632  0.1632  0.1241\n  209W        W  209   2.013   3.560   0.350  0.1429  0.1343 -0.2266\n  210W        W  210   0.159   2.067   0.047  0.0736  0.2662  0.1741\n  211W        W  211   0.894   2.478   3.173 -0.3180  0.0917 -0.1443\n  212W        W  212   0.415   1.645   2.804  0.0697  0.1659 -0.2034\n  213W        W  213   1.406   1.750   2.598 -0.1101 -0.0909 -0.4323\n  214W        W  214   3.174   0.109   2.860  0.2869  0.2211 -0.2049\n  215W        W  215   3.572   1.719   2.851 -0.2679 -0.3327 -0.1898\n  216W        W  216   3.560   1.919   0.537 -0.0468  0.0440 -0.0227\n  217W        W  217   2.423   2.251   0.636  0.1627 -0.0247 -0.1101\n  218W        W  218   2.897   3.551   0.331  0.2904  0.2679  0.4003\n  219W        W  219   2.074   1.541   3.473 -0.3816 -0.0773 -0.2144\n  220W        W  220   0.404   2.508   3.513 -0.1209  0.0617 -0.2356\n  221W        W  221   3.610   0.430   3.465 -0.0890  0.2018  0.1881\n  222W        W  222   3.328   1.358   2.086  0.1089 -0.2864  0.2393\n  223W        W  223   1.636   3.488   3.563 -0.1660 -0.0549 -0.1277\n  224W        W  224   1.491   0.117   0.290  0.0283 -0.2778 -0.1050\n  225W        W  225   3.030   2.716   0.233 -0.1028  0.2102 -0.0671\n  226W        W  226   0.799   0.809   3.119 -0.0318 -0.2495 -0.0825\n  227W        W  227   1.233   2.803   2.981 -0.4203 -0.1855 -0.1801\n  228W        W  228   3.359   0.794   2.779  0.0606  0.0013 -0.1655\n  229W        W  229   2.570   3.453   0.747  0.2826 -0.0724 -0.3583\n  230W        W  230   2.809   2.072   2.114  0.2060 -0.2617 -0.0318\n  231W        W  231   1.059   0.488   2.781  0.1035  0.0186  0.2764\n  232W        W  232   1.528   1.557   3.538  0.0193  0.1826  0.1863\n  233W        W  233   1.031   2.736   0.732  0.0386  0.1619 -0.1722\n  234W        W  234   2.786   2.234   0.996 -0.1380  0.2664 -0.1060\n  235W        W  235   0.554   0.860   2.720  0.2263  0.1479  0.0909\n  236W        W  236   1.883   2.922   1.184  0.1444  0.0463  0.0488\n  237W        W  237   3.633   1.482   1.365  0.2628  0.0579 -0.1360\n  238W        W  238   1.855   0.115   3.084 -0.4480  0.0381  0.1073\n  239W        W  239   2.635   2.820   2.163 -0.2661  0.0981  0.2005\n  240W        W  240   2.938   3.164   1.871  0.3226  0.1672  0.1097\n  241W        W  241   2.157   2.966   0.299 -0.0899  0.0659 -0.0941\n  242W        W  242   0.167   2.858   3.141  0.1301  0.2564 -0.1181\n  243W        W  243   0.910   0.850   2.379 -0.2019 -0.0276 -0.0066\n  244W        W  244   0.880   0.198   1.489  0.1648 -0.0170 -0.2094\n  245W        W  245   2.501   1.040   2.181 -0.0993 -0.3013  0.1034\n  246W        W  246   1.299   2.211   0.381 -0.1601  0.0454  0.0956\n  247W        W  247   3.297   2.153   1.880 -0.2972  0.1841  0.2394\n  248W        W  248   0.624   2.911   3.303 -0.0670 -0.3184 -0.0668\n  249W        W  249   3.011   3.593   1.100  0.2095  0.3316  0.2591\n  250W        W  250   1.110   0.613   1.545 -0.2387 -0.1888  0.3844\n  251W        W  251   0.879   2.106   1.635 -0.0918 -0.1489  0.0714\n  252W        W  252   1.192   3.517   2.385 -0.1807 -0.0120  0.0410\n  253W        W  253   3.259   3.613   3.417 -0.0392  0.2666 -0.1319\n  254W        W  254   2.418   1.801   2.350  0.1944  0.0602 -0.0408\n  255W        W  255   0.267   1.599   0.265 -0.0752  0.0587 -0.0030\n  256W        W  256   3.401   3.257   0.962 -0.0434  0.0868  0.3181\n  257W        W  257   3.238   0.581   0.581  0.0286  0.1256 -0.2563\n  258W        W  258   1.377   0.100   2.984  0.0045  0.3550  0.0966\n  259W        W  259   3.083   2.902   1.442 -0.1572  0.0446  0.0605\n  260W        W  260   3.112   1.458   1.347 -0.0089  0.1464  0.0217\n  261W        W  261   1.229   1.131   1.563  0.4212  0.3600  0.0628\n  262W        W  262   1.474   1.393   2.887 -0.3178 -0.2913 -0.1100\n  263W        W  263   2.585   2.546   2.750  0.2092  0.2829  0.0461\n  264W        W  264   1.235   1.000   2.862  0.2360  0.2210  0.2041\n  265W        W  265   1.214   3.466   1.478 -0.1663  0.2614  0.0767\n  266W        W  266   0.531   3.425   3.112  0.0776  0.2887  0.1490\n  267W        W  267   0.643   2.786   0.242  0.0309 -0.0676  0.1549\n  268W        W  268   3.160   2.544   1.052 -0.1451 -0.1053  0.0648\n  269W        W  269   0.764   1.336   2.938  0.1964 -0.2539 -0.1158\n  270W        W  270   3.537   1.266   0.419 -0.3717 -0.2321 -0.1516\n  271W        W  271   1.282   2.323   0.890  0.1052  0.0168  0.0542\n  272W        W  272   2.381   1.705   2.971 -0.0172  0.1827  0.1208\n  273W        W  273   2.272   1.333   2.515  0.0675 -0.0636 -0.1394\n  274W        W  274   0.832   1.705   2.544  0.0844  0.1260 -0.3240\n  275W        W  275   3.042   3.593   2.373  0.1920  0.1439  0.1513\n  276W        W  276   2.833   0.227   3.367 -0.0607  0.0134  0.2972\n  277W        W  277   0.497   0.377   2.954 -0.1567 -0.4302 -0.0395\n  278W        W  278   0.836   1.400   2.007 -0.0676 -0.1144 -0.2570\n  279W        W  279   1.205   1.214   0.129 -0.3122  0.2015  0.2236\n  280W        W  280   0.008   2.973   2.674  0.0501 -0.1139  0.1229\n  281W        W  281   1.593   1.682   2.117  0.1986  0.0823  0.0494\n  282W        W  282   1.416   0.707   0.222  0.0137 -0.0803  0.2679\n  283W        W  283   0.551   1.523   1.003 -0.4184 -0.0235  0.0295\n  284W        W  284   0.322   3.375   2.440  0.0241 -0.3097  0.2289\n  285W        W  285   3.117   1.601   0.548  0.1138 -0.3037 -0.0779\n  286W        W  286   3.212   0.517   3.234  0.1463 -0.1522  0.1318\n  287W        W  287   2.648   2.726   1.197 -0.4283  0.0105 -0.0491\n  288W        W  288   0.246   1.650   3.374  0.1430 -0.2762  0.0584\n  289W        W  289   3.212   0.114   0.670  0.2459  0.0850 -0.4919\n  290W        W  290   2.732   2.589   0.681 -0.1231  0.0609  0.0336\n  291W        W  291   1.634   1.647   0.766 -0.0282 -0.2216 -0.3378\n  292W        W  292   2.821   0.451   0.289  0.0077  0.0950  0.0690\n  293W        W  293   2.886   1.058   2.475  0.0722  0.1345  0.0404\n  294W        W  294   0.595   1.651   1.682 -0.0289 -0.0903  0.0656\n  295W        W  295   0.889   2.590   1.331 -0.3094  0.4324  0.0715\n  296W        W  296   2.652   1.210   1.296  0.0531  0.2386  0.0526\n  297W        W  297   1.675   0.589   1.992 -0.1765 -0.0275  0.0413\n  298W        W  298   3.637   2.500   2.503 -0.0165  0.0744  0.6199\n  299W        W  299   3.555   0.487   2.064  0.0899  0.0950  0.0222\n  300W        W  300   0.094   3.317   1.440  0.0363  0.2081  0.1416\n  301W        W  301   0.603   2.504   0.976 -0.2194  0.1315 -0.0227\n  302W        W  302   0.518   0.121   3.529 -0.3937  0.3828  0.2951\n  303W        W  303   1.637   2.974   3.631 -0.3231  0.1101 -0.0842\n  304W        W  304   3.392   1.942   1.421 -0.2026 -0.3168  0.3117\n  305W        W  305   2.746   2.119   0.301 -0.1762 -0.0749 -0.0256\n  306W        W  306   2.041   3.640   2.513  0.1103 -0.0128  0.1930\n  307W        W  307   2.898   2.362   3.550 -0.2284  0.0241  0.0767\n  308W        W  308   1.713   0.072   0.830 -0.1401  0.1203  0.1327\n  309W        W  309   2.426   0.941   1.689 -0.1412 -0.3995 -0.3513\n  310W        W  310   0.112   2.528   2.021 -0.3497  0.0786  0.0228\n  311W        W  311   1.365   0.490   0.662  0.2949 -0.2454  0.1967\n  312W        W  312   1.169   1.974   3.511 -0.1810 -0.1150  0.3257\n  313W        W  313   2.163   0.955   1.225  0.0111 -0.0602 -0.1369\n  314W        W  314   2.368   2.365   2.347  0.0124 -0.0210  0.1854\n  315W        W  315   0.522   2.970   2.782 -0.0240  0.0714 -0.0082\n  316W        W  316   1.113   2.953   3.505 -0.0578 -0.0523  0.4016\n  317W        W  317   0.964   0.750   0.519 -0.3118 -0.0358 -0.0294\n  318W        W  318   2.481   0.892   0.844  0.0362  0.1892 -0.1742\n  319W        W  319   2.439   3.000   3.081 -0.0106 -0.2847 -0.1368\n  320W        W  320   1.124   1.592   1.680  0.1703  0.3131  0.0347\n  321W        W  321   0.779   3.290   2.545  0.1385 -0.2117 -0.2508\n  322W        W  322   3.524   3.407   0.146 -0.1664  0.1630  0.0233\n  323W        W  323   0.387   2.079   3.142  0.0091 -0.0356 -0.0878\n  324W        W  324   2.382   2.341   1.830 -0.1970  0.1099  0.1859\n  325W        W  325   0.818   1.990   1.072  0.0571  0.0701  0.0504\n  326W        W  326   1.189   2.997   2.467  0.2549  0.4989  0.2753\n  327W        W  327   1.567   0.416   1.508 -0.0304 -0.0067  0.0017\n  328W        W  328   0.178   2.400   0.438 -0.2204 -0.2920 -0.0494\n  329W        W  329   1.697   1.351   0.308  0.0908  0.1461  0.0055\n  330W        W  330   0.343   1.900   1.327 -0.0430 -0.3669 -0.1231\n  331W        W  331   1.633   0.542   2.791  0.1245  0.0661 -0.2053\n  332W        W  332   3.305   0.251   0.218  0.2771 -0.4176 -0.3080\n  333W        W  333   3.398   1.997   3.317  0.1739 -0.2211 -0.3356\n  334W        W  334   2.052   3.254   3.469 -0.0978  0.2432 -0.1600\n  335W        W  335   3.610   2.348   2.991 -0.1393  0.0570  0.0705\n  336W        W  336   3.018   3.146   3.602  0.0979 -0.0154  0.0675\n  337W        W  337   0.757   3.602   2.080 -0.1619 -0.1484  0.3717\n  338W        W  338   3.616   2.224   0.954 -0.0945 -0.1854 -0.1266\n  339W        W  339   2.146   1.491   1.467  0.1428 -0.0216  0.1860\n  340W        W  340   1.913   0.103   1.357  0.1154  0.2005 -0.0996\n  341W        W  341   0.972   3.087   1.194  0.0561 -0.3476  0.0852\n  342W        W  342   1.471   0.828   2.419  0.0362 -0.3901  0.0307\n  343W        W  343   1.171   0.197   1.888 -0.1636  0.2570 -0.2182\n  344W        W  344   0.234   3.019   0.436 -0.0294 -0.1106 -0.0601\n  345W        W  345   2.050   2.383   1.154  0.0464 -0.1103 -0.1910\n  346W        W  346   0.925   3.178   3.033 -0.3117 -0.0874  0.0261\n  347W        W  347   0.402   2.001   0.729 -0.1724 -0.0188  0.0021\n  348W        W  348   0.287   0.544   1.728  0.1331 -0.0856  0.1751\n  349W        W  349   1.835   2.509   1.785  0.1764 -0.4253  0.0125\n  350W        W  350   0.087   0.363   0.805 -0.0935  0.0513  0.0230\n  351W        W  351   2.751   1.569   2.732  0.2966  0.0325  0.1324\n  352W        W  352   2.628   0.955   0.304 -0.1393 -0.1196 -0.0924\n  353W        W  353   3.145   2.627   1.952 -0.2149  0.0465 -0.0432\n  354W        W  354   1.806   3.111   1.783 -0.0874 -0.0098  0.3045\n  355W        W  355   1.218   3.215   0.694  0.0393 -0.1275 -0.1694\n  356W        W  356   2.500   0.118   0.124 -0.1562  0.2264  0.0358\n  357W        W  357   0.300   3.378   1.865  0.0193 -0.1698  0.1343\n  358W        W  358   2.497   1.136   3.500  0.1513  0.0075 -0.0186\n  359W        W  359   0.480   0.666   3.499  0.0831 -0.1698  0.1693\n  360W        W  360   0.211   1.266   0.832 -0.0126  0.1583 -0.2169\n  361W        W  361   0.902   1.255   0.557 -0.4227 -0.3078 -0.1509\n  362W        W  362   2.665   1.582   3.374  0.0784  0.2991 -0.0845\n  363W        W  363   1.008   1.863   2.083 -0.0415  0.1934 -0.3105\n  364W        W  364   2.482   0.149   1.203 -0.1083 -0.0615 -0.2930\n  365W        W  365   1.613   3.580   1.706  0.0204 -0.1087 -0.1716\n  366W        W  366   2.827   3.042   2.643  0.1265 -0.0611  0.0173\n  367W        W  367   2.411   3.191   1.754  0.1619  0.0130 -0.4041\n  368W        W  368   2.430   2.171   1.348 -0.1017  0.0671 -0.2733\n  369W        W  369   1.892   0.396   0.419 -0.1278  0.0346 -0.2232\n  370W        W  370   3.573   0.024   1.141 -0.1586  0.1127 -0.2725\n  371W        W  371   1.261   1.659   0.304  0.4082 -0.1876  0.1785\n  372W        W  372   3.427   0.996   1.294  0.0834 -0.0538  0.4157\n  373W        W  373   2.119   2.166   0.171 -0.3176  0.1516  0.0555\n  374W        W  374   3.055   2.366   2.919 -0.0247 -0.2816 -0.3012\n  375W        W  375   2.709   3.067   0.386 -0.1355 -0.1989 -0.2566\n  376W        W  376   1.324   0.742   3.292  0.0955 -0.0797  0.5514\n  377W        W  377   1.144   1.326   2.533 -0.0683  0.0474 -0.2325\n  378W        W  378   3.236   2.821   2.810 -0.3311 -0.0287 -0.2906\n  379W        W  379   2.336   0.771   3.187  0.1146  0.0371 -0.2916\n  380W        W  380   0.180   1.242   2.964 -0.0441 -0.1830 -0.1295\n  381W        W  381   1.940   1.719   2.690 -0.0144  0.0463 -0.1025\n  382W        W  382   0.297   2.477   1.347  0.0757 -0.0251 -0.1316\n  383W        W  383   0.444   1.774   2.237 -0.0694 -0.3087  0.0707\n  384W        W  384   2.761   2.060   2.587  0.3107  0.2012 -0.0691\n  385W        W  385   2.533   3.440   3.398 -0.0317 -0.1247 -0.1516\n  386W        W  386   2.429   1.179   2.951 -0.1043  0.1520 -0.1340\n  387W        W  387   2.706   3.233   1.356  0.0570  0.4550 -0.0556\n  388W        W  388   1.708   2.953   2.679  0.3637  0.0704  0.0366\n  389W        W  389   1.193   3.308   1.948 -0.0395  0.0042 -0.0031\n  390W        W  390   2.998   2.479   2.367  0.3160 -0.0373  0.0552\n  391W        W  391   2.048   2.551   3.504 -0.0849 -0.0297  0.1256\n  392W        W  392   0.653   2.504   1.843  0.0882 -0.0692 -0.2940\n  393W        W  393   2.999   2.263   1.488  0.1866  0.3055  0.3291\n  394W        W  394   2.640   3.329   2.239  0.2390  0.1363 -0.0792\n  395W        W  395   0.618   3.058   2.109 -0.0167 -0.0406 -0.0423\n  396W        W  396   1.639   1.110   3.319  0.0300 -0.3053  0.0710\n  397W        W  397   2.090   3.276   0.830 -0.0307  0.0044  0.1215\n  398W        W  398   1.398   2.531   3.468  0.0631 -0.0196 -0.4235\n  399W        W  399   1.524   2.769   0.845 -0.5337  0.0559  0.0075\n  400W        W  400   0.745   1.509   0.104  0.1616 -0.0161  0.1005\n   3.64428   3.64428   3.64428\n");
    
    
    
    fprintf(martini,"[ defaults ]\n1 1\n\n[ atomtypes ]\nP5 72.0 0.000 A 0.0 0.0\nP4 72.0 0.000 A 0.0 0.0\nP3 72.0 0.000 A 0.0 0.0\nP2 72.0 0.000 A 0.0 0.0\nP1 72.0 0.000 A 0.0 0.0\nNda 72.0 0.000 A 0.0 0.0 \nNd 72.0 0.000 A 0.0 0.0\nNa 72.0 0.000 A 0.0 0.0\nN0 72.0 0.000 A 0.0 0.0\nC5 72.0 0.000 A 0.0 0.0\nC4 72.0 0.000 A 0.0 0.0\nC3 72.0 0.000 A 0.0 0.0\nC2 72.0 0.000 A 0.0 0.0\nC1 72.0 0.000 A 0.0 0.0\nQda 72.0 0.000 A 0.0 0.0\nQd 72.0 0.000 A 0.0 0.0\nQa 72.0 0.000 A 0.0 0.0\nQ0 72.0 0.000 A 0.0 0.0\n\nSP5 45.0 0.000 A 0.0 0.0\nSP4 45.0 0.000 A 0.0 0.0\nSP3 45.0 0.000 A 0.0 0.0\nSP2 45.0 0.000 A 0.0 0.0\nSP1 45.0 0.000 A 0.0 0.0\nSNda 45.0 0.000 A 0.0 0.0\nSNd 45.0 0.000 A 0.0 0.0\nSNa 45.0 0.000 A 0.0 0.0\nSN0 45.0 0.000 A 0.0 0.0\nSC5 45.0 0.000 A 0.0 0.0\nSC4 45.0 0.000 A 0.0 0.0\nSC3 45.0 0.000 A 0.0 0.0\nSC2 45.0 0.000 A 0.0 0.0\nSC1 45.0 0.000 A 0.0 0.0\nSQda 45.0 0.000 A 0.0 0.0\nSQd 45.0 0.000 A 0.0 0.0\nSQa 45.0 0.000 A 0.0 0.0\nSQ0 45.0 0.000 A 0.0 0.0\n\nAC2 72.0 0.000 A 0.0 0.0\nAC1 72.0 0.000 A 0.0 0.0\n\nBP4 72.0 0.000 A 0.0 0.0\n\n[ nonbond_params ]\n  P5 \tP5 \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tSP5 \t1 \t0.10620E-00 \t0.67132E-03 \n  P4 \tP4 \t1 \t0.21558E-00 \t0.23238E-02 \n  BP4 \tBP4 \t1 \t0.21558E-00 \t0.23238E-02 \n  SP4 \tSP4 \t1 \t0.94820E-01 \t0.59939E-03 \n  P3 \tP3 \t1 \t0.21558E-00 \t0.23238E-02 \n  SP3 \tSP3 \t1 \t0.94820E-01 \t0.59939E-03 \n  P2 \tP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP2 \tSP2 \t1 \t0.85338E-01 \t0.53946E-03 \n  P1 \tP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP1 \tSP1 \t1 \t0.85338E-01 \t0.53946E-03 \n  Nda \tNda \t1 \t0.19402E-00 \t0.20914E-02 \n  SNda \tSNda \t1 \t0.85338E-01 \t0.53946E-03 \n  Nd \tNd \t1 \t0.17246E-00 \t0.18590E-02 \n  SNd \tSNd \t1 \t0.75856E-01 \t0.47952E-03 \n  Na \tNa \t1 \t0.17246E-00 \t0.18590E-02 \n  SNa \tSNa \t1 \t0.75856E-01 \t0.47952E-03 \n  N0 \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tSN0 \t1 \t0.66375E-01 \t0.41957E-03 \n  C5 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC5 \tSC5 \t1 \t0.66375E-01 \t0.41957E-03 \n  C4 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC4 \tSC4 \t1 \t0.66375E-01 \t0.41957E-03 \n  C3 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC3 \tSC3 \t1 \t0.66375E-01 \t0.41957E-03 \n  C2 \tC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  AC2 \tAC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC2 \tSC2 \t1 \t0.66375E-01 \t0.41957E-03 \n  C1 \tC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  AC1 \tAC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC1 \tSC1 \t1 \t0.66375E-01 \t0.41957E-03 \n  Qda \tQda \t1 \t0.24145E-00 \t0.26027E-02 \n  SQda \tSQda \t1 \t0.10620E-00 \t0.67132E-03 \n  Qd \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  SQd \tSQd \t1 \t0.94820E-01 \t0.59939E-03 \n  Qa \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  SQa \tSQa \t1 \t0.94820E-01 \t0.59939E-03 \n  Q0 \tQ0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SQ0 \tSQ0 \t1 \t0.66375E-01 \t0.41957E-03 \n  P5 \tSP5 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tP4 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tBP4 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tSP4 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tP3 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tSP3 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tP2 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tSP2 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tP1 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tSP1 \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tNda \t1 \t0.21558E-00 \t0.23238E-02 \n  P5 \tSNda \t1 \t0.21558E-00 \t0.23238E-02 \n  P5 \tNd \t1 \t0.21558E-00 \t0.23238E-02 \n  P5 \tSNd \t1 \t0.21558E-00 \t0.23238E-02 \n  P5 \tNa \t1 \t0.21558E-00 \t0.23238E-02 \n  P5 \tSNa \t1 \t0.21558E-00 \t0.23238E-02 \n  P5 \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  P5 \tSN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  P5 \tC5 \t1 \t0.13366E-00 \t0.14408E-02 \n  P5 \tSC5 \t1 \t0.13366E-00 \t0.14408E-02 \n  P5 \tC4 \t1 \t0.11642E-00 \t0.12549E-02 \n  P5 \tSC4 \t1 \t0.11642E-00 \t0.12549E-02 \n  P5 \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  P5 \tSC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  P5 \tC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  P5 \tAC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  P5 \tSC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  P5 \tC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  P5 \tAC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  P5 \tSC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  P5 \tQda \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tSQda \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tSQd \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tSQa \t1 \t0.24145E-00 \t0.26027E-02 \n  P5 \tQ0 \t1 \t0.21558E-00 \t0.23238E-02 \n  P5 \tSQ0 \t1 \t0.21558E-00 \t0.23238E-02 \n  SP5 \tP4 \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tBP4 \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tSP4 \t1 \t0.10620E-00 \t0.67132E-03 \n  SP5 \tP3 \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tSP3 \t1 \t0.10620E-00 \t0.67132E-03 \n  SP5 \tP2 \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tSP2 \t1 \t0.10620E-00 \t0.67132E-03 \n  SP5 \tP1 \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tSP1 \t1 \t0.10620E-00 \t0.67132E-03 \n  SP5 \tNda \t1 \t0.21558E-00 \t0.23238E-02 \n  SP5 \tSNda \t1 \t0.94820E-01 \t0.59939E-03 \n  SP5 \tNd \t1 \t0.21558E-00 \t0.23238E-02 \n  SP5 \tSNd \t1 \t0.94820E-01 \t0.59939E-03 \n  SP5 \tNa \t1 \t0.21558E-00 \t0.23238E-02 \n  SP5 \tSNa \t1 \t0.94820E-01 \t0.59939E-03 \n  SP5 \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP5 \tSN0 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP5 \tC5 \t1 \t0.13366E-00 \t0.14408E-02 \n  SP5 \tSC5 \t1 \t0.58789E-01 \t0.37162E-03 \n  SP5 \tC4 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP5 \tSC4 \t1 \t0.51203E-01 \t0.32367E-03 \n  SP5 \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP5 \tSC3 \t1 \t0.51203E-01 \t0.32367E-03 \n  SP5 \tC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  SP5 \tAC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  SP5 \tSC2 \t1 \t0.43617E-01 \t0.27572E-03 \n  SP5 \tC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  SP5 \tAC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  SP5 \tSC1 \t1 \t0.37928E-01 \t0.23976E-03 \n  SP5 \tQda \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tSQda \t1 \t0.10620E-00 \t0.67132E-03 \n  SP5 \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tSQd \t1 \t0.10620E-00 \t0.67132E-03 \n  SP5 \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  SP5 \tSQa \t1 \t0.10620E-00 \t0.67132E-03 \n  SP5 \tQ0 \t1 \t0.21558E-00 \t0.23238E-02 \n  SP5 \tSQ0 \t1 \t0.94820E-01 \t0.59939E-03 \n  P4 \tBP4 \t1 \t0.76824E-00 \t0.26348E-01 \n  P4 \tSP4 \t1 \t0.21558E-00 \t0.23238E-02 \n  P4 \tP3 \t1 \t0.21558E-00 \t0.23238E-02 \n  P4 \tSP3 \t1 \t0.21558E-00 \t0.23238E-02 \n  P4 \tP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  P4 \tSP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  P4 \tP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  P4 \tSP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  P4 \tNda \t1 \t0.17246E-00 \t0.18590E-02 \n  P4 \tSNda \t1 \t0.17246E-00 \t0.18590E-02 \n  P4 \tNd \t1 \t0.17246E-00 \t0.18590E-02 \n  P4 \tSNd \t1 \t0.17246E-00 \t0.18590E-02 \n  P4 \tNa \t1 \t0.17246E-00 \t0.18590E-02 \n  P4 \tSNa \t1 \t0.17246E-00 \t0.18590E-02 \n  P4 \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  P4 \tSN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  P4 \tC5 \t1 \t0.13366E-00 \t0.14408E-02 \n  P4 \tSC5 \t1 \t0.13366E-00 \t0.14408E-02 \n  P4 \tC4 \t1 \t0.11642E-00 \t0.12549E-02 \n  P4 \tSC4 \t1 \t0.11642E-00 \t0.12549E-02 \n  P4 \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  P4 \tSC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  P4 \tC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  P4 \tAC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  P4 \tSC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  P4 \tC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  P4 \tAC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  P4 \tSC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  P4 \tQda \t1 \t0.24145E-00 \t0.26027E-02 \n  P4 \tSQda \t1 \t0.24145E-00 \t0.26027E-02 \n  P4 \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  P4 \tSQd \t1 \t0.24145E-00 \t0.26027E-02 \n  P4 \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  P4 \tSQa \t1 \t0.24145E-00 \t0.26027E-02 \n  P4 \tQ0 \t1 \t0.24145E-00 \t0.26027E-02 \n  P4 \tSQ0 \t1 \t0.24145E-00 \t0.26027E-02 \n  BP4\tSP4 \t1 \t0.21558E-00 \t0.23238E-02 \n  BP4\tP3 \t1 \t0.21558E-00 \t0.23238E-02 \n  BP4\tSP3 \t1 \t0.21558E-00 \t0.23238E-02 \n  BP4 \tP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  BP4 \tSP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  BP4 \tP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  BP4 \tSP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  BP4 \tNda \t1 \t0.17246E-00 \t0.18590E-02 \n  BP4 \tSNda \t1 \t0.17246E-00 \t0.18590E-02 \n  BP4 \tNd \t1 \t0.17246E-00 \t0.18590E-02 \n  BP4 \tSNd \t1 \t0.17246E-00 \t0.18590E-02 \n  BP4 \tNa \t1 \t0.17246E-00 \t0.18590E-02 \n  BP4 \tSNa \t1 \t0.17246E-00 \t0.18590E-02 \n  BP4 \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  BP4 \tSN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  BP4 \tC5 \t1 \t0.13366E-00 \t0.14408E-02 \n  BP4 \tSC5 \t1 \t0.13366E-00 \t0.14408E-02 \n  BP4 \tC4 \t1 \t0.11642E-00 \t0.12549E-02 \n  BP4 \tSC4 \t1 \t0.11642E-00 \t0.12549E-02 \n  BP4 \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  BP4 \tSC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  BP4 \tC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  BP4 \tAC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  BP4 \tSC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  BP4 \tC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  BP4 \tAC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  BP4 \tSC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  BP4 \tQda \t1 \t0.24145E-00 \t0.26027E-02 \n  BP4 \tSQda \t1 \t0.24145E-00 \t0.26027E-02 \n  BP4 \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  BP4 \tSQd \t1 \t0.24145E-00 \t0.26027E-02 \n  BP4 \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  BP4 \tSQa \t1 \t0.24145E-00 \t0.26027E-02 \n  BP4 \tQ0 \t1 \t0.24145E-00 \t0.26027E-02 \n  BP4 \tSQ0 \t1 \t0.24145E-00 \t0.26027E-02 \n  SP4 \tP3 \t1 \t0.21558E-00 \t0.23238E-02 \n  SP4 \tSP3 \t1 \t0.94820E-01 \t0.59939E-03 \n  SP4 \tP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP4 \tSP2 \t1 \t0.85338E-01 \t0.53946E-03 \n  SP4 \tP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP4 \tSP1 \t1 \t0.85338E-01 \t0.53946E-03 \n  SP4 \tNda \t1 \t0.17246E-00 \t0.18590E-02 \n  SP4 \tSNda \t1 \t0.75856E-01 \t0.47952E-03 \n  SP4 \tNd \t1 \t0.17246E-00 \t0.18590E-02 \n  SP4 \tSNd \t1 \t0.75856E-01 \t0.47952E-03 \n  SP4 \tNa \t1 \t0.17246E-00 \t0.18590E-02 \n  SP4 \tSNa \t1 \t0.75856E-01 \t0.47952E-03 \n  SP4 \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP4 \tSN0 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP4 \tC5 \t1 \t0.13366E-00 \t0.14408E-02 \n  SP4 \tSC5 \t1 \t0.58789E-01 \t0.37162E-03 \n  SP4 \tC4 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP4 \tSC4 \t1 \t0.51203E-01 \t0.32367E-03 \n  SP4 \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP4 \tSC3 \t1 \t0.51203E-01 \t0.32367E-03 \n  SP4 \tC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  SP4 \tAC2 \t1 \t0.99167E-01 \t0.10690E-02 \n  SP4 \tSC2 \t1 \t0.43617E-01 \t0.27572E-03 \n  SP4 \tC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  SP4 \tAC1 \t1 \t0.86233E-01 \t0.92953E-03 \n  SP4 \tSC1 \t1 \t0.37928E-01 \t0.23976E-03 \n  SP4 \tQda \t1 \t0.24145E-00 \t0.26027E-02 \n  SP4 \tSQda \t1 \t0.10620E-00 \t0.67132E-03 \n  SP4 \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  SP4 \tSQd \t1 \t0.10620E-00 \t0.67132E-03 \n  SP4 \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  SP4 \tSQa \t1 \t0.10620E-00 \t0.67132E-03 \n  SP4 \tQ0 \t1 \t0.24145E-00 \t0.26027E-02 \n  SP4 \tSQ0 \t1 \t0.10620E-00 \t0.67132E-03 \n  P3 \tSP3 \t1 \t0.21558E-00 \t0.23238E-02 \n  P3 \tP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tSP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tSP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tNda \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tSNda \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tNd \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tSNd \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tSNa \t1 \t0.19402E-00 \t0.20914E-02 \n  P3 \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  P3 \tSN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  P3 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  P3 \tSC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  P3 \tC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  P3 \tSC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  P3 \tC3 \t1 \t0.13366E-00 \t0.14408E-02 \n  P3 \tSC3 \t1 \t0.13366E-00 \t0.14408E-02 \n  P3 \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  P3 \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  P3 \tSC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  P3 \tC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  P3 \tAC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  P3 \tSC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  P3 \tQda \t1 \t0.24145E-00 \t0.26027E-02 \n  P3 \tSQda \t1 \t0.24145E-00 \t0.26027E-02 \n  P3 \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  P3 \tSQd \t1 \t0.24145E-00 \t0.26027E-02 \n  P3 \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  P3 \tSQa \t1 \t0.24145E-00 \t0.26027E-02 \n  P3 \tQ0 \t1 \t0.21558E-00 \t0.23238E-02 \n  P3 \tSQ0 \t1 \t0.21558E-00 \t0.23238E-02 \n  SP3 \tP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP3 \tSP2 \t1 \t0.85338E-01 \t0.53946E-03 \n  SP3 \tP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP3 \tSP1 \t1 \t0.85338E-01 \t0.53946E-03 \n  SP3 \tNda \t1 \t0.19402E-00 \t0.20914E-02 \n  SP3 \tSNda \t1 \t0.85338E-01 \t0.53946E-03 \n  SP3 \tNd \t1 \t0.19402E-00 \t0.20914E-02 \n  SP3 \tSNd \t1 \t0.85338E-01 \t0.53946E-03 \n  SP3 \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  SP3 \tSNa \t1 \t0.85338E-01 \t0.53946E-03 \n  SP3 \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP3 \tSN0 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP3 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP3 \tSC5 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP3 \tC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  SP3 \tSC4 \t1 \t0.58789E-01 \t0.37162E-03 \n  SP3 \tC3 \t1 \t0.13366E-00 \t0.14408E-02 \n  SP3 \tSC3 \t1 \t0.58789E-01 \t0.37162E-03 \n  SP3 \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP3 \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP3 \tSC2 \t1 \t0.51203E-01 \t0.32367E-03 \n  SP3 \tC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  SP3 \tAC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  SP3 \tSC1 \t1 \t0.43617E-01 \t0.27572E-03 \n  SP3 \tQda \t1 \t0.24145E-00 \t0.26027E-02 \n  SP3 \tSQda \t1 \t0.10620E-00 \t0.67132E-03 \n  SP3 \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  SP3 \tSQd \t1 \t0.10620E-00 \t0.67132E-03 \n  SP3 \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  SP3 \tSQa \t1 \t0.10620E-00 \t0.67132E-03 \n  SP3 \tQ0 \t1 \t0.21558E-00 \t0.23238E-02 \n  SP3 \tSQ0 \t1 \t0.94820E-01 \t0.59939E-03 \n  P2 \tSP2 \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tSP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tNda \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tSNda \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tNd \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tSNd \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tSNa \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tN0 \t1 \t0.17246E-00 \t0.18590E-02 \n  P2 \tSN0 \t1 \t0.17246E-00 \t0.18590E-02 \n  P2 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  P2 \tSC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  P2 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  P2 \tSC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  P2 \tC3 \t1 \t0.13366E-00 \t0.14408E-02 \n  P2 \tSC3 \t1 \t0.13366E-00 \t0.14408E-02 \n  P2 \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  P2 \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  P2 \tSC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  P2 \tC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  P2 \tAC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  P2 \tSC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  P2 \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  P2 \tSQda \t1 \t0.21558E-00 \t0.23238E-02 \n  P2 \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  P2 \tSQd \t1 \t0.21558E-00 \t0.23238E-02 \n  P2 \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  P2 \tSQa \t1 \t0.21558E-00 \t0.23238E-02 \n  P2 \tQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  P2 \tSQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP2 \tP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP2 \tSP1 \t1 \t0.85338E-01 \t0.53946E-03 \n  SP2 \tNda \t1 \t0.19402E-00 \t0.20914E-02 \n  SP2 \tSNda \t1 \t0.85338E-01 \t0.53946E-03 \n  SP2 \tNd \t1 \t0.19402E-00 \t0.20914E-02 \n  SP2 \tSNd \t1 \t0.85338E-01 \t0.53946E-03 \n  SP2 \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  SP2 \tSNa \t1 \t0.85338E-01 \t0.53946E-03 \n  SP2 \tN0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SP2 \tSN0 \t1 \t0.75856E-01 \t0.47952E-03 \n  SP2 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP2 \tSC5 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP2 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP2 \tSC4 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP2 \tC3 \t1 \t0.13366E-00 \t0.14408E-02 \n  SP2 \tSC3 \t1 \t0.58789E-01 \t0.37162E-03 \n  SP2 \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP2 \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP2 \tSC2 \t1 \t0.51203E-01 \t0.32367E-03 \n  SP2 \tC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  SP2 \tAC1 \t1 \t0.99167E-01 \t0.10690E-02 \n  SP2 \tSC1 \t1 \t0.43617E-01 \t0.27572E-03 \n  SP2 \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  SP2 \tSQda \t1 \t0.94820E-01 \t0.59939E-03 \n  SP2 \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  SP2 \tSQd \t1 \t0.94820E-01 \t0.59939E-03 \n  SP2 \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  SP2 \tSQa \t1 \t0.94820E-01 \t0.59939E-03 \n  SP2 \tQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  SP2 \tSQ0 \t1 \t0.85338E-01 \t0.53946E-03 \n  P1 \tSP1 \t1 \t0.19402E-00 \t0.20914E-02 \n  P1 \tNda \t1 \t0.19402E-00 \t0.20914E-02 \n  P1 \tSNda \t1 \t0.19402E-00 \t0.20914E-02 \n  P1 \tNd \t1 \t0.19402E-00 \t0.20914E-02 \n  P1 \tSNd \t1 \t0.19402E-00 \t0.20914E-02 \n  P1 \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  P1 \tSNa \t1 \t0.19402E-00 \t0.20914E-02 \n  P1 \tN0 \t1 \t0.17246E-00 \t0.18590E-02 \n  P1 \tSN0 \t1 \t0.17246E-00 \t0.18590E-02 \n  P1 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  P1 \tSC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  P1 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  P1 \tSC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  P1 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  P1 \tSC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  P1 \tC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  P1 \tAC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  P1 \tSC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  P1 \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  P1 \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  P1 \tSC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  P1 \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  P1 \tSQda \t1 \t0.21558E-00 \t0.23238E-02 \n  P1 \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  P1 \tSQd \t1 \t0.21558E-00 \t0.23238E-02 \n  P1 \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  P1 \tSQa \t1 \t0.21558E-00 \t0.23238E-02 \n  P1 \tQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  P1 \tSQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SP1\tNda \t1 \t0.19402E-00 \t0.20914E-02 \n  SP1 \tSNda \t1 \t0.85338E-01 \t0.53946E-03 \n  SP1\tNd \t1 \t0.19402E-00 \t0.20914E-02 \n  SP1 \tSNd \t1 \t0.85338E-01 \t0.53946E-03 \n  SP1 \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  SP1 \tSNa \t1 \t0.85338E-01 \t0.53946E-03 \n  SP1 \tN0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SP1 \tSN0 \t1 \t0.75856E-01 \t0.47952E-03 \n  SP1 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP1 \tSC5 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP1 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP1 \tSC4 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP1 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  SP1 \tSC3 \t1 \t0.66375E-01 \t0.41957E-03 \n  SP1 \tC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  SP1 \tAC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  SP1 \tSC2 \t1 \t0.58789E-01 \t0.37162E-03 \n  SP1 \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP1 \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SP1 \tSC1 \t1 \t0.51203E-01 \t0.32367E-03 \n  SP1 \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  SP1 \tSQda \t1 \t0.94820E-01 \t0.59939E-03 \n  SP1 \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  SP1 \tSQd \t1 \t0.94820E-01 \t0.59939E-03 \n  SP1 \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  SP1 \tSQa \t1 \t0.94820E-01 \t0.59939E-03 \n  SP1 \tQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SP1 \tSQ0 \t1 \t0.75856E-01 \t0.47952E-03 \n  Nda \tSNda \t1 \t0.19402E-00 \t0.20914E-02 \n  Nda \tNd \t1 \t0.19402E-00 \t0.20914E-02 \n  Nda \tSNd \t1 \t0.19402E-00 \t0.20914E-02 \n  Nda \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  Nda \tSNa \t1 \t0.19402E-00 \t0.20914E-02 \n  Nda \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  Nda \tSN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  Nda \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  Nda \tSC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  Nda \tC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  Nda \tSC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  Nda \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nda \tSC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nda \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nda \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nda \tSC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nda \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nda \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nda \tSC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nda \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  Nda \tSQda \t1 \t0.21558E-00 \t0.23238E-02 \n  Nda \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  Nda \tSQd \t1 \t0.21558E-00 \t0.23238E-02 \n  Nda \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  Nda \tSQa \t1 \t0.21558E-00 \t0.23238E-02 \n  Nda \tQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  Nda \tSQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SNda \tNd \t1 \t0.19402E-00 \t0.20914E-02 \n  SNda \tSNd \t1 \t0.85338E-01 \t0.53946E-03 \n  SNda \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  SNda \tSNa \t1 \t0.85338E-01 \t0.53946E-03 \n  SNda \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SNda \tSN0 \t1 \t0.66375E-01 \t0.41957E-03 \n  SNda \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  SNda \tSC5 \t1 \t0.66375E-01 \t0.41957E-03 \n  SNda \tC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  SNda \tSC4 \t1 \t0.58789E-01 \t0.37162E-03 \n  SNda \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNda \tSC3 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNda \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNda \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNda \tSC2 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNda \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNda \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNda \tSC1 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNda \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  SNda \tSQda \t1 \t0.94820E-01 \t0.59939E-03 \n  SNda \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  SNda \tSQd \t1 \t0.94820E-01 \t0.59939E-03 \n  SNda \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  SNda \tSQa \t1 \t0.94820E-01 \t0.59939E-03 \n  SNda \tQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SNda \tSQ0 \t1 \t0.75856E-01 \t0.47952E-03 \n  Nd \tSNd \t1 \t0.17246E-00 \t0.18590E-02 \n  Nd \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  Nd \tSNa \t1 \t0.19402E-00 \t0.20914E-02 \n  Nd \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  Nd \tSN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  Nd \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  Nd \tSC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  Nd \tC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  Nd \tSC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  Nd \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nd \tSC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nd \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nd \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nd \tSC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nd \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nd \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nd \tSC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Nd \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  Nd \tSQda \t1 \t0.21558E-00 \t0.23238E-02 \n  Nd \tQd \t1 \t0.17246E-00 \t0.18590E-02 \n  Nd \tSQd \t1 \t0.17246E-00 \t0.18590E-02 \n  Nd \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  Nd \tSQa \t1 \t0.21558E-00 \t0.23238E-02 \n  Nd \tQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  Nd \tSQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SNd \tNa \t1 \t0.19402E-00 \t0.20914E-02 \n  SNd \tSNa \t1 \t0.85338E-01 \t0.53946E-03 \n  SNd \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SNd \tSN0 \t1 \t0.66375E-01 \t0.41957E-03 \n  SNd \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  SNd \tSC5 \t1 \t0.66375E-01 \t0.41957E-03 \n  SNd \tC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  SNd \tSC4 \t1 \t0.58789E-01 \t0.37162E-03 \n  SNd \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNd \tSC3 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNd \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNd \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNd \tSC2 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNd \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNd \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNd \tSC1 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNd \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  SNd \tSQda \t1 \t0.94820E-01 \t0.59939E-03 \n  SNd \tQd \t1 \t0.17246E-00 \t0.18590E-02 \n  SNd \tSQd \t1 \t0.75856E-01 \t0.47952E-03 \n  SNd \tQa \t1 \t0.21558E-00 \t0.23238E-02 \n  SNd \tSQa \t1 \t0.94820E-01 \t0.59939E-03 \n  SNd \tQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SNd \tSQ0 \t1 \t0.75856E-01 \t0.47952E-03 \n  Na \tSNa \t1 \t0.17246E-00 \t0.18590E-02 \n  Na \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  Na \tSN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  Na \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  Na \tSC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  Na \tC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  Na \tSC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  Na \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  Na \tSC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  Na \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Na \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Na \tSC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  Na \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Na \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Na \tSC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  Na \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  Na \tSQda \t1 \t0.21558E-00 \t0.23238E-02 \n  Na \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  Na \tSQd \t1 \t0.21558E-00 \t0.23238E-02 \n  Na \tQa \t1 \t0.17246E-00 \t0.18590E-02 \n  Na \tSQa \t1 \t0.17246E-00 \t0.18590E-02 \n  Na \tQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  Na \tSQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SNa \tN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SNa \tSN0 \t1 \t0.66375E-01 \t0.41957E-03 \n  SNa \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  SNa \tSC5 \t1 \t0.66375E-01 \t0.41957E-03 \n  SNa \tC4 \t1 \t0.13366E-00 \t0.14408E-02 \n  SNa \tSC4 \t1 \t0.58789E-01 \t0.37162E-03 \n  SNa \tC3 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNa \tSC3 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNa \tC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNa \tAC2 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNa \tSC2 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNa \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNa \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SNa \tSC1 \t1 \t0.51203E-01 \t0.32367E-03 \n  SNa \tQda \t1 \t0.21558E-00 \t0.23238E-02 \n  SNa \tSQda \t1 \t0.94820E-01 \t0.59939E-03 \n  SNa \tQd \t1 \t0.21558E-00 \t0.23238E-02 \n  SNa \tSQd \t1 \t0.94820E-01 \t0.59939E-03 \n  SNa \tQa \t1 \t0.17246E-00 \t0.18590E-02 \n  SNa \tSQa \t1 \t0.75856E-01 \t0.47952E-03 \n  SNa \tQ0 \t1 \t0.17246E-00 \t0.18590E-02 \n  SNa \tSQ0 \t1 \t0.75856E-01 \t0.47952E-03 \n  N0 \tSN0 \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tSC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tSC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tSC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  N0 \tAC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  N0 \tSC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  N0 \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  N0 \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  N0 \tSC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  N0 \tQda \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tSQda \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tQd \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tSQd \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tQa \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tSQa \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tQ0 \t1 \t0.15091E-00 \t0.16267E-02 \n  N0 \tSQ0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tSC5 \t1 \t0.66375E-01 \t0.41957E-03 \n  SN0 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tSC4 \t1 \t0.66375E-01 \t0.41957E-03 \n  SN0 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tSC3 \t1 \t0.66375E-01 \t0.41957E-03 \n  SN0 \tC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  SN0 \tAC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  SN0 \tSC2 \t1 \t0.58789E-01 \t0.37162E-03 \n  SN0 \tC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SN0 \tAC1 \t1 \t0.11642E-00 \t0.12549E-02 \n  SN0 \tSC1 \t1 \t0.51203E-01 \t0.32367E-03 \n  SN0 \tQda \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tSQda \t1 \t0.66375E-01 \t0.41957E-03 \n  SN0 \tQd \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tSQd \t1 \t0.66375E-01 \t0.41957E-03 \n  SN0 \tQa \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tSQa \t1 \t0.66375E-01 \t0.41957E-03 \n  SN0 \tQ0 \t1 \t0.15091E-00 \t0.16267E-02 \n  SN0 \tSQ0 \t1 \t0.66375E-01 \t0.41957E-03 \n  C5 \tSC5 \t1 \t0.15091E-00 \t0.16267E-02 \n  C5 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  C5 \tSC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  C5 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  C5 \tSC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  C5 \tC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tAC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tSC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tAC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tSC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tQda \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tSQda \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tQd \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tSQd \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tQa \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tSQa \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tQ0 \t1 \t0.13366E-00 \t0.14408E-02 \n  C5 \tSQ0 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC5 \tSC4 \t1 \t0.66375E-01 \t0.41957E-03 \n  SC5 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC5 \tSC3 \t1 \t0.66375E-01 \t0.41957E-03 \n  SC5 \tC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tAC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tSC2 \t1 \t0.58789E-01 \t0.37162E-03 \n  SC5 \tC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tAC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tSC1 \t1 \t0.58789E-01 \t0.37162E-03 \n  SC5 \tQda \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tSQda \t1 \t0.58789E-01 \t0.37162E-03 \n  SC5 \tQd \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tSQd \t1 \t0.58789E-01 \t0.37162E-03 \n  SC5 \tQa \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tSQa \t1 \t0.58789E-01 \t0.37162E-03 \n  SC5 \tQ0 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC5 \tSQ0 \t1 \t0.58789E-01 \t0.37162E-03 \n  C4 \tSC4 \t1 \t0.15091E-00 \t0.16267E-02 \n  C4 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  C4 \tSC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  C4 \tC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  C4 \tAC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  C4 \tSC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  C4 \tC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  C4 \tAC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  C4 \tSC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  C4 \tQda \t1 \t0.11642E-00 \t0.12549E-02 \n  C4 \tSQda \t1 \t0.11642E-00 \t0.12549E-02 \n  C4 \tQd \t1 \t0.11642E-00 \t0.12549E-02 \n  C4 \tSQd \t1 \t0.11642E-00 \t0.12549E-02 \n  C4 \tQa \t1 \t0.11642E-00 \t0.12549E-02 \n  C4 \tSQa \t1 \t0.11642E-00 \t0.12549E-02 \n  C4 \tQ0 \t1 \t0.11642E-00 \t0.12549E-02 \n  C4 \tSQ0 \t1 \t0.11642E-00 \t0.12549E-02 \n  SC4 \tC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC4 \tSC3 \t1 \t0.66375E-01 \t0.41957E-03 \n  SC4 \tC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC4 \tAC2 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC4 \tSC2 \t1 \t0.58789E-01 \t0.37162E-03 \n  SC4 \tC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC4 \tAC1 \t1 \t0.13366E-00 \t0.14408E-02 \n  SC4 \tSC1 \t1 \t0.58789E-01 \t0.37162E-03 \n  SC4 \tQda \t1 \t0.11642E-00 \t0.12549E-02 \n  SC4 \tSQda \t1 \t0.51203E-01 \t0.32367E-03 \n  SC4 \tQd \t1 \t0.11642E-00 \t0.12549E-02 \n  SC4 \tSQd \t1 \t0.51203E-01 \t0.32367E-03 \n  SC4 \tQa \t1 \t0.11642E-00 \t0.12549E-02 \n  SC4 \tSQa \t1 \t0.51203E-01 \t0.32367E-03 \n  SC4 \tQ0 \t1 \t0.11642E-00 \t0.12549E-02 \n  SC4 \tSQ0 \t1 \t0.51203E-01 \t0.32367E-03 \n  C3 \tSC3 \t1 \t0.15091E-00 \t0.16267E-02 \n  C3 \tC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  C3 \tAC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  C3 \tSC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  C3 \tC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  C3 \tAC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  C3 \tSC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  C3 \tQda \t1 \t0.99167E-01 \t0.10690E-02 \n  C3 \tSQda \t1 \t0.99167E-01 \t0.10690E-02 \n  C3 \tQd \t1 \t0.99167E-01 \t0.10690E-02 \n  C3 \tSQd \t1 \t0.99167E-01 \t0.10690E-02 \n  C3 \tQa \t1 \t0.99167E-01 \t0.10690E-02 \n  C3 \tSQa \t1 \t0.99167E-01 \t0.10690E-02 \n  C3 \tQ0 \t1 \t0.99167E-01 \t0.10690E-02 \n  C3 \tSQ0 \t1 \t0.99167E-01 \t0.10690E-02 \n  SC3 \tC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC3 \tAC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC3 \tSC2 \t1 \t0.66375E-01 \t0.41957E-03 \n  SC3 \tC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC3 \tAC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC3 \tSC1 \t1 \t0.66375E-01 \t0.41957E-03 \n  SC3 \tQda \t1 \t0.99167E-01 \t0.10690E-02 \n  SC3 \tSQda \t1 \t0.43617E-01 \t0.27572E-03 \n  SC3 \tQd \t1 \t0.99167E-01 \t0.10690E-02 \n  SC3 \tSQd \t1 \t0.43617E-01 \t0.27572E-03 \n  SC3 \tQa \t1 \t0.99167E-01 \t0.10690E-02 \n  SC3 \tSQa \t1 \t0.43617E-01 \t0.27572E-03 \n  SC3 \tQ0 \t1 \t0.99167E-01 \t0.10690E-02 \n  SC3 \tSQ0 \t1 \t0.43617E-01 \t0.27572E-03 \n  C2 \tAC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  C2 \tSC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  C2 \tC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  C2 \tAC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  C2 \tSC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  C2 \tQda \t1 \t0.45440E-00 \t0.25810E-01 \n  C2 \tSQda \t1 \t0.45440E-00 \t0.25810E-01 \n  C2 \tQd \t1 \t0.45440E-00 \t0.25810E-01 \n  C2 \tSQd \t1 \t0.45440E-00 \t0.25810E-01 \n  C2 \tQa \t1 \t0.45440E-00 \t0.25810E-01 \n  C2 \tSQa \t1 \t0.45440E-00 \t0.25810E-01 \n  C2 \tQ0 \t1 \t0.45440E-00 \t0.25810E-01 \n  C2 \tSQ0 \t1 \t0.45440E-00 \t0.25810E-01 \n  AC2 \tSC2 \t1 \t0.15091E-00 \t0.16267E-02 \n  AC2 \tC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  AC2 \tAC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  AC2 \tSC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  AC2 \tQda \t1 \t0.86233E-01 \t0.92953E-03 \n  AC2 \tSQda \t1 \t0.86233E-01 \t0.92953E-03 \n  AC2 \tQd \t1 \t0.86233E-01 \t0.92953E-03 \n  AC2 \tSQd \t1 \t0.86233E-01 \t0.92953E-03 \n  AC2 \tQa \t1 \t0.86233E-01 \t0.92953E-03 \n  AC2 \tSQa \t1 \t0.86233E-01 \t0.92953E-03 \n  AC2 \tQ0 \t1 \t0.86233E-01 \t0.92953E-03 \n  AC2 \tSQ0 \t1 \t0.86233E-01 \t0.92953E-03 \n  SC2 \tC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC2 \tAC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  SC2 \tSC1 \t1 \t0.66375E-01 \t0.41957E-03 \n  SC2 \tQda \t1 \t0.45440E-00 \t0.25810E-01 \n  SC2 \tSQda \t1 \t0.45440E-00 \t0.25810E-01 \n  SC2 \tQd \t1 \t0.45440E-00 \t0.25810E-01 \n  SC2 \tSQd \t1 \t0.45440E-00 \t0.25810E-01 \n  SC2 \tQa \t1 \t0.45440E-00 \t0.25810E-01 \n  SC2 \tSQa \t1 \t0.45440E-00 \t0.25810E-01 \n  SC2 \tQ0 \t1 \t0.45440E-00 \t0.25810E-01 \n  SC2 \tSQ0 \t1 \t0.45440E-00 \t0.25810E-01 \n  C1 \tAC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  C1 \tSC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  C1 \tQda \t1 \t0.45440E-00 \t0.25810E-01 \n  C1 \tSQda \t1 \t0.45440E-00 \t0.25810E-01 \n  C1 \tQd \t1 \t0.45440E-00 \t0.25810E-01 \n  C1 \tSQd \t1 \t0.45440E-00 \t0.25810E-01 \n  C1 \tQa \t1 \t0.45440E-00 \t0.25810E-01 \n  C1 \tSQa \t1 \t0.45440E-00 \t0.25810E-01 \n  C1 \tQ0 \t1 \t0.45440E-00 \t0.25810E-01 \n  C1 \tSQ0 \t1 \t0.45440E-00 \t0.25810E-01 \n  AC1 \tSC1 \t1 \t0.15091E-00 \t0.16267E-02 \n  AC1 \tQda \t1 \t0.86233E-01 \t0.92953E-03 \n  AC1 \tSQda \t1 \t0.86233E-01 \t0.92953E-03 \n  AC1 \tQd \t1 \t0.86233E-01 \t0.92953E-03 \n  AC1 \tSQd \t1 \t0.86233E-01 \t0.92953E-03 \n  AC1 \tQa \t1 \t0.86233E-01 \t0.92953E-03 \n  AC1 \tSQa \t1 \t0.86233E-01 \t0.92953E-03 \n  AC1 \tQ0 \t1 \t0.86233E-01 \t0.92953E-03 \n  AC1 \tSQ0 \t1 \t0.86233E-01 \t0.92953E-03 \n  SC1 \tQda \t1 \t0.45440E-00 \t0.25810E-01 \n  SC1 \tSQda \t1 \t0.45440E-00 \t0.25810E-01 \n  SC1 \tQd \t1 \t0.45440E-00 \t0.25810E-01 \n  SC1 \tSQd \t1 \t0.45440E-00 \t0.25810E-01 \n  SC1 \tQa \t1 \t0.45440E-00 \t0.25810E-01 \n  SC1 \tSQa \t1 \t0.45440E-00 \t0.25810E-01 \n  SC1 \tQ0 \t1 \t0.45440E-00 \t0.25810E-01 \n  SC1 \tSQ0 \t1 \t0.45440E-00 \t0.25810E-01 \n  Qda \tSQda\t1 \t0.24145E-00 \t0.26027E-02 \n  Qda \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  Qda \tSQd \t1 \t0.24145E-00 \t0.26027E-02 \n  Qda \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  Qda \tSQa \t1 \t0.24145E-00 \t0.26027E-02 \n  Qda \tQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  Qda \tSQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  SQda \tQd \t1 \t0.24145E-00 \t0.26027E-02 \n  SQda \tSQd  \t1 \t0.10620E-00 \t0.67132E-03 \n  SQda \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  SQda \tSQa  \t1 \t0.10620E-00 \t0.67132E-03 \n  SQda \tQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  SQda \tSQ0 \t1 \t0.85338E-01 \t0.53946E-03 \n  Qd \tSQd \t1 \t0.21558E-00 \t0.23238E-02 \n  Qd \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  Qd \tSQa \t1 \t0.24145E-00 \t0.26027E-02 \n  Qd \tQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  Qd \tSQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  SQd \tQa \t1 \t0.24145E-00 \t0.26027E-02 \n  SQd \tSQa  \t1 \t0.10620E-00 \t0.67132E-03 \n  SQd \tQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  SQd \tSQ0 \t1 \t0.85338E-01 \t0.53946E-03 \n  Qa \tSQa \t1 \t0.21558E-00 \t0.23238E-02 \n  Qa \tQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  Qa \tSQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  SQa \tQ0 \t1 \t0.19402E-00 \t0.20914E-02 \n  SQa \tSQ0 \t1 \t0.85338E-01 \t0.53946E-03 \n  Q0 \tSQ0 \t1 \t0.15091E-00 \t0.16267E-02 \n\n[ moleculetype ]\n  W \t    \t1\n\n[ atoms ]\n 1 \tP4 \t1 \tW \tW \t1 \t0 \n\n\n[ moleculetype ]\n  WF\t   \t 1\n                                                                                \n[ atoms ]\n 1 \tBP4 \t1 \tWF\tWF\t1 \t0\n\n\n");
    
    fprintf(dry_martini,"[ defaults ]\n  1      2         no        1.0     1.0\n\n#define dm_rrII        0.4700 4.5000   # regular/regular\n#define dm_rrIII       0.4700 4.0000\n#define dm_rrIV        0.4700 3.5000\n#define dm_rrV         0.4700 3.1000\n#define dm_rrVI        0.4700 2.7000\n#define dm_rrVII       0.4700 2.3000\n#define dm_rrVIII      0.4700 2.0000\n#define dm_rrIX        0.6200 2.0000\n#define dm_rrX         0.4700 1.5000\n#define dm_rrXI        0.4700 1.0000\n#define dm_rrXII       0.4700 0.5000\n#define dm_rrVI_qq     0.6000 2.7000   # regular/regular, charge/charge interactions\n#define dm_rrVII_qq    0.6000 2.3000\n#define dm_rrVIII_qq   0.6000 2.0000\n#define dm_ssII        0.4300 3.4000   # small/small\n#define dm_ssIII       0.4300 3.0000\n#define dm_ssIV        0.4300 2.6000\n#define dm_ssV         0.4300 2.3000\n#define dm_ssVI        0.4300 2.0000\n#define dm_ssVII       0.4300 1.7000\n#define dm_ssVIII      0.4300 1.5000\n#define dm_ssIX        0.6200 2.0000\n#define dm_ssX         0.4300 1.1250\n#define dm_ssXI        0.4300 0.7500\n#define dm_ssXII       0.4300 0.3750\n#define dm_ssVI_qq     0.5500 2.0000   # small/small, charge/charge interactions\n#define dm_ssVII_qq    0.5500 1.7000\n#define dm_ssVIII_qq   0.5500 1.5000\n#define dm_rsII        0.4700 4.5000   # regular/small\n#define dm_rsIII       0.4700 4.0000\n#define dm_rsIV        0.4700 3.5000\n#define dm_rsV         0.4700 3.1000\n#define dm_rsVI        0.4700 2.7000\n#define dm_rsVII       0.4700 2.3000\n#define dm_rsVIII      0.4700 2.0000\n#define dm_rsIX        0.6200 2.0000\n#define dm_rsX         0.4700 1.5000\n#define dm_rsXI        0.4700 1.0000\n#define dm_rsXII       0.4700 0.5000\n#define dm_rsVI_qq     0.6000 2.7000   # regular/small, charge/charge interactions\n#define dm_rsVII_qq    0.6000 2.3000\n#define dm_rsVIII_qq   0.6000 2.0000\n\n[ atomtypes ]\n  Qda     72.0 0.0  A     0.0 0.0   # regular beads\n  Qd      72.0 0.0  A     0.0 0.0\n  Qa      72.0 0.0  A     0.0 0.0\n  Q0      72.0 0.0  A     0.0 0.0\n  P5      72.0 0.0  A     0.0 0.0\n  P4      72.0 0.0  A     0.0 0.0\n  P3      72.0 0.0  A     0.0 0.0\n  P2      72.0 0.0  A     0.0 0.0\n  P1      72.0 0.0  A     0.0 0.0\n  Nda     72.0 0.0  A     0.0 0.0\n  Nd      72.0 0.0  A     0.0 0.0\n  Na      72.0 0.0  A     0.0 0.0\n  N0      72.0 0.0  A     0.0 0.0\n  C5      72.0 0.0  A     0.0 0.0\n  C4      72.0 0.0  A     0.0 0.0\n  C3      72.0 0.0  A     0.0 0.0\n  C2      72.0 0.0  A     0.0 0.0\n  C1      72.0 0.0  A     0.0 0.0\n  SQda    45.0 0.0  A     0.0 0.0   # small beads\n  SQd     45.0 0.0  A     0.0 0.0\n  SQa     45.0 0.0  A     0.0 0.0\n  SQ0     45.0 0.0  A     0.0 0.0\n  SP5     45.0 0.0  A     0.0 0.0\n  SP4     45.0 0.0  A     0.0 0.0\n  SP3     45.0 0.0  A     0.0 0.0\n  SP2     45.0 0.0  A     0.0 0.0\n  SP1     45.0 0.0  A     0.0 0.0\n  SNda    45.0 0.0  A     0.0 0.0\n  SNd     45.0 0.0  A     0.0 0.0\n  SNa     45.0 0.0  A     0.0 0.0\n  SN0     45.0 0.0  A     0.0 0.0\n  SC5     45.0 0.0  A     0.0 0.0\n  SC4     45.0 0.0  A     0.0 0.0\n  SC3     45.0 0.0  A     0.0 0.0\n  SC2     45.0 0.0  A     0.0 0.0\n  SC1     45.0 0.0  A     0.0 0.0\n  SP1c    45.0 0.0  A     0.0 0.0   # fix for dry cholesterol\n\n\n[ nonbond_params ]\n    Qda Qda    1    dm_rrVI_qq\n    Qda Qd     1    dm_rrVI_qq\n    Qda Qa     1    dm_rrVI_qq\n    Qda Q0     1    dm_rrVIII_qq\n    Qda P5     1    dm_rrXII\n    Qda P4     1    dm_rrXII\n    Qda P3     1    dm_rrXII\n    Qda P2     1    dm_rrXII\n    Qda P1     1    dm_rrXII\n    Qda Nda    1    dm_rrXII\n    Qda Nd     1    dm_rrXII\n    Qda Na     1    dm_rrXII\n    Qda N0     1    dm_rrXII\n    Qda C5     1    dm_rrXII\n    Qda C4     1    dm_rrXII\n    Qda C3     1    dm_rrXII\n    Qda C2     1    dm_rrIX\n    Qda C1     1    dm_rrIX\n    Qda SQda   1    dm_rsVI_qq\n    Qda SQd    1    dm_rsVI_qq\n    Qda SQa    1    dm_rsVI_qq\n    Qda SQ0    1    dm_rsVIII_qq\n    Qda SP5    1    dm_rsXII\n    Qda SP4    1    dm_rsXII\n    Qda SP3    1    dm_rsXII\n    Qda SP2    1    dm_rsXII\n    Qda SP1    1    dm_rsXII\n    Qda SNda   1    dm_rsXII\n    Qda SNd    1    dm_rsXII\n    Qda SNa    1    dm_rsXII\n    Qda SN0    1    dm_rsXII\n    Qda SC5    1    dm_rsXII\n    Qda SC4    1    dm_rsXII\n    Qda SC3    1    dm_rsXII\n    Qda SC2    1    dm_rsIX\n    Qda SC1    1    dm_rsIX\n    Qda SP1c   1    dm_rsVI\n     Qd Qd     1    dm_rrVII_qq\n     Qd Qa     1    dm_rrVI_qq\n     Qd Q0     1    dm_rrVIII_qq\n     Qd P5     1    dm_rrXII\n     Qd P4     1    dm_rrXII\n     Qd P3     1    dm_rrXII\n     Qd P2     1    dm_rrXII\n     Qd P1     1    dm_rrXII\n     Qd Nda    1    dm_rrXII\n     Qd Nd     1    dm_rrXII\n     Qd Na     1    dm_rrXII\n     Qd N0     1    dm_rrXII\n     Qd C5     1    dm_rrXII\n     Qd C4     1    dm_rrXII\n     Qd C3     1    dm_rrXII\n     Qd C2     1    dm_rrIX\n     Qd C1     1    dm_rrIX\n     Qd SQda   1    dm_rsVI_qq\n     Qd SQd    1    dm_rsVII_qq\n     Qd SQa    1    dm_rsVI_qq\n     Qd SQ0    1    dm_rsVIII_qq\n     Qd SP5    1    dm_rsXII\n     Qd SP4    1    dm_rsXII\n     Qd SP3    1    dm_rsXII\n     Qd SP2    1    dm_rsXII\n     Qd SP1    1    dm_rsXII\n     Qd SNda   1    dm_rsXII\n     Qd SNd    1    dm_rsXII\n     Qd SNa    1    dm_rsXII\n     Qd SN0    1    dm_rsXII\n     Qd SC5    1    dm_rsXII\n     Qd SC4    1    dm_rsXII\n     Qd SC3    1    dm_rsXII\n     Qd SC2    1    dm_rsIX\n     Qd SC1    1    dm_rsIX\n     Qd SP1c   1    dm_rsVI\n     Qa Qa     1    dm_rrVII_qq\n     Qa Q0     1    dm_rrVIII_qq\n     Qa P5     1    dm_rrXII\n     Qa P4     1    dm_rrXII\n     Qa P3     1    dm_rrXII\n     Qa P2     1    dm_rrXII\n     Qa P1     1    dm_rrXII\n     Qa Nda    1    dm_rrXII\n     Qa Nd     1    dm_rrXII\n     Qa Na     1    dm_rrXII\n     Qa N0     1    dm_rrXII\n     Qa C5     1    dm_rrXII\n     Qa C4     1    dm_rrXII\n     Qa C3     1    dm_rrXII\n     Qa C2     1    dm_rrIX\n     Qa C1     1    dm_rrIX\n     Qa SQda   1    dm_rsVI_qq\n     Qa SQd    1    dm_rsVI_qq\n     Qa SQa    1    dm_rsVII_qq\n     Qa SQ0    1    dm_rsVIII_qq\n     Qa SP5    1    dm_rsXII\n     Qa SP4    1    dm_rsXII\n     Qa SP3    1    dm_rsXII\n     Qa SP2    1    dm_rsXII\n     Qa SP1    1    dm_rsXII\n     Qa SNda   1    dm_rsXII\n     Qa SNd    1    dm_rsXII\n     Qa SNa    1    dm_rsXII\n     Qa SN0    1    dm_rsXII\n     Qa SC5    1    dm_rsXII\n     Qa SC4    1    dm_rsXII\n     Qa SC3    1    dm_rsXII\n     Qa SC2    1    dm_rsIX\n     Qa SC1    1    dm_rsIX\n     Qa SP1c   1    dm_rsVI\n     Q0 Q0     1    dm_rrVIII_qq\n     Q0 P5     1    dm_rrXII\n     Q0 P4     1    dm_rrXII\n     Q0 P3     1    dm_rrXII\n     Q0 P2     1    dm_rrXII\n     Q0 P1     1    dm_rrXII\n     Q0 Nda    1    dm_rrXII\n     Q0 Nd     1    dm_rrXII\n     Q0 Na     1    dm_rrXII\n     Q0 N0     1    dm_rrXII\n     Q0 C5     1    dm_rrXII\n     Q0 C4     1    dm_rrXII\n     Q0 C3     1    dm_rrXII\n     Q0 C2     1    dm_rrIX\n     Q0 C1     1    dm_rrIX\n     Q0 SQda   1    dm_rsVIII_qq\n     Q0 SQd    1    dm_rsVIII_qq\n     Q0 SQa    1    dm_rsVIII_qq\n     Q0 SQ0    1    dm_rsVIII_qq\n     Q0 SP5    1    dm_rsXII\n     Q0 SP4    1    dm_rsXII\n     Q0 SP3    1    dm_rsXII\n     Q0 SP2    1    dm_rsXII\n     Q0 SP1    1    dm_rsXII\n     Q0 SNda   1    dm_rsXII\n     Q0 SNd    1    dm_rsXII\n     Q0 SNa    1    dm_rsXII\n     Q0 SN0    1    dm_rsXII\n     Q0 SC5    1    dm_rsXII\n     Q0 SC4    1    dm_rsXII\n     Q0 SC3    1    dm_rsXII\n     Q0 SC2    1    dm_rsIX\n     Q0 SC1    1    dm_rsIX\n     Q0 SP1c   1    dm_rsVI\n     P5 P5     1    dm_rrV\n     P5 P4     1    dm_rrV\n     P5 P3     1    dm_rrV\n     P5 P2     1    dm_rrV\n     P5 P1     1    dm_rrX\n     P5 Nda    1    dm_rrXII\n     P5 Nd     1    dm_rrXII\n     P5 Na     1    dm_rrXII\n     P5 N0     1    dm_rrXII\n     P5 C5     1    dm_rrXII\n     P5 C4     1    dm_rrXII\n     P5 C3     1    dm_rrXII\n     P5 C2     1    dm_rrXII\n     P5 C1     1    dm_rrXII\n     P5 SQda   1    dm_rsXII\n     P5 SQd    1    dm_rsXII\n     P5 SQa    1    dm_rsXII\n     P5 SQ0    1    dm_rsXII\n     P5 SP5    1    dm_rsV\n     P5 SP4    1    dm_rsV\n     P5 SP3    1    dm_rsV\n     P5 SP2    1    dm_rsV\n     P5 SP1    1    dm_rsX\n     P5 SNda   1    dm_rsXII\n     P5 SNd    1    dm_rsXII\n     P5 SNa    1    dm_rsXII\n     P5 SN0    1    dm_rsXII\n     P5 SC5    1    dm_rsXII\n     P5 SC4    1    dm_rsXII\n     P5 SC3    1    dm_rsXII\n     P5 SC2    1    dm_rsXII\n     P5 SC1    1    dm_rsXII\n     P5 SP1c   1    dm_rsX\n     P4 P4     1    dm_rrVI\n     P4 P3     1    dm_rrVI\n     P4 P2     1    dm_rrVII\n     P4 P1     1    dm_rrX\n     P4 Nda    1    dm_rrXI\n     P4 Nd     1    dm_rrXI\n     P4 Na     1    dm_rrXI\n     P4 N0     1    dm_rrXI\n     P4 C5     1    dm_rrXI\n     P4 C4     1    dm_rrXI\n     P4 C3     1    dm_rrXI\n     P4 C2     1    dm_rrXII\n     P4 C1     1    dm_rrXII\n     P4 SQda   1    dm_rsXII\n     P4 SQd    1    dm_rsXII\n     P4 SQa    1    dm_rsXII\n     P4 SQ0    1    dm_rsXII\n     P4 SP5    1    dm_rsV\n     P4 SP4    1    dm_rsVI\n     P4 SP3    1    dm_rsVI\n     P4 SP2    1    dm_rsVII\n     P4 SP1    1    dm_rsX\n     P4 SNda   1    dm_rsXI\n     P4 SNd    1    dm_rsXI\n     P4 SNa    1    dm_rsXI\n     P4 SN0    1    dm_rsXI\n     P4 SC5    1    dm_rsXI\n     P4 SC4    1    dm_rsXI\n     P4 SC3    1    dm_rsXI\n     P4 SC2    1    dm_rsXII\n     P4 SC1    1    dm_rsXII\n     P4 SP1c   1    dm_rsX\n     P3 P3     1    dm_rrVI\n     P3 P2     1    dm_rrVII\n     P3 P1     1    dm_rrX\n     P3 Nda    1    dm_rrXI\n     P3 Nd     1    dm_rrXI\n     P3 Na     1    dm_rrXI\n     P3 N0     1    dm_rrX\n     P3 C5     1    dm_rrX\n     P3 C4     1    dm_rrX\n     P3 C3     1    dm_rrX\n     P3 C2     1    dm_rrX\n     P3 C1     1    dm_rrXI\n     P3 SQda   1    dm_rsXII\n     P3 SQd    1    dm_rsXII\n     P3 SQa    1    dm_rsXII\n     P3 SQ0    1    dm_rsXII\n     P3 SP5    1    dm_rsV\n     P3 SP4    1    dm_rsVI\n     P3 SP3    1    dm_rsVI\n     P3 SP2    1    dm_rsVII\n     P3 SP1    1    dm_rsX\n     P3 SNda   1    dm_rsXI\n     P3 SNd    1    dm_rsXI\n     P3 SNa    1    dm_rsXI\n     P3 SN0    1    dm_rsX\n     P3 SC5    1    dm_rsX\n     P3 SC4    1    dm_rsX\n     P3 SC3    1    dm_rsX\n     P3 SC2    1    dm_rsX\n     P3 SC1    1    dm_rsXI\n     P3 SP1c   1    dm_rsX\n     P2 P2     1    dm_rrVII\n     P2 P1     1    dm_rrVIII\n     P2 Nda    1    dm_rrXI\n     P2 Nd     1    dm_rrXI\n     P2 Na     1    dm_rrXI\n     P2 N0     1    dm_rrIX\n     P2 C5     1    dm_rrVI\n     P2 C4     1    dm_rrVI\n     P2 C3     1    dm_rrVII\n     P2 C2     1    dm_rrVIII\n     P2 C1     1    dm_rrX\n     P2 SQda   1    dm_rsXII\n     P2 SQd    1    dm_rsXII\n     P2 SQa    1    dm_rsXII\n     P2 SQ0    1    dm_rsXII\n     P2 SP5    1    dm_rsV\n     P2 SP4    1    dm_rsVII\n     P2 SP3    1    dm_rsVII\n     P2 SP2    1    dm_rsVII\n     P2 SP1    1    dm_rsVIII\n     P2 SNda   1    dm_rsXI\n     P2 SNd    1    dm_rsXI\n     P2 SNa    1    dm_rsXI\n     P2 SN0    1    dm_rsIX\n     P2 SC5    1    dm_rsVI\n     P2 SC4    1    dm_rsVI\n     P2 SC3    1    dm_rsVII\n     P2 SC2    1    dm_rsVIII\n     P2 SC1    1    dm_rsX\n     P2 SP1c   1    dm_rsVIII\n     P1 P1     1    dm_rrVII\n     P1 Nda    1    dm_rrVII\n     P1 Nd     1    dm_rrVII\n     P1 Na     1    dm_rrVII\n     P1 N0     1    dm_rrVII\n     P1 C5     1    dm_rrVI\n     P1 C4     1    dm_rrVI\n     P1 C3     1    dm_rrVI\n     P1 C2     1    dm_rrVI\n     P1 C1     1    dm_rrVII\n     P1 SQda   1    dm_rsXII\n     P1 SQd    1    dm_rsXII\n     P1 SQa    1    dm_rsXII\n     P1 SQ0    1    dm_rsXII\n     P1 SP5    1    dm_rsX\n     P1 SP4    1    dm_rsX\n     P1 SP3    1    dm_rsX\n     P1 SP2    1    dm_rsVIII\n     P1 SP1    1    dm_rsVII\n     P1 SNda   1    dm_rsVII\n     P1 SNd    1    dm_rsVII\n     P1 SNa    1    dm_rsVII\n     P1 SN0    1    dm_rsVII\n     P1 SC5    1    dm_rsVI\n     P1 SC4    1    dm_rsVI\n     P1 SC3    1    dm_rsVI\n     P1 SC2    1    dm_rsVI\n     P1 SC1    1    dm_rsVII\n     P1 SP1c   1    dm_rsVII\n    Nda Nda    1    dm_rrVI\n    Nda Nd     1    dm_rrVI\n    Nda Na     1    dm_rrVI\n    Nda N0     1    dm_rrVII\n    Nda C5     1    dm_rrVI\n    Nda C4     1    dm_rrVI\n    Nda C3     1    dm_rrVI\n    Nda C2     1    dm_rrVI\n    Nda C1     1    dm_rrVI\n    Nda SQda   1    dm_rsXII\n    Nda SQd    1    dm_rsXII\n    Nda SQa    1    dm_rsXII\n    Nda SQ0    1    dm_rsXII\n    Nda SP5    1    dm_rsXII\n    Nda SP4    1    dm_rsXI\n    Nda SP3    1    dm_rsXI\n    Nda SP2    1    dm_rsXI\n    Nda SP1    1    dm_rsVII\n    Nda SNda   1    dm_rsVI\n    Nda SNd    1    dm_rsVI\n    Nda SNa    1    dm_rsVI\n    Nda SN0    1    dm_rsVII\n    Nda SC5    1    dm_rsVI\n    Nda SC4    1    dm_rsVI\n    Nda SC3    1    dm_rsVI\n    Nda SC2    1    dm_rsVI\n    Nda SC1    1    dm_rsVI\n    Nda SP1c   1    dm_rsVII\n     Nd Nd     1    dm_rrVII\n     Nd Na     1    dm_rrVI\n     Nd N0     1    dm_rrVII\n     Nd C5     1    dm_rrVI\n     Nd C4     1    dm_rrVI\n     Nd C3     1    dm_rrVI\n     Nd C2     1    dm_rrVI\n     Nd C1     1    dm_rrVI\n     Nd SQda   1    dm_rsXII\n     Nd SQd    1    dm_rsXII\n     Nd SQa    1    dm_rsXII\n     Nd SQ0    1    dm_rsXII\n     Nd SP5    1    dm_rsXII\n     Nd SP4    1    dm_rsXI\n     Nd SP3    1    dm_rsXI\n     Nd SP2    1    dm_rsXI\n     Nd SP1    1    dm_rsVII\n     Nd SNda   1    dm_rsVI\n     Nd SNd    1    dm_rsVII\n     Nd SNa    1    dm_rsVI\n     Nd SN0    1    dm_rsVII\n     Nd SC5    1    dm_rsVI\n     Nd SC4    1    dm_rsVI\n     Nd SC3    1    dm_rsVI\n     Nd SC2    1    dm_rsVI\n     Nd SC1    1    dm_rsVI\n     Nd SP1c   1    dm_rsVII\n     Na Na     1    dm_rrVII\n     Na N0     1    dm_rrVII\n     Na C5     1    dm_rrVI\n     Na C4     1    dm_rrVI\n     Na C3     1    dm_rrVI\n     Na C2     1    dm_rrVI\n     Na C1     1    dm_rrVI\n     Na SQda   1    dm_rsXII\n     Na SQd    1    dm_rsXII\n     Na SQa    1    dm_rsXII\n     Na SQ0    1    dm_rsXII\n     Na SP5    1    dm_rsXII\n     Na SP4    1    dm_rsXI\n     Na SP3    1    dm_rsXI\n     Na SP2    1    dm_rsXI\n     Na SP1    1    dm_rsVII\n     Na SNda   1    dm_rsVI\n     Na SNd    1    dm_rsVI\n     Na SNa    1    dm_rsVII\n     Na SN0    1    dm_rsVII\n     Na SC5    1    dm_rsVI\n     Na SC4    1    dm_rsVI\n     Na SC3    1    dm_rsVI\n     Na SC2    1    dm_rsVI\n     Na SC1    1    dm_rsVI\n     Na SP1c   1    dm_rsVII\n     N0 N0     1    dm_rrV\n     N0 C5     1    dm_rrV\n     N0 C4     1    dm_rrIV\n     N0 C3     1    dm_rrIV\n     N0 C2     1    dm_rrIV\n     N0 C1     1    dm_rrIV\n     N0 SQda   1    dm_rsXII\n     N0 SQd    1    dm_rsXII\n     N0 SQa    1    dm_rsXII\n     N0 SQ0    1    dm_rsXII\n     N0 SP5    1    dm_rsXII\n     N0 SP4    1    dm_rsXI\n     N0 SP3    1    dm_rsX\n     N0 SP2    1    dm_rsIX\n     N0 SP1    1    dm_rsVII\n     N0 SNda   1    dm_rsVII\n     N0 SNd    1    dm_rsVII\n     N0 SNa    1    dm_rsVII\n     N0 SN0    1    dm_rsV\n     N0 SC5    1    dm_rsV\n     N0 SC4    1    dm_rsIV\n     N0 SC3    1    dm_rsIV\n     N0 SC2    1    dm_rsIV\n     N0 SC1    1    dm_rsIV\n     N0 SP1c   1    dm_rsVII\n     C5 C5     1    dm_rrIII\n     C5 C4     1    dm_rrIII\n     C5 C3     1    dm_rrIII\n     C5 C2     1    dm_rrIII\n     C5 C1     1    dm_rrIII\n     C5 SQda   1    dm_rsXII\n     C5 SQd    1    dm_rsXII\n     C5 SQa    1    dm_rsXII\n     C5 SQ0    1    dm_rsXII\n     C5 SP5    1    dm_rsXII\n     C5 SP4    1    dm_rsXI\n     C5 SP3    1    dm_rsX\n     C5 SP2    1    dm_rsVI\n     C5 SP1    1    dm_rsVI\n     C5 SNda   1    dm_rsVI\n     C5 SNd    1    dm_rsVI\n     C5 SNa    1    dm_rsVI\n     C5 SN0    1    dm_rsV\n     C5 SC5    1    dm_rsIII\n     C5 SC4    1    dm_rsIII\n     C5 SC3    1    dm_rsIII\n     C5 SC2    1    dm_rsIII\n     C5 SC1    1    dm_rsIII\n     C5 SP1c   1    dm_rsVI\n     C4 C4     1    dm_rrII\n     C4 C3     1    dm_rrII\n     C4 C2     1    dm_rrIII\n     C4 C1     1    dm_rrIII\n     C4 SQda   1    dm_rsXII\n     C4 SQd    1    dm_rsXII\n     C4 SQa    1    dm_rsXII\n     C4 SQ0    1    dm_rsXII\n     C4 SP5    1    dm_rsXII\n     C4 SP4    1    dm_rsXI\n     C4 SP3    1    dm_rsX\n     C4 SP2    1    dm_rsVI\n     C4 SP1    1    dm_rsVI\n     C4 SNda   1    dm_rsVI\n     C4 SNd    1    dm_rsVI\n     C4 SNa    1    dm_rsVI\n     C4 SN0    1    dm_rsIV\n     C4 SC5    1    dm_rsIII\n     C4 SC4    1    dm_rsII\n     C4 SC3    1    dm_rsII\n     C4 SC2    1    dm_rsIII\n     C4 SC1    1    dm_rsIII\n     C4 SP1c   1    dm_rsVI\n     C3 C3     1    dm_rrII\n     C3 C2     1    dm_rrII\n     C3 C1     1    dm_rrII\n     C3 SQda   1    dm_rsXII\n     C3 SQd    1    dm_rsXII\n     C3 SQa    1    dm_rsXII\n     C3 SQ0    1    dm_rsXII\n     C3 SP5    1    dm_rsXII\n     C3 SP4    1    dm_rsXI\n     C3 SP3    1    dm_rsX\n     C3 SP2    1    dm_rsVII\n     C3 SP1    1    dm_rsVI\n     C3 SNda   1    dm_rsVI\n     C3 SNd    1    dm_rsVI\n     C3 SNa    1    dm_rsVI\n     C3 SN0    1    dm_rsIV\n     C3 SC5    1    dm_rsIII\n     C3 SC4    1    dm_rsII\n     C3 SC3    1    dm_rsII\n     C3 SC2    1    dm_rsII\n     C3 SC1    1    dm_rsII\n     C3 SP1c   1    dm_rsVI\n     C2 C2     1    dm_rrII\n     C2 C1     1    dm_rrII\n     C2 SQda   1    dm_rsIX\n     C2 SQd    1    dm_rsIX\n     C2 SQa    1    dm_rsIX\n     C2 SQ0    1    dm_rsIX\n     C2 SP5    1    dm_rsXII\n     C2 SP4    1    dm_rsXII\n     C2 SP3    1    dm_rsX\n     C2 SP2    1    dm_rsVIII\n     C2 SP1    1    dm_rsVI\n     C2 SNda   1    dm_rsVI\n     C2 SNd    1    dm_rsVI\n     C2 SNa    1    dm_rsVI\n     C2 SN0    1    dm_rsIV\n     C2 SC5    1    dm_rsIII\n     C2 SC4    1    dm_rsIII\n     C2 SC3    1    dm_rsII\n     C2 SC2    1    dm_rsII\n     C2 SC1    1    dm_rsII\n     C2 SP1c   1    dm_rsVI\n     C1 C1     1    dm_rrII\n     C1 SQda   1    dm_rsIX\n     C1 SQd    1    dm_rsIX\n     C1 SQa    1    dm_rsIX\n     C1 SQ0    1    dm_rsIX\n     C1 SP5    1    dm_rsXII\n     C1 SP4    1    dm_rsXII\n     C1 SP3    1    dm_rsXI\n     C1 SP2    1    dm_rsX\n     C1 SP1    1    dm_rsVII\n     C1 SNda   1    dm_rsVI\n     C1 SNd    1    dm_rsVI\n     C1 SNa    1    dm_rsVI\n     C1 SN0    1    dm_rsIV\n     C1 SC5    1    dm_rsIII\n     C1 SC4    1    dm_rsIII\n     C1 SC3    1    dm_rsII\n     C1 SC2    1    dm_rsII\n     C1 SC1    1    dm_rsII\n     C1 SP1c   1    dm_rsVII\n   SQda SQda   1    dm_ssVI_qq\n   SQda SQd    1    dm_ssVI_qq\n   SQda SQa    1    dm_ssVI_qq\n   SQda SQ0    1    dm_ssVIII_qq\n   SQda SP5    1    dm_ssXII\n   SQda SP4    1    dm_ssXII\n   SQda SP3    1    dm_ssXII\n   SQda SP2    1    dm_ssXII\n   SQda SP1    1    dm_ssXII\n   SQda SNda   1    dm_ssXII\n   SQda SNd    1    dm_ssXII\n   SQda SNa    1    dm_ssXII\n   SQda SN0    1    dm_ssXII\n   SQda SC5    1    dm_ssXII\n   SQda SC4    1    dm_ssXII\n   SQda SC3    1    dm_ssXII\n   SQda SC2    1    dm_ssIX\n   SQda SC1    1    dm_ssIX\n   SQda SP1c   1    dm_ssXII\n    SQd SQd    1    dm_ssVII_qq\n    SQd SQa    1    dm_ssVI_qq\n    SQd SQ0    1    dm_ssVIII_qq\n    SQd SP5    1    dm_ssXII\n    SQd SP4    1    dm_ssXII\n    SQd SP3    1    dm_ssXII\n    SQd SP2    1    dm_ssXII\n    SQd SP1    1    dm_ssXII\n    SQd SNda   1    dm_ssXII\n    SQd SNd    1    dm_ssXII\n    SQd SNa    1    dm_ssXII\n    SQd SN0    1    dm_ssXII\n    SQd SC5    1    dm_ssXII\n    SQd SC4    1    dm_ssXII\n    SQd SC3    1    dm_ssXII\n    SQd SC2    1    dm_ssIX\n    SQd SC1    1    dm_ssIX\n    SQd SP1c   1    dm_ssXII\n    SQa SQa    1    dm_ssVII_qq\n    SQa SQ0    1    dm_ssVIII_qq\n    SQa SP5    1    dm_ssXII\n    SQa SP4    1    dm_ssXII\n    SQa SP3    1    dm_ssXII\n    SQa SP2    1    dm_ssXII\n    SQa SP1    1    dm_ssXII\n    SQa SNda   1    dm_ssXII\n    SQa SNd    1    dm_ssXII\n    SQa SNa    1    dm_ssXII\n    SQa SN0    1    dm_ssXII\n    SQa SC5    1    dm_ssXII\n    SQa SC4    1    dm_ssXII\n    SQa SC3    1    dm_ssXII\n    SQa SC2    1    dm_ssIX\n    SQa SC1    1    dm_ssIX\n    SQa SP1c   1    dm_ssXII\n    SQ0 SQ0    1    dm_ssVIII_qq\n    SQ0 SP5    1    dm_ssXII\n    SQ0 SP4    1    dm_ssXII\n    SQ0 SP3    1    dm_ssXII\n    SQ0 SP2    1    dm_ssXII\n    SQ0 SP1    1    dm_ssXII\n    SQ0 SNda   1    dm_ssXII\n    SQ0 SNd    1    dm_ssXII\n    SQ0 SNa    1    dm_ssXII\n    SQ0 SN0    1    dm_ssXII\n    SQ0 SC5    1    dm_ssXII\n    SQ0 SC4    1    dm_ssXII\n    SQ0 SC3    1    dm_ssXII\n    SQ0 SC2    1    dm_ssIX\n    SQ0 SC1    1    dm_ssIX\n    SQ0 SP1c   1    dm_ssXII\n    SP5 SP5    1    dm_ssV\n    SP5 SP4    1    dm_ssV\n    SP5 SP3    1    dm_ssV\n    SP5 SP2    1    dm_ssV\n    SP5 SP1    1    dm_ssX\n    SP5 SNda   1    dm_ssXII\n    SP5 SNd    1    dm_ssXII\n    SP5 SNa    1    dm_ssXII\n    SP5 SN0    1    dm_ssXII\n    SP5 SC5    1    dm_ssXII\n    SP5 SC4    1    dm_ssXII\n    SP5 SC3    1    dm_ssXII\n    SP5 SC2    1    dm_ssXII\n    SP5 SC1    1    dm_ssXII\n    SP5 SP1c   1    dm_rsX\n    SP4 SP4    1    dm_ssVI\n    SP4 SP3    1    dm_ssVI\n    SP4 SP2    1    dm_ssVII\n    SP4 SP1    1    dm_ssX\n    SP4 SNda   1    dm_ssXI\n    SP4 SNd    1    dm_ssXI\n    SP4 SNa    1    dm_ssXI\n    SP4 SN0    1    dm_ssXI\n    SP4 SC5    1    dm_ssXI\n    SP4 SC4    1    dm_ssXI\n    SP4 SC3    1    dm_ssXI\n    SP4 SC2    1    dm_ssXII\n    SP4 SC1    1    dm_ssXII\n    SP4 SP1c   1    dm_rsX\n    SP3 SP3    1    dm_ssVI\n    SP3 SP2    1    dm_ssVII\n    SP3 SP1    1    dm_ssX\n    SP3 SNda   1    dm_ssXI\n    SP3 SNd    1    dm_ssXI\n    SP3 SNa    1    dm_ssXI\n    SP3 SN0    1    dm_ssX\n    SP3 SC5    1    dm_ssX\n    SP3 SC4    1    dm_ssX\n    SP3 SC3    1    dm_ssX\n    SP3 SC2    1    dm_ssX\n    SP3 SC1    1    dm_ssXI\n    SP3 SP1c   1    dm_rsX\n    SP2 SP2    1    dm_ssVII\n    SP2 SP1    1    dm_ssVIII\n    SP2 SNda   1    dm_ssXI\n    SP2 SNd    1    dm_ssXI\n    SP2 SNa    1    dm_ssXI\n    SP2 SN0    1    dm_ssIX\n    SP2 SC5    1    dm_ssVI\n    SP2 SC4    1    dm_ssVI\n    SP2 SC3    1    dm_ssVII\n    SP2 SC2    1    dm_ssVIII\n    SP2 SC1    1    dm_ssX\n    SP2 SP1c   1    dm_rsVIII\n    SP1 SP1    1    dm_ssVII\n    SP1 SNda   1    dm_ssVII\n    SP1 SNd    1    dm_ssVII\n    SP1 SNa    1    dm_ssVII\n    SP1 SN0    1    dm_ssVII\n    SP1 SC5    1    dm_ssVI\n    SP1 SC4    1    dm_ssVI\n    SP1 SC3    1    dm_ssVI\n    SP1 SC2    1    dm_ssVI\n    SP1 SC1    1    dm_ssVII\n    SP1 SP1c   1    dm_rsVII\n   SNda SNda   1    dm_ssVI\n   SNda SNd    1    dm_ssVI\n   SNda SNa    1    dm_ssVI\n   SNda SN0    1    dm_ssVII\n   SNda SC5    1    dm_ssVI\n   SNda SC4    1    dm_ssVI\n   SNda SC3    1    dm_ssVI\n   SNda SC2    1    dm_ssVI\n   SNda SC1    1    dm_ssVI\n   SNda SP1c   1    dm_rsVII\n    SNd SNd    1    dm_ssVII\n    SNd SNa    1    dm_ssVI\n    SNd SN0    1    dm_ssVII\n    SNd SC5    1    dm_ssVI\n    SNd SC4    1    dm_ssVI\n    SNd SC3    1    dm_ssVI\n    SNd SC2    1    dm_ssVI\n    SNd SC1    1    dm_ssVI\n    SNd SP1c   1    dm_rsVII\n    SNa SNa    1    dm_ssVII\n    SNa SN0    1    dm_ssVII\n    SNa SC5    1    dm_ssVI\n    SNa SC4    1    dm_ssVI\n    SNa SC3    1    dm_ssVI\n    SNa SC2    1    dm_ssVI\n    SNa SC1    1    dm_ssVI\n    SNa SP1c   1    dm_rsVII\n    SN0 SN0    1    dm_ssV\n    SN0 SC5    1    dm_ssV\n    SN0 SC4    1    dm_ssIV\n    SN0 SC3    1    dm_ssIV\n    SN0 SC2    1    dm_ssIV\n    SN0 SC1    1    dm_ssIV\n    SN0 SP1c   1    dm_rsVII\n    SC5 SC5    1    dm_ssIII\n    SC5 SC4    1    dm_ssIII\n    SC5 SC3    1    dm_ssIII\n    SC5 SC2    1    dm_ssIII\n    SC5 SC1    1    dm_ssIII\n    SC5 SP1c   1    dm_rsVI\n    SC4 SC4    1    dm_ssII\n    SC4 SC3    1    dm_ssII\n    SC4 SC2    1    dm_ssIII\n    SC4 SC1    1    dm_ssIII\n    SC4 SP1c   1    dm_rsVI\n    SC3 SC3    1    dm_ssII\n    SC3 SC2    1    dm_ssII\n    SC3 SC1    1    dm_ssII\n    SC3 SP1c   1    dm_rsVI\n    SC2 SC2    1    dm_ssII\n    SC2 SC1    1    dm_ssII\n    SC2 SP1c   1    dm_rsVI\n    SC1 SC1    1    dm_ssII\n    SC1 SP1c   1    dm_rsVII\n   SP1c SP1c   1    dm_rsVII\n\n");
    
    fclose(min);
    fclose(term);
    fclose(Lterm);
    fclose(water);
    fclose(martini);
    fclose(dry_martini);
}




///LIBERACION DE MEMORIA Y LIMPIEZA/////////////////////////////////////
void LiberarMemoriaReservada(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt)
{
    int i;
    
    free(CANAL_punt->bond_fuerza);
    free(CANAL_punt->bending_fuerza);
    free(VARIABLE_punt->pull_rate);
    free(VARIABLE_punt->pull_fuerza);
    free(VARIABLE_punt->lista_nodos);
    
    free(VARIABLE_punt->pid_array_cpu);
    
    free(VARIABLE_punt->pid_array_nodo);
    
    for(i=0;i<VARIABLE_punt->N_simulaciones;i++)
        free(VARIABLE_punt->MatrizSeguridad[i]);
    free(VARIABLE_punt->MatrizSeguridad);
    
    free(VARIABLE_punt->ArraySeguridad);
    
    free(VARIABLE_punt->Ezfrecuencia);
    
    free(POL_punt->longitud);
    
}

void LimpiaEstructurasUsadas(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int index_simulacion)
{
    char comando[8192];
    FILE *pipe;
    if(fork()==0)
    {
        sprintf(comando,"rm SISTEMA_canal_%d.itp SISTEMA_dspc_%d.itp SISTEMA_pol_%d.itp topol_%d.top Sistema_term_%d.gro SISTEMA_index_%d.ndx\n",index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
}










/**************************
 *                        *
 * FUNCIONES ESPECIFICAS  *
 *                        *
 **************************/

void GeneraMDP(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt,struct VARIABLE_struct* VARIABLE_punt, int index_simulacion,int index_estad)
{
    FILE *sim;
    char Pcoupl[256];
    char pull_geometry[256];
    char martini_md_name[256];
    char integrator[256];
    float ref_p;
    int nstxout, nstcalcenergy, nstenergy,npullout;
    
    if(strcmp(VARIABLE_punt->pulling_modo,"velocidad")==0)
        VARIABLE_punt->N_sim=(int)(VARIABLE_punt->N_sim_cte*(POL_punt->longitud[index_simulacion]+CANAL_punt->longitudExterior+CANAL_punt->longitudInterior)/(0.01*VARIABLE_punt->pull_rate[index_simulacion]));
    
    if(strcmp(VARIABLE_punt->pulling_modo,"fuerza")==0)
        VARIABLE_punt->N_sim=(int)(VARIABLE_punt->N_sim_cte*((POL_punt->longitud[index_simulacion]+CANAL_punt->longitudExterior+CANAL_punt->longitudInterior)*72.0*((int)(POL_punt->longitud[index_simulacion]/POL_punt->bond_reposo)))/(0.01*VARIABLE_punt->pull_fuerza[index_simulacion]*VARIABLE_punt->tau_t));
    
    
    
    if(strcmp(VARIABLE_punt->Acoplo_presion,"si")==0)
    {
        sprintf(Pcoupl,"parrinello-rahman");
        sprintf(pull_geometry,"direction");
        
        if(strcmp(VARIABLE_punt->pulling,"si")==0)
            ref_p=0.5;
        
        if(strcmp(VARIABLE_punt->pulling,"no")==0)
            ref_p=1.0;
    }
    else
    {
        sprintf(Pcoupl,"no");
        sprintf(pull_geometry,"direction-periodic");
        ref_p=1.0;
    }
    
    if(strcmp(VARIABLE_punt->PintaTrayectoria,"si")==0 || strcmp(VARIABLE_punt->PintaPoro,"si")==0)
        nstxout=VARIABLE_punt->every_out;
    else
        nstxout=VARIABLE_punt->N_sim/1000;//10;

    nstcalcenergy=VARIABLE_punt->N_sim/1000;
    nstenergy=VARIABLE_punt->N_sim/1000;
    
    if(VARIABLE_punt->N_sim<35000)
        npullout=1;
    else
        npullout=1+VARIABLE_punt->N_sim/35000;
    
    if(VARIABLE_punt->tau_t<0)
        sprintf(integrator, "%s", "md");
    else
        sprintf(integrator, "%s", "sd");
    
    sprintf(martini_md_name,"Input/martini_md_%d_%d.mdp",index_simulacion,index_estad);
    
    sim=fopen(martini_md_name,"w");
    
    
    
    if(strcmp(VARIABLE_punt->pulling,"si")==0 && strcmp(VARIABLE_punt->campo_E,"si")==0)
    {
        if(strcmp(VARIABLE_punt->pulling_modo,"velocidad")==0)
            fprintf(sim,"\ntitle\t\t = Simulacion (Modo = Pull)\n\n\n\n\nintegrator\t\t = %s\ndt\t\t = %f  \nnsteps\t\t = %d\nnstcomm\t\t = 100\ncomm-mode\t\t = Linear\ncomm-grps\t\t = \n\n\n\nnstxout\t\t = %d\nnstvout\t\t = %d\nnstfout\t\t = %d\nnstlog\t\t = %d\nnstcalcenergy = %d\nnstenergy\t\t = %d\nnstxout-compressed\t\t = -1\ncompressed-x-grps\t\t = SECO\n\n\ncutoff-scheme\t\t = group\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\n\n\ntcoupl\t\t = v-rescale\ntc-grps\t\t = SECO\ntau_t\t\t = %.1f\nref_t\t\t = %.1f\nPcoupl\t\t = %s\nPcoupltype\t\t = semiisotropic  \ntau_p\t\t = 12.0\ncompressibility\t\t = 3e-4 3e-4\nref_p\t\t = %.2f %.2f\n\ngen_vel\t\t = yes \ngen_temp\t\t = 300 \ngen_seed\t\t = -1\n\npull\t\t\t = yes\npull-ngroups\t\t = 2\npull-ncoords\t\t = 1\npull-coord1-groups\t = 1 2 \npull-coord1-geometry\t = %s\npull-group1-name\t = CONTRAPESO\npull-group2-name\t = PULLING\npull-coord1-type\t = umbrella\npull-coord1-vec\t\t = 0.0 0.0 1.0\npull-coord1-start\t = no\npull-coord1-rate\t = %f\npull-coord1-k\t\t = %.1f\npull-nstxout\t = %d\npull-nstfout\t = %d\n\n \n\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\nE-z = 1 %.2f 0\nE-x = 1 %.2f 0\nE-y = 1 %.2f 0\nE-zt = 1 %.2f 0\nE-xt = 1 %.2f 0\nE-yt = 1 %.2f 0\nperiodic-molecules = yes\n",integrator, VARIABLE_punt->dt,VARIABLE_punt->N_sim,nstxout,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,nstcalcenergy, nstenergy,VARIABLE_punt->tau_t,VARIABLE_punt->ref_t,Pcoupl,ref_p,ref_p,pull_geometry,VARIABLE_punt->pull_rate[index_simulacion],VARIABLE_punt->pull_k,npullout,npullout,VARIABLE_punt->Ezintensidad,VARIABLE_punt->Exintensidad,VARIABLE_punt->Eyintensidad,2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion],2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion],2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion]);
        
        if(strcmp(VARIABLE_punt->pulling_modo,"fuerza")==0 && strcmp(VARIABLE_punt->campo_E,"si")==0)
            fprintf(sim,"\ntitle\t\t = Simulacion (Modo = Pull)\n\n\n\n\nintegrator\t\t = %s\ndt\t\t = %f  \nnsteps\t\t = %d\nnstcomm\t\t = 100\ncomm-mode\t\t = Linear\ncomm-grps\t\t = \n\n\n\nnstxout\t\t = %d\nnstvout\t\t = %d\nnstfout\t\t = %d\nnstlog\t\t = %d\nnstcalcenergy = %d\nnstenergy\t\t = %d\nnstxout-compressed\t\t = -1\ncompressed-x-grps\t\t = SECO\n\n\ncutoff-scheme\t\t = group\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\n\n\ntcoupl\t\t = v-rescale\ntc-grps\t\t = SECO\ntau_t\t\t = %.1f\nref_t\t\t = %.1f\nPcoupl\t\t = %s\nPcoupltype\t\t = semiisotropic  \ntau_p\t\t = 12.0\ncompressibility\t\t = 3e-4 3e-4\nref_p\t\t = %.2f %.2f\n\ngen_vel\t\t = yes \ngen_temp\t\t = 300 \ngen_seed\t\t = -1\n\npull\t\t\t = yes\npull-ngroups\t\t = 2\npull-ncoords\t\t = 1\npull-coord1-groups\t = 1 2 \npull-coord1-geometry\t = %s\npull-group1-name\t = CONTRAPESO\npull-group2-name\t = PULLING\npull-coord1-type\t = constant-force\npull-coord1-vec\t\t = 0.0 0.0 1.0\npull-coord1-start\t = no\npull-coord1-k\t\t = %f\npull-nstxout\t = %d\npull-nstfout\t = %d\n\n \n\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\nE-z = 1 %.2f 0\nE-x = 1 %.2f 0\nE-y = 1 %.2f 0\nE-zt = 1 %.2f 0\nE-xt = 1 %.2f 0\nE-yt = 1 %.2f 0\nperiodic-molecules = yes\n",integrator, VARIABLE_punt->dt,VARIABLE_punt->N_sim,nstxout,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,nstcalcenergy, nstenergy,VARIABLE_punt->tau_t,VARIABLE_punt->ref_t,Pcoupl,ref_p,ref_p,pull_geometry,-1*VARIABLE_punt->pull_fuerza[index_simulacion],npullout,npullout,VARIABLE_punt->Ezintensidad,VARIABLE_punt->Exintensidad,VARIABLE_punt->Eyintensidad,2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion],2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion],2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion]);
    }
    
    if(strcmp(VARIABLE_punt->pulling,"no")==0 && strcmp(VARIABLE_punt->campo_E,"si")==0)
        fprintf(sim,"\ntitle\t\t = Simulacion (Modo = Difusion)\n\n\n\n\nintegrator\t\t = %s\ndt\t\t = %f\nnsteps\t\t = %d\nnstcomm\t\t = 100\ncomm-mode\t\t = Linear\ncomm-grps\t\t = \n\n\n\nnstxout\t\t = %d\nnstvout\t\t = %d\nnstfout\t\t = %d\nnstlog\t\t = %d\nnstcalcenergy = %d\nnstenergy\t\t = %d\nnstxout-compressed\t\t = -1\ncompressed-x-grps\t\t = SECO\n\n\ncutoff-scheme\t\t = group\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\n\n\ntcoupl\t\t = v-rescale\ntc-grps\t\t = SECO\ntau_t\t\t = %.1f\nref_t\t\t = %.1f\nPcoupl\t\t = %s\nPcoupltype\t\t = semiisotropic\ntau_p\t\t = 12.0\ncompressibility\t\t = 3e-4 3e-4\nref_p\t\t = %.2f %.2f\n\ngen_vel\t\t = yes \ngen_temp\t\t = 300 \ngen_seed\t\t = -1\n\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\nE-z = 1 %.2f 0\nE-x = 1 %.2f 0\nE-y = 1 %.2f 0\nE-zt = 1 %.2f 0\nE-xt = 1 %.2f 0\nE-yt = 1 %.2f 0\nperiodic-molecules = yes\n",integrator, VARIABLE_punt->dt,VARIABLE_punt->N_sim,nstxout,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim, nstcalcenergy, nstenergy,VARIABLE_punt->tau_t,VARIABLE_punt->ref_t,Pcoupl,ref_p,ref_p,VARIABLE_punt->Ezintensidad,VARIABLE_punt->Exintensidad,VARIABLE_punt->Eyintensidad,2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion],2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion],2*PI*VARIABLE_punt->Ezfrecuencia[index_simulacion]);
    
    
    
    
    if(strcmp(VARIABLE_punt->pulling,"si")==0 && strcmp(VARIABLE_punt->campo_E,"no")==0)
    {
        if(strcmp(VARIABLE_punt->pulling_modo,"velocidad")==0)
            fprintf(sim,"\ntitle\t\t = Simulacion (Modo = Pull)\n\n\n\n\nintegrator\t\t = %s\ndt\t\t = %f  \nnsteps\t\t = %d\nnstcomm\t\t = 100\ncomm-mode\t\t = Linear\ncomm-grps\t\t = \n\n\n\nnstxout\t\t = %d\nnstvout\t\t = %d\nnstfout\t\t = %d\nnstlog\t\t = %d\nnstcalcenergy = %d\nnstenergy\t\t = %d\nnstxout-compressed\t\t = -1\ncompressed-x-grps\t\t = SECO\n\n\ncutoff-scheme\t\t = group\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\n\n\ntcoupl\t\t = v-rescale\ntc-grps\t\t = SECO\ntau_t\t\t = %.1f\nref_t\t\t = %.1f\nPcoupl\t\t = %s\nPcoupltype\t\t = semiisotropic  \ntau_p\t\t = 12.0\ncompressibility\t\t = 3e-4 3e-4\nref_p\t\t = %.2f %.2f\n\ngen_vel\t\t = yes \ngen_temp\t\t = 300 \ngen_seed\t\t = -1\n\npull\t\t\t = yes\npull-ngroups\t\t = 2\npull-ncoords\t\t = 1\npull-coord1-groups\t = 1 2 \npull-coord1-geometry\t = %s\npull-group1-name\t = CONTRAPESO\npull-group2-name\t = PULLING\npull-coord1-type\t = umbrella\npull-coord1-vec\t\t = 0.0 0.0 1.0\npull-coord1-start\t = no\npull-coord1-rate\t = %f\npull-coord1-k\t\t = %.1f\npull-nstxout\t = %d\npull-nstfout\t = %d\n\n \n\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\nperiodic-molecules = yes\n",integrator, VARIABLE_punt->dt,VARIABLE_punt->N_sim,nstxout,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,nstcalcenergy, nstenergy,VARIABLE_punt->tau_t,VARIABLE_punt->ref_t,Pcoupl,ref_p,ref_p,pull_geometry,VARIABLE_punt->pull_rate[index_simulacion],VARIABLE_punt->pull_k,npullout,npullout);
        
        if(strcmp(VARIABLE_punt->pulling_modo,"fuerza")==0 && strcmp(VARIABLE_punt->campo_E,"no")==0)
            fprintf(sim,"\ntitle\t\t = Simulacion (Modo = Pull)\n\n\n\n\nintegrator\t\t = %s\ndt\t\t = %f  \nnsteps\t\t = %d\nnstcomm\t\t = 100\ncomm-mode\t\t = Linear\ncomm-grps\t\t = \n\n\n\nnstxout\t\t = %d\nnstvout\t\t = %d\nnstfout\t\t = %d\nnstlog\t\t = %d\nnstcalcenergy = %d\nnstenergy\t\t = %d\nnstxout-compressed\t\t = -1\ncompressed-x-grps\t\t = SECO\n\n\ncutoff-scheme\t\t = group\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\n\n\ntcoupl\t\t = v-rescale\ntc-grps\t\t = SECO\ntau_t\t\t = %.1f\nref_t\t\t = %.1f\nPcoupl\t\t = %s\nPcoupltype\t\t = semiisotropic  \ntau_p\t\t = 12.0\ncompressibility\t\t = 3e-4 3e-4\nref_p\t\t = %.2f %.2f\n\ngen_vel\t\t = yes \ngen_temp\t\t = 300 \ngen_seed\t\t = -1\n\npull\t\t\t = yes\npull-ngroups\t\t = 2\npull-ncoords\t\t = 1\npull-coord1-groups\t = 1 2 \npull-coord1-geometry\t = %s\npull-group1-name\t = CONTRAPESO\npull-group2-name\t = PULLING\npull-coord1-type\t = constant-force\npull-coord1-vec\t\t = 0.0 0.0 1.0\npull-coord1-start\t = no\npull-coord1-k\t\t = %f\npull-nstxout\t = %d\npull-nstfout\t = %d\n\n \n\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\nperiodic-molecules = yes\n",integrator, VARIABLE_punt->dt,VARIABLE_punt->N_sim,nstxout,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,nstcalcenergy,nstenergy,VARIABLE_punt->tau_t,VARIABLE_punt->ref_t,Pcoupl,ref_p,ref_p,pull_geometry,-1*VARIABLE_punt->pull_fuerza[index_simulacion],npullout,npullout);
    }
    
    if(strcmp(VARIABLE_punt->pulling,"no")==0 && strcmp(VARIABLE_punt->campo_E,"no")==0)
        fprintf(sim,"\ntitle\t\t = Simulacion (Modo = Difusion)\n\n\n\n\nintegrator\t\t = %s\ndt\t\t = %f\nnsteps\t\t = %d\nnstcomm\t\t = 100\ncomm-mode\t\t = Linear\ncomm-grps\t\t = \n\n\n\nnstxout\t\t = %d\nnstvout\t\t = %d\nnstfout\t\t = %d\nnstlog\t\t = %d\nnstcalcenergy = %d\nnstenergy\t\t = %d\nnstxout-compressed\t\t = -1\ncompressed-x-grps\t\t = SECO\n\n\ncutoff-scheme\t\t = group\nnstlist\t\t = 10\nns_type\t\t = grid\npbc\t\t = xyz\nverlet-buffer-tolerance\t = 0.005\n\n\ncoulombtype\t\t = reaction-field\nrcoulomb\t\t = 1.1\nrlist = 1.1\nepsilon_r\t\t = 15\t\n\nvdw_type\t\t = User \nvdw-modifier\t\t = Potential-shift-verlet\nrvdw\t\t =  1.1\t\n\nenergygrps\t=\tPEG\nenergygrp-table\t=\tPEG PEG\n\n\n\ntcoupl\t\t = v-rescale\ntc-grps\t\t = SECO\ntau_t\t\t = %.1f\nref_t\t\t = %.1f\nPcoupl\t\t = %s\nPcoupltype\t\t = semiisotropic\ntau_p\t\t = 12.0\ncompressibility\t\t = 3e-4 3e-4\nref_p\t\t = %.2f %.2f\n\ngen_vel\t\t = yes \ngen_temp\t\t = 300 \ngen_seed\t\t = -1\n\n\nconstraints\t\t = none \nconstraint_algorithm\t\t = Lincs\nperiodic-molecules = yes\n",integrator, VARIABLE_punt->dt,VARIABLE_punt->N_sim,nstxout,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim,VARIABLE_punt->N_sim, nstcalcenergy, nstenergy,VARIABLE_punt->tau_t,VARIABLE_punt->ref_t,Pcoupl,ref_p,ref_p);
    
    
    
    fclose(sim);
    
}






void Prepara_FicherosParaTermalizacion(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt,struct MEMBRANA_struct* MEMBRANA_punt,int pinoffset,int ntmpi, int ntomp, int index_simulacion)
{
    float box_x;
    float box_y;
    float box_z;
    char comando[8192];
    char *pwd;
    FILE *pipe;
    
    pwd=(char*)malloc(sizeof(char)*256);
    pwd = getenv("PWD");
    
    
    if(fork()==0)
    {
        sprintf(comando,"mkdir ./CarpetaPrincipal_%d\n",index_simulacion);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    if(strcmp(VARIABLE_punt->pulling,"si")==0)
    {
        if(strcmp(VARIABLE_punt->Acoplo_presion,"si")==0)
        {
            if(VARIABLE_punt->N_sim_cte>1.0) box_x=2*VARIABLE_punt->N_sim_cte*(1.5+POL_punt->longitud[index_simulacion]);
            else box_z=2*(1.5+POL_punt->longitud[index_simulacion]);
            box_y=box_x;
            box_z=box_x;
        }
        
        if(strcmp(VARIABLE_punt->Acoplo_presion,"no")==0)
        {
            box_x=0.47+MEMBRANA_punt->lado;
            box_y=0.47+MEMBRANA_punt->lado;
            if(VARIABLE_punt->N_sim_cte>1.0) box_z=2*VARIABLE_punt->N_sim_cte*(1.5+POL_punt->longitud[index_simulacion]);
            else box_z=2*(1.5+POL_punt->longitud[index_simulacion]);
        }
    }
    
    if(strcmp(VARIABLE_punt->pulling,"no")==0) //En este caso la caja es siempre pequenya
    {
        box_x=0.47+MEMBRANA_punt->lado;
        box_y=0.47+MEMBRANA_punt->lado;
        box_z=2*(1.5+POL_punt->longitud[index_simulacion]);
    }
    
    if(fork()==0)
    {
        sprintf(comando,"gmx editconf -f SISTEMA_conf_%d.gro -o Sistema_%d.gro -box %.2f %.2f %.2f -c\n",index_simulacion,index_simulacion,box_x,box_y,box_z);
        pipe=popen(comando,"w");
        
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    
    
    
    if(fork()==0)
    {
        sprintf(comando,"cp ./Input/minim.mdp ./Input/term.mdp ./Input/Lterm.mdp ./Input/dry_martini_v2.1.itp ./Input/martini_v2.2.itp ./Input/water.gro Sistema_%d.gro SISTEMA_canal_%d.itp SISTEMA_pol_%d.itp SISTEMA_dspc_%d.itp SISTEMA_index_%d.ndx topol_%d.top CarpetaPrincipal_%d\n",index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    //if(strcmp(VARIABLE_punt->conf_externa,"no")==0)
    //{
        if(fork()==0)
        {
            sprintf(comando,"cd ./CarpetaPrincipal_%d\ngmx grompp -f minim.mdp -c Sistema_%d.gro -p topol_%d.top -o topol_%d.tpr -n SISTEMA_index_%d.ndx -maxwarn 3\n",index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion);
            pipe=popen(comando,"w");
            
            pclose(pipe);
            
            LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
        }
        wait(NULL);
        
        
        if(fork()==0)
        {
            
            sprintf(comando,"ssh compute0%d\n",VARIABLE_punt->nodo);
            pipe=popen(comando,"w");
            
            
            sprintf(comando,"cd %s/CarpetaPrincipal_%d\ngmx mdrun -s topol_%d.tpr -c Sistema_min_%d.gro -noddcheck -dds 0.99 -ntomp 0 -v\n",pwd,index_simulacion,index_simulacion,index_simulacion,index_simulacion);
            fprintf(pipe,"%s",comando);
            
            pclose(pipe);
            
            LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
        }
        wait(NULL);
    //}
    /*else
    {
        if(fork()==0)
        {
            sprintf(comando,"cp ./CarpetaPrincipal_%d/Sistema_%d.gro ./CarpetaPrincipal_%d/Sistema_min_%d.gro\n",index_simulacion,index_simulacion,index_simulacion,index_simulacion);
            
            pipe=popen(comando,"w");
            pclose(pipe);
            
            LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
        }
        wait(NULL);
    }*/
}


void Simula_Termalizacion(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int pinoffset,int ntmpi, int ntomp,int index_simulacion)
{
    int i;
    float radio_giro_esperado;
    float POL_long_trans;
    float derivada[3]={0,0,0};
    char comando[8192];
    char CarpetaPrincipal_name[256];
    char *pwd;
    FILE *pipe;
    
    pwd=(char*)malloc(sizeof(char)*256);
    pwd = getenv("PWD");
    
    //if(strcmp(VARIABLE_punt->conf_externa,"no")==0)
    //{
        sprintf(CarpetaPrincipal_name,"CarpetaPrincipal_%d",index_simulacion);
        
        if(fork()==0)
        {
            
            sprintf(comando,"cd ./CarpetaPrincipal_%d\ngmx grompp -f term.mdp -c Sistema_min_%d.gro -p topol_%d.top -o topol_%d.tpr -n SISTEMA_index_%d.ndx -maxwarn 3\n",index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion);
            pipe=popen(comando,"w");
            
            pclose(pipe);
            
            LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
        }
        wait(NULL);
        
        
        if(fork()==0)
        {
            
            sprintf(comando,"ssh compute0%d\n",VARIABLE_punt->nodo);
            pipe=popen(comando,"w");
            
            sprintf(comando,"cd %s/CarpetaPrincipal_%d\ngmx mdrun -s topol_%d.tpr -c Sistema_term_%d.gro -noddcheck -dds 0.99 -ntomp 0 -v\n",pwd,index_simulacion,index_simulacion,index_simulacion,index_simulacion);
            fprintf(pipe,"%s",comando);
            
            pclose(pipe);
            
            LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
        }
        wait(NULL);
        
        POL_long_trans=POL_punt->longitud[index_simulacion]-fabs((POL_punt->posicion+1)*0.5*POL_punt->longitud[index_simulacion]+CANAL_punt->bond_reposo*0.5*CANAL_punt->Nb);
        
        
    //}
/*     else
    {
        if(fork()==0)
        {
            sprintf(comando,"cp ./CarpetaPrincipal_%d/Sistema_min_%d.gro ./CarpetaPrincipal_%d/Sistema_term_%d.gro\n",index_simulacion,index_simulacion,index_simulacion,index_simulacion);
            
            pipe=popen(comando,"w");
            pclose(pipe);
            
            LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
        }
        wait(NULL);
    } */
}



void Prepara_FicherosParaTraslocacion(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt,struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int index_simulacion, int index_estad)
{
    char comando[8192];
    FILE *pipe;
    
    if(fork()==0)
    {
        sprintf(comando,"cp ./CarpetaPrincipal_%d/Sistema_term_%d.gro ./CarpetaPrincipal_%d/SISTEMA_index_%d.ndx ./\n",index_simulacion,index_simulacion,index_simulacion,index_simulacion);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    if(fork()==0)
    {
        sprintf(comando,"rm Sistema_%d.gro SISTEMA_conf_%d.gro\nrm -r CarpetaPrincipal_%d\n",index_simulacion,index_simulacion,index_simulacion);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    if(fork()==0)
    {
        sprintf(comando,"mkdir ./Simulacion__%d_%d\n",index_simulacion,index_estad);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    if(fork()==0)
    {
        sprintf(comando,"cp ./Input/martini_md_%d_%d.mdp ./Input/martini_v2.2.itp ./Input/dry_martini_v2.1.itp ./SISTEMA_canal_%d.itp ./SISTEMA_pol_%d.itp ./SISTEMA_dspc_%d.itp ./SISTEMA_index_%d.ndx ./topol_%d.top ./Sistema_term_%d.gro ./Simulacion__%d_%d\n",index_simulacion,index_estad,index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_simulacion,index_estad);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    
    wait(NULL);
}

void Simula_Traslocacion(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt,int pinoffset,int ntmpi, int ntomp,int index_simulacion,int index_estad)
{
    int i,j,k;
    float distancia;
    char comando[8192];
    char* pwd;
    FILE *pipe;
    
    pwd=(char*)malloc(sizeof(char)*256);
    pwd = getenv("PWD");
    
    if(fork()==0)
    {
        sprintf(comando,"cd ./Simulacion__%d_%d\ngmx grompp -f martini_md_%d_%d.mdp -c Sistema_term_%d.gro -p topol_%d.top -o topol_%d.tpr -n SISTEMA_index_%d.ndx -maxwarn 3\n",index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_simulacion,index_simulacion,index_simulacion);
        pipe=popen(comando,"w");
        
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    if(fork()==0)
    {
        
        sprintf(comando,"ssh compute0%d\n",VARIABLE_punt->nodo);
        pipe=popen(comando,"w");
        
        
        sprintf(comando,"cd %s/Simulacion__%d_%d\n",pwd,index_simulacion,index_estad);
        fprintf(pipe,"%s",comando);
        
        sprintf(comando,"gmx mdrun -s topol_%d.tpr -c Sistema_end_%d_%d.gro -o traj_%d_%d.trr -px pullx_%d_%d.xvg -pf pullf_%d_%d.xvg -ntomp 0 -v -dds 0.99 -noddcheck -pin on -pinoffset %d -pinstride 1\n",index_simulacion,index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_estad,VARIABLE_punt->cpu);
        fprintf(pipe,"%s",comando);
        
        
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt);
        exit(0);
    }
    wait(NULL);
    
    
}

void Prepara_FicherosParaAnalisis(struct VARIABLE_struct* VARIABLE_punt,struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt,struct MEMBRANA_struct* MEMBRANA_punt,int index_simulacion,int index_estad)
{
    
    char comando[8192];
    char tiempo_trans_name[256];
    float tiempo_trans;
    char* pwd;
    FILE *pipe,*tiempo_trans_file;
    
    pwd=(char*)malloc(sizeof(char)*256);
    pwd=getenv("PWD");
    
    if(fork()==0)
    {
        sprintf(comando,"ssh compute0%d\n",VARIABLE_punt->nodo);
        pipe=popen(comando,"w");
        
        sprintf(comando,"cd %s/Simulacion__%d_%d\ngmx trjconv -f traj_%d_%d.trr -s topol_%d.tpr -o traj_analisis_%d_%d.gro -pbc mol -n SISTEMA_index_%d.ndx\n",pwd,index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_simulacion,index_estad,index_simulacion);
        fprintf(pipe,"%s",comando);
        
        sprintf(comando,"5\n");
        fprintf(pipe,"%s",comando);
        
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt);
        exit(0);
    }
    wait(NULL);
    
    if(fork()==0)
    {
        sprintf(comando,"cp ./Simulacion__%d_%d/traj_analisis_%d_%d.gro ./Simulacion__%d_%d/pullf_%d_%d.xvg ./Simulacion__%d_%d/pullx_%d_%d.xvg ./\n",index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_estad);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    
    if(strcmp(VARIABLE_punt->PintaTrayectoria,"si")==0 || strcmp(VARIABLE_punt->PintaPoro,"si")==0)
    {
        if(strcmp(VARIABLE_punt->PintaTrayectoria,"si")==0)
        {
            if(fork()==0)
            {
                sprintf(comando,"ssh compute0%d\n",VARIABLE_punt->nodo);
                pipe=popen(comando,"w");
            
                sprintf(comando,"cd %s/Simulacion__%d_%d\ngmx trjconv -f traj_%d_%d.trr -s topol_%d.tpr -o traj_view_%d_%d.pdb -pbc mol -n SISTEMA_index_%d.ndx\n",pwd,index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_simulacion,index_estad,index_simulacion);
                fprintf(pipe,"%s",comando);
            
                sprintf(comando,"4\n");
                fprintf(pipe,"%s",comando);
            
                pclose(pipe);
            
                LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
            }
            wait(NULL);
        }

        if(strcmp(VARIABLE_punt->PintaPoro,"si")==0)
        {
            if(fork()==0)
            {
                sprintf(comando,"ssh compute0%d\n",VARIABLE_punt->nodo);
                pipe=popen(comando,"w");
            
                sprintf(comando,"cd %s/Simulacion__%d_%d\ngmx trjconv -f traj_%d_%d.trr -s topol_%d.tpr -o traj_poro_%d_%d.gro -pbc mol -n SISTEMA_index_%d.ndx\n",pwd,index_simulacion,index_estad,index_simulacion,index_estad,index_simulacion,index_simulacion,index_estad,index_simulacion);
                fprintf(pipe,"%s",comando);
            
                sprintf(comando,"0\n");
                fprintf(pipe,"%s",comando);
            
                pclose(pipe);
            
                LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
            }
            wait(NULL);
        }
    }
    
    else
    {
        if(fork()==0)
        {
            sprintf(comando,"rm -r Simulacion__%d_%d\n",index_simulacion,index_estad);
            
            pipe=popen(comando,"w");
            pclose(pipe);
            
            LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
        }
        wait(NULL);
    }
    
    tiempo_trans=TiempoTraslocacion(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt,index_simulacion,index_estad);
    sprintf(tiempo_trans_name,"TiempoTrans_%d_%d.txt",index_simulacion,index_estad);
    tiempo_trans_file=fopen(tiempo_trans_name,"w");
    fprintf(tiempo_trans_file,"%f",tiempo_trans);
    fclose(tiempo_trans_file);
}

void UtilizaEstructuraPredefinida(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt, int index_simulacion)
{
    char comando[8192];
    char elegido[256];
    FILE* pipe;

    sprintf(elegido, "sistema%d.gro", Dado(VARIABLE_punt->ultimocand-VARIABLE_punt->primercand+1)+VARIABLE_punt->primercand);
    
    if(fork()==0)
    {
        sprintf(comando,"rm ./SISTEMA_conf_%d.gro\n",index_simulacion);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
    
    if(fork()==0)
    {
        sprintf(comando,"cp %s/%s ./SISTEMA_conf_%d.gro\n",VARIABLE_punt->conf_externa, elegido, index_simulacion);
        
        pipe=popen(comando,"w");
        pclose(pipe);
        
        LiberarMemoriaReservada(CANAL_punt,POL_punt,MEMBRANA_punt,VARIABLE_punt); exit(0);
    }
    wait(NULL);
}


int GarantizoQueTodoEstaBien(struct CANAL_struct* CANAL_punt,struct POL_struct* POL_punt, struct MEMBRANA_struct* MEMBRANA_punt,struct VARIABLE_struct* VARIABLE_punt)
{
    int i;
    int inquisidor=0;
    int suma_inquisidor=0;
    char leido[256];
    char modelo_f[256];
    char modelo_x[256];
    char modelo_t[256];
    char file_name[256];
    char comando[256];
    int index_simulacion,index_estad;
    FILE *reader,*pipe;
    
    sprintf(file_name,"stdout.txt");
    pipe=popen("ls >> stdout.txt\n","w");
    pclose(pipe);
    
    reader=fopen(file_name,"r");
    
    if(strcmp(VARIABLE_punt->pulling,"si")==0)
    {
        for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        {
            
            for(index_estad=0;index_estad<VARIABLE_punt->N_estadistica;index_estad++)
            {
                sprintf(modelo_f,"pullf_%d_%d.xvg",index_simulacion,index_estad);
                sprintf(modelo_x,"pullx_%d_%d.xvg",index_simulacion,index_estad);
                sprintf(modelo_t,"TiempoTrans_%d_%d.txt",index_simulacion,index_estad);
                
                inquisidor=0;
                fseek(reader,0,SEEK_SET);
                for(i=0;i<1;i++)
                {
                    fscanf(reader,"%s",leido);
                    if(strcmp(leido,modelo_f)==0)
                        inquisidor++;
                    if(strcmp(leido,modelo_x)==0)
                        inquisidor++;
                    if(strcmp(leido,modelo_t)==0)
                        inquisidor++;
                    
                    if(inquisidor==3)
                        break;
                    
                    if(feof(reader)!=0)
                        break;
                    i=-1;
                }
                if(inquisidor==3)
                    VARIABLE_punt->MatrizSeguridad[index_simulacion][index_estad]=1;
                
                else
                    VARIABLE_punt->MatrizSeguridad[index_simulacion][index_estad]=0;
                
                suma_inquisidor+=inquisidor;
                
                printf("%.2f %%\n",(100.0*(index_estad+index_simulacion*VARIABLE_punt->N_estadistica))/(VARIABLE_punt->N_simulaciones*VARIABLE_punt->N_estadistica));
                
            }
            VARIABLE_punt->ArraySeguridad[index_simulacion]=1;
            for(i=0;i<VARIABLE_punt->N_estadistica;i++)
            {
                if(VARIABLE_punt->MatrizSeguridad[index_simulacion][i]==0)
                {
                    VARIABLE_punt->ArraySeguridad[index_simulacion]=0;
                    break;
                }
            }
        }
    }
    
    else
    {
        for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
        {
            for(index_estad=0;index_estad<VARIABLE_punt->N_estadistica;index_estad++)
            {
                sprintf(modelo_t,"TiempoTrans_%d_%d.txt",index_simulacion,index_estad);
                
                inquisidor=0;
                fseek(reader,0,SEEK_SET);
                for(i=0;i<1;i++)
                {
                    fscanf(reader,"%s",leido);
                    
                    if(strcmp(leido,modelo_t)==0)
                        inquisidor++;
                    
                    if(inquisidor==1)
                        break;
                    
                    if(feof(reader)!=0)
                        break;
                    i=-1;
                }
                if(inquisidor==1)
                    VARIABLE_punt->MatrizSeguridad[index_simulacion][index_estad]=1;
                
                else
                    VARIABLE_punt->MatrizSeguridad[index_simulacion][index_estad]=0;
                
                suma_inquisidor+=inquisidor;
                
                printf("%.2f %%\n",(100.0*(index_estad+index_simulacion*VARIABLE_punt->N_estadistica))/(VARIABLE_punt->N_simulaciones*VARIABLE_punt->N_estadistica));
                
            }
            VARIABLE_punt->ArraySeguridad[index_simulacion]=1;
            for(i=0;i<VARIABLE_punt->N_estadistica;i++)
            {
                if(VARIABLE_punt->MatrizSeguridad[index_simulacion][i]==0)
                {
                    VARIABLE_punt->ArraySeguridad[index_simulacion]=0;
                    break;
                }
            }
        }
    }
    
    
    
    
    printf("\n\n");
    for(index_simulacion=0;index_simulacion<VARIABLE_punt->N_simulaciones;index_simulacion++)
    {
        for(index_estad=0;index_estad<VARIABLE_punt->N_estadistica;index_estad++)
            printf("%d ",VARIABLE_punt->MatrizSeguridad[index_simulacion][index_estad]);
        printf("\n");
    }
    
    fclose(reader);
    
    sprintf(comando,"rm %s\n",file_name);
    pipe=popen(comando,"w");
    pclose(pipe);
    
    
    if(strcmp(VARIABLE_punt->pulling,"si")==0)
    {
        if(suma_inquisidor==3*VARIABLE_punt->N_simulaciones*VARIABLE_punt->N_estadistica)
            return 1;
        else
            return 0;
    }
    
    else
    {
        if(suma_inquisidor==VARIABLE_punt->N_simulaciones*VARIABLE_punt->N_estadistica)
            return 1;
        else
            return 0;
    }
    
    
}
