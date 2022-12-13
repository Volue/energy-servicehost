/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDCOMMANDMsg_
#define _PDCOMMANDMsg_

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

//---- PDCOMMANDMsg -----------------------------------------------------------

class PDCOMMANDMsg: public PDBASEMsg {
  public:
    PDCOMMANDMsg();
    ~PDCOMMANDMsg();
    virtual void *getMsg() { return _command;}
    void setMsg(void *p);
    PDDECLARE_COLLECTABLE(PDCOMMANDMsg,PDBASEMsg)
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    void *generateTestGuts(int n=0);
    void printGuts(void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType(){return PDCOMMANDMsgType;}
    virtual char* getClassName(){return "PDCOMMANDMsg";}
	virtual long getMsgTag() { return (_command ) ? _command->tag:0;}
	virtual int getMsgTagType() {return (_command )?_command->tag_type:0;}
	virtual long getMsgClientData() {return (_command )?_command->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_command->tag = tag;
		_command->tag_type = tag_type;
		_command->client_data = client_data;
	}
    
  protected:
	COMMAND *_command;
};
		
#define COMMANDGuts(x,op,z)\
{\
	x op _command->tag;\
	x op _command->tag_type;\
	x op _command->client_data;\
	strGuts(x,op,_command->command,z);\
}
#endif
