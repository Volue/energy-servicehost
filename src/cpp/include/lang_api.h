/**** File_Header *************************************************************
 *
 *      $Revision: 39190 $  $Date: 2005-01-19 13:47:47 +0000 (Wed, 19 Jan 2005) $
 *
 *	$RCSfile$
 *	Author    : Roland Kruse    EFI-20     DATE: 930524
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : gcc
 *
 *      Keywords  : Translation routines
 *	Contents  : 
 *
 *	Includes and defines:
 */
#ifndef _LANG_API_H_
#define _LANG_API_H_

/*	Externals and statics:
 */

/*	Function prototypes
 */
#if defined(__cplusplus)
extern "C" {
#endif
/* Modes */
#define LANG_UPDATE	1			/* Allow file updates	     */
#define NHLISTS 64				/* Number of hashed lists    */

/*
 *	String translation entry structure.
 */
struct stringtrans
{
    struct stringtrans	*s_link;		/* Ptr to next entry	     */
    unsigned		s_tag;			/* String tag		     */
    char		*s_orig;		/* Original (input) std::string   */
    char		*s_trans;		/* Translated (output) std::string*/
};

/*
 *	Translation table main structure
 */
struct transtab
{
    char 		*t_orgfname;		/* Original file name	     */
    char		*t_transfname;		/* Translation file name     */
    unsigned		t_mode;			/* mode			     */
    FILE		*t_orgf;		/* Original file pointer     */
    FILE		*t_transf;		/* Translation file pointer  */
    FILE		*t_logf;		/* Log file		     */
    int			t_log_blocked;		/* Log blocking indicator    */
    struct stringtrans 	*t_hlists[NHLISTS];	/* hashed  translation lists */
    unsigned		t_maxtag;		/* Highest tag used	     */
    struct stringtrans  **t_index;		/* Tag indexed translations  */
};    
typedef struct transtab *lang_id;
/* Open translation */
extern lang_id lang_open(char *name, char *language, unsigned mode);
/* Translate std::string->string */
extern const char *lang_translate(lang_id, const char *);
/* Reverse translate std::string->string */
extern char *lang_rtrans(lang_id, char *);
/* Tag-based std::string fetch    */
extern char *lang_get_by_tag(lang_id, unsigned);
/* Close translation */
extern int lang_close(lang_id);
/* Translate & format a std::string */
extern char *lang_ftrans(lang_id id, const char *format, ...);
/* Get tag number for std::string */
extern int lang_get_tag( lang_id, char *);
/* Get highest tag number used */
extern int lang_highest_tag( lang_id );
#if defined(__cplusplus)
}
#endif


/*****************************************************************************/
	
#endif
