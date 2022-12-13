/******************************************************************************
*******			Database Interface Generator v2 		*******
*******			Copyright Objekt Systemer / EFI			*******
*******************************************************************************
*
* File name....: Numeric.h
* Version......: %I%
* Last modified: $Date: 2009-05-14 22:40:41 +0000 (Thu, 14 May 2009) $
* Path.........: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 92/05/20
*
* DESCRIPTION:
*	Declaration file for the OSNumeric super class in the DIG system.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/05/20	OO	New file.
*	3.0	93/09/12	OO	Expanded for use with DIG v2.
*
*******************************************************************************
*/

#ifndef NUMERIC_H
#define NUMERIC_H

#include "OSDataType.h"

#ifndef NULL
#define NULL (0)
#endif

class DBMS_API OSNumeric : public OSDataType
{
public:

	OSNumeric	();
	OSNumeric (const OSNumeric &);
//	OSNumeric (const OSNumeric *);
	~OSNumeric	();

	const OSNumeric &operator=( const OSNumeric &n );
	
	virtual	DB_RETURN	Where(int cursorId , int whereUsed = 0, char prefix = 0) = 0;
	virtual void	BindField(int fieldNo, int cursorId ) = 0;
	virtual void	FirstValue() = 0;
	virtual void	NextValue() = 0;
	virtual void	SetField(int cursorId , int setUsed = 0) = 0;
	virtual void	ValueOfField(int cursorId , char separator = ',') = 0;

private:

};
inline OSNumeric :: OSNumeric () {
}

inline OSNumeric :: OSNumeric (const OSNumeric &n) : OSDataType( n ) {
}


inline const OSNumeric &OSNumeric :: operator=(const OSNumeric &n) {
    this->OSDataType::operator=(n);
    return *this;
}

#endif	// NUMERIC_H
