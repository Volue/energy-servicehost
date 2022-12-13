/****** -*- C++ -*- *********************************************************
 *
 *	File name : DBI_ERRNO.h
 *	Author    : Odd Broenner    EFI           DATE: 920717
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : AT&T C++ 2.1
 *
 *	Path         : %P%
 *	$Revision: 193705 $
 *	Last changed : $Date: 2012-01-09 09:53:13 +0000 (Mon, 09 Jan 2012) $
 *
 *	Contents : Enum ERRNO , which can be used from C and C++ code.
 *	
 *	Includes and defines:
 */

/*	Externals and statics:
 */

/*****************************************************************************/
	

#ifndef DBI_ERRNO_H
#define DBI_ERRNO_H

#ifdef NO_ERROR
#undef NO_ERROR		/* NO_ERROR is #define'd in	*/
#endif				/* MSVC 2.0 and 4.0			*/

enum ERRNO
{
	NO_ERROR = 0,									/*    */
	FILE_BAD_FD = 1,								/* Bad file descriptor */
	FILE_BUSY,										/* File busy */
	FILE_CHANGE_SIZE,								/* Can't change file size */
	FILE_CLOSE_ERROR,								/* During file close */
	FILE_CREATE_ERROR,								/* Can't create file */
	FILE_ENV_MISSING,								/* Environment missing */
	FILE_EXIST_DURING_CREATE,						/* File exists during create */
	FILE_LOCK_ERROR,								/* File lock */
	FILE_MISSING_CR,								/* Missing  CR in record */
	FILE_NAME_ILLEGAL,								/* Illegal file name */
	FILE_NEW_FAILED,								/* Frestore allocation failure */
	FILE_NOT_EXCLUSIVE,								/* File not opened in EXCLUSIVE mode */
	FILE_NOT_FOUND,									/* File not found */
	FILE_NOT_FOUND_IN_ANY_ENV,						/* File not found in any environment */
	FILE_NOT_FOUND_IN_ENV,							/* File not found in environment */
	FILE_NOT_OPENED,								/* File not opened */
	FILE_RANGE_ERROR,								/* Range error */
	PD_FILE_READ_ERROR,								/* File read error */
	PD_FILE_READ_ONLY,								/* File opened in READ-only mode */
	FILE_REC_TO_LONG,								/* Record to long */
	FILE_SEEK_ERROR,								/* File seek error */
	FILE_UB_ENV_MISSING,							/* 'UB_ENV' not initialized */
	FILE_UNLINK_ERROR,								/* Can't remove file */
	FILE_WRITE_ERROR,								/* File write error */
	FILE_SEEK_BAD_ORIGIN,							/* Bad origin during file seek */
	ISAM_CANTOPENTEXTFILE = 50,						/* Can't open text file for ISAM */
	ISAM_DUPLICATE_KEY,								/* ISAM:Illegal insert of dup. key attempted */
	ISAM_FILEALREADYOPEN,							/* ISAM file has already been opened */
	ISAM_FILENOTOPEN,								/* ISAM file has not been opened */
	ISAM_INCOMPATIBLE_VERSION,						/* Incompatible ISAM versions */
	ISAM_INSERT_INVALID_POS,						/* ISAM:No valid insert position */
	ISAM_KEYNOTFOUND,								/* Couldn't find ISAM key */
	ISAM_NORECORDBUF,								/* No record buffer for ISAM read */
	ISAM_TEXTRECORDNOTFOUND,						/* Couldn't find ISAM text record */
	ISAM_ZEROKEY,									/* Illegal zero (NULL) ISAM key */
	ISAM_NO_CURRENT_RECORD,							/* ISAM:No current record established */
	ISAM_STRUCTURE,									/* ISAM structure fault */
	AIM_BADDELPARMS = 100,							/* Bad parameters sent to AIM delete */
	AIM_BADREADPARMS,								/* Bad parameters sent to AIM read */
	AIM_BADKEYTYPE,									/* Bad key type in AIM read (use EXACT/LEFT/RIGHT/FLOAT) */
	AIM_CANTOPENTEXT,								/* Can't open text file for AIM */
	AIM_FILENOTOPEN,								/* AIM file is not open */
	AIM_FILEALREADYOPEN,							/* AIM file was already open */
	AIM_INSUFFKEY,									/* Insufficient key for AIM read */
	AIM_NOCURRENTPOS,								/* In AIM: No current position in text file */
	AIM_NORECORDBUFF,								/* No record buffer for AIM read */
	AIM_NORECORD,									/* No record for AIM write */
	AIM_NOSHAREDFILE,								/* No shared text file for this AIM file */
	AIM_TEXTRECORDNOTFOUND,							/* Couldn't find AIM text record */
	AIM_OUTOFMEM,									/* Operator new failed in AIM functions */
	PRT_IO_ERROR = 150,								/* Printer I/O error */
	PRT_BAD_FILE_SPEC,								/* Bad file name specification */
	IO_BAD_FILE_TYPE,								/* Illegal file type */
	IO_BAD_SEQUENTIAL_NUMBER,						/* Bad sequential number in READ/WRITE */
	IO_BAD_TAB_VALUE,								/* Illegal tab value used */
	IO_BAD_VAR_TYPE,								/* Illegal variable type */
	IO_FILE_ALREADY_OPEN,							/* File already open */
	IO_FIX_RECORD_EXPECTED,							/* Fixed record length expected */
	IO_FORMAT_FAILURE,								/* Format failure */
	IO_MEMORY_ALLOCATION_ERROR,						/* Can't allocate memory */
	IO_RECORD_OVERSIZED,							/* Record over-sized */
	IO_RECORD_UNDERSIZED,							/* Record Undersized */
	IO_WRONG_KEYLEN,								/* Wrong key length in ISAM file */
	UB_PIPE_CLOSE_FAIL,								/* Pipe close failure */
	UB_SYSTEM_FAILURE,								/* Error occurred in system() */
#ifndef ID_TIPP
	P_SERVER_UNKNOWN_HOST = 180,					/* Server host name unknown */
	P_SERVER_SOCKET,								/* Server socket could not be created */
	P_SERVER_CONNECT,								/* Could not connect to server */
	P_SERVER_DISCONNECT,							/* Could not disconnect server */
	P_SERVER_WRITE,									/* Could not write to server */
	P_SERVER_READ,									/* Could not read from server */
	P_SERVER_FAILED,								/* Server command not accepted */
	P_SERVER_NO_SERVICE,							/* No macs entry in /etc/services */
#endif
	UTIL_ABORT = 200,								/* Program aborted */
	UTIL_OPTION_CONFLICT,							/* Conflicting options set */
	UTIL_OPTION_INSUFF,								/* Insufficient options set */
	UTIL_INPUT_FILE,								/* Input file must be specified */
	UTIL_OUTPUT_FILE,								/* Output file must be specified */
	UTIL_ILLEGAL_FILESPEC,							/* Illegal file specification */
	UTIL_EXTENSION_MISSING,							/* Extension missing */
	UTIL_ENV_MISSING,								/* Environment must be specified */
	UTIL_ENV_NOT_DEFINED,							/* Environment is not defined */
	UTIL_TOO_MANY_FILES,							/* Too many file specifications */

