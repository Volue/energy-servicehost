/*
Description:
	The PDComMsqQ implements the message queue mechanism needed for
	queueint incoming messages in a MT-safe manner.
	The message queue is implementet by means of the
	RW class library.


Copyright (C) Powel Data AS
All rights reserved.
*/
#pragma once
#ifndef _PDComMsgQ_h_
#define _PDComMsgQ_h_

#include <PDCom.h> 
// forward decl

//---- PDComMsgQ -----------------------------------------------------------
class PDComMsqQItem {
  public:
    PDComMsqQItem();
    PDComMsqQItem(const char* msgBufferA,unsigned int msgSizeA,const PDCom& replyToA);
    const char *msgBuffer;
    unsigned int msgSize;
    PDCom replyTo;
};
inline PDComMsqQItem::PDComMsqQItem() {msgBuffer=0;msgSize=0;};
inline PDComMsqQItem::PDComMsqQItem(const char* msgBufferA,unsigned int msgSizeA,const PDCom& replyToA) {
        msgBuffer=msgBufferA;
        msgSize=msgSizeA;
        replyTo=replyToA;
};
class PDComDisconnectMsg : public PDCollectable {
  public:
  PDComDisconnectMsg() {};
  ~PDComDisconnectMsg() {};
   virtual void saveGuts(RWvostream&) const;
   virtual void restoreGuts(RWvistream&);

   PDDECLARE_COLLECTABLE(PDComDisconnectMsg,PDCollectable)
};
#include <rw/tvslist.h>
//#include <atlbase.h>

class PDComMsgQ {
public:
    PDComMsgQ();
    ~PDComMsgQ();
    void addMsg(const char* msg,unsigned int sz,const PDCom& replyTo);
    int  getMsg(const char*& msg,unsigned int& sz,PDCom& replyTo);
    void clientDisconnected(PDCom& replyTo);
    PDCollectable* getMsg(PDCom& replyTo);
    int entries();
    void setServerThread(DWORD threadId) {serverThread=threadId;}
    DWORD getServerThread() {return serverThread;}
protected:
    RWTValSlist< PDComMsqQItem > msqQ;
    CComAutoCriticalSection msqQSection;
    DWORD serverThread; // thread id of the server process
    void lock();
    void unlock();
    
};
extern PDComMsgQ *globalMsgQ; // There is ONE global message queue

#endif
