/******************************************************************************
*******			  Database Interface Generator			*******
*******			Copyright Scanvest Olivetti a/s			*******
*******************************************************************************
*
* File name....: DateTime.h
* Version......: %I%
* Last modified: $Date: 2019-12-17 12:37:45 +0000 (Tue, 17 Dec 2019) $
* Path name....: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 92/04/07
*
* DESCRIPTION:
*	Declaration file for the OSDateTime super class in the DIG system.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/04/07	OO	New file.
*	2.0 	93/10/20	JR 	DIG v2 Nothoing doone File removable??
*
*******************************************************************************
*/

#ifndef DATETIME_H
#define DATETIME_H

#include "OSDataType.h"
#ifdef M
#undef M
#endif
#include <PDTime.h>

#ifndef NULL
#define NULL (0)
#endif

#define MAX_DATESTRING (50)

class DBMS_API OSDateTime : public OSDataType
{
public:

	OSDateTime () : t(NOPDTIME) { Nullify(); }
	OSDateTime (const pdtime &dt) : t(dt)	// NB! Gir UTC i DB. Ikke helt etter våre definisjoner som antar Zdb.
		{ if (t == NOPDTIME) Nullify(); else Enable(); }
	OSDateTime (const OSDateTime &dt) : OSDataType(dt), t(dt.t)	{}
	OSDateTime (const OSDateTime *dt) : OSDataType(dt), t(dt->t)	{}
	OSDateTime (const char *p,const PDZone* Z = Timezone())	{ t = PDTime::aspdtime(p,Z); if (t == NOPDTIME) Nullify(); else Enable(); }
	
	OSDateTime		&operator=(const pdtime pdt) { t = pdt; if (t == NOPDTIME) Nullify(); else Enable(); return *this;}
					operator pdtime() const { return IsNull() ? NOPDTIME : t; }

	const char		*Value() const;
	const char		*GetString(const char *format = NULL,const PDZone& Z = *Timezone()) const;
	void			GetTime();	// Sets this to PDTime::now()


	DB_RETURN       Where(int cursorId , int whereUsed = 0, char prefix = 0);
	int				JoinProfile(int cursor, char pfx, int didSomething);
	void            BindField(int columnNo, int cursorId );
	void            FirstValue();
	void            NextValue();
	void            SetField(int cursorId , int setUsed = 0);
	void            ValueOfField(int cursorId , char separator = ',');

	ERRNO			WriteToFile( std::fstream * );
	ERRNO			ReadFromFile( std::fstream * );
	DBHostVariable  *GetDBHostVariable( OSDataType *, int &);

	static PDZone* Timezone();
private:

	pdtime			t;

	static PDZone* tz;
};

#endif	// DATETIME_H
