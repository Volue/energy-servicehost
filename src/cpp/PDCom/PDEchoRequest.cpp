/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: PDEchoRequest.cpp 13376 1998-05-05 09:12:21Z sih $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid

#include <PrePDCom.h>
#include <PDEchoRequest.h>

RWDEFINE_NAMED_COLLECTABLE(PDEchoRequest, "PDEchoRequest");

PDEchoRequest::PDEchoRequest() {
    echoMsg="Hello";
}
PDEchoRequest::~PDEchoRequest() {
}
//-- comparison & sorting
//int  PDEchoRequest::compareTo(const RWCollectable* x) const {
//    if( x->isA() == isA()) {
//        
//    }
//}
//RWBoolean  PDEchoRequest::isEqual(const RWCollectable* c) const {
//}
//-- persistence
RWspace  PDEchoRequest::binaryStoreSize() const {
    RWspace count = RWCollectable::binaryStoreSize() + echoMsg.binaryStoreSize();
    return count;
}
void PDEchoRequest::saveGuts(RWFile& o) const {
    PDCollectable::saveGuts(o);
    o<<echoMsg;
}
void PDEchoRequest::saveGuts(RWvostream& o) const {
    PDCollectable::saveGuts(o);
    o<<echoMsg;
}

void PDEchoRequest::restoreGuts(RWFile& i) {
    PDCollectable::restoreGuts(i);
    i>>echoMsg;
}
void PDEchoRequest::restoreGuts(RWvistream& i) {
    PDCollectable::restoreGuts(i);
    i>>echoMsg;
}
