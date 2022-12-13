/*
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef  _PDHbvResultInfoMsg_H_
#define _PDHbvResultInfoMsg_H_

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

//---- PDHbvResultInfoMsg -----------------------------------------------------------

class PDHbvResultInfoMsg: public PDBASEMsg {
public:
    PDHbvResultInfoMsg ();
    ~PDHbvResultInfoMsg ();
    virtual void *getMsg () { return _resultInfo;}
    void setMsg (void *p);
    PDDECLARE_COLLECTABLE(PDHbvResultInfoMsg,PDCollectable)
    virtual RWspace binaryStoreSize () const;
    virtual void restoreGuts (RWvistream&);
    virtual void restoreGuts (RWFile&);
    virtual void saveGuts (RWvostream&) const;
    virtual void saveGuts (RWFile&) const;
    void *generateTestGuts (int n=0);
    void printGuts (void *p,int n=0);
    virtual PDMESSAGETYPE getMessageType (){return PDHbvResultInfoMsgType;}
    virtual char* getClassName(){return "PDHbvResultInfoMsg";}
	virtual long getMsgTag() {  return (_resultInfo )?_resultInfo->tag:0;}
	virtual int getMsgTagType() {return (_resultInfo )?_resultInfo->tag_type:0;}
	virtual long getMsgClientData() {return (_resultInfo )?_resultInfo->client_data:0;}
    virtual void setMsgInfo(long tag, int tag_type, long client_data) { 
		_resultInfo->tag = tag;
		_resultInfo->tag_type = tag_type;
		_resultInfo->client_data = client_data;
	}
   
    // BS! Nye funksjoner.    
    RWBoolean operator== (const PDHbvResultInfoMsg& msg) const {return (compareTo(msg) == 0) ? TRUE : FALSE;}
    RWBoolean operator!= (const PDHbvResultInfoMsg& msg) const {return (compareTo(msg) != 0) ? TRUE : FALSE;}
    RWBoolean operator<  (const PDHbvResultInfoMsg& msg) const {return (compareTo(msg) <  0) ? TRUE : FALSE;}
    RWBoolean operator<= (const PDHbvResultInfoMsg& msg) const {return (compareTo(msg) <= 0) ? TRUE : FALSE;}
    RWBoolean operator>  (const PDHbvResultInfoMsg& msg) const {return (compareTo(msg) >  0) ? TRUE : FALSE;}
    RWBoolean operator>= (const PDHbvResultInfoMsg& msg) const {return (compareTo(msg) >= 0) ? TRUE : FALSE;}
    
    RWBoolean clear (void);
    
    
protected:
    HBV_RESULT_INFO* _resultInfo;
    
private:
    // BS! Ny funksjon.    
    int compareTo (const PDHbvResultInfoMsg& msg) const;
    void initiate (void);
};
		
#define HBV_RESULT_INFOGuts(x,op,z)\
{\
	x op _resultInfo->tag;\
	x op _resultInfo->tag_type;\
	x op _resultInfo->client_data;\
	strGuts(x,op,_resultInfo->catchment_name,z);\
	strGuts(x,op,_resultInfo->__padding_3__,z);\
	x op _resultInfo->update_flag;\
	strGuts(x,op,_resultInfo->update_start,z);\
	strGuts(x,op,_resultInfo->update_end,z);\
	x op _resultInfo->update_time_unit;\
	x op _resultInfo->update_time_step;\
	x op _resultInfo->calibration_flag;\
	strGuts(x,op,_resultInfo->calibration_start,z);\
	strGuts(x,op,_resultInfo->calibration_end,z);\
	x op _resultInfo->calibration_time_unit;\
	x op _resultInfo->calibration_time_step;\
	x op _resultInfo->forecast_flag;\
	strGuts(x,op,_resultInfo->forecast_start,z);\
	strGuts(x,op,_resultInfo->forecast_end,z);\
	x op _resultInfo->forecast_time_unit;\
	x op _resultInfo->forecast_time_step;\
	x op _resultInfo->forecast_type;\
	x op _resultInfo->num_met_forecasts;\
	x op _resultInfo->first_hist_year;\
	x op _resultInfo->last_hist_year;\
}
#endif		// _PDHbvResultInfoMsg_H_
