#ifndef  _PDLogHeader_
#define _PDLogHeader_

#include <PDCollectable.h>
#include <rw/rwint.h>
#include <rw/cstring.h>

class PDLogHeader : public PDCollectable
{

  public:

	PDDECLARE_COLLECTABLE(PDLogHeader,PDCollectable)
	
	PDLogHeader() {;}
	PDLogHeader( const PDLogHeader &);
	PDLogHeader &operator= (PDLogHeader &);
	~PDLogHeader() {;}
		
	void	SetBinName( char *p)				{ binName = p;}
	void	SetApplicationName( char *p)		{ applicationName = p;}
	void	SetApplicationNumber( int i)		{ applicationNumber = i;}
	void	SetOsUser( char *p)					{ osUser = p;}
	void	SetDbUser( char *p)					{ dbUser = p;}
	void	SetMisc( char *p)					{ misc = p;}

	RWCString	GetBinName( void )				{ return binName;}
	RWCString	GetApplicationName( void )		{ return applicationName;}
	RWInteger	GetApplicationNumber( void )	{ return applicationNumber;}
	RWCString	GetOsUser( void )				{ return osUser;}
	RWCString	GetDbUser( void )				{ return dbUser;}
	RWCString	GetMisc( void )					{ return misc;}
	
	char		*debug( void );
	
	// redefined from RWCollectable
	virtual int compareTo( const RWCollectable *) const { return TRUE;}
	virtual RWBoolean isEqual( const RWCollectable *) const { return 0;}

	
	// persistance
	virtual void saveGuts(RWvostream&) const;
	virtual void saveGuts(RWFile&) const;
	virtual void restoreGuts(RWvistream&);
	virtual void restoreGuts(RWFile&);

  private:


	RWCString	binName;
	RWCString	bName;
	RWCString	applicationName;
	RWInteger	applicationNumber;
	RWCString	osUser;
	RWCString	dbUser;
	RWCString	misc;
};

#define PDLOGHEADERGuts( x,op )\
{\
	x op binName;\
	x op applicationName;\
	x op applicationNumber;\
	x op osUser;\
	x op dbUser;\
	x op misc;\
}
	
#endif
