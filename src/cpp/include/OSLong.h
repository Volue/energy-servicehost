/******************************************************************************
*******			Object Database Generator v2 			*******
*******			Copyright Objekt Systemer / EFI			*******
*******************************************************************************
*
* File name....: Long.h
* Version......: %I%
* Last modified: $Date: 2014-05-16 08:04:14 +0000 (Fri, 16 May 2014) $
* Path.........: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 92/05/20
*
* DESCRIPTION:
*	Declaration file for the OSLong super class in the DIG system.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/05/20	OO	New file.
*	3.0	93/09/12	OO	Expanded for use with DIG v2.
*
*******************************************************************************
*/

#ifndef LONG_H
#define LONG_H

#include "OSNumeric.h"
#include "DBI_enums.h"
#ifdef PDDEBUG
#include <stdlib.h>
#include "icc_debug.h"
#endif

#ifndef NULL
#define NULL (0)
#endif

class DBMS_API OSLong;
typedef OSLong *pLong;

class DBMS_API OSLong : public OSNumeric
{
public:

	OSLong	();
	OSLong (const OSLong &);
//	OSLong (const OSLong *);
	OSLong (const long);
	OSLong (const int);
	~OSLong	();

	OSLong& operator=(const long val);
	OSLong& operator=(const int);
	OSLong& operator=(const OSLong& l);
	OSLong& operator=(const OSLong *l);
	OSLong& operator*(OSLong *l);

	operator long () const;

        // Field value manipulation functions.
	void            Set(OSDataType *);
	void		Set(const char *string);
	OSDataType *      Get();
	char *		Value();

	// Database interaction functions.
	DB_RETURN	Where(int cursorId , int whereUsed = 0, char prefix = 0);
	int		JoinProfile(int cursor, char pfx, int didSomething);
	void		BindField(int columnNo, int cursorId );
	void		FirstValue();
	void		NextValue();
	void		SetField(int cursorId , int setUsed = 0);
	void		ValueOfField(int cursorId , char separator = ',');

	ERRNO		WriteToFile( std::fstream * );
	ERRNO		ReadFromFile( std::fstream * );
	DBHostVariable  *GetDBHostVariable( OSDataType *p, int &count);	
private:
	long	value;
};
inline OSLong :: OSLong () {
	value = 0;
}


inline OSLong :: OSLong (const OSLong& l) : OSNumeric( l ) {
	value = l.value;
}

inline OSLong :: OSLong ( const long l ) {
    value = l;
    Enable();
}

inline OSLong :: OSLong ( const int l )  {
    value = l;
    Enable();
}

inline OSLong :: ~OSLong() {
}

inline OSLong::operator long() const {
#ifdef PDDEBUG
	if ( IsNull() && value != 0 )
	{
		if ( AbortWhenIsNull() )
		{
			PD_FATAL(fprintf(stderr,"IsNull for column %s (value=%ld)\n", Name() ? Name() : "<not set>", value));
		}
		else
		{
			PD_WARNING(fprintf(stderr,"IsNull for column %s (value=%ld)\n", Name() ? Name() : "<not set>", value));
		}
	}
#endif
	return value;
}

inline OSLong &OSLong::operator=(const int i) {
	value = i;
	Enable();
        return *this;
}


inline OSLong &OSLong::operator=(const long l) {
	value = l;
	Enable();

	return *this;
}

inline OSLong& OSLong::operator=(const OSLong& l) {
	this->OSNumeric::operator=(l);
	value = (long)l;
	return *this;
}
inline OSDataType * OSLong :: Get() {
	return this;
}

#endif	// LONG_H
