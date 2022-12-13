/*** File_header *** -*- C++ -*- ********************************************
*****************    R T R  -  C L A S S   L I B R A R Y    *****************
*****		     Copyright Scanvest Olivetti - Norway		*****
*****************************************************************************
*
* File name....: %M%
* Version......: %I%
* Last modified: $Date: 2016-09-26 07:39:58 +0000 (Mon, 26 Sep 2016) $
* Path name....: %P%
* Created by...: Jan Alvestad
* Created date.: 91/03/26
*
* DESCRIPTION:
*	Class DBInterfaceBasic definition.
*
* MODIFICATION HISTORY:
*	Version Date		Sign	Description
*	1.1	91/03/26	JA	New file.
*		91/10/08	JA	Added date conversion functions for
*					SELECT, UPDATE and INSERT.
*	2.0	91/10/29	JA	Redesign to enable use of Q+E
*					Database Lib.
*
*	2.1	92/05/12	OO	Changed most user function return
*					values from
*					INT to DB_RETURN, added some new
*					functions...
*					Removed unused dbBind function.
*					Made Open() function private and called
*					it in the Login() function.
*		92/07/10	JR	File moved to DBIntefacebasic.h.
*					Class name changed to
*					DBInterfaceBasicBasic to allow for an
*					extended DBInteface class that inherit
*					the old DBInterfaceBasic class.
*		92/08/15	JR	TestCursor function added as protected
*					function. ( Juast a renaming of the
*					modified SetCursor function.
*		92/09/02	JR	UpdateColumn function added to
*					store large byte values in DB.	
*		92/09/03	JR	MONEY GetString moved to
*					DBSybaseBasic.C to make it non-inline
*					to satisfy SolBorne C++.
*		92/09/07	JR	TestDBError function added.
*		92/10/23	JR	TransactionNesting function added.
*		92/11/27	JR	Clear function rewritten using dbcancel.
*					Clear changed from void Clear() to
*					DB_RETURN Clear()
*		93/07/09	OB	Added function GetSAUserName and
*					GetUserName
*		93/07/09	OB	Added function GetErrnoText
*		96/03/26	TEJ	Added function SecureServerLogin,
*					SecureLogin and EncryptPassword.
*					Included enum DBSecureMode and 	
*		96/09/30	OB	Added class SqlFunc as friend class
*
****************************************************************************/ 

#ifndef DBINTERF_H
#define DBINTERF_H

#include "OSTypes.h"
#include "OSString.h"
#include "OSDateTime.h"
#include "ErrMessage.h"
#include "ByteStream.h"

#include "DBInterfaceErr.h"

 
#include <stdlib.h>
#include <stdarg.h>
#include "lang_api.h"


//	 D e f i n i t i o n s

#define DBI_MAX_CURSORS 15


//  E n u m e r a t i o n s

#include "DBI_enums.h"
#include "icc_db_programs.h"

enum DB_TYPE
{
	DB_dBASE,
	DB_Excel,
	DB_DB2,
	DB_Netware_SQL,
	DB_Sybase,
	DB_Oracle,
	DB_Paradox,
	DB_SQL_Server,
	DB_Text_Files
};


enum STORAGE_MODE
{
	INSERT_THEN_UPDATE,
	UPDATE_THEN_INSERT
};
    
struct DBMS_API DBKeyWords
{
	pCHAR	And;
	pCHAR	OrderBy;
	pCHAR	Where;
	pCHAR	Lock;
};
extern DBMS_API DBKeyWords k;//[sih] incredible short name for a global ! who invented this !!

class DBMS_API MONEY
{
public:
	MONEY() { value1 = value2 = 0L; }
	MONEY(const MONEY *m) { value1 = m->value1 ; value2 = m->value2; }
	pCHAR GetString(); // Used to be defined inline. Moved to DBSybase
			   // because some C++ compiler did not like
			   // static in inline functions.
	long value1;
	unsigned long value2;
};

