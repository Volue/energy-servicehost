/*
Description:
    This class is the callback interface for a PDCom server.
    It resides in the memory-space of the client. Replies from
    the server are directed back to this interface.
    The main purpose of this class is to unpack the message,
    put on a reference to where to send the next
    reply.

Copyright (C) Powel Data AS
All rights reserved.
*/

#ifndef _PDComClient_h_
#define _PDComClient_h_

#pragma once
#ifdef SNIFF_PARSER
#include <atlbase.h>
#include <atlcom.h>
#endif
#ifndef STRICT
#define STRICT // Enable STRICT Type Checking in Windows headers
#endif
// Seems use of PDCom_i.h does not compile with WIN32_LEAN_AND_MEAN.
//#ifndef WIN32_LEAN_AND_MEAN 
//#define WIN32_LEAN_AND_MEAN // To speed the build process exclude rarely-used services from Windows headers
//#endif
#ifndef NOMINMAX
#define NOMINMAX // Exclude min/max macros from Windows header
#endif
#include "PDCom_i.h" // Note: Includes Windows.h
#include "PDCom.h"
class PDComMsgQ;

//---- PDComClient -----------------------------------------------------------

class PDComClient:
 	public IDispatchImpl<IPDCom, &IID_IPDCom, &LIBID_IPDComLib>,  // implements
    //public IPDCom,
	public CComObjectRoot  // a callback connectionpoint interface IPDCom
  {
  public:
	PDComClient();
    ~PDComClient();
    //--
    //  COM stuff goes here --
    BEGIN_COM_MAP(PDComClient)
        COM_INTERFACE_ENTRY(IDispatch)
        COM_INTERFACE_ENTRY(IPDCom)
    END_COM_MAP()
    // IPDCom, the server sends replies back via this method.
    virtual HRESULT STDMETHODCALLTYPE 
           SendMsg( /* [in] */ BSTR stringMsg,/* [in] */ DWORD cpid);
  
	//--
	// Other methods goes here
	_ThreadModel::AutoCriticalSection m_cs;
    void setMsgQ( PDComMsgQ *msqQueue);
    int attach  ( const CLSID &clsid );
    int detach  ();
    void uponServerShutdown( void (*finale)(void *),void  *finaleArg) { exitCB=finale;exitArg=finaleArg;}
    int sendMsg( PDCollectable *msg );
//    int sendCmdMsg(const char *msg,int sz); // Send a message to server 
	IPDCom * getServer() { return server.p;}
	PDCom getPDCom() { return PDCom(server.p,cookie);}  
  protected:
  bool detachInProgress;// true if doing detach()
  PDComMsgQ *msgQ;// Messages received from a server are handed off to msgQ.
  CComPtr< IPDCom >   server;// Reference to the server we are client for
  DWORD cookie; // our connect cookie at the server, sent as a ref
  void  (*exitCB)(void *); // this is called in destructor to notify that the server is finished
  void *exitArg; // this argument is passed with exitCB
  ULONG InternalRelease() ; //-- catch up serverside shutdown through this function.
};

typedef CComObject<PDComClient> PDComClient_;
#endif