	DBI_MEMORY_ALLOCATION_ERROR = 250,				/* DBI: Memory allocation error */
	DBI_DB_NOT_CONNECTED,							/* DBI: Database Interface not connected */
	DBI_DB_PARMS_NOT_INITIALIZED,					/* DBI: Uninitialized DBI parameters */
	DBI_NOT_ENOUGH_MEMORY,							/* DBI: Not enough memory */
	DBI_READ_MODE_NOT_DEFINED,						/* DBI: Invalid read mode for DBSelect() */
	DBI_ROW_UPDATE_FAILURE,							/* DBI: Failed to update row */
	DBI_ROW_DELETE_FAILURE,							/* DBI: Failed to delete row */
	DBI_ROW_INSERT_FAILURE,							/* DBI: Failed to insert row */
	DBI_SELECT_WITH_NO_KEY,							/* DBI: Select with no key */
	DBI_CONNECT_FAILED,								/* DBI: Connect failed */
	DBI_ALREADYCONNECTED,							/* DBI: Already connected to RDBMS */
	DBI_BAD_PARMS_FOR_CONNECT,						/* DBI: Illegal parameters, not connected */ 
	DBI_BAD_CURSOR_ID,								/* DBI: Bad cursor id */
	DBI_TOO_MANY_CURSORS,							/* DBI: Attempt to open too many cursors */
	DBI_DELETE_WITH_WHERE_CLAUSE_NOT_IMPLEMENTED,	/* DBI: Delete with where-clause is not implemented */
	DBI_TOO_FEW_ROWS,								/* DBI: Too few rows existed */
	DBI_INCONSISTENT_ROW,							/* DBI: Inconsistent row */
	DBI_BYTE_STREAM_ERROR,							/* DBI: Error in the storage of a byte stream in the database. */
	DBI_PROGRAM_ERROR,								/* DBI: Error in the DBI code detected */
	DBI_TIME_SERIES_TYPE_ERROR,						/* DBI: Time series type or interval type not as expected */
	DBI_NEXTROW_FAILED,								/* DBI: Error detected in DBInterfaceBasic::NextRow */
	DBI_EXECCMD_FAILED,								/* DBI: Error detected in DBInterfaceBasic::ExecCmd */
	DBI_CLEAR_FAILED,								/* DBI: Error detected in DBInterfaceBasic::Clear */
	DBI_OWNER_ROW_NOT_FOUND,						/* DBI: Inconsistent database. A foreign key did not refer to owner */
	DBI_DBOPEN_FAILED,								/* DBI: Attempt to open dbprocess failed */
	DBI_ILLEGAL_OWERWRITE,							/* DBI: Attempt to append to/overwrite data with an OW_NO-flag or overwrite data with an OW_APPEND-flag */
	DBI_REFERENCED_ROW_DOES_NOT_EXIST,				/* DBI: Referenced row does not exist */
	DBI_REFERENCES_TO_ROW_EXIST,					/* DBI: References to row exists */
	DBI_TOO_MANY_ROWS,								/* DBI: More rows than expected were found */
	DBI_OWNER_VIOLATION,							/* DBI: Row owner violation */
	DBI_INTERNAL_LIMIT_EXCEEDED,					/* DBI: Internal limit exceeded */
	DBI_TOO_MANY_SELECT_ITEMS,						/* DBI: Too many items selected */
	DBI_INVALID_DATATYPE,							/* DBI: Invalid data type from RDBMS */
	DBI_INVALID_DATABASE_TYPE,						/* DBI: Invalid database type */
	DBI_BAD_TIME_SERIES_TYPE,						/* DBI: Bad time series type or steptype */
	DBI_JOIN_INSUFFICIENT,							/* DBI: Insufficient join conditions */
	DBI_ATTEMPT_TO_INSERT_EXISTING_ROW,				/* DBI: Attempt to INSERT an existing row */
	DBI_ATTEMPT_TO_UPDATE_NONEXISTING_ROW,			/* DBI: Attempt to UPDATE a non-existing row */
	DBI_CANT_MODIFY_A_VIEW,							/* DBI: Only DBSelect allowed on views */
	DBI_UNKNOWN_FIELD_NAME,							/* DBI: Unknown field name in select distinct */
	DBI_NO_SUCH_TIME_SERIES,						/* DBI: Unknown time series. */
	DBI_ILLEGAL_OPERATION_FOR_VIRTUAL_TIMES_SERIES,	/* DBI: Virtual series value cannot be read/written/deleted */

