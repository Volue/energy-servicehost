/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: PDEchoReply.cpp 13376 1998-05-05 09:12:21Z sih $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid

#include <PrePDCom.h>
#include <PDEchoReply.h>

RWDEFINE_NAMED_COLLECTABLE(PDEchoReply, "PDEchoReply");
PDEchoReply::PDEchoReply() {
    echoMsg="Hello";
}
PDEchoReply::PDEchoReply(RWCString x) {
    echoMsg=x;
}
PDEchoReply::~PDEchoReply() {
}
//-- comparison & sorting
//int  PDEchoReply::compareTo(const RWCollectable* x) const {
//    if( x->isA() == isA()) {
//        
//    }
//}
//RWBoolean  PDEchoReply::isEqual(const RWCollectable* c) const {
//}
//-- persistence
RWspace  PDEchoReply::binaryStoreSize() const {
    RWspace count = RWCollectable::binaryStoreSize() + echoMsg.binaryStoreSize();
    return count;
}
void PDEchoReply::saveGuts(RWFile& o) const {
    PDCollectable::saveGuts(o);
    o<<echoMsg;
}
void PDEchoReply::saveGuts(RWvostream& o) const {
    PDCollectable::saveGuts(o);
    o<<echoMsg;
}

void PDEchoReply::restoreGuts(RWFile& i) {
    PDCollectable::restoreGuts(i);
    i>>echoMsg;
}
void PDEchoReply::restoreGuts(RWvistream& i) {
    PDCollectable::restoreGuts(i);
    i>>echoMsg;
}
