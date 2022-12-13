/*
$Id: PDACKMsg.cpp 39185 2005-01-19 12:43:48Z teh $
Description:
Copyright (C) Powel Data AS
All rights reserved.
*/


static char rcsid[]="$Id: PDACKMsg.cpp 39185 2005-01-19 12:43:48Z teh $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid
//---- PDACKMsg ---------------------------------------------------------
//
#include <PrePDCom.h>
#include <PDACKMsg.h>

using namespace std;
RWDEFINE_NAMED_COLLECTABLE(PDACKMsg, "PDACKMsg");
PDACKMsg::PDACKMsg() {
    _ack=NULL;
}

PDACKMsg::~PDACKMsg() {
	_ack=NULL;
}

void PDACKMsg::setMsg(void *p) {
	_ack = (ACK *)p;
}

RWspace  PDACKMsg::binaryStoreSize() const {
	RWspace count=0;
	if (!_ack)return count;
    count = RWCollectable::binaryStoreSize() + 	sizeof(ACK);
    return count;
}

void PDACKMsg::saveGuts(RWFile& o) const {
    PDCollectable::saveGuts(o);
	if(!_ack)return;
	ACKGuts(o,<<);
}

void PDACKMsg::saveGuts(RWvostream& o) const {
    PDCollectable::saveGuts(o);
	if(!_ack)return;
	ACKGuts(o,<<);
}


void PDACKMsg::restoreGuts(RWFile& i) {
    PDCollectable::restoreGuts(i);
	if (!_ack){
		_ack = (ACK *)malloc(sizeof(ACK));
	}
	ACKGuts(i,>>);    
}

void PDACKMsg::restoreGuts(RWvistream& i) {
    PDCollectable::restoreGuts(i);
	if (!_ack)	{
		_ack = (ACK *)malloc(sizeof(ACK));
	}
	ACKGuts(i,>>);
}

void * PDACKMsg::generateTestGuts(int n) {	
	ACK *test_ack;
	test_ack = (ACK *)malloc(sizeof(ACK));	
	test_ack->tag=1;
	test_ack->tag_type=2;
	test_ack->client_data=3;
	test_ack->sender_id=4;
	test_ack->sender_inst=5;
	test_ack->retval=6;
	return test_ack;
}

void PDACKMsg::printGuts(void *p,int n) {
#ifdef PDCOM_TEST
	ACK *ack = (ACK *)p;
	cout << "--------------ACK--(start)--------------\n";
	cout << "tag:"<< ack->tag << endl;
	cout << "tag_type:" << ack->tag_type <<endl;
	cout << "client_data:" << ack->client_data <<endl;
	cout << "sender_id" << ack->sender_id<< endl;
	cout << "sender_inst" << ack->sender_inst<< endl;
	cout << "retval" << ack->retval<< endl;
	cout << "--------------ACK--(end)----------------\n";
#endif
}

