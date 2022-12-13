/******************************************************************************
*******			Object Database Generator v2			*******
*******			Copyright Objekt Systemer / EFI			*******
*******************************************************************************
*
* File name....: Double.h
* Version......: %I%
* Last modified: $Date: 2014-07-17 14:39:59 +0000 (Thu, 17 Jul 2014) $
* Path.........: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 92/05/20
*
* DESCRIPTION:
*	Declaration file for the OSDouble super class in the DIG system.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/05/20	OO	New file.
*	3.0	93/09/12	OO	Expanded for use with DIG v2.
*
*******************************************************************************
*/

#ifndef DOUBLE_H
#define DOUBLE_H

#include "OSNumeric.h"
#include "DBI_enums.h"
#include <genlib.h>
#ifdef PDDEBUG
#include <stdlib.h>
#include "icc_debug.h"
#endif
#ifndef NULL
#define NULL (0)
#endif

class DBMS_API OSDouble;
typedef OSDouble *pDouble;

class DBMS_API OSDouble : public OSNumeric
{
public:

	OSDouble	();
	OSDouble (const OSDouble &);
//	OSDouble (const OSDouble *);
	OSDouble (const double);
	~OSDouble	();

	OSDouble& operator=(const double);

	OSDouble& operator=(const OSDouble&);
	OSDouble& operator=(const OSDouble*);

	operator double () const;// this one is SLOW

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
	double	value;
};

inline OSDouble :: OSDouble () {
	value = 0;
}


inline OSDouble :: OSDouble (const OSDouble &d) : OSNumeric(d) {
	value = d.value;
}


inline OSDouble::OSDouble( const double d ) {
	value = d;
	if (icc_isdouble(d))
		Enable();
	else
		Nullify();
}

inline OSDouble :: ~OSDouble() {
}

inline OSDouble::operator double () const {
#ifdef PDDEBUG
	if ( IsNull() && value != 0 ) {
		if ( AbortWhenIsNull() ) {
			PD_FATAL(fprintf(stderr,"IsNull for column %s (value=%lf)\n", Name() ? Name() : "<not set>", value));
		}
		else
		{
			PD_WARNING(fprintf(stderr,"IsNull for column %s (value=%lf)\n", Name() ? Name() : "<not set>", value));
		}
	}
#endif
	return value;
}

inline OSDouble &OSDouble::operator=(const double d) {
	value = d;
	if (icc_isdouble(d))
		Enable();
	else
		Nullify();
	return *this;
}

inline OSDouble &OSDouble::operator=(const OSDouble& d) {
	this->OSDataType::operator=(d);
	if (!d.IsNull())
		value = (double)d;
	return *this;
}


#endif	// DOUBLE_H
