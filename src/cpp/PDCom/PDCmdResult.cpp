/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: PDCmdResult.cpp 26959 2001-12-11 09:40:55Z jma $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid

#include <PrePDCom.h>
#include <PDCmdResult.h>

RWDEFINE_NAMED_COLLECTABLE(PDCmdResult, "PDCmdResult");
PDCmdResult::PDCmdResult() {
    ErrMsg="OK";
    Success=false;
}
PDCmdResult::PDCmdResult(RWCString x, RWBoolean y) {
    ErrMsg=x;
    Success=y;
}
PDCmdResult::~PDCmdResult() {
}
RWspace  PDCmdResult::binaryStoreSize() const {
    RWspace count = 0;
//    RWspace count = RWCollectable::binaryStoreSize() + ErrMsg.binaryStoreSize() + Success.binaryStoreSize();
    return count;
}
void PDCmdResult::saveGuts(RWFile& o) const {
    PDCollectable::saveGuts(o);
    o<<ErrMsg<<Success ;
}
void PDCmdResult::saveGuts(RWvostream& o) const {
    PDCollectable::saveGuts(o);
    o<<ErrMsg<<Success;
}

void PDCmdResult::restoreGuts(RWFile& i) {
    PDCollectable::restoreGuts(i);
    i>>ErrMsg>>Success;
}
void PDCmdResult::restoreGuts(RWvistream& i) {
    PDCollectable::restoreGuts(i);
    i>>ErrMsg>>Success;
}
