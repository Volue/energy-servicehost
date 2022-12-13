/*
Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDTXYMsg_
#define _PDTXYMsg_

#if __GNUG__ >= 2
#  pragma interface
#endif
extern "C"
{
#include <TXY_api.h>
}
#include <PDCollectable.h>
#include <rw/cstring.h>
#include <PDBASEMsg.h>

class PDTXYMsg: public PDBASEMsg {
public:
	PDTXYMsg();
	~PDTXYMsg();
	virtual void *getMsg() { return _txy;}
	void setMsg(void *p);
	PDDECLARE_COLLECTABLE(PDTXYMsg,PDBASEMsg)
	virtual RWspace binaryStoreSize() const;
	virtual void restoreGuts(RWvistream&);
	virtual void restoreGuts(RWFile&);
	virtual void saveGuts(RWvostream&) const;
	virtual void saveGuts(RWFile&) const;
	void *generateTestGuts(int n=0);
	void printGuts(void *p,int n=0);
	virtual PDMESSAGETYPE getMessageType(){return PDTXYMsgType;}
	virtual char* getClassName(){return "PDTXYMsg";}
	virtual long getMsgTag() { return (_txy ) ? _txy->tag:0;}
	virtual int getMsgTagType() {return (_txy ) ?_txy->tag_type:0;}
	virtual long getMsgClientData() {return (_txy ) ?_txy->client_data:0;}
	virtual void setMsgInfo(long tag, int tag_type, long client_data) {
		_txy->tag = tag;
		_txy->tag_type = tag_type;
		_txy->client_data = client_data;
	}
protected:
	TXY_SERIES *_txy;
};

#define txyGuts(x,op,y,z)\
{\
	x op _txy->tag;\
	x op _txy->tag_type;\
	x op _txy->client_data;\
	x op _txy->kind;\
	strGuts(x,op,_txy->name,z);\
	strGuts(x,op,_txy->text,z);\
	strGuts(x,op,_txy->start_time,z);\
	strGuts(x,op,_txy->__padding_6__,z);\
	strGuts(x,op,_txy->__padding_18__,z);\
	x op _txy->id;\
	x op _txy->group;\
	x op _txy->x_unit;\
	x op _txy->x_step;\
	x op _txy->ref;\
	x op _txy->val_unit;\
	x op _txy->data_type;\
	x op _txy->number_of_missing_data;\
	x op _txy->update;\
	x op _txy->period;\
	x op _txy->number;\
	x op _txy->flags;\
	x op _txy->max_y;\
	x op _txy->min_y;\
	x op _txy->origin;\
	x op _txy->vlcode;\
	x op _txy->producer;\
	strGuts(x,op,_txy->x_format,z);\
	strGuts(x,op,_txy->y_format,z);\
	int num_values=_txy->num_values;\
	x op num_values;\
	y;\
	_txy->num_values=num_values;\
	int j;\
	for (j = 0; j < _txy->num_values; j++)\
	{\
	x op _txy->values[j];\
	x op (TXY_STATUS(_txy,j));\
	}\
}
#endif
