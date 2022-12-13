/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDSYNCMsg_
#define _PDSYNCMsg_

#if __GNUG__ >= 2
#  pragma interface
#endif
extern "C"
{
#ifndef ADM_REASON_PARENT_CRASH
#include <icc_messages.h>
#endif // ADM_REASON_PARENT_CRASH 
}
#include <PDCollectable.h>
#include <rw/cstring.h>
#include <PDBASEMsg.h>

//---- PDSYNCMsg -----------------------------------------------------------

class PDSYNCMsg: public PDBASEMsg {
  public:
    PDSYNCMsg();
    ~PDSYNCMsg();
    virtual void *getMsg() { return _sync;}
    void setMsg(void *p);
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    void *generateTestGuts(int n=0);
    void printGuts(void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType(){return PDSYNCMsgType;}
    virtual char* getClassName(){return "PDSYNCMsg";}
    PDDECLARE_COLLECTABLE(PDSYNCMsg,PDBASEMsg)
		virtual long getMsgTag() { return _sync ?_sync->tag:0;}
	virtual int getMsgTagType() {return _sync ?_sync->tag_type:0;}
	virtual long getMsgClientData() {return _sync ?_sync->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_sync->tag = tag;
		_sync->tag_type = tag_type;
		_sync->client_data = client_data;
	}
  protected:
	SYNC *_sync;
};
		
#define SYNCGuts(x,op)\
{\
	x op _sync->tag;\
	x op _sync->tag_type;\
	x op _sync->client_data;\
	x op _sync->status;\
}
#endif
