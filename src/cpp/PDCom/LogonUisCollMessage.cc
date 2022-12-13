/**** File_Header ** -*- C++ -*- **********************************************
 *
 * 	$Id: LogonUisCollMessage.cc 18365 2000-02-01 10:50:24Z sih $
 *
 *	$RCSfile$
 *	Author    : Merete Hjelsvold    PD     DATE: 981125
 *      Copyright : Powel Data AS
 *
 *      Keywords  : 
 *	Contents  : 
 *
 *	Includes and defines:
 */


/*	Function prototypes
 */

/*****************************************************************************/
#include <PrePDCom.h>
#include <LogonUisCollMessage.h>

/*	Externals and statics:
 */


RWDEFINE_NAMED_COLLECTABLE(LogonUisCollMessage, "LogonUisCollMessage");

LogonUisCollMessage::LogonUisCollMessage()
{
}

LogonUisCollMessage::LogonUisCollMessage( const LogonUisCollMessage &ref )
{
    environmentList	= ref.environmentList;
}


LogonUisCollMessage::~LogonUisCollMessage() 
{
}

void LogonUisCollMessage::Add( LogonUisMessage *p)
{
    environmentList.insert(  *p );
}
    
