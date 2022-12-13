/**** File_Header *************************************************************
 *
 *	Author		:	Roland Kruse    EFI-20     DATE: 930212
 *	Copyright	:	Norwegian Electric Power Research Institute
 *	Compiler	:	gcc
 *
 *	Keywords	:	General-purpose library functions, independent of the
 *					ICC Basic Concept.
 *	Contents	:	
 *
 *	Includes and defines:
 */
#ifndef _GENLIB_H_
#define _GENLIB_H_

#include <stdio.h>
#include "icc.h"

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef WIN32
/*  Due to extreme timeconsume on NT
    copied and modified from C runtime  library:*/
#define PD_D0 3
#define PD_DOFF 4
#define PD_DFRAC  ((1<< PD_DOFF) -1)
#define PD_DMASK  (0x7fff & ~PD_DFRAC)
#define PD_DMAX	((1 << (15 - PD_DOFF)) - 1)
/*
    isnan(double *x) {
	unsigned short *ps = (unsigned short *)px;

	if ((ps[_D0] & _DMASK) == _DMAX << _DOFF)
		return ((ps[_D0] & _DFRAC) != 0 || ps[_D1] != 0
			|| ps[_D2] != 0 || ps[_D3] != 0 ? NAN : INF);
    }
*/

// SIV. Defining icc_isdouble as macro is not type safe. 
// It accepts any type of argument and if it is not double then result is unpredictable.
// Inline method is type safe, but in C context does not compile (I do not know why)
#if defined(__cplusplus)
inline bool icc_isdouble(double x) {	return (((unsigned short*)&((x)))[PD_D0] & PD_DMASK) != (PD_DMAX << PD_DOFF); }
#else
#define icc_isdouble(x) (!((((unsigned short*)&((x)))[PD_D0] & PD_DMASK) == PD_DMAX << PD_DOFF))
#endif

extern char dNaN[8];
extern char fNaN[4];
#define icc_nodouble() (*((double*) (&dNaN[0])))
#define icc_nofloat()  (*((float*) (&fNaN[0])))
#define icc_isfloat(x) icc_isdouble(x)
extern float	_icc_nofloat(void);	/*old funcs for compat */
extern double	_icc_nodouble(void);
extern int	_icc_isfloat(double x);
#define _icc_isdouble(x) _icc_isfloat(x)

#elif defined(sun) || defined(hpux) || defined(__alpha)
#include <nan.h>
extern int	_icc_isfloat(double x);
#define _icc_isdouble(x) _icc_isfloat(x)

#ifdef __alpha
#define icc_isdouble(x) (!(IsNANorINF(x) || (x) == -FLT_MAX ))
#else
#define icc_isdouble(x) (!IsNANorINF(x))
#endif
#define icc_isfloat(x) icc_isdouble(x)
extern char dNaN[8];
extern char fNaN[4];
extern float	_icc_nofloat(void);	
extern double	_icc_nodouble(void);

#define icc_nodouble() (*((double*) (&dNaN[0])))
#define icc_nofloat()  (*((float*) (&fNaN[0])))
#else
#define icc_isdouble(x) icc_isfloat(x)
extern float	icc_nofloat(void);	
extern double	icc_nodouble(void);
extern int	icc_isfloat(double x);
#endif

/* === rounding =============================================================== */
#define MAX_SIGNIFICANT_DOUBLE_DIGITS 15
extern const double genlibRoundingFactorPrecomputed[];

/*=== inline_double_rounding ==================================================
 * Assigns 'res' the rounded value of 'value' to the 'num_decimals' decimal.
 * Use this macro instead of double_rounding() when maximum performance is wanted.
 * Rounds away from zero keeping the specified number of decimals. 
 * If 'num_decimals' is below zero or equals or greater or equal to MAX_SIGNIFICANT_DOUBLE_DIGITS
 * 'value' is used.
 */
