/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDNAKMsg_
#define _PDNAKMsg_

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

//---- PDNAKMsg -----------------------------------------------------------

class PDNAKMsg: public PDBASEMsg {
  public:
    PDNAKMsg();
    ~PDNAKMsg();
    virtual void *getMsg() { return _nak;}
    void setMsg(void *p);
    PDDECLARE_COLLECTABLE(PDNAKMsg,PDBASEMsg)
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    void *generateTestGuts(int n=0);
    void printGuts(void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType(){return PDNAKMsgType;}
    virtual char* getClassName(){return "PDNAKMsg";}
	virtual long getMsgTag() { return (_nak )? _nak->tag:0;}
	virtual int getMsgTagType() {return (_nak )?_nak->tag_type:0;}
	virtual long getMsgClientData() {return (_nak )?_nak->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_nak->tag = tag;
		_nak->tag_type = tag_type;
		_nak->client_data = client_data;
	}
  protected:
	NAK *_nak;
};
		
#define NAKGuts(x,op)\
{\
	x op _nak->tag;\
	x op _nak->tag_type;\
	x op _nak->client_data;\
	x op _nak->sender_id;\
	x op _nak->sender_inst;\
	x op _nak->reason;\
}
#endif
