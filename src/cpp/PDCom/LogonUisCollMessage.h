#ifndef  _LogonUisCollMessage_
#define _LogonUisCollMessage_

#include <PDCollectable.h>
#include <LogonUisMessage.h>
#include <rw/tvslist.h>
typedef  RWTValSlist<LogonUisMessage> EnvironmentList;
typedef   RWTValSlistIterator<LogonUisMessage> EnvironmentListIterator;
class LogonUisCollMessage : public PDCollectable {
	PDDECLARE_COLLECTABLE(LogonUisCollMessage,PDCollectable);
	virtual void saveGuts(RWvostream&o) const {
		PDCollectable::saveGuts(o);
		o<<environmentList;
	}
	virtual void restoreGuts(RWvistream&i) {
		PDCollectable::restoreGuts(i);
		i>>environmentList;
	}

    public:
	LogonUisCollMessage();
	LogonUisCollMessage( const LogonUisCollMessage &);
	LogonUisCollMessage& operator=(LogonUisCollMessage&x) {
		environmentList=x.environmentList;
		return *this;
	}
	~LogonUisCollMessage();
	EnvironmentList	&GetMessages( void ) { return  environmentList;}
	
	char		*debug( void );
	void		Add( LogonUisMessage *);

	EnvironmentList *GetCollection( void ) { return &environmentList;}
	void	clearAndDestroy( void ) { environmentList.clear();}
		    
    private:
	EnvironmentList	environmentList;
};

#endif
