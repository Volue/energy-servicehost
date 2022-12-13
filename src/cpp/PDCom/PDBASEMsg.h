/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDBASEMsg_
#define _PDBASEMsg_

#if __GNUG__ >= 2
#  pragma interface
#endif


#include <PDCollectable.h>
#include <rw/cstring.h>
#include <stdlib.h>
#include <malloc.h>


enum PDMESSAGETYPE{PDBASEMsgType,
					PDMULTIMsgType,
					PDTXYMsgType,
					PDCOMMANDMsgType,
					PDACKMsgType,
					PDNAKMsgType,
					PDSTRINGSMsgType,
					PDSYNCMsgType,
					PDHBVSETTINGSMsgType,
					PDHbvResultInfoMsgType,
					PDHbvSimPeriodMsgType,
					PDHbvSnowValuesMsgType};

//---- PDBASEMsg -----------------------------------------------------------

class PDBASEMsg :public PDCollectable
{
  public:
    PDBASEMsg();
    ~PDBASEMsg();
    virtual void *getMsg() { return NULL;}
    virtual void setMsg(void *dummy);
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    virtual void *generateTestGuts(int n=0);
    virtual void printGuts(void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType(){return PDBASEMsgType;}
    virtual char* getClassName(){return "PDBASEMsg";}
    virtual long getMsgTag() {return 0;}
    virtual int getMsgTagType() {return 0;}
    virtual long getMsgClientData() {return 0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { ; }
    PDDECLARE_COLLECTABLE(PDBASEMsg,PDCollectable)
};
#define strGuts(x,op,y,z)\
{\
	if (z)\
	{ RWCString dummy(y);\
		x op dummy;\
	}\
	else\
	{\
		RWCString dummy;\
		x op dummy;\
		strcpy(y,dummy.data());\
	}\
}
#endif
