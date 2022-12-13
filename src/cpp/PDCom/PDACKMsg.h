/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDACKMsg_
#define _PDACKMsg_

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

//---- PDACKMsg -----------------------------------------------------------

class PDACKMsg: public PDBASEMsg {
  public:
    PDACKMsg();
    ~PDACKMsg();
    virtual void *getMsg() { return _ack;}
    void setMsg(void *p);
    PDDECLARE_COLLECTABLE(PDACKMsg,PDBASEMsg)
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    void *generateTestGuts(int n=0);
    void printGuts(void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType(){return PDACKMsgType;}
    virtual char* getClassName(){return "PDACKMsg";}
	virtual long getMsgTag() { return (_ack ) ? _ack->tag:0;}
	virtual int getMsgTagType() {return (_ack ) ?_ack->tag_type:0;}
	virtual long getMsgClientData() {return (_ack ) ?_ack->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_ack->tag = tag;
		_ack->tag_type = tag_type;
		_ack->client_data = client_data;
	}
  protected:
	ACK *_ack;
};
		
#define ACKGuts(x,op)\
{\
	x op _ack->tag;\
	x op _ack->tag_type;\
	x op _ack->client_data;\
	x op _ack->sender_id;\
	x op _ack->sender_inst;\
	x op _ack->retval;\
}
#endif
