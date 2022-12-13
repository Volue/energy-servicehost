#ifndef  _PDLogCollMessage_
#define _PDLogCollMessage_

#include <PDCollectable.h>
//#include <PDLogMessage.h>
#include <rw/tpslist.h>

typedef RWTPtrSlist<RWCollectable>		LogMessageList;
typedef RWTPtrSlistIterator<RWCollectable>	LogMessageListIterator;

class PDLogHeader;
class PDLogMailTo;
class PDLogMessage;
class PDLogExtMessage;

class PDLogCollMessage : public PDCollectable
{
	PDDECLARE_COLLECTABLE(PDLogCollMessage,PDCollectable)
	
	public:
	
	virtual void saveGuts(RWvostream &o) const;	
	virtual void restoreGuts(RWvistream &i);
	virtual void saveGuts(RWFile &o) const;
	virtual void restoreGuts(RWFile &i);
	
	PDLogCollMessage()	{;}
	~PDLogCollMessage()	{;}
	PDLogCollMessage( const PDLogCollMessage &);
	PDLogCollMessage &operator= (PDLogCollMessage &obj)
	{
	    logMessageList = obj.logMessageList;
	    return *this;
	}
	
	char		*debug( void );
	void		Add( PDLogHeader *);
	void		Add( PDLogMailTo *);
	void		Add( PDLogMessage *);
	void		Add( PDLogExtMessage *);

	LogMessageList	&GetMessages( void )		{ return logMessageList; }
	
	LogMessageList  *GetCollection( void )		{ return &logMessageList;}
	void		clearAndDestroy( void )		{ logMessageList.clearAndDestroy();}
			    

    private:

	LogMessageList logMessageList;
};
#endif
