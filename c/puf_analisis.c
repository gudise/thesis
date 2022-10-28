#include "main.h"
#include "matematicas.h"
#include "digital.h"

#define N_1             0
#define one_out_of_2    1
#define All_pairs       2
#define k_modular       3
#define compare_to_1	4
#define file_externo    5
#define random_pairs    6



int main(int N_opcion, char** opcion)
{
	long    mpfr_prec  =   53;
    char    in_name[256]    =   {"rawdata.mtz"},
            input   =   0;
    char    out_r_name[256] =   {"rawdata_out.mtz"},
            out_d_name[256] =   {"dist_out.mtz"},
            out_x_name[256] =   {"pufexp_out.pex"},
            out_m_name[256] =   {"map.txt"},
            out_h_name[256] =   {"histograma.txt"},            
            output_r        =   0,
            output_d        =   0,
            output_x        =   0,
            output_m        =   0,
            output_h        =   0;
    char    hist_type   =   0,
            hist_norm   =   0,
            resize      =   0;
    int     hist_resolucion,
            trunc       =   -1,
            resize_retos=   1,
            resize_inst =   1,
            resize_rep  =   1,
            resize_y    =   1;
    char    intrad      =   0,
            intrad_h    =   0,
            intrad_a    =   0,
            intrad_d    =   0;
    char    intrad_ajuste_name[256] =   {"intrad_ajuste.txt"},
            intrad_dist_name[256]   =   {"intrad_fist.txt"},
            intrad_hist_name[256]   =   {"intrad_hist.txt"};
    double  caja_intrad     =   1.0,
            xmin_intrad     =   -1,
            xmax_intrad     =   -1,
            media_intrad    =   -1.0,
            desv_intrad, p_intrad;
    char    interd      =   0,
            interd_h    =   0,
            interd_a    =   0,
            interd_d    =   0;
    char    interd_ajuste_name[256] =   {"interd_ajuste.txt"},
            interd_dist_name[256]   =   {"interd_fist.txt"},
            interd_hist_name[256]   =   {"interd_hist.txt"};
    double  caja_interd     =   1.0,
            xmin_interd     =   -1,
            xmax_interd     =   -1,
            media_interd    =   -1.0,
            desv_interd, p_interd;
    char    far_name[256]={"far.txt"},
            frr_name[256]={"frr.txt"},
            roc_name[256]={"roc.txt"};
    char    far =   0,
            frr =   0,
            eer =   0,
            roc =   0;
    double  eer_value;
    int     eer_index=-1;
    int     Nx  =   0,
            Ny  =   0;
    char    fcorr   =   0;
    int     x_coord, y_coord;
    double  frec_esperada       =   100.0,
            desv_frec_esperada  =   1.0,
            factorx             =   1.0,
            factory             =   1.0,
            cteT                =   0;
    int     N_prec  =   1,
            N_rep   =   1,
            N_inst  =   1,
            N_retos =   1,
            N_pdl =     1,
            N_cells =   0;
    char    puftype =   0,
            topol   =   0;
    int     k_mod   =   3,
            N_bits  =   0,
            N_claves, **index_array, contador, contador1;
    int     ntrials     =   1,
            N_exitos    =   0,
            bias_orden  =   1;
    float   umbral_id   =   -1.0;
    char    adversario  =   0,
            verbose     =   1,
            entrop      =   0,
            minentrop   =   0,
            gsort       =   0;
    char    topolfile[256]  =   {"topol.cmt"};
    double  *****pufarray;
    int     aux, *aux_array, aux_rand0, aux_rand1;
    double  daux, *daux_array, entropia, minentropia;
    MATRIZ* rawdata, *set_intrad, *set_interd, *frec_nominal, *bitwrd_bias;
	MPFR_MATRIZ* data;
    PUFEXP* pufexp, *pufexp_ilegit, *pufexp_temp, *pufexp_resize;
    DISTRI* intrad_hist, *intrad_ajuste=NULL, *interd_hist, *interd_ajuste=NULL, *far_curve, *frr_curve, *roc_curve;
    MPFR_DISTRI* histograma;
    BINARIO** bitstring, *caux;
    CMTOPOL* topologia;
    mpfr_t caja, fcaja, xmin, xmax;
    FILE *punte;
	
	for(int i=0; i<N_opcion; i++)
	{
		if( strcmp(opcion[i], "*")==0 )
			sscanf(opcion[i+1], "%ld", &mpfr_prec);
	}
	mpfra_set_default_prec(mpfr_prec);
    mpfra_init_set_ui(caja, 0);
	mpfra_init_set_ui(fcaja, 0);
	mpfra_init_set_ui(xmin, 0);
	mpfra_init_set_ui(xmax, 0);
    
    
	// LEER OPCIONES
    char helptxt[256];
    for(int i=0; i<N_opcion; i++)
    {
        if( strcmp(opcion[i], "-help")==0 )
        {
            sprintf(helptxt, "%s/c/help/puf_analisis.help", getenv("MYREPO"));
            punte = fopen(helptxt, "r");
            while(!feof(punte))
                printf("%c", fgetc(punte));
            
            exit(0);
        }
        
        if( strcmp(opcion[i], "-in")==0 )
        {
            for(int j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0] == '-')
                    break;
                else
                {
                    if( strcmp(opcion[j], "raw")==0 || strcmp(opcion[j], "r")==0)
                    {
                        input = 'r';
                        sprintf(in_name, "rawdata.mtz");
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-')
                                sscanf(opcion[j+1], "%s", in_name);
                        }
                    }
                    else if( strcmp(opcion[j], "dist")==0 || strcmp(opcion[j], "d")==0)
                    {
                        input = 'd';
                        sprintf(in_name, "dist.mtz");
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-')
                                sscanf(opcion[j+1], "%s", in_name);
                        }
                    }
                    else if( strcmp(opcion[j], "pufexp")==0 || strcmp(opcion[j], "x")==0)
                    {
                        input = 'x';
                        sprintf(in_name, "pufexp.pex");
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-')
                                sscanf(opcion[j+1], "%s", in_name);
                        }
                    }
                }
            }
        }
        
        if( strcmp(opcion[i], "-out")==0 )
        {
            for(int j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0] == '-')
                    break;
                else
                {
                    if( strcmp(opcion[j], "raw")==0 || strcmp(opcion[j], "r")==0)
                    {
                        output_r = 1;
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-' && !wordIsInSet(opcion[j+1], "raw r dist d pufexp x map m hist h"))
                                sscanf(opcion[j+1], "%s", out_r_name);
                        }
                    }
                    if( strcmp(opcion[j], "dist")==0 || strcmp(opcion[j], "d")==0)
                    {
                        output_d = 1;
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-' && !wordIsInSet(opcion[j+1], "raw r dist d pufexp x map m hist h"))
                                sscanf(opcion[j+1], "%s", out_d_name);
                        }
                    }
                    if( strcmp(opcion[j], "pufexp")==0 || strcmp(opcion[j], "x")==0)
                    {
                        output_x = 1;
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-' && !wordIsInSet(opcion[j+1], "raw r dist d pufexp x map m hist h"))
                                sscanf(opcion[j+1], "%s", out_x_name);
                        }
                    }
                    if( strcmp(opcion[j], "map")==0 || strcmp(opcion[j], "m")==0)
                    {
                        output_m = 1;
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-' && !wordIsInSet(opcion[j+1], "raw r dist d pufexp x map m hist h"))
                                sscanf(opcion[j+1], "%s", out_m_name);
                        }
                    }
                    if( strcmp(opcion[j], "hist")==0 || strcmp(opcion[j], "h")==0)
                    {
                        output_h = 1;
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-' && !wordIsInSet(opcion[j+1], "raw r dist d pufexp x map m hist h caja xmin xmax resolucion type norm"))
                                sscanf(opcion[j+1], "%s", out_h_name);
                        }
                        for(int k=j+1; k<N_opcion; k++)
                        {
                            if(opcion[k][0]=='-' || wordIsInSet(opcion[k], "raw r dist d pufexp x map m hist h"))
                                break;
                            else
                            {
                                if(strcmp(opcion[k], "caja")==0)
                                {
                                    if(strcmp(opcion[k+1], "f")==0)
                                        mpfra_set_str(fcaja, opcion[k+2], 10);
                                    else
                                        mpfra_set_str(caja, opcion[k+1], 10);
                                }
                                
                                if(strcmp(opcion[k], "xmin")==0)
                                    mpfra_set_str(xmin, opcion[k+1], 10);
                                
                                if(strcmp(opcion[k], "xmax")==0)
                                    mpfra_set_str(xmax, opcion[k+1], 10);
                                
                                if(strcmp(opcion[k], "resolucion")==0)
                                    sscanf(opcion[k+1], "%d", &hist_resolucion);
                                
                                if(strcmp(opcion[k], "sspace")==0)
                                    hist_type=1;
                                
                                if(strcmp(opcion[k], "norm")==0)
                                {
                                    if(k+1 < N_opcion)
                                    {
                                        if(strcmp(opcion[k+1], "alt")==0)
                                            hist_norm=1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        if( strcmp(opcion[i], "-resize_pufexp")==0 || strcmp(opcion[i], "-resx")==0 )
        {
            resize=1;
            sscanf(opcion[i+1], "%d,%d,%d,%d", &resize_retos, &resize_inst, &resize_rep, &resize_y);
        }
        
        if( strcmp(opcion[i], "-truncate_pufexp")==0 || strcmp(opcion[i], "-trux")==0 )
            sscanf(opcion[i+1], "%d", &trunc);
        
        if( strcmp(opcion[i], "-geom")==0 )
        {
            if(i+1 < N_opcion)
            {
                if(opcion[i+1][0] != '-')
                {
                    for(int j=i+1; j<N_opcion; j++)
                    {
                        if( strcmp(opcion[j], "nx")==0)
                        {
                            if(j+1<N_opcion)
                            {
                                if(opcion[j+1][0] != '-' && !wordIsInSet(opcion[j+1], "nx ny"))
                                    sscanf(opcion[j+1], "%d", &Nx);
                            }
                        }
                        if( strcmp(opcion[j], "ny")==0)
                        {
                            if(j+1<N_opcion)
                            {
                                if(opcion[j+1][0] != '-' && !wordIsInSet(opcion[j+1], "nx ny"))
                                    sscanf(opcion[j+1], "%d", &Ny);
                            }
                        }
                    }
                }
            }
        }
        
        if( strcmp(opcion[i], "-sim")==0 )
        {
            for(int j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0] == '-')
                    break;
                else
                {
                    if( strcmp(opcion[j], "frec")==0)
                    {
                        if(j+1 < N_opcion)
                        {
                            if(opcion[j+1][0]!='-' && !wordIsInSet(opcion[j+1], "frec desv fcorr ftemp"))
                                sscanf(opcion[j+1], "%lf", &frec_esperada);
                        }
                    }
                    
                    if( strcmp(opcion[j], "desv")==0)
                    {
                        if(j+1 < N_opcion)
                        {
                            if(opcion[j+1][0]!='-' && !wordIsInSet(opcion[j+1], "frec desv fcorr ftemp"))
                                sscanf(opcion[j+1], "%lf", &desv_frec_esperada);
                        }
                    }
                    
                    if( strcmp(opcion[j], "fcorr")==0)
                    {
                        fcorr=1;
                        for(int k=j+1; k<N_opcion; k++)
                        {
                            if(opcion[k][0] == '-' || wordIsInSet(opcion[k], "frec desv fcorr ftemp"))
                                break;
                            else
                            {
                                if( strcmp(opcion[k], "lineal_x")==0)
                                {
                                    fcorr=1;
                                    if(k+1<N_opcion)
                                    {
                                        if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "frec desv fcorr ftemp"))
                                            sscanf(opcion[k+1], "%lf", &factorx);
                                    }
                                    break;
                                }
                                else if( strcmp(opcion[k], "lineal_xy")==0)
                                {
                                    fcorr=2;
                                    if(k+1<N_opcion)
                                    {
                                        if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "frec desv fcorr ftemp"))
                                            sscanf(opcion[k+1], "%lf", &factorx);
                                    }
                                    if(k+2<N_opcion)
                                    {
                                        if(opcion[k+2][0]!='-' && !wordIsInSet(opcion[k+2], "frec desv fcorr ftemp"))
                                            sscanf(opcion[k+2], "%lf", &factory);
                                    }
                                    break;
                                }
                                else if( strcmp(opcion[k], "producto_xy")==0)
                                {
                                    fcorr=3;
                                    if(k+1<N_opcion)
                                    {
                                        if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "frec desv fcorr ftemp"))
                                            sscanf(opcion[k+1], "%lf", &factorx);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    if( strcmp(opcion[j], "ftemp")==0)
                    {
                        if(j+1<N_opcion)
                        {
                            if(opcion[j+1][0] != '-' && !wordIsInSet(opcion[j+1], "frec desv fcorr ftemp"))
                                sscanf(opcion[j+1], "%lf", &cteT);
                        }
                    }
                }
            }
        }
        
        if( strcmp(opcion[i], "-nretos")==0 )
            sscanf(opcion[i+1], "%d", &N_retos);
        
        if( strcmp(opcion[i], "-ninst")==0 )
            sscanf(opcion[i+1], "%d", &N_inst);
        
        if( strcmp(opcion[i], "-nrep")==0 )
            sscanf(opcion[i+1], "%d", &N_rep);
        
        if( strcmp(opcion[i], "-npdl")==0 )
            sscanf(opcion[i+1], "%d", &N_pdl);
        
        if( strcmp(opcion[i], "-nprec")==0 )
            sscanf(opcion[i+1], "%d", &N_prec);
        
        if(strcmp(opcion[i], "-puf")==0)
        {
            for(int j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0] == '-')
                    break;
                else if(strcmp(opcion[j], "ringoscillator")==0 || strcmp(opcion[j], "ro")==0)
                {
                    puftype=0;
                    for(int k=j+1; k<N_opcion; k++)
                    {
                        if(opcion[k][0]=='-')
                            break;
                        else if(strcmp(opcion[k], "n-1")==0)
                        {
                            topol=N_1;
                        }
                        else if(strcmp(opcion[k], "1_out_of_2")==0 || strcmp(opcion[k], "1o2")==0)
                        {
                            topol=one_out_of_2;
                        }
                        else if(strcmp(opcion[k], "all_pairs")==0 || strcmp(opcion[k], "ap")==0)
                        {
                            topol=All_pairs;
                        }
                        else if(strcmp(opcion[k], "k_modular")==0 || strcmp(opcion[k], "km")==0)
                        {
                            topol=k_modular;
                            for(int l=k+1; l<N_opcion; l++)
                            {
                                if(opcion[l][0]=='-' || wordIsInSet(opcion[l], "ringoscillator ro n-1 1_out_of_2 1o2 all_pairs ap k_modular km compare_to_one c1 file random_pairs rp arbiter arb"))
                                    break;
                                else
                                {
                                    sscanf(opcion[l], "%d", &k_mod);
                                    break;
                                }
                            }
                        }
                        else if(strcmp(opcion[k], "compare_to_one")==0 || strcmp(opcion[k], "c1")==0)
                        {
                            topol=compare_to_1;
                        }
                        else if(strcmp(opcion[k], "file")==0)
                        {
                            topol=file_externo;
                            for(int l=k+1; l<N_opcion; l++)
                            {
                                if(opcion[l][0]=='-' || wordIsInSet(opcion[l], "ringoscillator ro n-1 1_out_of_2 1o2 all_pairs ap k_modular km compare_to_one c1 file random_pairs rp arbiter arb"))
                                    break;
                                else
                                {
                                    sscanf(opcion[l], "%s", topolfile);
                                    break;
                                }
                            }
                        }
                        else if(strcmp(opcion[k], "random_pairs")==0 || strcmp(opcion[k], "rp")==0)
                        {
                            topol=random_pairs;
                            for(int l=k+1; l<N_opcion; l++)
                            {
                                if(opcion[l][0]=='-' || wordIsInSet(opcion[l], "ringoscillator ro n-1 1_out_of_2 1o2 all_pairs ap k_modular km compare_to_one c1 file random_pairs rp arbiter arb"))
                                    break;
                                else
                                {
                                    sscanf(opcion[l], "%d", &N_bits);
                                    break;
                                }
                            }
                        }
                        else
                            break;
                    }
                }
                else if(strcmp(opcion[j], "arbiter")==0 || strcmp(opcion[j], "arb")==0)
                    puftype=1;
                else
                    break;
            }
        }
        
        if( strcmp(opcion[i], "-pufanalysis")==0 || strcmp(opcion[i], "-pa")==0 )
        {
            for(int j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0] == '-')
                    break;
                
                if( strcmp(opcion[j], "intradist")==0 )
                {
                    intrad=1;
                    for(int k=j+1; k<N_opcion; k++)
                    {
                        if(opcion[k][0]=='-' || wordIsInSet(opcion[k], "intradist interdist far frr eer roc"))
                            break;
                        else
                        {
                            if( strcmp(opcion[k], "hist")==0 || strcmp(opcion[k], "h")==0 )
                            {
                                intrad_h=1;
                                if(k+1<N_opcion)
                                {
                                    if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "intradist interdist far frr eer roc hist h dist d ajuste a caja xmin xmax"))
                                    sscanf(opcion[k+1], "%s", intrad_hist_name);
                                }
                                for(int l=k+1; l<N_opcion; l++)
                                {
                                    if(opcion[l][0]=='-' || wordIsInSet(opcion[l], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                                        break;
                                    else
                                    {
                                        if( strcmp(opcion[l], "caja")==0 )
                                            sscanf(opcion[l+1], "%lf", &caja_intrad);
                                
                                        if( strcmp(opcion[l], "xmin")==0 )
                                            sscanf(opcion[l+1], "%lf", &xmin_intrad);
                                
                                        if( strcmp(opcion[l], "xmax")==0 )
                                            sscanf(opcion[l+1], "%lf", &xmax_intrad);
                                    }
                                }
                            }
                            if( strcmp(opcion[k], "dist")==0 || strcmp(opcion[k], "d")==0 )
                            {
                                intrad_d=1;
                                if(k+1<N_opcion)
                                {
                                    if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                                        sscanf(opcion[k+1], "%s", intrad_dist_name);
                                }
                            }
                            if( strcmp(opcion[k], "ajuste")==0 || strcmp(opcion[k], "a")==0 )
                            {
                                intrad_a=1;
                                if(k+1<N_opcion)
                                {
                                    if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                                        sscanf(opcion[k+1], "%s", intrad_ajuste_name);
                                }
                            }
                        }
                    }
                }
                if( strcmp(opcion[j], "interdist")==0 )
                {
                    interd=1;
                    for(int k=j+1; k<N_opcion; k++)
                    {
                        if(opcion[k][0]=='-' || wordIsInSet(opcion[k], "intradist interdist far frr eer roc"))
                            break;
                        else
                        {
                            if( strcmp(opcion[k], "hist")==0 || strcmp(opcion[k], "h")==0 )
                            {
                                interd_h=1;
                                if(k+1<N_opcion)
                                {
                                    if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "intradist interdist far frr eer roc hist h dist d ajuste a caja xmin xmax"))
                                    sscanf(opcion[k+1], "%s", interd_hist_name);
                                }
                                for(int l=k+1; l<N_opcion; l++)
                                {
                                    if(opcion[l][0]=='-' || wordIsInSet(opcion[l], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                                        break;
                                    else
                                    {
                                        if( strcmp(opcion[l], "caja")==0 )
                                            sscanf(opcion[l+1], "%lf", &caja_interd);
                                
                                        if( strcmp(opcion[l], "xmin")==0 )
                                            sscanf(opcion[l+1], "%lf", &xmin_interd);
                                
                                        if( strcmp(opcion[l], "xmax")==0 )
                                            sscanf(opcion[l+1], "%lf", &xmax_interd);
                                    }
                                }
                            }
                            if( strcmp(opcion[k], "dist")==0 || strcmp(opcion[k], "d")==0 )
                            {
                                interd_d=1;
                                if(k+1<N_opcion)
                                {
                                    if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                                        sscanf(opcion[k+1], "%s", interd_dist_name);
                                }
                            }
                            if( strcmp(opcion[k], "ajuste")==0 || strcmp(opcion[k], "a")==0 )
                            {
                                interd_a=1;
                                if(k+1<N_opcion)
                                {
                                    if(opcion[k+1][0]!='-' && !wordIsInSet(opcion[k+1], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                                        sscanf(opcion[k+1], "%s", interd_ajuste_name);
                                }
                            }
                        }
                    }
                }
                if( strcmp(opcion[j], "far")==0 )
                {
                    far=1;
                    if(j+1<N_opcion)
                    {
                        if(opcion[j+1][0]!='-' && !wordIsInSet(opcion[j+1], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                            sscanf(opcion[j+1], "%s", far_name);
                    }
                }
                if( strcmp(opcion[j], "frr")==0 )
                {
                    frr=1;
                    if(j+1<N_opcion)
                    {
                        if(opcion[j+1][0]!='-' && !wordIsInSet(opcion[j+1], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                            sscanf(opcion[j+1], "%s", frr_name);
                    }
                }
                if( strcmp(opcion[j], "eer")==0 )
                    eer=1;
                
                if( strcmp(opcion[j], "roc")==0 )
                {
                    roc=1;
                    if(j+1<N_opcion)
                    {
                        if(opcion[j+1][0]!='-' && !wordIsInSet(opcion[j+1], "intradist interdist far frr eer roc hist h dist d ajuste a"))
                            sscanf(opcion[j+1], "%s", roc_name);
                    }
                }
            }
        }
        
        if( strcmp(opcion[i], "-adversario")==0 || strcmp(opcion[i], "-adv")==0 )
        {
            adversario=1;
            for(int j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0] == '-')
                    break;
                else
                {
                    if( strcmp(opcion[j], "ignorant")==0 || strcmp(opcion[j], "i")==0 )
                        adversario=1;
                    else if( strcmp(opcion[j], "global_bias")==0 || strcmp(opcion[j], "gb")==0 )
                    {
                        adversario=2;
                        for(int k=j+1; k<N_opcion; k++)
                        {
                            if(opcion[k][0]=='-' || wordIsInSet(opcion[k], "ntrials umbral_id"))
                                break;
                            else
                            {
                                sscanf(opcion[j+1], "%d", &bias_orden);
                                break;
                            }
                        }
                    }
                    else if( strcmp(opcion[j], "local_bias")==0 || strcmp(opcion[j], "lb")==0 )
                    {
                        adversario=3;
                        for(int k=j+1; k<N_opcion; k++)
                        {
                            if(opcion[k][0]=='-' || wordIsInSet(opcion[k], "ntrials umbral_id"))
                                break;
                            else
                            {
                                sscanf(opcion[j+1], "%d", &bias_orden);
                                break;
                            }
                        }
                    }
                    else if( strcmp(opcion[j], "machine_learning")==0 || strcmp(opcion[j], "ml")==0 )
                        adversario=4;
                    
                    if(strcmp(opcion[j], "ntrials")==0)
                        sscanf(opcion[j+1], "%d", &ntrials);
                    
                    if(strcmp(opcion[j], "umbral_id")==0)
                        sscanf(opcion[j+1], "%f", &umbral_id);
                }
            }
        }
        
        if( strcmp(opcion[i], "-entropia")==0 )
        {
            entrop = 1;
            for(int j=i+1; j<N_opcion; j++)
            {
                if(opcion[j][0]!='-')
                {
                    if( strcmp(opcion[j], "pb")==0)
                        entrop=2;
                    else if(strcmp(opcion[j], "pn")==0)
                        entrop=3;
                    else
                        entrop = 1;
                }
            }
        }
            
        if( strcmp(opcion[i], "-minentropia")==0 )
            minentrop=1;

		if(strcmp(opcion[i], "-nv")==0)
			verbose = 0;
			
		if( strcmp(opcion[i], "gsort") ==0)
			gsort=1;
    }
    
    
    // LEER ENTRADA / SIMULAR:
    if(input==0 || input=='r')
    {
        if(input==0)
        {
            if(Nx==0)
                Nx=1;
            if(Ny==0)
                Ny=1;
            if(N_retos==0)
                N_retos=1;
            if(N_inst==0)
                N_inst=1;
            if(N_rep==0)
                N_rep=1;
        }
        else
        {
            rawdata = leerMatriz(in_name);
            
            if(Nx>0 && Ny>0)
            {
                if(rawdata->N_columnas != Nx*Ny)
                {
                    printf("\nERROR: el numero de columnas introducidas no coincide con el producto Nx*Ny introducido.\n");                    
                    return -1;
                }
            }
            else if(Nx>0 && Ny==0)
            {
                if(rawdata->N_columnas%Nx!=0)
                {
                    printf("\nERROR: el valor Nx introducido no es divisor del numero de columnas leidas.\n");                    
                    return -1;
                }
                else
                    Ny = rawdata->N_columnas/Nx;
            }
            else if(Nx==0 && Ny>0)
            {
                if(rawdata->N_columnas%Ny!=0)
                {
                    printf("\nERROR: el valor Nx introducido no es divisor del numero de columnas leidas.\n");                    
                    return -1;
                }
                else
                    Nx = rawdata->N_columnas/Ny;
            }
            else
            {
                Nx = rawdata->N_columnas;
                Ny=1;
            }
            
            if(N_pdl>0 && N_inst>0 && N_rep>0)
            {
                if(rawdata->N_filas != N_pdl*N_inst*N_rep)
                {
                    printf("\nERROR: el numero de filas introducidas no coincide con los numeros (Nretos, Ninst, Nrep) introducidos.\n");                    
                    return -1;
                }
            }
            else if(N_pdl>0 && N_inst>0 && N_rep==0)
            {
                if(rawdata->N_filas%(N_pdl*N_inst) != 0)
                {
                    printf("\nERROR: el producto de los valores (Nretos, Ninst) introducidos no es divisor del numero de filas leidas.\n");                    
                    return -1;
                }
                else
                    N_rep = rawdata->N_filas/(N_pdl*N_inst);       
            }
            else if(N_pdl>0 && N_inst==0 && N_rep>0)
            {
                if(rawdata->N_filas%(N_pdl*N_rep) != 0)
                {
                    printf("\nERROR: el producto de los valores (Nretos, Nrep) introducidos no es divisor del numero de filas leidas.\n");                    
                    return -1;
                }
                else
                    N_inst = rawdata->N_filas/(N_pdl*N_rep);
            }
            else if(N_pdl==0 && N_inst>0 && N_rep>0)
            {
                if(rawdata->N_filas%(N_rep*N_inst) != 0)
                {
                    printf("\nERROR: el producto de los valores (Ninst, N_rep) introducidos no es divisor del numero de filas leidas.\n");                    
                    return -1;
                }
                else
                    N_pdl = rawdata->N_filas/(N_rep*N_inst);
            }
            else if(N_pdl>0 && N_inst==0 && N_rep==0)
            {
                if(rawdata->N_filas%N_pdl != 0)
                {
                    printf("\nERROR: el valor Nretos introducido no es divisor del numero de filas leidas.\n");                    
                    return -1;
                }
                else
                {
                    N_rep = rawdata->N_filas/N_pdl;
                    N_inst = 1;
                }
            }
            else if(N_pdl==0 && N_inst>0 && N_rep==0)
            {
                if(rawdata->N_filas%N_inst != 0)
                {
                    printf("\nERROR: el valor Ninst introducido no es divisor del numero de filas leidas.\n");                    
                    return -1;
                }
                else
                {
                    N_rep = rawdata->N_filas/N_inst;
                    N_pdl = 1;
                }
            }
            else if(N_pdl==0 && N_inst==0 && N_rep>0)
            {
                if(rawdata->N_filas%N_inst != 0)
                {
                    printf("\nERROR: el valor Nrep introducido no es divisor del numero de filas leidas.\n");                    
                    return -1;
                }
                else
                {
                    N_inst = rawdata->N_filas/N_rep;
                    N_pdl = 1;
                }
            }
            else
            {
                N_rep = rawdata->N_filas;
                N_inst = 1;
                N_pdl = 1;
            }
        }
        
        if(topol==file_externo)
        {
            topologia=leerCmtopol(topolfile);
            
            if(Nx*Ny>topologia->N_celdas)
            {
                Nx = topologia->N_celdas;
                Ny=1;
            }
            else
            {
                printf("\n\nERROR: el fichero de topologia introducido tiene mas celdas que el fichero de datos\n");
                return -1;
            }
        }
        
        N_cells = Nx*Ny;
        N_claves = N_retos*N_inst*N_rep*N_pdl;
        pufarray = (double*****)malloc(sizeof(double****)*N_retos);
        for(int i=0; i<N_retos; i++)
        {
            pufarray[i] = (double****)malloc(sizeof(double***)*N_inst);
            for(int j=0; j<N_inst; j++)
            {
                pufarray[i][j] = (double***)malloc(sizeof(double**)*N_rep);
                for(int k=0; k<N_rep; k++)
                {
                    pufarray[i][j][k] = (double**)malloc(sizeof(double*)*N_pdl);
                    for(int l=0; l<N_pdl; l++)
                        pufarray[i][j][k][l] = (double*)malloc(sizeof(double)*N_cells);
                }
            }
        }
        
        if(input==0)
        {
            frec_nominal = mallocMatriz(Nx, Ny);
            for(int j=0; j<N_inst; j++)
            {
                for(int k=0; k<Nx; k++)
                {
                    for(int l=0; l<Ny; l++)
                        frec_nominal->elemento[k][l] = ran1_gauss(frec_esperada, desv_frec_esperada);
                }
                for(int k=0; k<N_rep; k++)
                {
                    for(int l=0; l<N_pdl; l++)
                    {
                        for(int m=0; m<N_cells; m++)
                        {   
                            x_coord = m%Nx;
                            y_coord = m/Nx;
                            
                            pufarray[0][j][k][l][m] = frec_nominal->elemento[x_coord][y_coord] + ran1_gauss(0, cteT);//valores nominales y acoplo termico
                            
                            if(fcorr==1)
                                pufarray[0][j][k][l][m] += factorx*x_coord;
                            else if(fcorr==2)
                                pufarray[0][j][k][l][m] += factorx*x_coord+factory*y_coord;
                            else if(fcorr==3)
                                pufarray[0][j][k][l][m] += factorx*x_coord*y_coord;
                        }
                    }
                }
            }
            freeMatriz(frec_nominal);
        }
        else
        {
            for(int j=0; j<N_inst; j++)
            {
                for(int k=0; k<N_rep; k++)
                {
                    for(int l=0; l<N_pdl; l++)
                    {
                        for(int m=0; m<N_cells; m++)
                            pufarray[0][j][k][l][m]=rawdata->elemento[(j*N_rep+k)*N_pdl+l][m];
                    }
                }
            }
        }
        
        if(output_m)
        {
            punte = fopen(out_m_name, "w");

            for(int m=0; m<N_cells; m++)
            {
                x_coord = m%Nx;
                y_coord = m/Nx;
                fprintf(punte, "%d\t%d\t%lf\n", x_coord, y_coord, pufarray[0][0][0][0][m]);
            }
            fclose(punte);
        }
        
        index_array = (int**)malloc(sizeof(int*)*N_retos);
        for(int i=0; i<N_retos; i++)
        {
            index_array[i] = (int*)malloc(sizeof(int)*N_cells);
            for(int m=0; m<N_cells; m++)
                index_array[i][m] = m;
        }
        
        for(int i=1; i<N_retos; i++)
            gsl_ran_shuffle(idum, index_array[i], N_cells, sizeof(int));
        
        for(int i=0; i<N_retos; i++)
        {
            for(int j=0; j<N_inst; j++)
            {
                for(int k=0; k<N_rep; k++)
                {
                    for(int l=0; l<N_pdl; l++)
                    {
                        for(int m=0; m<N_cells; m++)
                            pufarray[i][j][k][l][m] = pufarray[0][j][k][l][index_array[i][m]];
                    }
                }
            }
        }
        
        //Ahora debemos implementar la topologia
        switch(topol)
        {
            case N_1:
                N_bits = N_cells-1;
            break;
            
            case one_out_of_2:
                N_bits = N_cells/2;
            break;
            
            case All_pairs:
                N_bits = (N_cells*(N_cells-1))/2;
            break;
            
            case k_modular:
                N_bits = (k_mod-1)*N_cells/2;
            break;
            
            case compare_to_1:
                N_bits = N_cells-1;
            break;
            
            case file_externo:
                N_bits = topologia->N_bits;
            break;
            
            case random_pairs:
                if(N_bits==0)
                    N_bits = 1;
            break;
        }
        N_bits*=N_prec;
        
        bitstring = (BINARIO**)malloc(sizeof(BINARIO*)*N_claves);
        for(int i=0; i<N_claves; i++)
            bitstring[i] = mallocBinario(N_bits);
        
        caux = mallocBinario(N_prec);
        
        pufexp_temp = mallocPufexp(N_retos, N_inst, N_rep, 2*N_bits, N_pdl*N_bits);

        contador=0;
        for(int i=0; i<N_retos; i++)
        {
            for(int j=0; j<N_inst; j++)
            {
                for(int k=0; k<N_rep; k++)
                {
                    for(int l=0; l<N_pdl; l++)
                    {
                        switch(topol) //El objetivo es rellenar bitstring: deben recorrerse todos los bits!
                        {
                            case file_externo:
                                contador1=0; //contador de 0 a N_bits-1
                                for(int m=0; m<N_cells; m++)
                                {
                                    for(int n=0; n<N_cells; n++)
                                    {
                                        if(topologia->matriz[m][n]>0)
                                        {
                                            daux = pufarray[i][j][k][l][m]-pufarray[i][j][k][l][n];
                                            daux*=1e+6;
                                            digitalizarComparacion(caux, (long)daux); 
                                            for(int n=0; n<N_prec; n++)
                                                bitstring[contador]->digito[contador1*N_prec+n]=caux->digito[n];
                                            
                                            pufexp_temp->elementox[i][j][k][2*contador1]=index_array[i][m];
                                            pufexp_temp->elementox[i][j][k][2*contador1+1]=index_array[i][n];
                                            
                                            contador1++;
                                        }
                                    }
                                }
                                
                                break;
                                
                            case N_1:
                                contador1=0; //contador de 0 a N_bits-1
                                for(int m=0; m<N_cells-1; m++)
                                {
                                    daux = pufarray[i][j][k][l][m]-pufarray[i][j][k][l][m+1];
                                    daux*=1e+6;
                                    digitalizarComparacion(caux, (long)daux);
                                    for(int n=0; n<N_prec; n++)
                                        bitstring[contador]->digito[contador1*N_prec+n]=caux->digito[n];
                                    
                                    pufexp_temp->elementox[i][j][k][2*contador1]=index_array[i][m];
                                    pufexp_temp->elementox[i][j][k][2*contador1+1]=index_array[i][m+1];
                                    
                                    contador1++;
                                }
                                break;
                                
                            case compare_to_1:
                                contador1=0; //contador de 0 a N_bits-1
                                for(int m=1; m<N_cells; m++)
                                {
                                    daux = pufarray[i][j][k][l][0]-pufarray[i][j][k][l][m];
                                    daux*=1e+6;
                                    digitalizarComparacion(caux, (long)daux);
                                    for(int n=0; n<N_prec; n++)
                                        bitstring[contador]->digito[contador1*N_prec+n]=caux->digito[n];
                                    
                                    pufexp_temp->elementox[i][j][k][2*contador1]=index_array[i][0];
                                    pufexp_temp->elementox[i][j][k][2*contador1+1]=index_array[i][m];
                                    
                                    contador1++;
                                }
                                break;
                                
                            case one_out_of_2:
                                contador1=0; //contador de 0 a N_bits-1
                                for(int m=0; m<N_cells; m+=2)
                                {
                                    daux = pufarray[i][j][k][l][m]-pufarray[i][j][k][l][m+1];
                                    daux*=1e+6;
                                    digitalizarComparacion(caux, (long)daux);
                                    for(int n=0; n<N_prec; n++)
                                        bitstring[contador]->digito[contador1*N_prec+n]=caux->digito[n];
                                    
                                    pufexp_temp->elementox[i][j][k][2*contador1]=index_array[i][m];
                                    pufexp_temp->elementox[i][j][k][2*contador1+1]=index_array[i][m+1];
                                    
                                    contador1++;
                                }
                                break;
                                
                                
                            case All_pairs:
                                contador1=0; //contador de 0 a N_bits-1
                                for(int m=0; m<N_cells; m++)
                                {
                                    for(int n=m+1; n<N_cells; n++)
                                    {
                                        daux = pufarray[i][j][k][l][m]-pufarray[i][j][k][l][n];
                                        daux*=1e+6;
                                        digitalizarComparacion(caux, (long)daux);
                                        for(int n=0; n<N_prec; n++)
                                            bitstring[contador]->digito[contador1*N_prec+n]=caux->digito[n];
                                        
                                        pufexp_temp->elementox[i][j][k][2*contador1]=index_array[i][m];
                                        pufexp_temp->elementox[i][j][k][2*contador1+1]=index_array[i][n];
                                        
                                        contador1++;
                                    }
                                }
                                break;
                                
                                
                            case k_modular:
                                contador1=0;
                                for(int m=0; m<N_cells/k_mod; m++)
                                {
                                    for(int n=0; n<k_mod; n++)
                                    {
                                        for(int q=n+1; q<k_mod; q++)
                                        {
                                            daux = pufarray[i][j][k][l][m*k_mod+n]-pufarray[i][j][k][l][m*k_mod+q];
                                            daux*=1e+6;
                                            digitalizarComparacion(caux, (long)daux);
                                            for(int n=0; n<N_prec; n++)
                                                bitstring[contador]->digito[contador1*N_prec+n]=caux->digito[n];
                                            
                                            pufexp_temp->elementox[i][j][k][2*contador1]=index_array[i][m*k_mod+n];
                                            pufexp_temp->elementox[i][j][k][2*contador1+1]=index_array[i][m*k_mod+q];
                                            
                                            contador1++;
                                        }
                                    }
                                }
                                break;
                                
                            case random_pairs:
                                for(int m=0; m<N_bits; m++)
                                {
                                    aux_rand0 = dado(N_cells);
                                    while(1)
                                    {
                                        aux_rand1=dado(N_cells);
                                        if(aux_rand0!=aux_rand1)
                                            break;
                                    }
                                    if(aux_rand0>aux_rand1)
                                        daux = pufarray[0][j][k][l][aux_rand0]-pufarray[0][j][k][l][aux_rand1];
                                    else
                                        daux = pufarray[0][j][k][l][aux_rand1]-pufarray[0][j][k][l][aux_rand0];
                                    daux*=1e+6;
                                    digitalizarComparacion(caux, (long)daux);
                                    for(int n=0; n<N_prec; n++)
                                        bitstring[contador]->digito[l*N_prec+n]=caux->digito[n];
                                    
                                    pufexp_temp->elementox[i][j][k][2*l]=aux_rand0;
                                    pufexp_temp->elementox[i][j][k][2*l+1]=aux_rand1;
                                    
                                }
                                break;
                        }
                        //mpfra_set_str(data->elemento[contador][0], bitstring[contador]->digito, 2);
                        for(int m=0; m<N_bits; m++)
                        {
                            if(bitstring[contador]->digito[m]=='0')
                                pufexp_temp->elementoy[i][j][k][l*N_bits+m]=0;
                            else
                                pufexp_temp->elementoy[i][j][k][l*N_bits+m]=1;
                        }
                        
                        contador++; //contador de 0 a N_claves-1
                    }
                }
            }
        }
        for(int i=0; i<N_claves; i++)
            freeBinario(bitstring[i]);
        free(bitstring);
        
        for(int i=0; i<N_retos; i++)
            free(index_array[i]);
        free(index_array);
        freeBinario(caux);

        if(resize && trunc>0) //primero resize, luego trunc
        {
            pufexp_resize = resize_pex(pufexp_temp, resize_retos, resize_inst, resize_rep, resize_y);
            pufexp = truncate_pex(pufexp_resize, trunc);
            freePufexp(pufexp_resize);
        }
        else if(resize && trunc<=0) //solo resize
            pufexp = resize_pex(pufexp_temp, resize_retos, resize_inst, resize_rep, resize_y);
        
        else if(!resize && trunc>0) // solo trunc
            pufexp = truncate_pex(pufexp_temp, trunc);
            
        else // ninguno de los dos
            pufexp = copiar_pex(pufexp_temp);
        
        freePufexp(pufexp_temp);

        N_bits=pufexp->tamano_respuesta;
        N_retos=pufexp->N_retos;
        N_inst=pufexp->N_instancias;
        N_rep=pufexp->N_repeticiones;
        
        data = pex_to_datamtz(pufexp);
    }
    else if(input=='x')
    {
        pufexp_temp = leerPufexp(in_name);
        
        if(resize && trunc>0) //primero resize, luego trunc
        {
            pufexp_resize = resize_pex(pufexp_temp, resize_retos, resize_inst, resize_rep, resize_y);
            pufexp = truncate_pex(pufexp_resize, trunc);
            freePufexp(pufexp_resize);
        }
        else if(resize && trunc<=0) //solo resize
            pufexp = resize_pex(pufexp_temp, resize_retos, resize_inst, resize_rep, resize_y);
        
        else if(!resize && trunc>0) // solo trunc
            pufexp = truncate_pex(pufexp_temp, trunc);
        
        else // ninguno de los dos
            pufexp = copiar_pex(pufexp_temp);
        
        freePufexp(pufexp_temp);
        
        N_bits=pufexp->tamano_respuesta;
        N_retos=pufexp->N_retos;
        N_inst=pufexp->N_instancias;
        N_rep=pufexp->N_repeticiones;
        N_claves=N_retos*N_inst*N_rep;
        
        data = pex_to_datamtz(pufexp);

    }
    else if(input=='d')
    {
        data = MPFR_leerMatriz(in_name);
    }
    
    if(gsort==0)
        MPFR_ordenarData(data);
    else
        MPFR_GSL_ordenarData(data);
    
    
    // PINTAR RESULTADOS
    if((input==0 || input=='r') && output_r)
    {
        punte = fopen(out_r_name, "w");
        fprintf(punte, "#[N_filas] %d\n#[N_columnas] %d\n\n#[matriz]\n", N_claves, N_cells);
        
        for(int i=0; i<N_retos; i++)
        {
            for(int j=0; j<N_inst; j++)
            {
                for(int k=0; k<N_rep; k++)
                {
                    for(int l=0; l<N_pdl; l++)
                    {
                        for(int m=0; m<N_cells; m++)
                            fprintf(punte, "%lg\t",pufarray[i][j][k][l][m]);
                    }
                    fprintf(punte, "\n");
                }
            }
        }
        fprintf(punte, "#[fin]\n");
        fclose(punte);
    }

    if(output_d)
    {
        punte = fopen(out_d_name, "w");
        fprintf(punte, "#[N_filas] %d\n#[N_columnas] 1\n\n#[matriz]\n", N_claves);
        
        for(int i=0; i<N_claves; i++)
        {
            mpfra_out_str(punte, 10, 0, data->elemento[i][0]);
            fprintf(punte, "\n");
        }
        fprintf(punte, "#[fin]\n");
        
        fclose(punte);
    }
    
    if(output_h)
    {
        if(mpfr_zero_p(caja) && hist_type==0)
        {
            MPFR_FreedmanDiaconis(&caja, data, 0);
            if(!mpfr_zero_p(fcaja))
                mpfra_mul(caja, caja, fcaja);
        }

        if(hist_type==0) 
            histograma = MPFR_construyeHistograma(data, caja, xmin, xmax, hist_norm, 0, verbose);
        else 
            histograma = MPFR_construyeHistograma_Sspace(data, hist_resolucion, hist_norm, 0);
        
        MPFR_pintarDistribucion(histograma, out_h_name);
        
        MPFR_freeDistribucion(histograma);

    }
    
    if(input!='d' && output_x)
    {
        punte=fopen(out_x_name, "w");
        pintarPufexp(pufexp, punte);
        fclose(punte);
    }
    
    if(input!='d' && (intrad || interd || far || frr || roc || (adversario && umbral_id<0)))
    {
        if(intrad)
        {
            intradistanciaStat(pufexp, &media_intrad, &desv_intrad);
            if(verbose)
                printf("\n\n Intra-distancia media +/- desv = %lg +/- %lg = %lg %% +/- %lg %%\n\n", media_intrad, desv_intrad, media_intrad*100.0/pufexp->tamano_respuesta, desv_intrad*100.0/pufexp->tamano_respuesta); 
            else
                printf("%lg", media_intrad);
        }
        if(intrad_h || intrad_d)
        {
            set_intrad = intradistanciaSet(pufexp, 0);
            if(intrad_h)
            {
                intrad_hist = construyeHistograma(set_intrad, caja_intrad, xmin_intrad, xmax_intrad, 1, 1, 0);
                pintarDistribucion(intrad_hist, intrad_hist_name);
                freeDistribucion(intrad_hist);
            }
            if(intrad_d)
            {
                punte = fopen(intrad_dist_name, "w");
                pintarMatriz(set_intrad, punte);
                fclose(punte); 
            }     
            freeMatriz(set_intrad);
        }
        if(interd)
        {
            interdistanciaStat(pufexp, &media_interd, &desv_interd);
            if(verbose)
                printf("\n\n Inter-distancia media +/- desv = %lg +/- %lg = %lg %% +/- %lg %%\n\n", media_interd, desv_interd, media_interd*100.0/pufexp->tamano_respuesta, desv_interd*100.0/pufexp->tamano_respuesta);
            else
                printf("%lg", media_interd);
        }
        if(interd_h || interd_d)
        {
            set_interd = interdistanciaSet(pufexp, 0);
            if(interd_h)
            {
                interd_hist = construyeHistograma(set_interd, caja_interd, xmin_interd, xmax_interd, 1, 1, 0);
                pintarDistribucion(interd_hist, interd_hist_name);
                freeDistribucion(interd_hist);
            }
            if(interd_d)
            {
                punte = fopen(interd_dist_name, "w");
                pintarMatriz(set_interd, punte);
                fclose(punte); 
            }     
            freeMatriz(set_interd);
        }
        
        if(intrad_a || interd_a || far || frr || eer || roc || (adversario && umbral_id<0))
        {
            if(eer || roc || (adversario && umbral_id<0))
            {
                intrad_ajuste = mallocDistribucion(N_bits+1); 
                for(int i=0; i<N_bits+1; i++)
                    intrad_ajuste->ejex[i]=i;
                calculaBinomial(intrad_ajuste, N_bits+1, media_intrad/(N_bits+1));
                
                interd_ajuste = mallocDistribucion(N_bits+1); 
                for(int i=0; i<N_bits+1; i++)
                    interd_ajuste->ejex[i]=i;
                calculaBinomial(interd_ajuste, N_bits+1, media_interd/(N_bits+1));
                
                far_curve = acumularDistribucion(interd_ajuste);
                frr_curve = desacumularDistribucion(intrad_ajuste);
                
                if(intrad_a)
                    pintarDistribucion(intrad_ajuste, intrad_ajuste_name);
                
                if(interd_a)
                    pintarDistribucion(interd_ajuste, interd_ajuste_name);
                
                if(far)
                    pintarDistribucion(far_curve, far_name);
                
                if(frr)
                    pintarDistribucion(frr_curve, frr_name);

                if(roc)
                {
                    roc_curve=mallocDistribucion(N_bits-1);
                    for(int i=0; i<N_bits-1; i++)
                    {
                        roc_curve->ejex[i]=log10(far_curve->ejey[i+1]);
                        roc_curve->ejey[i]=log10(frr_curve->ejey[i+1]);
                    }
                    pintarDistribucion(roc_curve, roc_name);
                    freeDistribucion(roc_curve);
                }
                if(eer || (adversario && umbral_id<0))
                {
                    eer_index=0;
                    eer_value=1; //aqui uso esta cantidad como auxiliar (no realmente como eer).
                    for(int i=0; i<N_bits; i++)
                    {
                        if(far_curve->ejey[i]>frr_curve->ejey[i])
                        {
                            if(far_curve->ejey[i]<eer_value)
                            {
                                eer_value=far_curve->ejey[i];
                                eer_index=i;
                            }
                        }
                        else
                        {
                            if(frr_curve->ejey[i]<eer_value)
                            {
                                eer_value=frr_curve->ejey[i];
                                eer_index=i;
                            }
                        }
                    }
                    if(eer)
                        printf("\n\n EER =\t%lg\n\n index EER = \t%d\n\n", eer_value, eer_index);
                    if(adversario && umbral_id<0)
                        umbral_id = (100.0*eer_index)/N_bits;
                }
                freeDistribucion(intrad_ajuste);
                freeDistribucion(interd_ajuste);
                freeDistribucion(far_curve);
                freeDistribucion(frr_curve);
            }
            else
            {
                if(intrad_a || frr)
                {
                    intrad_ajuste = mallocDistribucion(N_bits);
                    for(int i=0; i<N_bits+1; i++)
                        intrad_ajuste->ejex[i]=i;
                    calculaBinomial(intrad_ajuste, N_bits, media_intrad/N_bits);
                    
                    if(intrad_a)
                        pintarDistribucion(intrad_ajuste, intrad_ajuste_name);
                    
                    if(frr)
                    {
                        frr_curve = desacumularDistribucion(intrad_ajuste);
                        pintarDistribucion(frr_curve, frr_name);
                        freeDistribucion(frr_curve);
                    }
                    
                    freeDistribucion(intrad_ajuste);
                }
                if(interd_a || far)
                {
                    interd_ajuste = mallocDistribucion(N_bits);
                    for(int i=0; i<N_bits+1; i++)
                        interd_ajuste->ejex[i]=i;
                    calculaBinomial(interd_ajuste, N_bits, media_interd/N_bits);
                    
                    if(interd_a)
                        pintarDistribucion(interd_ajuste, interd_ajuste_name);
                    
                    if(far)
                    {
                        far_curve = acumularDistribucion(interd_ajuste);
                        pintarDistribucion(far_curve, far_name);
                        freeDistribucion(far_curve);
                    }
                    
                    freeDistribucion(interd_ajuste);
                }
            }
        }
    }

    if(input!='d' && adversario)
    {
        if(adversario==4)
        {}
        else
        {
            pufexp_ilegit = mallocPufexp(pufexp->N_retos, pufexp->N_instancias, ntrials, pufexp->tamano_reto, pufexp->tamano_respuesta);
            
            if(adversario==1)
            {
                for(int j=0; j<N_inst; j++)
                {
                    for(int i=0; i<N_retos; i++)
                    {
                        for(int k=0; k<ntrials; k++)
                        {
                            for(int l=0; l<pufexp->tamano_reto; l++)
                                pufexp_ilegit->elementox[i][j][k][l] = pufexp->elementox[i][j][0][l];
                            
                            for(int l=0; l<pufexp->tamano_respuesta; l++)
                                pufexp_ilegit->elementoy[i][j][k][l] = dado(2);
                        }
                    }
                }
            }
            else if(adversario==2)
            {
                bitwrd_bias = mallocMatriz((int)(1<<bias_orden), 1);
                
                for(int j=0; j<N_inst; j++)
                {
                    for(int i=0; i<bitwrd_bias->N_filas; i++)
                        bitwrd_bias->elemento[i][0]=0;

                    for(int i=0; i<N_retos; i++)
                    {
                        for(int k=0; k<N_rep; k++)
                        {
                            for(int l=0; l<pufexp->tamano_respuesta; l+=bias_orden)
                            {
                                aux_rand0=0;
                                for(int m=0; m<bias_orden; m++)
                                {
                                    if(pufexp->elementoy[i][j][k][bias_orden*l+m]==1)
                                        aux_rand0+=1<<m;
                                }
                                bitwrd_bias->elemento[aux_rand0][0]++;
                            }
                        }
                    }
                    aux_rand0=0; //'aux_rand0' contendra el maximo valor de bitwrd_bias
                    daux=0; // 'daux' contendra el area de bitwrd_bias
                    for(int i=0; i<bitwrd_bias->N_filas; i++)
                    {
                        if(bitwrd_bias->elemento[i][0]>aux_rand0)
                            aux_rand0 = bitwrd_bias->elemento[i][0];
                        daux+=bitwrd_bias->elemento[i][0];
                    }
                    for(int i=0; i<bitwrd_bias->N_filas; i++)
                        bitwrd_bias->elemento[i][0]/=daux;

                    for(int i=0; i<N_retos; i++)
                    {
                        for(int k=0; k<ntrials; k++)
                        {
                            for(int l=0; l<pufexp->tamano_reto; l++)
                                pufexp_ilegit->elementox[i][j][k][l] = pufexp->elementox[i][j][0][l];
                            
                            for(int l=0; l<pufexp->tamano_respuesta; l+=bias_orden)
                            {
                                //generamos un numero de bias_orden digitos que siga la distribucion bitwrd_bias[][0]:
                                aux_rand1=dado(bitwrd_bias->N_filas);
                                while(ran1()*(aux_rand0/daux)>bitwrd_bias->elemento[aux_rand1][0])
                                    aux_rand1=dado(bitwrd_bias->N_filas);
                                for(int m=0; m<bias_orden; m++)
                                {
                                    //pasar 'aux_rand1' a binario y colocarlo en pufexp_ilegit:
                                    pufexp_ilegit->elementoy[i][j][k][bias_orden*l+m]=aux_rand1%2;
                                    aux_rand1/=2;
                                }
                            }
                        }
                    }
                }
                freeMatriz(bitwrd_bias);
            }
            else if(adversario==3)
            {
                bitwrd_bias = mallocMatriz((int)(1<<bias_orden), pufexp->tamano_respuesta/bias_orden);
                aux_array = malloc(sizeof(int)*(pufexp->tamano_respuesta/bias_orden));
                daux_array = malloc(sizeof(double)*(pufexp->tamano_respuesta/bias_orden));
                
                for(int j=0; j<N_inst; j++)
                {
                    for(int i=0; i<bitwrd_bias->N_filas; i++)
                    {
                        for(int k=0; k<bitwrd_bias->N_columnas; k++)
                            bitwrd_bias->elemento[i][k]=0;
                    }
                    
                    for(int i=0; i<N_retos; i++)
                    {
                        for(int k=0; k<N_rep; k++)
                        {
                            for(int l=0; l<pufexp->tamano_respuesta; l+=bias_orden)
                            {
                                aux=0;
                                for(int m=0; m<bias_orden; m++)
                                {
                                    if(pufexp->elementoy[i][j][k][bias_orden*l+m]==1)
                                        aux+=1<<m;
                                }
                                bitwrd_bias->elemento[aux][l/bias_orden]++;
                            }
                        }
                    }
                    for(int i=0; i<bitwrd_bias->N_columnas; i++)
                    {
                        aux_array[i]=0; //'aux_array[i]' contendra el maximo valor de bitwrd_bias[i]
                        daux_array[i]=0; // 'daux_array[i]' contendra el area de bitwrd_bias[i]
                        for(int k=0; k<bitwrd_bias->N_filas; k++)
                        {
                            if(bitwrd_bias->elemento[k][i]>aux_array[i])
                                aux_array[i] = bitwrd_bias->elemento[k][i];
                            daux_array[i]+=bitwrd_bias->elemento[k][i];
                        }
                        for(int k=0; k<bitwrd_bias->N_filas; k++)
                            bitwrd_bias->elemento[k][i]/=daux_array[i];
                    }
                    
                    for(int i=0; i<N_retos; i++)
                    {
                        for(int k=0; k<ntrials; k++)
                        {
                            for(int l=0; l<pufexp->tamano_reto; l++)
                                pufexp_ilegit->elementox[i][j][k][l] = pufexp->elementox[i][j][0][l];
                            
                            for(int l=0; l<pufexp->tamano_respuesta; l+=bias_orden)
                            {
                                //generamos un numero de bias_orden digitos que siga la distribucion bitwrd_bias[][0]:
                                aux_rand1=dado(bitwrd_bias->N_filas);
                                while(ran1()*(aux_array[l]/daux_array[l])>bitwrd_bias->elemento[aux_rand1][l/bias_orden])
                                    aux_rand1=dado(bitwrd_bias->N_filas);
                                for(int m=0; m<bias_orden; m++)
                                {
                                    //pasar 'aux_rand1' a binario y colocarlo en pufexp_ilegit:
                                    pufexp_ilegit->elementoy[i][j][k][bias_orden*l+m]=aux_rand1%2;
                                    aux_rand1/=2;
                                }
                            }
                        }
                    }
                }
                freeMatriz(bitwrd_bias);
                free(aux_array);
                free(daux_array);
            }
        }
        
        //Ahora ya hemos construido pufexp_ilegit; toca evaluarlo
        printf("\n\n Umbral_id = %lg %%\n\n Tasa de exito del adversario:\n\n", umbral_id);
        for(int j=0; j<N_inst; j++)
        {
            N_exitos=0;
            for(int i=0; i<N_retos; i++)
            {
                for(int k=0; k<N_rep; k++)
                {
                    for(int l=0; l<ntrials; l++)
                    {
                        if(100*distanciaHamming(pufexp->elementoy[i][j][k], pufexp_ilegit->elementoy[i][j][l], pufexp->tamano_respuesta)/pufexp->tamano_respuesta < umbral_id)
                            N_exitos++;
                    }
                }
            }
            printf("\tInstancia %d:\t%lg %%\n", j, (100.0*N_exitos)/(N_retos*N_rep*ntrials));
        }
        printf("\n");
        
        freePufexp(pufexp_ilegit);
    }
    
    if(entrop)
    {
        entropia = MPFR_calculaEntropiaGrassberger(data, 0);
        
        if(entrop==1)
        { 
            if(verbose) printf("\n\n Entropia = %lg\n\n", entropia);
            else printf("%lg\t", entropia);
        }
        
        else if(entrop==2)
        { 
            if(verbose) printf("\n\n Entropia por bit = %lg\n\n", entropia/N_bits);
            else printf("%lg\t", entropia/N_bits);
        }
        
        else if(entrop==3)
        { 
            if(verbose) printf("\n\n Entropia por oscilador = %lg\n\n", entropia/N_cells);
            else printf("%lg\t", entropia/N_cells);
        }
    }
        
    if(minentrop)
    {
        minentropia=MPFR_calculaMinEntropiaGrassberger(data, 0);
            
        if(verbose) printf("\n\n Min-entropia = %lg\n\n", minentropia);
        else printf("%lg", minentropia);  
    }
    
	// LIBERAR MEMORIA:
	if(input==0 || input=='r')
    {
        for(int i=0; i<N_retos; i++)
        {
            for(int j=0; j<N_inst; j++)
            {
                for(int k=0; k<N_rep; k++)
                {
                    for(int l=0; l<N_pdl; l++)
                        free(pufarray[i][j][k][l]);
                }
            }
        }
        for(int i=0; i<N_retos; i++)
        {
            for(int j=0; j<N_inst; j++)
            {
                for(int k=0; k<N_rep; k++)
                    free(pufarray[i][j][k]);
            }
        }
        for(int i=0; i<N_retos; i++)
        {
            for(int j=0; j<N_inst; j++)
                free(pufarray[i][j]);
        }
        for(int i=0; i<N_retos; i++)
            free(pufarray[i]);
        free(pufarray);
        
        if(topol==file_externo)
            freeCmtopol(topologia);
        
        if(input=='r')
            freeMatriz(rawdata);
    }

    if(input!='d')
        freePufexp(pufexp);

    MPFR_freeMatriz(data);

    mpfr_clear(caja);
    mpfr_clear(fcaja);
    mpfr_clear(xmin);
    mpfr_clear(xmax);
    
    mpfr_free_cache();
    
    if(verbose)
        printf("\n\n");

    return 0;
}

