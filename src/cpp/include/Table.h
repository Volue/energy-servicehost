/******************************************************************************
*******			  Database Interface Generator v2		*******
*******			Copyright Objekt Systemer / EFI			*******
*******************************************************************************
*
* File name....: Table.h
* Version......: %I%
* Last modified: $Date: 2009-05-14 22:40:41 +0000 (Thu, 14 May 2009) $
* Path name....: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 92/04/07
*
* DESCRIPTION:
*	Declaration file for the Table super class in the DIG system.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0		92/04/07	OO		New file.
*	3.0     93/09/12	OO		Expanded for use with DIG v2.
*
*******************************************************************************
*/

#ifndef TABLE_H
#define TABLE_H

#include "DBInterf.h"
#include "OSString.h"
//#include <fstream>
#include "DBI_ERRNO.h"

extern DBMS_API DBInterface *dbi;

enum AutoKeyMechanismType
{
	AKM_OFF,		// Automatic key generation disabled.
	AKM_ON,			// Automatic key generation enabled.
	AKM_UNDEFINED	// Automatic key generation not specified.
};

enum DB_STORE_MODE
{
	DB_STORE,		// Store the row with update or insert.
	DB_UPDATE,		// Update the row, requiring that it already
				// exists
	DB_INSERT		// Insert the row, requiring that it doesn't
				// already exist.
};

enum OBJECT_TYPE
{
	TABLE,			// It's a table.
        TABLE_SELECT_VIEW,      // It's a table, but use view for select.
	VIEW			// It's a view.
};

enum	FILE_ACCESS_MODE
{
    FILE_READ_MODE,
    FILE_WRITE_MODE
};


class DBMS_API Table
{
friend class RelTable;
friend class ExTable;
friend class EFISchema;

protected:			// This is an abstract base class.

	Table(		char *name,
			short noFields, OSDataType **fieldsDBO, 
			short noKeyFields, OSDataType **keyFieldssDBO, 
			short useAutoKeyMechanism = 0,
			OBJECT_TYPE type = TABLE);

	Table(		char *name,
			short noFields = 0,
			short noKeyFields = 0,
			short useAutoKeyMechanism = 0,
			OBJECT_TYPE type = TABLE);
			
	Table (const Table *);


	DB_RETURN	DBRealStore(int cursorId, DB_STORE_MODE mode, DB_BATCH_MODE batchMode );
	DB_RETURN	UpdateStatement(int cursorId);
	DB_RETURN	InsertStatement(int cursorId);
        int             deadLocked(const int gotAutoKey );

	void		ClearDistinct();
	void		AddDistinctField(int fieldNumber, char *name);
	int		BuildDistinctFieldNos();
	int		FetchFieldPosition( const char *field);

	~Table	();
public:

	// Automatic key generation settings.

	AutoKeyMechanismType	AutoKeyMechanism()
							{ return (autoKeyMechanism == AKM_UNDEFINED) ?
								defaultAutoKeyMechanism : autoKeyMechanism; }

	void	SetDefaultAutoKeyMechanism	(AutoKeyMechanismType akm)
							{ defaultAutoKeyMechanism = akm; }
	void	SetAutoKeyMechanism			(AutoKeyMechanismType akm)
							{ autoKeyMechanism = akm; }


        int             NumberOfFields( void )          { return numberOfFields; }
        int             NumberOfKeyFields( void )       { return numberOfKeyFields; }
	OSDataType	*Get( char *field);
	OSDataType	*FieldRef( const int i )        { return (OSDataType *) field[i];}
	OSDataType	*KeyFieldRef( const int i )     { return (OSDataType *) keyField[i];}


	// User specified where clauses.

	void		SetWhere	(const pString whereClause,
					 OSDataType **pHost = NULL);

	void		SetWhere	(const OSString &whereClause,
					 OSDataType **pHost = NULL);

	void		SetWhere	(const char *whereClause,
					 OSDataType  **pHost = NULL);

	char		*GetWhere	() { return (whereClause) ?
						 whereClause->GetString() : NULL;
					   }


	// User specified order by clauses.

	void		SetOrderBy	(const pString orderByClause);
	void		SetOrderBy	(const char *orderByClause);

	char		*GetOrderBy	() { return (orderByClause) ?
					orderByClause->GetString() : NULL; }
	
	// Select distinct option.

	void		SetDistinct(char *f1, char *f2 = NULL, char *f3 = NULL,
			     char *f4 = NULL, char *f5 = NULL, char *f6 = NULL,
			     char *f7 = NULL, char *f8 = NULL, char *f9 = NULL,
			     char *f10= NULL, char *f11= NULL, char *f12= NULL);


	char		*SetErrorText(const char *tableName);

	// File manipulation functions.
	char		*SetErrorFileText(const char *tableName);
	void		ConnectStream( std::fstream *pio, FILE_ACCESS_MODE fileMode);
	void		DisconnectStream( void );
	DB_RETURN	ReadFromFile ( DB_READ_MODE rMode= READ_CURRENT, std::fstream *pio = NULL );
	DB_RETURN	WriteToFile( DB_READ_MODE rMode = READ_CURRENT, std::fstream *pio = NULL );
	void		DescribeObject( void );
	


	// Field list manipulation functions.

	void		AddField		(OSDataType *, char *);
	void		AddKeyField		(OSDataType *);
	void		Clear();


	// Name access function

	char		*Name() { return myName; }
	char		*ViewName() { return (viewName) ? viewName->GetString() : myName; }


	// Dump field names for select (from the Join class)

	int		DumpFieldList(char prefix, char *buffer,
				int *fieldNumbers);
	int		FieldIndex(char *fieldName);
	int		BindFields(int cursor, int column, int *fieldNumbers);
	int		JoinProfile(int cursor, char prefix, int didSomething);


	// Database access functions.

	DB_RETURN	DBSelect(DB_READ_MODE mode,int cursorId);
	DB_RETURN	DBStore(int cursorId, DB_BATCH_MODE batchMode = DB_NO_BATCH );
	DB_RETURN	DBUpdate(int cursorId, DB_BATCH_MODE batchMode = DB_NO_BATCH );
	DB_RETURN	 DBInsert(int cursorId, DB_BATCH_MODE batchMode = DB_NO_BATCH );
	DB_RETURN	 DBDelete(int cursorId, DB_BATCH_MODE batchMode = DB_NO_BATCH );

	// Object type access function.

	int		IsAView() { return (int) isAView; }

private:

	static	AutoKeyMechanismType	defaultAutoKeyMechanism;

	AutoKeyMechanismType	autoKeyMechanism;

	pString		whereClause;
	pString		orderByClause;
	char		**distinctFieldNames;
	int		*distinctFieldNos;
	int		noDistinctFields;
        char		*myName;
        pString		viewName;
	short		knownFields;
	short		knownKeyFields;

	short		isAView;

	std::fstream		*pLocalio;
	int		fstreamAllocated;

	

protected:

        OSDataType              **pHost;

	short			mayIDeleteFieldsWhenDone;

	OSDataType		**field;
	short			numberOfFields;

	OSDataType		**keyField;
	short			numberOfKeyFields;
};

#endif	// TABLE_H

