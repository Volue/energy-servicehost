#ifndef TBIccAppl_H
#define TBIccAppl_H

#include "OSTypes.h"
#include "DBInterf.h"
#include "OSString.h"
#include "Table.h"

// T y p e   D e f i n i t i o n s

class TBIccAppl;
typedef TBIccAppl * pTBIccAppl;


// E x t e r n a l l y   d e f i n e d   v a r i a b l e s

// extern DBInterface *dbi;


// C l a s s   d e c l a r a t i o n

class TBIccAppl : public Table
{
public:
	TBIccAppl	();
	TBIccAppl	(const pTBIccAppl iccAppl );
	~TBIccAppl	();


	// Get functions.
	const OSLong&	GetAppKey	(void) const { return appKey; }
	const OSLong&	GetRcount	(void) const { return rcount; }
	const OSString *	GetDatetim	(void) const { return &datetim; }
	const OSSInt&	GetInfotype	(void) const { return infotype; }
	const OSSInt&	GetIlevel	(void) const { return ilevel; }
	const OSString *	GetDescript	(void) const { return &descript; }
	const OSLong&	GetProfileKey	(void) const { return profileKey; }
	const OSSInt&	GetSignThisEvent	(void) const { return signThisEvent; }
	const OSString *	GetSignOsuser	(void) const { return &signOsuser; }
	const OSString *	GetSignDatetim	(void) const { return &signDatetim; }

	// Set functions.
	void	SetAppKey	(const pLong _appKey)	{ appKey = *_appKey; }
	void	SetAppKey	(const OSLong& _appKey)	{ appKey = _appKey; }
	void	SetAppKey	(const long& _appKey)	{ appKey = _appKey; }
	void	SetRcount	(const pLong _rcount)	{ rcount = *_rcount; }
	void	SetRcount	(const OSLong& _rcount)	{ rcount = _rcount; }
	void	SetRcount	(const long& _rcount)	{ rcount = _rcount; }
	void	SetDatetim	(const OSString * _datetim)	{ datetim = *_datetim; }
	void	SetInfotype	(const pSInt _infotype)	{ infotype = *_infotype; }
	void	SetInfotype	(const OSSInt& _infotype)	{ infotype = _infotype; }
	void	SetInfotype	(const short& _infotype)	{ infotype = _infotype; }
	void	SetIlevel	(const pSInt _ilevel)	{ ilevel = *_ilevel; }
	void	SetIlevel	(const OSSInt& _ilevel)	{ ilevel = _ilevel; }
	void	SetIlevel	(const short& _ilevel)	{ ilevel = _ilevel; }
	void	SetDescript	(const OSString * _descript)	{ descript = *_descript; }
	void	SetProfileKey	(const pLong _profileKey)	{ profileKey = *_profileKey; }
	void	SetProfileKey	(const OSLong& _profileKey)	{ profileKey = _profileKey; }
	void	SetProfileKey	(const long& _profileKey)	{ profileKey = _profileKey; }
	void	SetSignThisEvent	(const pSInt _signThisEvent)	{ signThisEvent = *_signThisEvent; }
	void	SetSignThisEvent	(const OSSInt& _signThisEvent)	{ signThisEvent = _signThisEvent; }
	void	SetSignThisEvent	(const short& _signThisEvent)	{ signThisEvent = _signThisEvent; }
	void	SetSignOsuser	(const OSString * _signOsuser)	{ signOsuser = *_signOsuser; }
	void	SetSignDatetim	(const OSString * _signDatetim)	{ signDatetim = *_signDatetim; }

protected:

	void	AddAllFields( void );

	OSLong	appKey;
	OSLong	rcount;
	OSString	datetim;
	OSSInt	infotype;
	OSSInt	ilevel;
	OSString	descript;
	OSLong	profileKey;
	OSSInt	signThisEvent;
	OSString	signOsuser;
	OSString	signDatetim;
};
#endif	// TBIccAppl_H
