/******************************************************************************
*******			  Database Interface Generator			*******
*******			Copyright Scanvest Olivetti a/s			*******
*******************************************************************************
*
* File name....: Int.h
* Version......: %I%
* Last modified: $Date: 2004-09-29 17:43:15 +0000 (Wed, 29 Sep 2004) $
* Path.........: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 92/05/20
*
* DESCRIPTION:
*	Declaration file for the OSInt super class in the DIG system.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/05/20	OO	New file.
*
*******************************************************************************
*/

#ifndef INT_H
#define INT_H

#include "OSNumeric.h"

#ifndef NULL
#define NULL (0)
#endif

class DBMS_API OSInt : public OSNumeric
{
public:

	OSInt	();
	OSInt (const OSInt &);
	OSInt (const OSInt *);
	~OSInt	();

	operator int ()	const	{ return value; }

private:

	int	value;
};

#endif	// INT_H
