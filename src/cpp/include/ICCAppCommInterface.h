#ifndef _ICCAppCommInterface_h_
#define _ICCAppCommInterface_h_
/*
Description:
	class ICCAppCommInterface

	Purpose: 	Simplify the interface to the ICC system, that is; starting applications
				sending and receiving commands
				
	Usage:		ONE instance of the ICCAppCommInterface object should be created
				somewhere early in the program. If using ILOG, the Application
				object could contain ICCAppCommInterface icc; as a member.
				Below follows some examples on how to use this class:
				
			o	Construction example:
				class MyApplication:public IlvApplication {
					public:
					void makePanels() {
						icc.setDisplay(getDisplay());
						if(icc.open(argc,argv) ) {
							// some kind of error
						}
					}
					protected:
					IlvAppCommInterface icc;
				}
			o  icc.open(argc,argv,programid)
			o when ready to dispatch messages on timer tick
				icc.activateAutoDispatch().
			o	To start an application:
					icc.startProgram("tsreport","<program args>");
			o	To send a message to the default application:
					icc.sendCommand("Hello");
			o	To send a message to the ICC message window:
					icc.sendLogMessage("<Some message>");
			o	To get a callback to your class MikMak when receiving a specific command:
					class MikMak ...  {
					public:
						void cmdMsg(RWCString theMessage) {
						}
						void close(RWCString optionalArgs) {
						}
					};
					MikMak mikMak;
					icc.uponCommandString("CLOSE", new CBX<RWCString,MikMak>(&mikMak,close) );
			o	To reply on the last received command message:
					icc.replyCommand("Your reply");
			o	To get a callback for ALL command messages:
					icc.uponCommandMsg( new CBX<RWCString,MikMak>(&mikMak,cmdMsg) );
				(This callback is called before the individual command strings callbacks)
			o	To close the ICC interface:
					icc.close()
					or delete icc
	Compiling&linking:
				Include: ICCAppCommInterface.h
				Include Paths: icc/api/ICC icc/api/CB (+ ilog views and rw)
				Libraries: -L icc/api/ICC -lICC (+ ilog views and rwtools)
	Other info:
			o	In order to run without ICC, use -Z -l command line options
			o	The implementation uses IlvTimer, 0.1 second poll rate for
				checking incoming messages.
			o	Example of use: icc/mvs/src/tspanel/app/
			o	Author first version, SiH

Copyright (C) Powel Data AS
All rights reserved.
*/
#include <rw/cstring.h>
#include <rw/tvsldict.h>
#include <rw/tpslist.h>
#include <rw/tvslist.h>
#include <rw/tvdlist.h>
#include <CBBase.h>
#include <ICCAppCommProgram.h>
//class IlvTimer;
//class IlvDisplay;
class ICCTimer;
typedef void (*ICCTimerProc) (ICCTimer *t,void *usrArg);
class ICCTimer { /*
	We need a timer to poll the msgqueue, but we dont want to link it to a specific 
	gui-library.
	The tactic is to provide a dummy class that can be overridden by a subclass
	in a library that takes the gui-components into account, e.g. IlogViews.
	*/
public:
	virtual void setProc( ICCTimerProc, void * usrCbArg );
	virtual void run();
	virtual void run(int secs, int millis=0);
	virtual void suspend();
	//-- system interface
	virtual long getAppInstance(); // for some reason it ended here..
	virtual void exit(long res);
	virtual void processEvents();
};
#ifndef WIN32
extern "C" {
#include <icc.h>
#include <icc_debug.h>
#include <icc_api.h>
#include <icc_adm.h>
int icc_adm_msg(int msg_type, void *data);
};
#define defineProgramId(x) static int any_##x
#else
#define COM_NO_WINDOWS_H
// NB: COM_NO_WINDOWS_H makes PDCom_i.h not include windows.h directly, but it always
// includes rpc.h which pulls in windows.h anyways (at least in VS2015)!
#ifndef STRICT
#define STRICT // Enable STRICT Type Checking in Windows headers
#endif
// Seems use of PDCom_i.h does not compile with WIN32_LEAN_AND_MEAN.
//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN // To speed the build process exclude rarely-used services from Windows headers
//#endif
#ifndef NOMINMAX
#define NOMINMAX // Exclude min/max macros from Windows header
#endif
#include "../PDCom/PDCom_i.h"
#include "../PDCom/allpdcomrc.h"
#define defineProgramId(x) extern "C" {\
CLSID pdcom_server_clsid=CLSID_##x; \
int pdcom_server_registry=IDR_##x;\
};
#endif

typedef RWTValDlist<ICCAppCommMsgCB> ICCMsgQueue;
typedef RWTValDlistIterator<ICCAppCommMsgCB> ICCMsgQueueIterator;
class IccMsgTypeData {
public:
	int type;
	void* data;
};