#define inline_double_rounding(res, value, num_decimals) {\
    if( ((num_decimals) >= 0) && ((num_decimals) < MAX_SIGNIFICANT_DOUBLE_DIGITS)) {\
		register double X__fact = genlibRoundingFactorPrecomputed[num_decimals];\
		register double X__d = (value)*X__fact;\
		res =((X__d)>=0.0 ? floor((X__d)+0.5) : -floor(0.5-(X__d))) / X__fact;\
    } else {\
		res=value;\
    }\
}

/*=== inline_nonstandard_double_rounding ==================================================
* This is the old EFI/Powel definition of rounding which is mathematically dubious. It is used
* indirectly by some export algorithms that dislike negative values.
*           -2.5 is rounded to -2
*			2.5  is rounded to 3
*
* Assigns 'res' the rounded value of 'value' to the 'num_decimals' decimal.
* Use this macro instead of double_rounding() when maximum performance is wanted.
* Rounds away from zero keeping the specified number of decimals. 
* If 'num_decimals' is below zero or equals or greater or equal to MAX_SIGNIFICANT_DOUBLE_DIGITS
* 'value' is used.
*/
#define inline_nonstandard_double_rounding(res, value, num_decimals) {\
	if( ((num_decimals) >= 0) && ((num_decimals) < MAX_SIGNIFICANT_DOUBLE_DIGITS)) {\
		register double X__fact = genlibRoundingFactorPrecomputed[num_decimals];\
		register double X__d = (value)*X__fact;\
		res =((X__d)>=0.0 ? floor((X__d)+0.5) : -floor(0.49999999-(X__d))) / X__fact;\
	} else {\
		res=value;\
	}\
}



/*
 *      This tremendous kludge is needed on HP/UX to correct an even more
 *      perverse kludge made by hp: #define index strchr   ....#@z%!
 */
#ifdef index
#undef index
#endif

/*	Externals and statics:
 */

/*	Function prototypes
 */
 

#ifdef WIN32
extern int getopt(int argc, char *argv[], char *opts);
extern int optind,opterr,optopt;
extern char *optarg;
#endif

int		icc_utility_header(const char *program,
				   const char *version,
				   const char *date);
extern int	PDSystem(const char * cmd);
extern int	PDWSystem(const char* cmd);
extern int	PDW_System(const char * cmd, int runInvisible, int runCmdShell) ;
extern char * const PDW_GetStatusMessage();
extern void	PDsleep( ICC_TIME_UNITS unit, signed long nounits );
extern char    *PDTmpName(void);
extern char    *PDgetcwd(void);
extern FILE    *fpopen(char *filename, char *type, char *env_var);
extern char    *findfile(char *name, char *path);
extern char    *get_username(void);
extern char    *get_full_username(void);
extern char    *get_sa_username(void);
extern char    *get_hostname(void);
extern const char *basename(const char *pathname);
extern char    *sim_basic_dataset(void);
extern double	double_rounding( double value, int num_decimals);
extern int	TSValueRoundAndTruncate( int intPart, int fractPart, double *values, int numVals );
extern void	icc_reg_cleanup(void (*func)(void));
extern void	icc_cleanup(void);
extern char *bytes2hex( void *p, int n);
extern void	swapbyteorder( void *buffer, int bytesPerValue, long noBytesTotal);
extern int
icc_lock(unsigned wait, char *myname,
    int (*cb)(unsigned remain, char *date, char *user, char *prog, int pid));
extern int icc_unlock(void);
extern int icc_lockstat(char **date, char **user, char **progname, int *pid);
extern FILE *PDpopen( const char *command, const char *type);
extern int PDpclose( FILE *p);
extern int isEanNumber(const char *str);
extern int isLongInEnvVar(long num, const char *env, const char *sep);
extern int isStringInEnvVar(const char *str, const char *env, const char *sep);
extern const char* PDgetenv(const char* Env,const char* Def);
extern int InCheckReportsSemantics();

#if defined(WIN32)
#define nint(x) ((x)>0.0 ? floor((x)+0.5) : -floor(0.5-(x)))
#define aint(x) ((x)>0 ? floor(x) : -floor(-(x)))
#else
#include "math_machdep.h"
#endif
#if defined(__cplusplus)
};
#endif

#endif /*_GENLIB_H_*/

/*****************************************************************************/
	
