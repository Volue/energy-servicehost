/*
Description:
	Implements the PDComMsqQ class, ref. .h-file
	
Copyright (C) Powel Data AS
All rights reserved.
*/
#include <PrePDCom.h>
#ifdef WIN32
#include <PDComMsgQ.h>
RWDEFINE_NAMED_COLLECTABLE(PDComDisconnectMsg,"PDComDisconnectMsg");
void PDComDisconnectMsg::saveGuts(RWvostream&o) const {
    PDCollectable::saveGuts(o);
}
void PDComDisconnectMsg::restoreGuts(RWvistream&i) {
    PDCollectable::restoreGuts(i);
}

PDComMsgQ *globalMsgQ;
//protected:
//    RWTPtrSlist<PDComMsgQItem> msqQ;
PDComMsgQ::PDComMsgQ() {
    serverThread=0;
}
PDComMsgQ::~PDComMsgQ() {
}
void PDComMsgQ::addMsg(const char* msg,unsigned int sz,const PDCom& replyTo) {
    lock();
    msqQ.append(PDComMsqQItem(msg,sz,replyTo));
    unlock();
    // post a message to server:
    if(serverThread) PostThreadMessage(serverThread,PDCOM_NEW_MESSAGE_ARRIVED,0,0);
}
void PDComMsgQ::clientDisconnected(PDCom& replyTo) {
    int res=0;
    lock();
    if(msqQ.entries()) {
        RWTValSlistIterator< PDComMsqQItem > i(msqQ);
        while( i()) {
            if( i.key().replyTo.getInterface() == replyTo.getInterface()) {
                i.key().replyTo.setDisconnected();
            }
        }
    }
    //-- fake an internal message for disconnection
    PDComDisconnectMsg dx;
    unsigned int sz=0;
    char *msg=PDCom::pack(&dx,sz);
    msqQ.append(PDComMsqQItem(msg,sz,replyTo));
    unlock();
    if(serverThread) PostThreadMessage(serverThread,PDCOM_NEW_MESSAGE_ARRIVED,0,0);

}

int PDComMsgQ::getMsg(const char*& msg,unsigned int& sz,PDCom& replyTo) {
    int res=0;
    msg=0;sz=0;
    lock();
    if(msqQ.entries()) {
        PDComMsqQItem item=msqQ.removeFirst();
        msg=item.msgBuffer;
        sz=item.msgSize;
        replyTo=item.replyTo;
        res=1;
    }
    unlock();
    return res;
}
PDCollectable* PDComMsgQ::getMsg(PDCom& replyTo) {
	const char* rawMsg;
	unsigned int rawMsgSz;
	if(getMsg(rawMsg,rawMsgSz,replyTo)) {
		if(rawMsgSz <=0) return 0;
		PDCollectable *res=replyTo.unpack(const_cast<char*>(rawMsg),rawMsgSz);
		delete[] rawMsg; // this class own this chunk of memory
		return res;
	}
	return (PDCollectable*)0;
}

void PDComMsgQ::lock() {
    msqQSection.Lock();
}
void PDComMsgQ::unlock() {
    msqQSection.Unlock();
}
int PDComMsgQ::entries() {
    int res=0;
    lock();
    res=msqQ.entries();
    unlock();
    return res;
}
#endif
