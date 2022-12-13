/****************************************************************************
 **********       Copyright Objekt Systemer / EFI                    ********
 *****************************************************************************
 *
 * File name....: String.h
 * Version......: %I%
 * Last modified: $Date: 2013-01-10 07:49:09 +0000 (Thu, 10 Jan 2013) $
 * Path name....: %P%
 * Created by...: JA
 * Created date.: 90/06/09
 *
 * DESCRIPTION:
 *
 * MODIFICATION HISTORY:
 *	Ver	Date		Sign	Description
 *	1.1	90/06/09	JA	New header file.
 *		92/02/06	SK	New std::string concatenating
 *					operator += (LONG)
 *	1.2	92/09/16	JR	Default constructor.
 *		93/03/23	JR/OB	Moved friend operators to .C file
 *					(g++ problems with inline functions)
 *		92/02/06	SK	New std::string concatenating `
 *					operator += (LONG)
 *	3.0	93/09/12	OO	Expanded for use with DIG v2.
 *
 ****************************************************************************/

#ifndef	SOSTRING_H
#define	SOSTRING_H

#ifndef MSWIND
// #include <Ostream.h> GIVES a compile ERROR, replaced by the next two lines!!
// class std::istream;
// class std::ostream;
#endif


#include <string.h>

#include "OSTypes.h"

class DBMS_API OSString;
typedef OSString *	pString;

class DBMS_API OSString : public OSDataType
{
public:
	OSString();
	OSString(const CHAR *);
	OSString(const OSString &);
	OSString(const OSString *);
//	OSString(const pString);

	~OSString();		// OSString destructor

	operator const pCHAR() const	{ return GetString(); }

//--- Overloaded assignment operator using a referenced OSString or a  ---
//--- a char pointer as input ------------------------------------------
//----------------------------------------------------------------------
	void operator = (const OSString &);
	void operator = (const OSString *);
//	void operator = (const pString);
	void operator = (const CHAR *);

//--- Overloaded comparison operators given a OSString reference input ---
//----------------------------------------------------------------------
	int operator < (const OSString &s) const	{ return strcmp(p, s.p) < 0; }
	int operator > (const OSString &s) const	{ return strcmp(p, s.p) > 0; }
	int operator <=(const OSString &s) const	{ return strcmp(p, s.p) <= 0; }
	int operator >=(const OSString &s) const	{ return strcmp(p, s.p) >= 0; }
	int operator ==(const OSString &s) const    { return strcmp(p, s.p) == 0; }
	int operator !=(const OSString &s) const	{ return !(*this==s); }


//--- Overloaded comparison operators given a pString input ---
//----------------------------------------------------------------------
	int operator < (const pString s) const	{ return strcmp(p, s->p) < 0; }
	int operator > (const pString s) const	{ return strcmp(p, s->p) > 0; }
	int operator <=(const pString s) const	{ return strcmp(p, s->p) <= 0; }
	int operator >=(const pString s) const	{ return strcmp(p, s->p) >= 0; }
	int operator ==(const pString s) const	{ return strcmp(p, s->p) == 0; }
	int operator !=(const pString s) const	{ return strcmp(p, s->p) != 0; }

//--- Overloaded comparison operators with a char pointer as input   ---
//----------------------------------------------------------------------
	int operator < (const pCHAR cs) const	{ return strcmp(p, cs) < 0; }   
	int operator > (const pCHAR cs) const	{ return strcmp(p, cs) > 0; }   
	int operator <=(const pCHAR cs) const	{ return strcmp(p, cs) <= 0; }
	int operator >=(const pCHAR cs) const	{ return strcmp(p, cs) >= 0; }
	int operator ==(const pCHAR cs) const	{ return strcmp(p, cs) == 0; }
	int operator !=(const pCHAR cs) const	{ return strcmp(p, cs) != 0; }

//--- Overloaded comparison operators with a char pointer and a      ---
//--- referenced OSString as input ---------------------------------------
//----------------------------------------------------------------------
	friend int operator <(const pCHAR cs, const OSString &s);
	friend int operator >(const pCHAR cs, const OSString &s);
	friend int operator<=(const pCHAR cs, const OSString &s);
	friend int operator>=(const pCHAR cs, const OSString &s);
	friend int operator==(const pCHAR cs, const OSString &s);
	friend int operator!=(const pCHAR cs, const OSString &s);

//--- OSString concatenating operator ---
//-------------------------------------
	OSString operator + (const OSString &s) const;
	OSString operator + (const pCHAR cs) const;
	friend DBMS_API OSString operator + (const pCHAR cs, const OSString &s);
	OSString& operator +=(const OSString &);
	OSString& operator +=(const pCHAR cs);
	OSString& operator +=(const int i);
	OSString& operator +=(const long l);

//--- OSString subtractor operator ---
//----------------------------------
	OSString operator - (const OSString &s) const;
	OSString operator - (const pCHAR cs) const;
	OSString& operator -=(const OSString &);
	OSString& operator -=(const pCHAR cs);

//--- OSString input and output operators ---
//-------------------------------------
#ifndef MSWIND
//	friend std::ostream &operator<<(std::ostream &o, const OSString &s);
//	friend std::istream &operator>>(std::istream &i, OSString &s);
#endif

	void toAscii();
	void toLower();
	void toUpper();

	pCHAR GetString(void) const { return IsNull() ? (pCHAR) "\0" : p; }
	int GetLen(void) const { return len; }


	// Field value manipulation functions.
	void Set(OSDataType *);
	void Set(const char *string);
	OSDataType * Get();
	char * Value();

	// Database interaction functions.
	DB_RETURN Where(int cursorId , int whereUsed = 0, char prefix = 0);
	int  JoinProfile(int cursor, char pfx, int didSomething);
	void BindField(int columnNo, int cursorId );
	void FirstValue();
	void NextValue();
	void SetField(int cursorId , int setUsed = 0);
	void ValueOfField(int cursorId , char separator = ',');

	ERRNO WriteToFile( std::fstream * );
	ERRNO ReadFromFile( std::fstream * );
	DBHostVariable *GetDBHostVariable( OSDataType * p, int &count);	

private:
	// character string
	pCHAR p;
	// length of std::string, excluding NULL character
	int len;
};

#endif
