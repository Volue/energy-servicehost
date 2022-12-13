#ifndef TBIccApp_H
#define TBIccApp_H

#include "OSTypes.h"
#include "DBInterf.h"
#include "OSString.h"
#include "Table.h"

// T y p e   D e f i n i t i o n s

class TBIccApp;
typedef TBIccApp * pTBIccApp;


// E x t e r n a l l y   d e f i n e d   v a r i a b l e s

// extern DBInterface *dbi;


// C l a s s   d e c l a r a t i o n

class TBIccApp : public Table
{
public:
	TBIccApp	();
	TBIccApp	(const pTBIccApp iccApp );
	~TBIccApp	();


	// Get functions.
	const OSLong&	GetAppKey	(void) const { return appKey; }
	const OSString *	GetName	(void) const { return &name; }
	const OSString *	GetStartime	(void) const { return &startime; }
	const OSString *	GetFintime	(void) const { return &fintime; }
	const OSString *	GetOsuser	(void) const { return &osuser; }
	const OSString *	GetDbuser	(void) const { return &dbuser; }
	const OSSInt&	GetAppstat	(void) const { return appstat; }
	const OSLong&	GetNomessages	(void) const { return nomessages; }

	// Set functions.
	void	SetAppKey	(const pLong _appKey)	{ appKey = *_appKey; }
	void	SetAppKey	(const OSLong& _appKey)	{ appKey = _appKey; }
	void	SetAppKey	(const long& _appKey)	{ appKey = _appKey; }
	void	SetName	(const OSString * _name)	{ name = *_name; }
	void	SetStartime	(const OSString * _startime)	{ startime = *_startime; }
	void	SetFintime	(const OSString * _fintime)	{ fintime = *_fintime; }
	void	SetOsuser	(const OSString * _osuser)	{ osuser = *_osuser; }
	void	SetDbuser	(const OSString * _dbuser)	{ dbuser = *_dbuser; }
	void	SetAppstat	(const pSInt _appstat)	{ appstat = *_appstat; }
	void	SetAppstat	(const OSSInt& _appstat)	{ appstat = _appstat; }
	void	SetAppstat	(const short& _appstat)	{ appstat = _appstat; }
	void	SetNomessages	(const pLong _nomessages)	{ nomessages = *_nomessages; }
	void	SetNomessages	(const OSLong& _nomessages)	{ nomessages = _nomessages; }
	void	SetNomessages	(const long& _nomessages)	{ nomessages = _nomessages; }

protected:

	void	AddAllFields( void );

	OSLong	appKey;
	OSString	name;
	OSString	startime;
	OSString	fintime;
	OSString	osuser;
	OSString	dbuser;
	OSSInt	appstat;
	OSLong	nomessages;
};
#endif	// TBIccApp_H
