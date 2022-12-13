/*
	typedef for dbi error function pointer
*/
	
#ifndef _DBI_INTERFACE_ERR_H
#define _DBI_INTERFACE_ERR_H
#include "DBI_ERRNO.h"
typedef void (*DBI_error_function)(const char*, enum ERRNO);
#endif /* _DBI_INTERFACE_ERR_H */
