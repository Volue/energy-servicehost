/*
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDHbvSimPeriodMsg_H_
#define _PDHbvSimPeriodMsg_H_

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

//---- PDHbvSimPeriodMsg -----------------------------------------------------------

class PDHbvSimPeriodMsg: public PDBASEMsg {
  public:
    PDHbvSimPeriodMsg ();
    ~PDHbvSimPeriodMsg ();
    virtual void *getMsg () { return _simPeriod;}
    void setMsg (void *p);
    PDDECLARE_COLLECTABLE(PDHbvSimPeriodMsg,PDCollectable)
    virtual RWspace binaryStoreSize () const;
    virtual void restoreGuts (RWvistream&);
    virtual void restoreGuts (RWFile&);
    virtual void saveGuts (RWvostream&) const;
    virtual void saveGuts (RWFile&) const;
    void *generateTestGuts (int n=0);
    void printGuts (void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType (){return PDHbvSimPeriodMsgType;}
    virtual char* getClassName(){return "PDHbvSimPeriodMsg";}
	virtual long getMsgTag() { return (_simPeriod )?_simPeriod->tag:0;}
	virtual int getMsgTagType() {return (_simPeriod )?_simPeriod->tag_type:0;}
	virtual long getMsgClientData() {return (_simPeriod )?_simPeriod->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_simPeriod->tag = tag;
		_simPeriod->tag_type = tag_type;
		_simPeriod->client_data = client_data;
	}
   
    // BS! Nye funksjoner.    
    RWBoolean operator== (const PDHbvSimPeriodMsg& msg) const {return (compareTo(msg) == 0) ? TRUE : FALSE;}
    RWBoolean operator!= (const PDHbvSimPeriodMsg& msg) const {return (compareTo(msg) != 0) ? TRUE : FALSE;}
    RWBoolean operator<  (const PDHbvSimPeriodMsg& msg) const {return (compareTo(msg) <  0) ? TRUE : FALSE;}
    RWBoolean operator<= (const PDHbvSimPeriodMsg& msg) const {return (compareTo(msg) <= 0) ? TRUE : FALSE;}
    RWBoolean operator>  (const PDHbvSimPeriodMsg& msg) const {return (compareTo(msg) >  0) ? TRUE : FALSE;}
    RWBoolean operator>= (const PDHbvSimPeriodMsg& msg) const {return (compareTo(msg) >= 0) ? TRUE : FALSE;}
    
    RWBoolean clear (void);
        
protected:
    HBV_SIM_PERIOD* _simPeriod;
    
private:
    // BS! Ny funksjon.    
    int compareTo (const PDHbvSimPeriodMsg& msg) const;
    void initiate (void);
};

#define HBV_SIM_PERIODGuts(x,op,z)\
{\
	x op _simPeriod->tag;\
	x op _simPeriod->tag_type;\
	x op _simPeriod->client_data;\
	strGuts(x,op,_simPeriod->default_start,z);\
	strGuts(x,op,_simPeriod->default_end,z);\
	strGuts(x,op,_simPeriod->upper_time_limit,z);\
	strGuts(x,op,_simPeriod->__padding_5__,z);\
	x op _simPeriod->num_sim_steps;\
	x op _simPeriod->time_unit;\
	x op _simPeriod->step_length;\
}
#endif		// _PDHbvSimPeriodMsg_H_
