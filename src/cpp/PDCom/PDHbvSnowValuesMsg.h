/*
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDHbvSnowValuesMsg_H_
#define _PDHbvSnowValuesMsg_H_

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
#include <rw/defs.h>
#include <PDBASEMsg.h>

//---- PDHbvSnowValuesMsg -----------------------------------------------------------

class PDHbvSnowValuesMsg: public PDBASEMsg {
public:
    PDHbvSnowValuesMsg ();
    ~PDHbvSnowValuesMsg ();
    virtual void *getMsg () { return _snowValues;}
    void setMsg (void *p);
    PDDECLARE_COLLECTABLE(PDHbvSnowValuesMsg,PDCollectable)
    virtual RWspace binaryStoreSize () const;
    virtual void restoreGuts (RWvistream&);
    virtual void restoreGuts (RWFile&);
    virtual void saveGuts (RWvostream&) const;
    virtual void saveGuts (RWFile&) const;
    void *generateTestGuts (int n=0);
    void printGuts (void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType (){return PDHbvSnowValuesMsgType;}
    virtual char* getClassName(){return "PDHbvSnowValuesMsg";}
	virtual long getMsgTag() { return (_snowValues ) ? _snowValues->tag:0;}
	virtual int getMsgTagType() {return (_snowValues ) ?_snowValues->tag_type:0;}
	virtual long getMsgClientData() {return (_snowValues ) ?_snowValues->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_snowValues->tag = tag;
		_snowValues->tag_type = tag_type;
		_snowValues->client_data = client_data;
	}
    
        // BS! Nye funksjoner.    
    RWBoolean operator== (const PDHbvSnowValuesMsg& msg) const {return (compareTo(msg) == 0) ? TRUE : FALSE;}
    RWBoolean operator!= (const PDHbvSnowValuesMsg& msg) const {return (compareTo(msg) != 0) ? TRUE : FALSE;}
    RWBoolean operator<  (const PDHbvSnowValuesMsg& msg) const {return (compareTo(msg) <  0) ? TRUE : FALSE;}
    RWBoolean operator<= (const PDHbvSnowValuesMsg& msg) const {return (compareTo(msg) <= 0) ? TRUE : FALSE;}
    RWBoolean operator>  (const PDHbvSnowValuesMsg& msg) const {return (compareTo(msg) >  0) ? TRUE : FALSE;}
    RWBoolean operator>= (const PDHbvSnowValuesMsg& msg) const {return (compareTo(msg) >= 0) ? TRUE : FALSE;}
    
    RWBoolean clear (void);

protected:
	HBV_SNOW_VALUES* _snowValues;
	
private:
    // BS! Ny funksjon.    
    int compareTo (const PDHbvSnowValuesMsg& msg) const;
    void initiate (void);
};

//typedef struct HBV_SNOW_VALUES_struct
//{
//
//  long   tag ;
//  int    tag_type ;
//  long   client_data ;
//  char   state_date [ M_TIME_SIZE ] ;
//  char   __padding_3__ [ 2 ] ;
//  int    state_version ;
//  int    no_elevations ;
//  int    period ;
//  char   text [ M_NAME_SIZE ] ;
//  char   __padding_7__ [ 3 ] ;
//  int    num_values 		/* DEFAULT = 1 */;
//  char   __padding_8__ [ 4 ] ;
//  double values [ 1 /****> num_values <****/ ] ;
//
//} HBV_SNOW_VALUES ;
		
#define HBV_SNOW_VALUESGuts(x,op,y,z)\
{\
	x op _snowValues->tag;\
	x op _snowValues->tag_type;\
	x op _snowValues->client_data;\
	strGuts(x,op,_snowValues->state_date,z);\
	strGuts(x,op,_snowValues->__padding_3__,z);\
	x op _snowValues->state_version;\
	x op _snowValues->no_elevations;\
	x op _snowValues->period;\
	strGuts(x,op,_snowValues->text,z);\
	strGuts(x,op,_snowValues->__padding_7__,z);\
	int num_values = _snowValues->num_values;\
	x op num_values;\
	strGuts(x,op,_snowValues->__padding_8__,z);\
	y;\
	_snowValues->num_values = num_values;\
	int j;\
	for (j = 0; j < _snowValues->num_values; j++)\
	{\
	x op _snowValues->values[j];\
	}\
}
#endif		// _PDHbvSnowValuesMsg_H_
