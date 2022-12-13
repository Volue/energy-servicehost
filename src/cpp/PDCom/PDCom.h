/*-*-Mode: C++;-*-
Description:
    PDCom encapsulates a IPDCom interface, makes it easy to 
    send messages to servers. The class also holds the cookie
    that is assigned by the server. This cookie is passed by
    each message such that the server know where to direct
    eventual answers to the request.
    
    How the messages is actually sent is determined inside this
    class.    

	Currently the IIDs for the class factory of the servers are
	keept in this file.
	
Copyright (C) Powel Data AS
All rights reserved.
*/

#pragma once
#ifndef _PDCom_h_
#define _PDCom_h_

//---- PDCom -----------------------------------------------------------
#include <PDCollectable.h>
struct IPDCom;
class PDCom  {
public:
    PDCom(IPDCom* ipdCom=0,DWORD cpid=0);
    int sendCmdMsg(const char* msg,unsigned int sz);
    int sendMsg(PDCollectable* msg);
    static PDCollectable* unpack(const char* rawMsg,unsigned int sz);
    static char* pack(PDCollectable* msg,unsigned int& sz);
    IPDCom* getInterface() {return ipdCom;}
    void setInterface(IPDCom* x) {ipdCom=x;}
    void setDisconnected() {disconnected=TRUE;}
    BOOL isDisconnected() {return disconnected;}
protected:
    IPDCom* ipdCom;// interface of a client or server implementation
    DWORD cookie;  // if talking to a server interface, reply via this connpoint id
    BOOL disconnected;// if disconnected (before message processed)
};

//inline int sendCmdMsg(const char *msg,int sz, IPDCom *dest) {
//    PDCom x(dest);
//    return x.sendCmdMsg(msg,sz);
//}
//-- Define server ID and class IDs
// {68EC5351-D239-11d1-90D1-0000F80355F3}
//const IID ECHO_CLSID = { 0x68ec5351, 0xd239, 0x11d1, { 0x90, 0xd1, 0x0, 0x0, 0xf8, 0x3, 0x55, 0xf3 } };
// {68EC5352-D239-11d1-90D1-0000F80355F3}
//const IID ECHO_APPID = { 0x68ec5352, 0xd239, 0x11d1, { 0x90, 0xd1, 0x0, 0x0, 0xf8, 0x3, 0x55, 0xf3 } };


#endif
