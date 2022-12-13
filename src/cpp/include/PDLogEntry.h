/*
Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef _PDLogEntry_h_
#define _PDLogEntry_h_
#include <stdarg.h>
#include <PDLogHeader.h>
#include <PDLogMessage.h>

#include <dig_h/TBIccApp.h>
#include <dig_h/TBIccAppl.h>
#include <dig_h/TBProgram.h>

#include <rw/slistcol.h>
#include <rw/cstring.h>

#include <PDLogCollMessage.h>
#include <PDTime.h>
#include "ActivityLogDefinitions.h"

//---- PDLogEntry -----------------------------------------------------------
class ICCAppCommProgram;

class PDLogEntry
{
public:

	PDLogEntry( ICCAppCommProgram *_sender, PDLogHeader *_pdLogHeader );
	~PDLogEntry();

  public:
	int					Initialize( void );
	void				Cleanup( void );
	PDLogHeader			*GetLogHeader( void ) { return &pdLogHeader;}
	int					AddLogMessage( PDLogMessage *_logMessage );
	int					AddLogMessage( PDLogExtMessage *_logMessage );
	int					LogToFile( PDLogMessage *_logMessage );
	int					LogToFile( PDLogExtMessage *_logMessage );
	int					LogToSystemLog( PDLogExtMessage *_logMessage );
	ICCAppCommProgram	*GetSender( void ) { return sender;}
	int					StoreMessages( void );
	char				*CreateLogFile( void );
	int					ResetMessages( void );
	int					Stop( void );
	void				ResetErrorCondition( void ) { errorCondition = 0;}
	int					GetErrorCondition( void ) { return errorCondition;}
	int					SetErrorCondition( const int errorCondition ) { 
							int tmp=errorCondition;
							this->errorCondition = errorCondition;
							return tmp;
						}
	PDLogCollMessage	*GetApplicationLog( RWCString where );
	PDLogMessage		*GetInternalError( void ) { return &internalError ;}
	long				GetLogId(void);
	RWCString			GetApplicationName( void ) { return pdLogHeader.GetApplicationName();}

	int					isUsingOwnLogKey( void ) { return useLogKey == 0 ? TRUE:FALSE; }

  private:

	char 				*getMessageType( const AL_MESSAGE_TYPE messageType, const char *date );
	char				*GetDate( const char *date ) const {
							static char newDate[32];
							TIME_formdate( date, dateFormat, newDate );
							return (char*)newDate;
						}

	void				checkLogFile( void );
	void				openFile( void );
	
	static PDTime			nextDay;
	static int				fileHandle;
	static char*			dateFormat;

	bool					firstTime;

	RWCString				logFileName;

	ICCAppCommProgram		*sender;

	AL_HANDLER_TYPE			handler;

	int 					errorCondition;

	PDLogHeader 			pdLogHeader;
	RWSlistCollectables 	logMessages;

	PDLogMessage			internalError;
	TBIccApp				IccApplication;
	TBIccAppl				IccApplicationLog;

	char 					buffer[2048];
	char 					newDate[32];

	TBProgram				*program;
	int						noLog;
	int						noAppLog;
	int						noUserLog;

	long					useLogKey;
	long					logKeyOwner;
};
#endif	/* ifndef _PDLogEntry_h_     */
