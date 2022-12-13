/*
$Header$
Description:
	Implements the PDComServer class, ref. .h-file
	
Copyright (C) Powel Data AS
All rights reserved.
*/
#include <PrePDCom.h>
#ifdef WIN32
#include <PDComServer.h>

PDComServer::PDComServer() {
    msgQ=0;
}


PDComServer::~PDComServer() {

}
HRESULT STDMETHODCALLTYPE  PDComServer::InterfaceSupportsErrorInfo(REFIID riid)
{
	if (riid == IID_IPDCom )
		return S_OK;
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE 
PDComServer::SendMsg( /* [in] */ BSTR stringMsg,
                      /* [in] */ DWORD cookie) {
    HRESULT res=S_OK;
    char* msgPtr= (char*)stringMsg;
    int msgSize=SysStringByteLen(stringMsg);
    char *localMsg= new char[msgSize];
    memcpy(localMsg,msgPtr,msgSize);

#if _MSC_VER < 1300
    IPDCom* replyI= cookie?(IPDCom*) _CDV::GetUnknown(cookie /*DWORD dwCookie*/):(IPDCom*)0;
#else
    IPDCom* replyI= cookie?(IPDCom*)m_vec.GetUnknown(cookie /*DWORD dwCookie*/):(IPDCom*)0;
#endif
    // It *really* returns a IPDCom interface (ref. ConnPointImpl->Advise())
    PDCom replyInterface(replyI);//Replying via this hits the PDComClient class SendMsg, that already have a
                                 // pointer to this interface, thus, the cookie is obsolete
    if(msgQ) msgQ->addMsg(localMsg,msgSize,replyInterface);// Reply back via this interface
    else if( globalMsgQ) globalMsgQ->addMsg(localMsg,msgSize,replyInterface);// Use global if no special case
    return res;
}
HRESULT STDMETHODCALLTYPE
PDComServer::Advise(IUnknown* pUnkSink, DWORD* pdwCookie) {
    return IConnectionPointImpl<PDComServer, &IID_IPDCom, CComDynamicUnkArray >::Advise( pUnkSink, pdwCookie);
};
HRESULT STDMETHODCALLTYPE
PDComServer::Unadvise(DWORD cookie) {
 #if _MSC_VER < 1300
    IPDCom* replyI= cookie?(IPDCom*) _CDV::GetUnknown(cookie /*DWORD dwCookie*/):(IPDCom*)0;
#else
    IPDCom* replyI= cookie?(IPDCom*)m_vec.GetUnknown(cookie /*DWORD dwCookie*/):(IPDCom*)0;
#endif
   
    PDCom replyInterface(replyI);
    replyInterface.setDisconnected();
    if(msgQ) msgQ->clientDisconnected(replyInterface);// No Reply back via this interface any more
    else if( globalMsgQ) globalMsgQ->clientDisconnected(replyInterface);// Use global if no special case
    
    return IConnectionPointImpl<PDComServer, &IID_IPDCom, CComDynamicUnkArray >::Unadvise( cookie);
}

#endif
