/*
$Header$
Description:
    This class is implements a  PDCom server.
    It resides in the memory-space of the Server. 
    The main purpose of this class is to unpack the message,
    put on a reference to where to send the next
    reply.
    Usage:
        One instance pr. server process.
        define somewhere:
        CLSID pdcom_server_clsid= YourServerClassId
        const char * pdcom_server_registry=YourRegistryString
        
    App->ClassId->Interface
    ClassId->App-> Class and then Interface(object).
    

Copyright (C) Powel Data AS
All rights reserved.
*/

#ifndef _PDComServer_h_
#define _PDComServer_h_
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
#include "PDComMsgQ.h"
//---- PDComServer -----------------------------------------------------------
extern "C" {
extern  CLSID pdcom_server_clsid;   // the clsid that can produce PDComServer objects
extern  int   pdcom_server_registry;// registry script that defines clsid - app id relationship
};
class PDComServer:
 	public IDispatchImpl<IPDCom, &IID_IPDCom, &LIBID_IPDComLib>,  // implements
//    public IPDCom,
	public IConnectionPointContainerImpl<PDComServer>,
	public IConnectionPointImpl<PDComServer, &IID_IPDCom, CComDynamicUnkArray >,
	public ISupportErrorInfo,
	public CComObjectRoot,  // a callback connectionpoint interface IPDCom
	public CComCoClass< PDComServer,  &pdcom_server_clsid >
  {
  public:
	PDComServer();
    ~PDComServer();
    //--
    //  COM stuff goes here --
    BEGIN_COM_MAP(PDComServer)
        COM_INTERFACE_ENTRY2(IDispatch,IPDCom)
        COM_INTERFACE_ENTRY(IPDCom)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
        COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
    END_COM_MAP()
    //	DECLARE_NOT_AGGREGATABLE(PDComServer) 
    // Remove the comment from the line above if you don't want your object to 
    // support aggregation.  The default is to support it
    //DECLARE_OBJECT_DESCRIPTION("PDCom Server implementation")
    DECLARE_REGISTRY_RESOURCEID(pdcom_server_registry)

    // Connection Point
    BEGIN_CONNECTION_POINT_MAP(PDComServer)
       CONNECTION_POINT_ENTRY(IID_IPDCom)
    END_CONNECTION_POINT_MAP()
	
    // ISupportsErrorInfo
	virtual HRESULT STDMETHODCALLTYPE 
        InterfaceSupportsErrorInfo(REFIID riid);
    
    // IPDCom
    virtual HRESULT STDMETHODCALLTYPE 
        SendMsg( /* [in] */ BSTR stringMsg,/* [in] */ DWORD cookie);
  
	//--
	virtual HRESULT STDMETHODCALLTYPE Advise(IUnknown* pUnkSink, DWORD* pdwCookie);
	virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwCookie);
	
	// Other methods goes here
	_ThreadModel::AutoCriticalSection m_cs;
    void setMsgQ(PDComMsgQ *m) {msgQ=m;}	
  protected:
    PDComMsgQ *msgQ;// Messages received from a server are handed off to msgQ.
};



#endif
