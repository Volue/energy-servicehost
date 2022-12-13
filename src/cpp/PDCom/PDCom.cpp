/*
Description:
    PDCom encapsulates a IPDCom interface, makes it easy to 
    send messages to servers.
    How the messages is actually sent is determined inside this
    class.    

Copyright (C) Powel Data AS
All rights reserved.
*/

#include <PrePDCom.h>

#ifdef WIN32
#include <PDCom.h>
#include <strstream>
#include <rw/bstream.h>
#include <rw/pstream.h>
#include <rw/rwerr.h>


using namespace std;
//In case portable streams
//typedef RWpistream PDistream;
//typedef RWpostream PDostream;
typedef RWbistream PDistream;
typedef RWbostream PDostream;

//---- PDCom -----------------------------------------------------------


PDCom::PDCom(IPDCom* ipdComA,DWORD cookieA) {
    ipdCom=ipdComA;cookie=cookieA; disconnected=FALSE;  
}

int PDCom::sendCmdMsg(const char* msg,unsigned int sz) {
    if(ipdCom == 0 || disconnected) return 0;
    BSTR bstrToSend;
    bstrToSend=SysAllocStringByteLen(msg,sz);
    HRESULT hr=-1;
    try {
	   hr=ipdCom->SendMsg(bstrToSend,cookie); // IF ipdCom refers to
        // a server interface, then the cookie refers to the CLIENT interface
        // where the answer should be directed.
        // IF ipdCom refers to a client (i.e. this is a reply), then
        // the client interface KNOW which server it connects
     } catch (...) {
 		ipdCom=0;cookie=0;    
     }   
    SysFreeString(bstrToSend);
    return 1;
}

PDCollectable* PDCom::unpack(const char* rawMsg,unsigned int sz) {
	istrstream msgMemoryImage(rawMsg,sz);
	PDistream msgMemoryImageFormatter(msgMemoryImage);
	RWCollectable *msg=0;
	try {
		msgMemoryImageFormatter>>msg;
	} catch (RWxmsg x) {
		;// just ignore message
	}
	return (PDCollectable*)msg; // dynamic_cast<PDCollectable*>(msg);
}

char* PDCom::pack(PDCollectable* msg,unsigned int& sz) {
	strstream msgMemoryImage;
#ifdef PD_IOSTD	
	msgMemoryImage.rdbuf()->pubsetbuf(0,100000);
#else
	msgMemoryImage.rdbuf()->setbuf(0,100000);
#endif	
	PDostream msgMemoryImageFormatter(msgMemoryImage);
	msgMemoryImageFormatter<<(*msg);
	sz=(unsigned int)msgMemoryImage.pcount(); // NB: Conversion from std::streamsize, which is 64-bits signed integer (__int64) when compiling for x64 and 32-bits signed integer (int) when compiling for x86, to 32-bits unsigned integer (unsigned int) used in calls to underlying COM API.
	return msgMemoryImage.str();
}

int PDCom::sendMsg(PDCollectable* msg) {
    if(ipdCom == 0 || disconnected) return 0;
	unsigned int msgSz=0;
	char* msgStr=pack(msg,msgSz);
	BSTR bstrMsg=SysAllocStringByteLen(msgStr,msgSz);
	HRESULT res=-1;
	try {
		res=ipdCom->SendMsg(bstrMsg,cookie); // IF ipdCom refers to
        // a server interface, then the cookie refers to the CLIENT interface
        // where the answer should be directed.
        // IF ipdCom refers to a client (i.e. this is a reply), then
        // the client interface KNOW which server it connects        
	} catch (...) { //-- catch segm. fault in case server is gone
		ipdCom=0;cookie=0;
	}
    SysFreeString(bstrMsg);
	//delete msgStr;
	free(msgStr);
	return SUCCEEDED(res);		
}
#endif

