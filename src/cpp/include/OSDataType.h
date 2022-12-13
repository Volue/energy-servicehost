/******************************************************************************
*******			Database Interface Generator v2			*******
*******			Copyright Objekt Systemer / EFI			*******
*******************************************************************************
*
* File name....: OSDataType.h
* Version......: %I%
* Last modified: $Date: 2014-05-16 08:04:14 +0000 (Fri, 16 May 2014) $
* Path.........: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 92/05/20
*
* DESCRIPTION:
*	Declaration file for the OSDataType super class in the DIG system.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/05/20	OO	New file.
*	3.0	93/09/12	OO	Expanded for use with DIG v2.
*
*******************************************************************************
*/

#ifndef DATATYPE_H
#define DATATYPE_H

#include "DBI_enums.h"
#include <assert.h>


//#include <fstream>
//#include <iomanip>
#ifdef PD_IOSTD
#include <iosfwd>
#else
class std::ostream;
class std::istream;
class std::fstream;
#endif
#include "DBI_ERRNO.h"



#ifndef NULL
#define NULL (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef DBMS_API

#ifdef WIN32
#if !defined(STATIC_DBMS_API)
#	ifdef BUILD_DBMS_API
#		define DBMS_API __declspec(dllexport)
#	else
#		define DBMS_API __declspec(dllimport)
#	endif
#else
#	define DBMS_API
#endif
#else
#   define DBMS_API
#endif

#endif

#include "DBHostVariable.h"

class DBMS_API OSDataType
{
public:

	OSDataType	();
	OSDataType 	(const OSDataType &);
	OSDataType 	(const OSDataType *);
	virtual ~OSDataType	();

	void	SetName(char *name)	{ myName = name; }
	const	char *Name() 		const 	{ return myName; }
	int		IsKey()				const	{ return isKey; }
	void	DefAsKey()			{ isKey	= TRUE; }
	
	const OSDataType &operator=( const OSDataType & );

	// Field value manipulation functions.
	virtual void		Set(OSDataType *);
	virtual void		Set(const char *string);
	virtual OSDataType*	Get();
	virtual char *		Value();	// Get character formatted val.
	char *				HostName( void );

	// Database interaction functions.
	virtual	DB_RETURN	Where(int cursorId , int whereUsed = 0, char prefix = 0) = 0;
	virtual int		JoinProfile(int cursor, char pfx, int didSomething) = 0;
	virtual void	BindField(int fieldNo, int cursorId = 0) = 0;
	virtual void	FirstValue() = 0;
	virtual void	NextValue() = 0;
	virtual void	SetField(int cursorId , int setUsed = 0) = 0;
	virtual void	ValueOfField(int cursorId , char separator = ',') = 0;
	friend  std::ostream &operator<<(std::ostream &o, OSDataType *os);
	friend  std::istream &operator<<(std::istream &i, OSDataType *os);

	virtual ERRNO	WriteToFile( std::fstream * ) = 0;
	virtual ERRNO	ReadFromFile( std::fstream * ) = 0;
	virtual DBHostVariable  *GetDBHostVariable( OSDataType *, int &) = 0;

	int		Width( std::fstream * );
	void	SkipDelim( std::fstream & );

	int		IsNull()  const { return isNull; }
	int		DbNull()  const
			{
			 //   assert( (!isNull) || dbNull );
				return dbNull;
			}
	// Set that object contains no usefull information.
	void	Nullify() 	{ isNull = TRUE; dbNull = FALSE; }
	// Set that the objeckt contains an explicit dbNull
	void	DbNullify()	{ isNull = TRUE; dbNull = TRUE;}
	// Set that the object contain som value.
	void	Enable()	{ isNull = FALSE; dbNull = FALSE; }


private:
	void	CopyNullStatus( const OSDataType &d )
	{
		isNull = d.isNull;
		dbNull = d.dbNull;
	}


protected :
	// Rules for isNull and dbNull.
	// There is three valid states:
	//	Object has no value at all : 	isNull == TRUE dbNull == FALSE
	//				 	This setting is used for
	//					unitiliesed values and when
	//					the value <dbms> NULL is read
	//					from the db.
	//	Object has the value <dbms> NULL :
	//					isNull  == dbNull == TRUE
	//					This value is used when we
	//					want to store NULL in the db.
	//	Object has a value :		isNull == FALSE dbNull == FALSE
	//
	//	isNull = FALSE, dbNull = TRUE is an illegal combination.
	//
	// Some comments:
	// This rules make sure that one only needs to test dbNull to decide
	// if the "value" part of the object contains usefull information.
	// It was also considered to use the combination isNull = FALSE,
	// dbNull = TRUE as <dbms> NULL (after all <dbms> NULL amy be
	// considered a value) but that might break existing kode:
	//
	//	if ( !a.IsNull() )
	//	   use the value of a;
	//
	// It was also considered to set dbNull when a NULL coloumn was read,
	// but it was dropped to maintain existing behavior. Setting dbNull
	// in such situations would result in that a DBSelect followed by a
	// DBUpdate would explisitly renullify any NULL coloums read.
	//	
	char isNull; 
	char dbNull;

	DBHostVariable *pHost;

	short AbortWhenIsNull() const;

private:
	static short abortWhenIsNull;
	char isKey;
	char *myName;
};

inline OSDataType :: OSDataType () {
	isNull = TRUE; dbNull = FALSE;// SIH was Nullify(), but I dont trust compiler inline at this point
		myName = (char *)NULL;
	isKey  = FALSE;
	pHost     = NULL;
}


#endif	// DATATYPE_H

