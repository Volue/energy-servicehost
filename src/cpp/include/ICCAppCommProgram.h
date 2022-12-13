#ifndef _ICCAppCommProgram_h_
#define _ICCAppCommProgram_h_
/*
$Header$
Description:
	class ICCAppCommProgram
	Purpose:	Extend the ICCAppCommInterface class to provide an adressing 
				mechanism that allows the application program to:
				
				o	Start another program and get a handle that can
					be used to send messages to that specific program.
					
				o	Keep (started) programs alive, (when needed or
					all times). This is needed because the user can
					stop the started program and cause the 'main' program
					to malfunction.
					
	Usage:		This class is used internally by the ICCAppCommInterface
				to implement the functionality as described above.
				ICCAppCommInterface icc;
				..
				ICCAppCommProgram *tsreport=icc.createProgram("tsreport");
				..
				icc.sendMessage("xxx",tsreport);
	Other info:
				o	Ref. class ICCAppCommInterface
				o	Author first version, SiH
___________________________
Copyright (C) Powel Data AS
All rights reserved.
*/
#include <rw/cstring.h>
#include <CBBase.h>
#include <PDCollectable.h>
#include <icc.h>


class ICCAppCommInterface;
class ICCAppCommProgram;
#ifdef WIN32
class PDCom;
struct IPDCom;
#endif
struct ICCAppCommMsgCB { /*
	The one and only purpose of this class is to support a callback argument
	that contains both the message and the sender of the message in one
	argument. This could have been solved using a queryMsg() when receiving
	a callback, however, that would require another member variable in the
	class.
	Alternatively, we could create a callback base class with two arguments,
	like CBBase<arg1,arg2>
	*/
	ICCAppCommMsgCB(ICCAppCommProgram *s,PDCollectable *m) {sender=s;msg=m;}
	PDCollectable *msg;
	ICCAppCommProgram *sender;
};

class ICCAppCommProgram {
public:
	ICCAppCommProgram();
	~ICCAppCommProgram();
	RWBoolean operator ==(const ICCAppCommProgram &x)const;
	//-- sending messages
	RWBoolean sendMsg(PDCollectable& x); // new & preferred method
	RWBoolean sendCommand(RWCString cmdMsg);
	RWBoolean sendCommand(RWCString cmdMsg, long tag, int tag_type, long client_data);
	RWBoolean sendMsg(TXY_SERIES *x);
	RWBoolean sendMsg(STRINGS *x);
	RWBoolean sendMsg(ACK *x);
	RWBoolean sendMsg(NAK *x);
	RWBoolean sendMsg(MULTI_MSG *x);
	RWBoolean sendMsg(COMMAND *x);
	RWBoolean sendMsg(SYNC *x);

	//-- more sendXXXX methods for old messages like TXY SYNC goes here
	RWBoolean waitForSync(RWBoolean justSend=FALSE);
	//-- event handling, receive a CB when something happens
	void uponMsgReceived(CBBase<ICCAppCommMsgCB&> *cb);
	void uponSyncReceived(CBBase<ICCAppCommProgram*> *cb);
	void uponProgramStopped(CBBase<ICCAppCommProgram*> *cb);
	void uponProgramFailedToStart(CBBase<ICCAppCommProgram*> *cb);
	//-- diagnostics when a method returns false, use this to get diag
	RWCString getLastError() { return lastError;}
	RWBoolean isStopped();// returns true if server has stopped
protected: 
	void syncReceived();// - when SYNC type message received
	void stopped(); // -called when program stops/crashes
	void failedToStart();// -called when failing to start the program 
	void msgReceived(PDCollectable *msg);
	RWBoolean keepAlive;//-- if program stops, start it (according to startrule below)
	
#ifndef WIN32
	public:
	int getgpid() { return gpid;}
protected:
	volatile int	gpid; //-- identify remote program:ref. nvoy, icc/api/icc/src/*.c/*.h
	volatile int admState;//-- the lastknown state of the remote program
	RWCString startCommand;
	RWBoolean startImmediate;//-- when to start the program
	RWBoolean useSetProt; //-- to control the use of icc_setprot, clients only
#else
	PDCom &clientConnection; // route all message through here 
	void *serverObject;	// PDComClient_ If we started the program,  this is 
						// the client side object, 
						// that has a ptr to the server object.
	const void *serverClsId;   // the class factory for the server side
	friend void serverShutDown(void *vptrToICCAppCommProgram);// called when serverObjec do unexpected exits
public:
	void * getServerObject() const { return serverObject;}// PDComClient_
	RWBoolean isEqualInterface(IPDCom *x);
protected:
#endif
	RWBoolean serverOk();// check and if necessary start it
	ICCAppCommInterface *iccApp; //-- ref. to central msg-handler/dispatcher
	RWCString lastError;
	RWBoolean setError(RWCString msg) { lastError=msg;return FALSE;}
	RWBoolean waitFlag;
	CBBase<ICCAppCommProgram*> *stoppedCB;
	CBBase<ICCAppCommProgram*> *syncCB;
	CBBase<ICCAppCommProgram*>  *failedToStartCB;
	CBBase<ICCAppCommMsgCB&>     *msgCB;
	friend class ICCAppCommInterface;
};

#endif
