/******************************************************************************
*******			Database Interface Generator v2			*******
*******			Copyright Objekt Systemer / EFI			*******
*******************************************************************************
*
* File name....: SInt.h
* Version......: %I%
* Last modified: $Date: 2009-05-14 22:40:41 +0000 (Thu, 14 May 2009) $
* Path.........: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 92/05/20
*
* DESCRIPTION:
*	Declaration file for the OSSInt super class in the DIG system.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/05/20	OO	New file.
*	3.0	93/09/12	OO	Expanded for use with DIG v2.
*
*******************************************************************************
*/

#ifndef SINT_H
#define SINT_H

#include "OSNumeric.h"
#include "DBI_enums.h"

#ifndef NULL
#define NULL (0)
#endif

class DBMS_API OSSInt;
typedef OSSInt	*pSInt;

class DBMS_API OSSInt : public OSNumeric
{
public:

	OSSInt	();
	OSSInt (const OSSInt &);
	OSSInt (const OSSInt *);
	OSSInt (const short);
	~OSSInt	();

	operator short () const;
	OSSInt &operator =(const short);
	OSSInt &operator=(const OSSInt &si);
	OSSInt &operator=(const OSSInt *si);

	void		Set(OSDataType *);
	void		Set(const char *string);
	OSDataType *	Get();
	char *		Value();

	DB_RETURN	Where(int cursorId , int whereUsed = 0, char prefix = 0);
	int		JoinProfile(int cursor, char pfx, int didSomething);
	void		BindField(int columnNo, int cursorId );
	void		FirstValue();
	void		NextValue();
	void		SetField(int cursorId , int setUsed = 0);
	void		ValueOfField(int cursorId , char separator = ',');

	ERRNO		WriteToFile( std::fstream * );
	ERRNO		ReadFromFile( std::fstream * );
	DBHostVariable  *GetDBHostVariable( OSDataType *p, int &count );	


private:

	short value;
};

#endif	// SINT_H
