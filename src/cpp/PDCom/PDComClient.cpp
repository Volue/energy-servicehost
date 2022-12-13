/*
$Header$
Description:
	Implements the PDComClient class, ref. .h-file
	
Copyright (C) Powel Data AS
All rights reserved.
*/
#include <PrePDCom.h>

#include <PDComClient.h>
#include <PDComMsgQ.h>

extern int exitLockCount;
PDComClient::PDComClient() {
    cookie=0;msgQ=0;exitCB=0;exitArg=0;
    detachInProgress=false;
}

PDComClient::~PDComClient() {
    if(server) detach();
}
  ULONG PDComClient::InternalRelease() { //-- catch up serverside shutdown through this function.
      ULONG refCount= CComObjectRoot::InternalRelease();
      if(refCount ==1 ) { // server called Release
          if(!detachInProgress) { // some kind of errorhandling to avoid using dead comptr
              IPDCom ** p= &server;// finish our side, may detatch is an even better solution
              *p=0;
          }
          // but that is not so easy to guess since this is the server initiated
          // shutdown
          if(exitCB) { (*exitCB)(exitArg);exitCB=0;exitArg=0;} // notify level above
      }
      if(!detachInProgress) {
          if(exitLockCount>0) exitLockCount--;
      }
      return refCount;
  }

HRESULT STDMETHODCALLTYPE 
PDComClient::SendMsg( /* [in] */ BSTR stringMsg,
                      /* [in] */ DWORD cpid) {
    HRESULT res=S_OK;
    char* msgPtr= (char*)stringMsg;
    int msgSize=SysStringByteLen(stringMsg);
    char *localMsg= new char[msgSize];
    memcpy(localMsg,msgPtr,msgSize);
    if(msgQ) msgQ->addMsg(localMsg,msgSize,PDCom(server.p,cookie));// Reply back via this interface
    else if(globalMsgQ) globalMsgQ->addMsg(localMsg,msgSize,PDCom(server.p,cookie));
    return res;
}

void PDComClient::setMsgQ(PDComMsgQ *m) {
    msgQ=m;
}
int  PDComClient::attach( const CLSID &clsid) { // attach to server exposing clsid, get IID_IPDCom interface
    //HRESULT hres=CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IPDCom, (void**)&server);
    IClassFactory *pCF;
    HRESULT res=!S_OK;
    try {
		res = CoGetClassObject(clsid, CLSCTX_ALL, NULL, IID_IClassFactory,(void**) &pCF); 
    } catch ( ... ) {
		server=0;cookie=0;
	}

    if( res == S_OK) {
	    try {
        res = pCF->CreateInstance(NULL, IID_IPDCom,(void**)&server) ;
	    }  catch ( ... ) {
			server=0;cookie=0;
		}
        pCF->Release(); 
    }
    if( server ==0) return 0;
    try {
        detachInProgress=true;// such that internal release know its clientside action
		res= AtlAdvise(server,GetUnknown(),IID_IPDCom,&cookie);
		detachInProgress=false;
		if(FAILED(res)) {
			server=0;
			cookie=0;
			return 0;
		}
		exitLockCount++;
	    return 1;
    } catch ( ... ) {
		server=0;cookie=0;
        detachInProgress=false;
	}
	return 0;
}
int  PDComClient::detach() {
	try {
		if(server != 0) {
			if(cookie) {
                if(exitLockCount>0) exitLockCount--;
                detachInProgress=true;
				HRESULT res=AtlUnadvise(server,IID_IPDCom,cookie);
				cookie=0;
				detachInProgress=false;
				if(SUCCEEDED(res)) {
					server=0;
					return 1;
				}
				IPDCom **p=&server;*p=0;
			} else { //--
				IPDCom **p=&server;*p=0;
				return 1;
			}
		}
	}  catch ( ... ) {
		IPDCom **p=&server;*p=0;
		cookie=0;
		detachInProgress=false;
	}
	return 0;
}
//int PDComClient::sendCmdMsg(const char *msg,int sz) {// Send a message to server   
//    PDCom x(server.p,cookie);
//    return x.sendCmdMsg(msg,sz);
//}
int PDComClient::sendMsg(PDCollectable *msg) {
	PDCom x(server.p,cookie);
	return x.sendMsg(msg);
}