	DBI_NO_ROWS_FOUND = 10250,						/* DBI: No rows found */

	SQL_NOMORECONNECTS = 300,						/* No more SQL connections available */
	SQL_ALREADYCONNECTED,							/* SQL: Already connected to server */
	SQL_DEFAULTALREADYCONNECTED,					/* SQL: Default connection already in use */
	SQL_NOTCONNECTED,								/* SQL: Not connected to server */
	SQL_BADDBIDFORCONNECT,							/* SQL connect (internal): Bad database ID */
	SQL_BADDBIDFORDISCONNECT,						/* SQL disconnect (internal): Bad database ID */
	SQL_BADDBIDFORCOMMIT,							/* SQL commit (internal): Bad database ID */
	SQL_BADDBIDFORROLLBACK,							/* SQL rollback (internal): Bad database ID */
	SQL_BADDBIDFORSQLEXEC,							/* SQLexec (internal): Bad database ID */
	SQL_TOOMANYBINDVARS,							/* SQL: Too many variables in bind */
	SQL_TOOMANYSELECTITEMS,							/* SQL: Too many items in select list */
	SQL_NORECORDBUFFER,								/* SQL: No record buffer pointer supplied */
	SQL_ILLEGALDATATYPE,							/* SQL: Only CHAR/NUMBER/DATE/ROWID allowed */
	SQL_BADCURSORIDFORSQLEXEC,						/* SQLexec (internal): Bad cursor ID */
	SQL_BADCURSORIDFORFETCH,						/* SQL Fetch (internal): Bad cursor ID */
	SQL_BADCURSORIDFORCLOSE,						/* SQL Close (internal): Bad cursor ID */
	SQL_ERRORDESCINCA,								/* SQL: Use SQLCA to get error information */
	SQL_INVALIDDBSTRING,							/* SQL: Invalid database string for connect */
	SQL_MACSUNABLE,									/* SQL: Unable to connect via MACS */

	ZZ_ERROR										/* Last message */
};

#ifndef __cplusplus
typedef enum ERRNO ERRNO;
#endif /* __cplusplus */

#endif  /* DBI_ERRNO_H */
