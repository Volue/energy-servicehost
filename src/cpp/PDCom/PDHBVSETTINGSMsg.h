/*
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDHBVSETTINGSMsg_H_
#define _PDHBVSETTINGSMsg_H_

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

//---- PDHBVSETTINGSMsg -----------------------------------------------------------

class PDHBVSETTINGSMsg: public PDBASEMsg {
public:
    PDHBVSETTINGSMsg ();
    ~PDHBVSETTINGSMsg ();
    virtual void *getMsg () { return _settings;}
    void setMsg (void *p);
    PDDECLARE_COLLECTABLE(PDHBVSETTINGSMsg,PDCollectable)
    virtual RWspace binaryStoreSize () const;
    virtual void restoreGuts (RWvistream&);
    virtual void restoreGuts (RWFile&);
    virtual void saveGuts (RWvostream&) const;
    virtual void saveGuts (RWFile&) const;
    void *generateTestGuts (int n=0);
    void printGuts (void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType (){return PDHBVSETTINGSMsgType;}
    virtual char* getClassName(){return "PDHBVSETTINGSMsg";}
	virtual long getMsgTag() { return (_settings )? _settings->tag:0;}
	virtual int getMsgTagType() {return (_settings )?_settings->tag_type:0;}
	virtual long getMsgClientData() {return (_settings )?_settings->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_settings->tag = tag;
		_settings->tag_type = tag_type;
		_settings->client_data = client_data;
	}

    // BS! Nye funksjoner.    
    RWBoolean operator== (const PDHBVSETTINGSMsg& msg) const {return (compareTo(msg) == 0) ? TRUE : FALSE;}
    RWBoolean operator!= (const PDHBVSETTINGSMsg& msg) const {return (compareTo(msg) != 0) ? TRUE : FALSE;}
    RWBoolean operator<  (const PDHBVSETTINGSMsg& msg) const {return (compareTo(msg) <  0) ? TRUE : FALSE;}
    RWBoolean operator<= (const PDHBVSETTINGSMsg& msg) const {return (compareTo(msg) <= 0) ? TRUE : FALSE;}
    RWBoolean operator>  (const PDHBVSETTINGSMsg& msg) const {return (compareTo(msg) >  0) ? TRUE : FALSE;}
    RWBoolean operator>= (const PDHBVSETTINGSMsg& msg) const {return (compareTo(msg) >= 0) ? TRUE : FALSE;}
    
    RWBoolean clear (void);

protected:
    HBV_SETTINGS* _settings;
	
private:
    // BS! Ny funksjon.    
    int compareTo (const PDHBVSETTINGSMsg& msg) const;
    void initiate (void);
};
		
#define HBV_SETTINGSGuts(x,op,z)\
{\
	x op _settings->tag;\
	x op _settings->tag_type;\
	x op _settings->client_data;\
	x op _settings->settings_type;\
	strGuts(x,op,_settings->catchment_name,z);\
	strGuts(x,op,_settings->__padding_4__,z);\
	x op _settings->catchment_area;\
	strGuts(x,op,_settings->start_simulation,z);\
	strGuts(x,op,_settings->end_simulation,z);\
	x op _settings->start_version;\
	strGuts(x,op,_settings->step_length,z);\
	strGuts(x,op,_settings->__padding_9__,z);\
	x op _settings->simulation_length;\
	strGuts(x,op,_settings->run_type,z);\
	strGuts(x,op,_settings->forecast_type,z);\
	strGuts(x,op,_settings->__padding_12__,z);\
	x op _settings->first_year;\
	x op _settings->last_year;\
	x op _settings->num_of_alternatives;\
	x op _settings->breakpoint;\
}
#endif		// _PDHBVSETTINGSMsg_H_
