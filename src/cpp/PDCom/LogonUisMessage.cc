/**** File_Header ** -*- C++ -*- **********************************************
 *
 * 	$Id: LogonUisMessage.cc 18365 2000-02-01 10:50:24Z sih $
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


/*	Externals and statics:
 */
#include <PrePDCom.h>
#include <LogonUisMessage.h>

RWDEFINE_NAMED_COLLECTABLE(LogonUisMessage, "LogonUisMessage");

LogonUisMessage::LogonUisMessage() {
	nbEnvVar=0;
}



   