class ICCAppCommInterface {
	// prohibit use
	ICCAppCommInterface(const ICCAppCommInterface&){};
	ICCAppCommInterface& operator=(const ICCAppCommInterface&rhs) { return *this;}
public:
	ICCAppCommInterface();
	virtual      ~ICCAppCommInterface();
	
	//-- open/close connection to ICC
	virtual RWBoolean  open(int argc,char **argv,int P_XXXX=P_UIS);// return 0 =ok, other failure
	virtual void activateAutoDispatch(int on=1,int ms=100); // turn timerbased dispatching of messages on/off
	virtual void close(int nvoyClose=1) ;
	void setDisplay(void *displ);
	void* getDisplay() { return display;}
	
	//-- starting programs, sending messages
	RWBoolean restart(ICCAppCommProgram *remoteProgram);
	virtual ICCAppCommProgram* startProgram(RWCString progname, RWCString args, int keepAlive=1 ,int startNow=1);// return 0: ok, otherwise error
	virtual RWBoolean sendCommand(RWCString cmd,ICCAppCommProgram *remoteProgram);
	virtual RWBoolean waitForSync(ICCAppCommProgram* program,RWBoolean justSend=0);
	virtual RWBoolean replyCommand(RWCString command) { return lastSender? lastSender->sendCommand(command):FALSE ;}
	virtual RWBoolean sendLogMessage(RWCString msg,int level=LOG_INFORMATION,int outputTo=OUT_WINDOW);
	RWBoolean waitForFlagTrue(volatile RWBoolean &waitFlag);
	//-- diagnostics when something returns FALSE
	RWCString getLastError() { return lastError;}
	//-- receiving callbacks when receiving messages
	ICCAppCommProgram *getLastSender() { return lastSender;}
	void uponMsg(CBBase<ICCAppCommMsgCB&> *cb) {msgCB=cb;}
	void uponAdmMsg(CBBase<ADM_CALLBACK*> *cb) {admMsgCB=cb;}
	void uponCommandMsg(CBBase<RWCString> *cb) {commandMsgCB=cb;}
	void uponOtherMsg(CBBase<IccMsgTypeData*> *cb) {otherMsgCB=cb;}
	void uponCommandString(RWCString cmd, CBBase<RWCString> *cb) ;
	//-- debugging the interface
	void setDebugLevel(int x) { debugLevel=x;}
	virtual void processMessages(ICCAppCommProgram *fromSenderOnly=0);
	
protected:
	friend class ICCAppCommProgram;
	void queueMessages(ICCMsgQueue& q);
	void registerProgram(ICCAppCommProgram *p) { if( !program.contains(p)) program.append(p);}
	//-- utility functions
	virtual void showCommandInterfaceCmd(RWCString optionList);// displays available commands
	virtual void setDebugLevelCmd(RWCString optionList);
	int debugLevel;// 0: no debug, >0 more debug
	//-- internals:
	virtual void execCommandString(RWCString cmdReceived) ;// exec std::string callbacks if match
	virtual int  isOpen() { return _isopen; }
	virtual void doArgList(int argc, char **argv);
	typedef CBBase<RWCString> * CBBaseRWCStringPtr;
	RWTValSlistDictionary<RWCString,CBBaseRWCStringPtr > commandStringMap; // maps cmdStrings with callback
	CBBase<RWCString> *commandMsgCB;
	CBBase<ADM_CALLBACK*> *admMsgCB;
	CBBase<IccMsgTypeData*> *otherMsgCB;
	CBBase<ICCAppCommMsgCB&> *msgCB;// route all messages through this cb
	void *display; // needed when creating IlvTimer..
	int use_nvoy;  // used during startup to signal no nvoy connection (-l option)
	//ICCTimer *iccPollTimer;     // used to check for messages
	int defaultMsgTarget; // last process created is default target
	ICCMsgQueue msgQueue; // used to queue other messages when reading messages from a specific program
	static  void timerCallback(ICCTimer *t,void* p_ICCAppCommInterface);
	RWCString keepAliveString;
	ICCAppCommProgram *findProgram(int gpid);
	RWTPtrSlist<ICCAppCommProgram> program;//-- a list over remote programs
	ICCAppCommProgram* startingProgram; //-- points to the program we tries to start
	ICCAppCommProgram* lastSender;//-- processMessages sets this before doing any msghandler callbacks
								  //-- such that replyTo works.
	RWCString lastError;
	int _isopen;// True if active interface
	RWBoolean setError( RWCString err) { lastError=err; return FALSE;}
#ifdef WIN32
	RWBoolean moduleInitDone; // TRUE if Com module is initialized (needs shutdown at exit)
	typedef const void *CLSIDPtr;
	RWTValSlistDictionary<RWCString, CLSIDPtr > clsidMap; // maps cmdStrings with clsids
#endif
public:// not really..
	virtual int admCallback(int msgType, void *msgData);
};

#endif
