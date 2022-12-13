#ifndef TBProgram_H
#define TBProgram_H

#include "OSTypes.h"
#include "DBInterf.h"
#include "OSString.h"
#include "Table.h"

// T y p e   D e f i n i t i o n s

class TBProgram;
typedef TBProgram * pTBProgram;


// E x t e r n a l l y   d e f i n e d   v a r i a b l e s

// extern DBInterface *dbi;


// C l a s s   d e c l a r a t i o n

class TBProgram : public Table
{
public:
	TBProgram	();
	TBProgram	(const pTBProgram program );
	~TBProgram	();


	// Get functions.
	const OSLong&	GetProgKey	(void) const { return progKey; }
	const OSString *	GetCode	(void) const { return &code; }
	const OSString *	GetName	(void) const { return &name; }
	const OSString *	GetDescript	(void) const { return &descript; }
	const OSSInt&	GetSecurity	(void) const { return security; }
	const OSLong&	GetStorflag	(void) const { return storflag; }
	const OSSInt&	GetLoginfo	(void) const { return loginfo; }
	const OSLong&	GetParaflag	(void) const { return paraflag; }

	// Set functions.
	void	SetProgKey	(const pLong _progKey)	{ progKey = *_progKey; }
	void	SetProgKey	(const OSLong& _progKey)	{ progKey = _progKey; }
	void	SetProgKey	(const long& _progKey)	{ progKey = _progKey; }
	void	SetCode	(const OSString * _code)	{ code = *_code; }
	void	SetName	(const OSString * _name)	{ name = *_name; }
	void	SetDescript	(const OSString * _descript)	{ descript = *_descript; }
	void	SetSecurity	(const pSInt _security)	{ security = *_security; }
	void	SetSecurity	(const OSSInt& _security)	{ security = _security; }
	void	SetSecurity	(const short& _security)	{ security = _security; }
	void	SetStorflag	(const pLong _storflag)	{ storflag = *_storflag; }
	void	SetStorflag	(const OSLong& _storflag)	{ storflag = _storflag; }
	void	SetStorflag	(const long& _storflag)	{ storflag = _storflag; }
	void	SetLoginfo	(const pSInt _loginfo)	{ loginfo = *_loginfo; }
	void	SetLoginfo	(const OSSInt& _loginfo)	{ loginfo = _loginfo; }
	void	SetLoginfo	(const short& _loginfo)	{ loginfo = _loginfo; }
	void	SetParaflag	(const pLong _paraflag)	{ paraflag = *_paraflag; }
	void	SetParaflag	(const OSLong& _paraflag)	{ paraflag = _paraflag; }
	void	SetParaflag	(const long& _paraflag)	{ paraflag = _paraflag; }

protected:

	void	AddAllFields( void );

	OSLong	progKey;
	OSString	code;
	OSString	name;
	OSString	descript;
	OSSInt	security;
	OSLong	storflag;
	OSSInt	loginfo;
	OSLong	paraflag;
};
#endif	// TBProgram_H
