/*
	mpfra --> mpfr alias

	Estas definiciones permiten utilizar la biblioteca "mpfr" sin preocuparse por la direccion del redondeo, tomando MPFR_RNDN
*/


//Initialization Functions
	#define mpfra_set_default_prec(x) mpfr_set_default_prec(x)

//Assignment Functions
	#define mpfra_set(x, y) mpfr_set(x, y, MPFR_RNDN)
	#define mpfra_set_d(x, y) mpfr_set_d(x, y, MPFR_RNDN)
    #define mpfra_set_ui(x, y) mpfr_set_ui(x, y, MPFR_RNDN)
	#define mpfra_set_str(x, y, z) mpfr_set_str(x, y, z, MPFR_RNDN)

//Combined Initialization and Assignment Functions
    #define mpfra_init_set(x, y) mpfr_init_set(x, y, MPFR_RNDN)
	#define mpfra_init_set_ui(x, y) mpfr_init_set_ui(x, y, MPFR_RNDN)
	#define mpfra_init_set_d(x, y) mpfr_init_set_d(x, y, MPFR_RNDN)

//Conversion Functions
    #define mpfra_get_ui(x) mpfr_get_ui(x, MPFR_RNDN)
	#define mpfra_get_d(x) mpfr_get_d(x, MPFR_RNDN)
    #define mpfra_get_str(x, y, z, t, u) mpfr_get_str(x, y, z, t, u, MPFR_RNDN);

//Arithmetic Functions
    #define mpfra_sub_ui(x, y, z) mpfr_sub_ui(x, y, z, MPFR_RNDN)
	#define mpfra_ui_sub(x, y, z) mpfr_ui_sub(x, y, z, MPFR_RNDN)
	#define mpfra_sub(x, y, z)  mpfr_sub(x, y, z, MPFR_RNDN)
	#define mpfra_add(x, y, z) mpfr_add(x, y, z, MPFR_RNDN)
	#define mpfra_add_ui(x, y, z) mpfr_add_ui(x, y, z, MPFR_RNDN)
	#define mpfra_mul(x, y, z) mpfr_mul(x, y, z, MPFR_RNDN)
	#define mpfra_mul_ui(x, y, z) mpfr_mul_ui(x, y, z, MPFR_RNDN)
	#define mpfra_mul_d(x, y, z) mpfr_mul_d(x, y, z, MPFR_RNDN)
	#define mpfra_mul_2ui(x, y, z) mpfr_mul_2ui(x, y, z, MPFR_RNDN)
	#define mpfra_div(x, y, z) mpfr_div(x, y, z, MPFR_RNDN)
	#define mpfra_ui_div(x, y, z) mpfr_ui_div(x, y, z, MPFR_RNDN)
	#define mpfra_div_ui(x, y, z) mpfr_div_ui(x, y, z, MPFR_RNDN)
    #define mpfra_div_d(x, y, z) mpfr_div_d(x, y, z, MPFR_RNDN)
    #define mpfra_d_div(x, y, z) mpfr_d_div(x, y, z, MPFR_RNDN)
	#define mpfra_abs(x, y) mpfr_abs(x, y, MPFR_RNDN)

//Comparison Functions
	#define mpfra_cmp(x, y) mpfr_cmp(x, y)
	#define mpfra_cmp_d(x, y) mpfr_cmp_d(x, y)

//Trascendental Functions
    #define mpfra_exp2(x, y) mpfr_exp2(x, y, MPFR_RNDN)
    #define mpfra_log(x, y) mpfr_log(x, y, MPFR_RNDN)
	#define mpfra_log2(x, y) mpfr_log2(x, y, MPFR_RNDN)
    #define mpfra_sqrt(x, y) mpfr_sqrt(x, y, MPFR_RNDN)
    #define mpfra_cbrt(x, y) mpfr_cbrt(x, y, MPFR_RNDN)
    #define mpfra_digamma(x, y) mpfr_digamma(x, y, MPFR_RNDN)

//Input and Output Functions
	#define mpfra_out_str(x, y, z, t) mpfr_out_str(x, y, z, t, MPFR_RNDN)
    #define mpfra_inp_str(x, y, z) mpfr_inp_str(x, y, z, MPFR_RNDN)
	
//Integer and Remainder Functions
    #define mpfra_rint_floor(x, y) mpfr_rint_floor(x, y, MPFR_RNDN)