struct HostArray;
class DBMS_API DBMS;
class DBMS_API SqlCache;
class PowelOracleInterface;
class DBMS_API DBInterfaceBasic
{
	friend DBMS_API DB_RETURN DBI_func(OSDataType **(&), DB_READ_MODE, int, int, char *, va_list);
	friend class DBMS_API SqlFunc;
protected:
	PowelOracleInterface& db;
public:
	DBInterfaceBasic();
	~DBInterfaceBasic();
	PowelOracleInterface& getDb();
	// Login and set functions used with SQL Server (not Q+E implementation).
	DB_RETURN Login (pCHAR password = (pCHAR)NULL,
					 pCHAR user = (pCHAR)NULL, 
					 pCHAR host = (pCHAR)NULL,
					 pCHAR appl = (pCHAR)NULL,
					 pCHAR clientUser = (pCHAR)NULL);
	int		SetPwd(pCHAR password = (pCHAR)NULL);
	int		SetUser(pCHAR user = (pCHAR)NULL);
	int		SetClientUser(pCHAR clientUser = (pCHAR)NULL);
	int		SetHost(pCHAR host = (pCHAR)NULL);
	int		SetAppl(pCHAR appl = (pCHAR)NULL);
	char*	GetUser(void) { return user;}
	char*   GetPassword(void) { return password;}
	char*   GetServer(void) {return server;}
	// Functions for security functions
	void	SetGrupKey( const int _grupKey) { grupKey = _grupKey; } 
	int		GetGrupKey( void ) { return grupKey; }

	DB_RETURN	ServerLogin (pCHAR pwd=NULL, pCHAR usr=NULL,
					pCHAR host=NULL, pCHAR appl=NULL,
					pCHAR srv=NULL, pCHAR clientUser = NULL);
	// Login aux routines..
	DB_RETURN	SetDBServer( char * sqlServer );
	char		*GetDBServer( ) { return sqlServer; }

	char 		*LoginInfo( void );
	pCHAR		GetPath (void)	{ return filePath; }

	DB_RETURN 	Use(pCHAR dbName, int cursorId);
	//DB_RETURN	Cmd		(pCHAR sqlStr, ...);
	DB_RETURN	Cmd		(int cursorId, pCHAR sqlStr, ...);
	DB_RETURN	Cmd		(int cursorId, OSDataType  *pHost, pCHAR sqlStr, ...);
	DB_RETURN	Cmd		(int cursorId, OSDataType **pHost, pCHAR sqlStr, ...);

	void		AddHost			(int cursorId, OSDataType  *pHost );
	void		AddHost			(int cursorId, OSDataType **pHost );
	void		MakeHostName	(int cursorId, OSDataType  *pHost );
	void		MakeHostName	(int cursorId, OSDataType **pHost );

	DB_RETURN	ExecCmd (int cursorId);
	DB_RETURN	ExecACmd (int cursorId, HostArray *pHost ); 
	
	UCHAR		*Bind	(int column, UCHAR * p , int cursorId);
	pCHAR		Bind	(int column, pCHAR, int cursorId);
	CHAR		Bind	(int column, CHAR, int cursorId);
	short int	Bind	(int column, short int, int cursorId);
	int			Bind	(int column, int, int cursorId);
	long		Bind	(int column, long, int cursorId);
	double		Bind	(int column, double, int cursorId);
	pCHAR		BindDate(int column, int cursorId);
	pSInt		Bind	(int column, pSInt, int cursorId);
	pLong		Bind	(int column, pLong, int cursorId);
	pDouble 	Bind	(int column, pDouble, int cursorId);
	pString 	Bind	(int column, pString, int cursorId);
//	ByteStrRef 	Bind	(int column, pByteStream, int cursorId );
	MONEY		*Bind	(int column, MONEY *src, int cursorId);// used with SYBASE
	OSDateTime 	*Bind	(int column, OSDateTime *src, int cursorId);// used with SYBASE

	void	Convert (pCHAR src, MONEY *money);	// used in connection with SYBASE
	double	Convert (MONEY *money, double d);	// used in connection with SYBASE

