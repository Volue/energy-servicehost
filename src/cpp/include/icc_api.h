/**** File_Header *************************************************************
 *
 *      $Revision: 37171 $  $Date: 2004-09-29 17:43:15 +0000 (Wed, 29 Sep 2004) $
 *
 *	$RCSfile$
 *	Author    : Roland Kruse    EFI-20     DATE: 921029
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : gcc
 *
 *      Keywords  : ADM API definition
 *	Contents  : 
 *
 *	Includes and defines:
 */
#ifndef _ICC_API_H_
#define _ICC_API_H_

#include "genlib.h"
#include "default_log.h"
#include "icc.h"
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif

/* 'flags' argument definitions for icc_init() */
#define I_CONTINUE	 1	/* Don't exit on STOP message */
#define	I_POLL		(1<<1)	/* Disable interrupt by signal */
#define I_NOBLOCK	(1<<2)	/* Don't block in icc_get_msg() */
#define I_ISUIS		(1<<3)	/* This process is UIS process */
#define I_ISADM		(1<<4)	/* This process is ADM process */

/* 'which_msg' argument definitions for icc_dispose_msg() */
#define ICC_MSG_INPUT	1
#define ICC_MSG_OUTPUT	2

/* 'msg_dest' argument for icc_send_msg() */
#define ICC_MSG_DEFAULT			-1	/* Default dest.	     */
#define ICC_MSG_BROADCAST		-2	/* Broadcast to all app. UIS */
#define ICC_MSG_RETURN_TO_SENDER 	-3	/* Return to sender	     */
#define ICC_MSG_UIS			-4	/* Send to common UIS	     */
#define ICC_MSG_ADM			-5	/* Send to ADM process	     */

/* 'msg_info_code' argument for icc_query_msg() */
#define ICC_MSG_SENDER		1
#define ICC_MSG_ORG_SENDER	2
#define ICC_MSG_ARR_TIME	3
#define ICC_MSG_DEP_TIME	4
#define ICC_MSG_SIZE		5

/*
 *	Structure used in action table for icc_msg_handler()
 */
typedef struct icc_action
{
    int	type;				/* Message type */
#if (defined(__STDC__) || defined(__cplusplus))    
    void (*handler)(int msg_type, void *msg_data, void *user_data);
					/* Handler function */
#else    
    void (*handler)();
#endif    
} ICC_Action;

/*
 *	Structure used to store info from icc_query_msg()
 */
typedef union icc_msginfo
{
    int	ival;
    struct
    {
	long	seconds;
	long	microseconds;
    } timeval;
} ICC_Msginfo;


/*	Externals and statics:
 */


/*	Function prototypes
 */
#if (defined(__STDC__) || defined(__cplusplus))
/* ANSI C function prototypes */
extern int	icc_init(int argc, char **argv, int id, void (*stopf)(int),
		    void (*swsf)(char*), void (*rwsf)(char*), unsigned flags);
extern int	icc_msg_handler(void (*cmd_handler)(char*, void*),
	            struct icc_action *actions);
extern void    *icc_cmd_user_data(void*);
extern void    *icc_msg_user_data(void*);	    
extern int	icc_poll(void);	    
extern int      icc_query_msg(int msg_info_code, ICC_Msginfo *info);
extern void	icc_dispose_msg(int which_msg);
extern void    *icc_get_msg(int *msg_type);
extern int	icc_send_msg(int msg_type, int msg_dest);
extern void    *icc_create_msg(int msg_type, char *msg_name,
	            int msg_dyn_size);
extern int 	icc_get_prot( long *msg_tag, int *msg_tag_type,
		    long *msg_client_data );
extern void 	icc_set_prot(long msg_tag, int msg_tag_type,
		    long msg_client_data);
extern void	icc_exit(int status);
extern void	icc_close(void);
extern int	icc_msg_timeout(long seconds, long microseconds);
extern int 	icc_send_log(int mess_ref, char *data,
		    int sev_lev, int out);

extern int 	icc_send_flog(int sev_lev, int out, char *format, ... ); 

extern int	icc_getparam(char **name, int *inst, char **domain, int *gpid);
extern int	icc_q_uis(void);
extern int	icc_q_adm(void);
extern int	icc_q_category(void);
extern int 	icc_set_admcb(void (*admcallback)(int, void*, void*),
		    void *client_data);
extern int 	icc_clear_admcb(void (*admcallback)(int, void*, void*),
		    void *client_data);
extern int	icc_adm_client_cmd(char *s);
extern int	icc_q_def_dest(void);
/* The following function is only to be used in ICC-ADM */
extern int 	icc_adm_terminate(char*, int);


#else
/* K&R C function declarations */
extern int 	icc_init();
extern int 	icc_msg_handler();
extern void    *icc_cmd_user_data(void*);	    
extern void    *icc_msg_user_data(void*);
extern int 	icc_poll();
extern int      icc_query_msg();
extern void	icc_dispose_msg();
extern void    *icc_get_msg();
extern int	icc_send_msg();
extern void    *icc_create_msg();
extern int 	icc_get_prot();
extern void 	icc_set_prot();
extern void	icc_exit();
extern void	icc_close();
extern int	icc_msg_timeout();
extern int	icc_send_log();
extern int	icc_getparam();
extern int	icc_q_uis();
extern int	icc_q_adm();
extern int	icc_q_category();
extern int 	icc_set_admcb();
extern int 	icc_clear_admcb();
extern int	icc_adm_client_cmd();
extern int 	icc_adm_terminate();
extern int	icc_q_def_dest();

#endif

/*****************************************************************************/
	


#ifdef __cplusplus
};
#endif
#endif /* _ICC_API_H_ */
