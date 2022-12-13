#ifndef  _LogonUisMessage_
#define _LogonUisMessage_

#include <PDCollectable.h>
#include <rw/rwint.h>
#include <rw/cstring.h>

class LogonUisMessage : public PDCollectable {    
	PDDECLARE_COLLECTABLE(LogonUisMessage,PDCollectable);
	// persistance
	virtual void saveGuts(RWvostream&o) const {
		PDCollectable::saveGuts(o);
		o<<nbEnvVar<<variable<<value;
	}
	virtual void restoreGuts(RWvistream&i) {
		PDCollectable::restoreGuts(i);
		i>>nbEnvVar>>variable>>value;
	}
  public:
	LogonUisMessage();
	LogonUisMessage(int _nbEnvVar, char* _variable, char* _value) {
	    nbEnvVar 		= _nbEnvVar;
		variable		= _variable;
		value			= _value;
	}
	LogonUisMessage(const LogonUisMessage& i) {
		value=i.value;variable=i.variable;
		nbEnvVar=i.nbEnvVar;
	}
	LogonUisMessage& operator=(LogonUisMessage& i) {
		value=i.value;variable=i.variable;
		nbEnvVar=i.nbEnvVar;
		return *this;
	}
	~LogonUisMessage() {};
	
	void		SetNbEnvVar( int i ) { nbEnvVar = i;}
	void		SetValue( char *p ) { value = p;}
	void		SetVariable( char *p ) { variable = p;}

	RWInteger	GetNbEnvVar( void ) { return nbEnvVar;}
	RWCString	GetVariable( void ) { return variable;}
	RWCString	GetValue( void ) { return value;}
  private:
	RWInteger		nbEnvVar;
	RWCString		variable;
	RWCString		value;
	
};
	
#endif
