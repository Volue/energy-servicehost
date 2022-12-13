/**************************************************************************
*******	    Database Interface Derived Class Declaration File	    *******
*******		     Copyright Scanvest Olivetti a.s		    *******
***************************************************************************
*
* File name....: TBExTseriVl.h
* Version......: %I%
* Last modified: 92/08/24 18:46
* Path name....: %P%
* Created by...: Jarand Roeynstrand
* Created date.: 92/08/24 18:46
*
* DESCRIPTION:
*	Class declaration file for derived class TBExTseriVl.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/08/24 18:46	jr	New file.
*
***************************************************************************
*/


N O T   T O    B E  U S E D   B Y   A N Y O N E  ! ! ! !   JR!

#ifndef TBExTseriVl_H
#define TBExTseriVl_H

#include "TBTseriVl.h"
#include "TBTimeser.h"
#include "TBTsquaVl.h"
#include "TBTsquaVl.h"

// T y p e   D e f i n i t i o n s

class	TBExTseriVl;
typedef TBExTseriVl * pTBExTseriVl;


// C l a s s   d e c l a r a t i o n

class TBExTseriVl : public TBTseriVl
{
public:
	TBExTseriVl	(	pLong timsKey = (pLong) NULL,
				pLong stabyte = (pLong) NULL,
				pLong numbbyte = (pLong) NULL,
				pString chval = (pString) NULL,
				pString timeserCode = (pString) NULL,
				pString timeserName = (pString) NULL,
				pString tsquaVlChval = (pString) NULL,
				OS_BOOL fetchTypeCode = STATE_FALSE );
	TBExTseriVl	(	const pTBExTseriVl TBExTseriVl );
	~TBExTseriVl	();

	// Database interface functions.
	DB_RETURN DBSelect	(DB_READ_MODE mode = READ_CURRENT, int cursorId = 0);
	DB_RETURN DBStore	(int cursorId = 0);
	DB_RETURN DBDelete	(int cursorId = 0);

	// Clear function (to clear all fields).
	void Clear	(void);

	// Get functions.
	pString	GetTimeserCode	(void)	{ return &timeserCode; }
	pString	GetTimeserName	(void)	{ return &timeserName; }
	pString	GetTsquaVlChval	(void)	{ return &tsquaVlChval; }
	OS_BOOL 	GetFetchTypeCode	(void) const {return fetchTypeCode; }

	// Set functions.
	void	SetTimeserCode	(const pString timeserCode);
	void	SetTimeserName	(const pString timeserName);
	void	SetTsquaVlChval	(const pString tsquaVlChval);
	void	SetFetchTypeCode	(const OS_BOOL  fetchTypeCode);

private:
	OSString	timeserCode;
	OSString	timeserName;
	OSString	tsquaVlChval;
	TBTimeser	timeser;
	TBTsquaVl	tsquaVl;
	TBTsquaVl	tsquaVl;
	OS_BOOL 	fetchTypeCode;
};
#endif	// TBExTseriVl_H









