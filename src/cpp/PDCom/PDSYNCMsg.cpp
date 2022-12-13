/*
$Header: 
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: PDSYNCMsg.cpp 39185 2005-01-19 12:43:48Z teh $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid

//---- PDSYNCMsg ---------------------------------------------------------
//
#include <PrePDCom.h>
#include <PDSYNCMsg.h>

using namespace std;

RWDEFINE_NAMED_COLLECTABLE(PDSYNCMsg, "PDSYNCMsg");

PDSYNCMsg::PDSYNCMsg() 
{
    _sync=NULL;
}
PDSYNCMsg::~PDSYNCMsg() 
{
	_sync=NULL;
}
void
PDSYNCMsg::setMsg(void *p)
{
	_sync = (SYNC *)p;
}

RWspace  PDSYNCMsg::binaryStoreSize() const 
{
	RWspace count=0;
	if (!_sync)return count;
    count = RWCollectable::binaryStoreSize() + 
			sizeof(SYNC);
    return count;
}
void PDSYNCMsg::saveGuts(RWFile& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_sync)return;
	SYNCGuts(o,<<);
}
void PDSYNCMsg::saveGuts(RWvostream& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_sync)return;
	SYNCGuts(o,<<);
}

void PDSYNCMsg::restoreGuts(RWFile& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_sync)
	{
		_sync = (SYNC *)malloc(sizeof(SYNC));
	}
	SYNCGuts(i,>>);    
}
void PDSYNCMsg::restoreGuts(RWvistream& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_sync)
	{
		_sync = (SYNC *)malloc(sizeof(SYNC));
	}
	SYNCGuts(i,>>);
}
void *
PDSYNCMsg::generateTestGuts(int n)
{	
	SYNC *test_sync;
	test_sync = (SYNC *)malloc(sizeof(SYNC));	
	test_sync->tag=1;
	test_sync->tag_type=2;
	test_sync->client_data=3;
	test_sync->status=4;
	return test_sync;
}
void PDSYNCMsg::printGuts(void *p,int n)
{
#ifdef PDCOM_TEST
	SYNC *sync = (SYNC *)p;
	cout << "--------------SYNC--(start)--------------\n";
	cout << "tag:"<< sync->tag << endl;
	cout << "tag_type:" << sync->tag_type <<endl;
	cout << "client_data:" << sync->client_data <<endl;
	cout << "status" << sync->status<< endl;
	cout << "--------------SYNC--(end)----------------\n";
#endif	
}
