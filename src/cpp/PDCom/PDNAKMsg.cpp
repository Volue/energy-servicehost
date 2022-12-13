/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: PDNAKMsg.cpp 39185 2005-01-19 12:43:48Z teh $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid

//---- PDNAKMsg ---------------------------------------------------------
//
#include <PrePDCom.h>
#include <PDNAKMsg.h>

using namespace std;

RWDEFINE_NAMED_COLLECTABLE(PDNAKMsg, "PDNAKMsg");

PDNAKMsg::PDNAKMsg() 
{
    _nak=NULL;
}
PDNAKMsg::~PDNAKMsg() 
{
	_nak=NULL;
}
void
PDNAKMsg::setMsg(void *p)
{
	_nak = (NAK *)p;
}

RWspace  PDNAKMsg::binaryStoreSize() const 
{
	RWspace count=0;
	if (!_nak)return count;
    count = RWCollectable::binaryStoreSize() + 
			sizeof(NAK);
    return count;
}
void PDNAKMsg::saveGuts(RWFile& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_nak)return;
	NAKGuts(o,<<);
}
void PDNAKMsg::saveGuts(RWvostream& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_nak)return;
	NAKGuts(o,<<);
}

void PDNAKMsg::restoreGuts(RWFile& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_nak)
	{
		_nak = (NAK *)malloc(sizeof(NAK));
	}
	NAKGuts(i,>>);    
}
void PDNAKMsg::restoreGuts(RWvistream& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_nak)
	{
		_nak = (NAK *)malloc(sizeof(NAK));
	}
	NAKGuts(i,>>);
}
void *
PDNAKMsg::generateTestGuts(int n)
{	
	NAK *test_nak;
	test_nak = (NAK *)malloc(sizeof(NAK));	
	test_nak->tag=1;
	test_nak->tag_type=2;
	test_nak->client_data=3;
	test_nak->sender_id=4;
	test_nak->sender_inst=5;
	test_nak->reason=6;
	return test_nak;
}
void PDNAKMsg::printGuts(void *p,int n)
{
	NAK *nak = (NAK *)p;
	cout << "--------------NAK--(start)--------------\n";
	cout << "tag:"<< nak->tag << endl;
	cout << "tag_type:" << nak->tag_type <<endl;
	cout << "client_data:" << nak->client_data <<endl;
	cout << "sender_id" << nak->sender_id<< endl;
	cout << "sender_inst" << nak->sender_inst<< endl;
	cout << "reason" << nak->reason<< endl;
	cout << "--------------NAK--(end)----------------\n";
}