	// CurRow and CurCmd should not be used if Q+E Lib is installed
	long		CurRow	(int cursorId);
	int			CurCmd	(int cursorId);

	DB_RETURN	NextRow (int cursorId);
	int			EnableFetch(void);
	int			Count	(void);
	int			RowsAffected	(int cursorId); // Number of rows
									// affected by last
									// command.
	DB_RETURN	Results (int cursorId);
	DB_RETURN	Clear	(int cursorId ); 	// clear any unprocessed
								// data

	STORAGE_MODE GetStorageMode(void) const { return storageMode; }
	STORAGE_MODE SetStorageMode( STORAGE_MODE mode)
	{	STORAGE_MODE oldMode = storageMode;
		storageMode = mode;
		return oldMode;
	}

	// DBLock and DBUnlock are only necessary with MS Windows 3.0
	// SQL Server implementation (not Q+E implementation).
	void	DBLock	(void);
	void	DBUnlock (void);

	// Set up statement (NOT including SELECT) to select a date and
	// convert it to a char pointer. Example: "convert(char(12), attr, 3)"
	// will be a part of a select statement converting the DATETIME
	// attibute to a string using the English/French date format (example
	// is based upon SYBASE Transact-SQL).
	pCHAR	DateSelectFromDB(pCHAR attr, pCHAR destination);

	// Use string from DateConvertToDB as part of an INSERT, DELETE or UPDATE
	// to provide the database with its required date input format.
	pCHAR	DateConvertToDB(int year, int month, int day, pCHAR destination);

	// Datatype money needs to be converted from database format to a
	// double to be readable (SELECT).
	pCHAR	MoneySelectFromDB(pCHAR attr, pCHAR destination);

	// QELIB needs to know which database you want to access.
	DB_TYPE GetCurrentDB(void);
	OS_BOOL SetCurrentDB(DB_TYPE currentDB);	// tell interface which DB


	// SQL interface statements.

	// Some databases don't have a way of determining the highest or
	// lowest value a field currently has. Where max(fieldName) and
	// min(fieldName) can't be used, GetMinFieldValue or GetMaxFieldName
	// must be used instead. Generated syntax: "select <fieldName> from
	// <tableName> where <fieldName> [<|>] ['<startValue>'|<startValue>]
	// <andClause> order by <fieldName>".
	pCHAR	GetMinFieldValue(pCHAR tableName, pCHAR fieldName,
				 pCHAR startValue, pCHAR andClause);
	long	GetMinFieldValue(pCHAR tableName, pCHAR fieldName,
				 long startValue, pCHAR andClause);
	pCHAR	GetMaxFieldValue(pCHAR tableName, pCHAR fieldName,
				 pCHAR startValue, pCHAR andClause);
	long	GetMaxFieldValue(pCHAR tableName, pCHAR fieldName,
				 long startValue, pCHAR andClause);
	long	GetMaxSerialNo(pCHAR tableName, pCHAR fieldName);

	// BEGIN or COMMIT TRANSACTION is not supported by non relat. databases
	DB_RETURN	BeginTransaction(int cursorId);
	DB_RETURN	CommitTransaction(int cursorId);
	DB_RETURN	RollBackTransaction(int cursorId);
	DB_RETURN	TransactionNesting( int &nesting, int cursorId );

	// Test on equality statement.
	// Generated syntax: " <keyWord> <field> = [<value>|'<value>']".
	DB_RETURN	Equal(const char *keyWord, const char *field, pCHAR value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, int value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, long value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, double value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, MONEY *value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, pSInt value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, pLong value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, pDouble value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, pString value, int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, OSDateTime value,int cursorId);
	DB_RETURN	Equal(const char *keyWord, const char *field, OSDateTime *value,int cursorId);



