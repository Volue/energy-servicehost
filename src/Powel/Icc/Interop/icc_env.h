/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

#ifndef _ICC_ENV_H_
#define _ICC_ENV_H_
#ifdef __cplusplus
extern "C" {
#endif

/*    Externals and statics:
 */

/*    Function prototypes
 */

/* Read environmnent from registry key */
int read_env_from_reg(char *key, 
                      void (*handler)(char *key, char *name, char *value,
                            int type),char *usrname);

/* Set up environment from registry key & return fully expanded environment */
char **icc_env_setup(char *key, int *count);

/* Translate a path variable from DOS to UNIX format.
char *icc_env_dos2unix(char *in, char *out, char *mode);


/*****************************************************************************/
#ifdef __cplusplus
};
#endif
#endif /* _ICC_ENV_H_ */




