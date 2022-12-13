/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDMULTIMsg_
#define _PDMULTIMsg_

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

//---- PDMULTIMsg -----------------------------------------------------------

class PDMULTIMsg: public PDBASEMsg {
  public:
    PDMULTIMsg();
    ~PDMULTIMsg();
    virtual void *getMsg() { return _multi;}
    void setMsg(void *p);
    PDDECLARE_COLLECTABLE(PDMULTIMsg,PDBASEMsg)
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    void *generateTestGuts(int n=0);
    void printGuts(void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType(){return PDMULTIMsgType;}
    virtual char* getClassName(){return "PDMULTIMsg";}
	virtual long getMsgTag() { return (_multi ) ? _multi->tag:0;}
	virtual int getMsgTagType() {return (_multi ) ?_multi->tag_type:0;}
	virtual long getMsgClientData() {return (_multi ) ?_multi->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_multi->tag = tag;
		_multi->tag_type = tag_type;
		_multi->client_data = client_data;
	}
  protected:
	MULTI_MSG *_multi;
};
		
#define MULTIGuts(x,op)\
{\
	x op _multi->tag;\
	x op _multi->tag_type;\
	x op _multi->client_data;\
	x op _multi->state;\
}
#endif
