#ifndef  _PDLogMessage_
#define _PDLogMessage_

#include <PDCollectable.h>
#include <time_api.h>
#include <rw/rwint.h>
#include <rw/cstring.h>
#include <stdarg.h>

class PDLogMessage : public PDCollectable
{
	PDDECLARE_COLLECTABLE(PDLogMessage,PDCollectable)

	virtual void saveGuts(RWvostream&) const;
	virtual void restoreGuts(RWvistream&);
	virtual void saveGuts(RWFile &) const;
	virtual void restoreGuts(RWFile &);
    
public:
    
	PDLogMessage();
	PDLogMessage( const PDLogMessage &obj);
	PDLogMessage &operator= (PDLogMessage &obj);
	~PDLogMessage()		{;}
	
	PDLogMessage(int _logToDB, int _logToWindow, int _logType, int _logLevel,const char *_logMessage);

	void		SetLogToDB( RWBoolean i ) 	{ logToDB = i;}
	void		SetLogToFile( RWBoolean  i ) 	{ logToFile = i;}
	void		SetLogToWindow( RWBoolean i ) 	{ logToWindow = i;}
	void		SetLogType( int i ) 		{ logType = i;}
	void		SetLogLevel( int i ) 		{ logLevel = i;}
	void		SetLogDate( char *p ) 		{ logDate = p;}
	void 		SetLogDate( void ) {
					char now[18];
					TIME_today( now,3);
					logDate = now;
				}
	void		SetLogMessage( char *p )	{ logMessage = p;}
	void		SetLogHandler( int i )		{ logHandler = i;}

	RWInteger	GetLogToDB( void )		{ return logToDB;}
	RWInteger	GetLogToFile( void )		{ return logToFile;}
	RWInteger	GetLogToWindow( void )		{ return logToWindow;}
	RWInteger	GetLogType( void )		{ return logType;}
	RWInteger	GetLogLevel( void )		{ return logLevel;}
	RWCString	GetLogDate( void )		{ return logDate;}
	RWCString	GetLogMessage( void )		{ return logMessage;}
	RWInteger	GetLogHandler( void )		{ return logHandler;}

	char		*debug( void );
			    
private:

	RWInteger		logToDB;
	RWInteger		logToFile;
	RWInteger		logToWindow;
	RWInteger		logType;
	RWInteger		logLevel;
	RWCString		logDate;
	RWCString		logMessage;
	RWInteger		logHandler;
};

#define PDLOGMESSAGEGuts( x,op)\
{\
	x op logToDB;\
	x op logToFile;\
	x op logToWindow;\
	x op logType;\
	x op logLevel;\
	x op logDate;\
	x op logMessage;\
	x op logHandler;\
}

class PDLogExtMessage : public PDCollectable
{
	PDDECLARE_COLLECTABLE(PDLogExtMessage,PDCollectable)

	virtual void saveGuts(RWvostream&) const;
	virtual void restoreGuts(RWvistream&);
	virtual void saveGuts(RWFile &) const;
	virtual void restoreGuts(RWFile &);

public:

	PDLogExtMessage();
	PDLogExtMessage( const PDLogExtMessage &obj);
	PDLogExtMessage &operator= (PDLogExtMessage &obj);
	PDLogExtMessage( int moduleNumber, int messageId ); 
	
	~PDLogExtMessage() {;}
	
	char	*GetFormatString( void) const { return (char*)logMessageFormat.data(); }
	int		SetLogValues(  char *format, ... );
	int		buildLogText ( va_list ap );
	
	void GenerateKey( void );
	void SetLogDate( char *p ) { logDate = p;}
	void SetLogDate( void ) {
		char now[18];
		TIME_today( now,3);
		logDate = now;
	}
	void SetIccModuleNumber( int i )		{ iccModuleNumber = i;}
	void SetIccMessageNumber( int i )		{ iccMessageNumber = i;}
	void SetLogThisEvent( int i )			{ logThisEvent= i;}
	void SetLogType( int i )				{ logType = i;}
	void SetLogDestination( int i )			{ logDestination = i;}
	void SetLogMessageTag( const char *p )	{ logMessageTag = p;}
	void SetLogMessageText( const char *p )	{ logMessageText = p;}
	void SetLogMessageFormat( char *p )		{ logMessageFormat = p;}
	void SetLogMessageValues( char *p )		{ logMessageValues = p;}
	void SetLogProfile( int i )				{ logProfile = i;}
	void SetSignThisEvent( int i )			{ signThisEvent = i;}
	void SetArgList( char *p )				{ argList = p; }
	void SetLogHandler( int i )				{ logHandler = i;}	
	void SetNotifyDefKey(long notifyDefKey) { _notifyDefKey = notifyDefKey; }

	RWCString	GetKey( void )					{ return key; }
	RWCString	GetLogDate( void )				{ return logDate;}
	RWInteger	GetIccModuleNumber( void )		{ return iccModuleNumber;}
	RWInteger	GetIccMessageNumber( void )		{ return iccMessageNumber;}
	RWInteger	GetLogThisEvent( void )			{ return logThisEvent;}
	RWInteger	GetLogType( void )				{ return logType;}
	RWInteger	GetLogDestination( void )		{ return logDestination;}
	const RWCString&	GetLogMessageTag( void ) const	{ return logMessageTag;}
	const RWCString&	GetLogMessageText( void ) const	{ return logMessageText;}
	const RWCString&	GetLogMessageFormat( void )	const { return logMessageFormat;}
	RWCString	GetLogMessageValues( void )		{ return logMessageValues;}
	RWInteger	GetLogProfile( void )			{ return logProfile;}
	RWInteger	GetSignThisEvent( void )		{ return signThisEvent;}
	RWCString	GetArgList( void )				{ return argList;}
	RWInteger	GetLogHandler( void )			{ return logHandler;}
	long GetNotifyDefKey() { return _notifyDefKey; }
	char		*debug( void );

	// Overloading RWColleatable member functions
	virtual int			compareTo( const RWCollectable *) const;
	virtual RWBoolean	isEqual( const RWCollectable *) const;
	
private:

	RWCString		key;

	RWCString		logDate;
	RWInteger		iccModuleNumber;
	RWInteger		iccMessageNumber;
	
	RWInteger		logThisEvent;
	RWInteger		logDestination;
	RWInteger		logType;
	
	RWCString		logMessageTag;
	RWCString		logMessageText;
	RWCString		logMessageFormat;
	RWCString		logMessageValues;
	
	RWInteger		logProfile;
	RWInteger		signThisEvent;

	RWCString		argList;
	
	RWInteger		logHandler;
	
	long _notifyDefKey;
};

#define PDLOGEXTMESSAGEGuts( x,op)\
{\
	x op key;\
	x op iccModuleNumber;\
	x op iccMessageNumber;\
	x op logThisEvent;\
	x op logDestination;\
	x op logType;\
	x op logMessageText;\
	x op logMessageFormat;\
	x op logMessageValues;\
	x op logProfile;\
	x op signThisEvent;\
	x op logHandler;\
}

#endif
