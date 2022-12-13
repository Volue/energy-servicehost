/*
$Id: PDBASEMsg.cpp 39185 2005-01-19 12:43:48Z teh $
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/
static char rcsid[]="$Id: PDBASEMsg.cpp 39185 2005-01-19 12:43:48Z teh $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid
//---- PDBASEMsg ---------------------------------------------------------
//

#include <PrePDCom.h>
#include <PDBASEMsg.h>

using namespace std;
RWDEFINE_NAMED_COLLECTABLE(PDBASEMsg,"PDBASEMsg");
PDBASEMsg::PDBASEMsg() {
}

PDBASEMsg::~PDBASEMsg() {
}

void PDBASEMsg::setMsg(void *dummy) {
}

RWspace  PDBASEMsg::binaryStoreSize() const {
	RWspace count=0;
    return count;
}

void PDBASEMsg::saveGuts(RWFile& o) const {

}

void PDBASEMsg::saveGuts(RWvostream& o) const {

}



void PDBASEMsg::restoreGuts(RWFile& o) {

}

void PDBASEMsg::restoreGuts(RWvistream& o) {

}

void * PDBASEMsg::generateTestGuts(int n) {	
#ifdef PDCOM_TEST
	cout << "no data generated\n";
#endif
		return NULL;

}

void PDBASEMsg::printGuts(void *dummy,int n) {

}

