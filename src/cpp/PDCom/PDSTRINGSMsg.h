/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDSTRINGSMsg_
#define _PDSTRINGSMsg_

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

//---- PDSTRINGSMsg -----------------------------------------------------------

class PDSTRINGSMsg: public PDBASEMsg {
  public:
    PDSTRINGSMsg();
    ~PDSTRINGSMsg();
    virtual void *getMsg() { return _strings;}
    void setMsg(void *p);
    PDDECLARE_COLLECTABLE(PDSTRINGSMsg,PDCollectable)
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    void *generateTestGuts(int n=0);
    void printGuts(void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType(){return PDSTRINGSMsgType;}
    virtual char* getClassName(){return "PDSTRINGSMsg";}
	virtual long getMsgTag() { return (_strings ) ? _strings->tag:0;}
	virtual int getMsgTagType() {return (_strings ) ?_strings->tag_type:0;}
	virtual long getMsgClientData() {return (_strings ) ?_strings->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_strings->tag = tag;
		_strings->tag_type = tag_type;
		_strings->client_data = client_data;
	}
  protected:
	STRINGS *_strings;
};
		
#define STRINGSGuts(x,op,y,z)\
{\
	x op _strings->tag;\
	x op _strings->tag_type;\
	x op _strings->client_data;\
	x op _strings->sender_id;\
	x op _strings->sender_inst;\
	x op _strings->kind;\
	x op _strings->s_length;\
	x op _strings->delimiter;\
	strGuts(x,op,_strings->__padding_7__,z);\
	x op _strings->num_values;\
	y;\
	strGuts(x,op,_strings->strings,z);\
}
#endif