	// Assignment statement. If 'useComma' is set to STATE_FALSE, no
	// comma will be appended to the statement.
	// Generated syntax: " <field> = [<value>|'<value>'][,]".
#if 0
// sih:not used ?
	DB_RETURN	SetEqual(const char *field, pCHAR value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, int value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, long value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, double value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, MONEY *value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, pSInt value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, pLong value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, pDouble value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, OSDateTime *value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, pString value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId);
	DB_RETURN	SetEqual(const char *field, OSString value,
			 OS_BOOL useComma = STATE_TRUE, int cursorId)
			{ return SetEqual( field, &value, useComma, cursorId ); }

#endif
	// Error handling functions. Moved to DBInterfaceBasic class from ErrMessage
	// by Oddbjorn, 92/04/27.

	DB_RETURN	ReturnedErrorCode(ERRNO error) const;
	DB_RETURN	SetDBError(ERRNO error, int supress_printing=FALSE);
	DB_RETURN	SetDBError(const char *errorText, const ERRNO error,
							int supress_printing=FALSE );
	pCHAR		GetDBError( void );
	pCHAR		GetDBMSError( void );
	ERRNO		GetDBErrorNumber( void );
	const char	*GetErrnoText( void );
	char		*GetUserErrString( void );
	int			GetProcedureStatus( void ) { return procedureStatus;}
	DB_RETURN	ClearDBError( void );
	OS_BOOL		TestDBError();
	int			DeadLocked();	// That error was just a deadlock,
								// you can try again...

	lang_id LangId( void ) { return langId ;}

	SqlCache        *Psc( void ) { return psc; }

	// Sett error reporting function. The error reporting function is
	// called from SetDBError()
	DBI_error_function SetErrorFunction( DBI_error_function newFunc );

	// Very DBMS dependent. Drop SQLsentenc created
	void FreeCmdBuff( int cursorId );

	long NumberOfResultColumnsFound(int cursorId);	// ABo 20071010

protected:
	// Open used with SQL Server (not Q+E implementation).
	DB_RETURN Open( char *server , int cursorId);
	
	// Close used by Sybase implementation. (JR, EFI)
	void Close( int cursorId );

	// Function to test if a given CursorId is valid. Basically a renaming
	// and declassifying of the old function SetCursor, which no longer
	// acts like its name indicates.
	int	TestCursor( int cursorId ) { return SetCursor( cursorId ); };

	DBMS	*dbms;			// Pointer to database management specific part
	
private:
	// Internal Cmd which is called by the two public functions with or without
	// a cursor ID.
	DB_RETURN	Cmd(int cursorId, int dummy, pCHAR sqlStr, va_list ap);
	int			SetCursor(int cursorId);
	DB_RETURN 	CreateResultingObjects( OSDataType **(&result), int cursorId);

	pCHAR	password;			// Login password.
	pCHAR	appl;				// Name of application.
	pCHAR	clientUser;			// Client user identifier to mark the session with.
	pCHAR	server;				// The name of the server database.
	char 	*sqlServer;			// Sybase: The name of the DBSERVER.
	pCHAR	user;				// User name logged on current database
	char*	charSet;
	int		grupKey;			// ICC Group ID for UNIX logon user.
	lang_id langId;				// Pointer to language file for error messages

	DB_TYPE currentDB;		// defaults to SQL Server in constructor
	pCHAR	driver;			// driver specified in QELIB DB connect.
	pCHAR	filePath;		// path for data files (non-RDBMS)

	int	hdbc;			// handle to database connection
	int	hstmt;			// handle to current SQL statement
	int	resCode;		// value returned from database DLL calls
						// of DBInterfaceBasic object.

	SqlCache *psc;				// Pointer to sql cache class;
			

	OS_BOOL 	SQLStarted;		// SQL statement started

	ErrMessage	*errMsg;		// Error handler
	ERRNO		userErrNumber;	// Error Number
	pCHAR		userErrString;	// Error string
	int			procedureStatus;// Return value from stored procedures 
	
	// Error handling function.
	DBI_error_function dbiErrorHandler;
	
	STORAGE_MODE	storageMode;// Flag for DBStore function: 
								// 0 = insert, if error update
								// 1 = update, if error insert
};

#endif

