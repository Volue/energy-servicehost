/*
$Id: PDMULTIMsg.cpp 39185 2005-01-19 12:43:48Z teh $
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: PDMULTIMsg.cpp 39185 2005-01-19 12:43:48Z teh $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid

//---- PDMULTIMsg ---------------------------------------------------------
//
#include <PrePDCom.h>
#include <PDMULTIMsg.h>

using namespace std;

RWDEFINE_NAMED_COLLECTABLE(PDMULTIMsg, "PDMULTIMsg");

PDMULTIMsg::PDMULTIMsg() 
{
    _multi=NULL;
}
PDMULTIMsg::~PDMULTIMsg() 
{
	_multi=NULL;
}
void
PDMULTIMsg::setMsg(void *p)
{
	_multi = (MULTI_MSG *)p;
}

RWspace  PDMULTIMsg::binaryStoreSize() const 
{
	RWspace count=0;
	if (!_multi)return count;
    count = RWCollectable::binaryStoreSize() + 
			sizeof(MULTI_MSG);
    return count;
}
void PDMULTIMsg::saveGuts(RWFile& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_multi)return;
	MULTIGuts(o,<<);
}
void PDMULTIMsg::saveGuts(RWvostream& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_multi)return;
	MULTIGuts(o,<<);
}

void PDMULTIMsg::restoreGuts(RWFile& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_multi)
	{
		_multi = (MULTI_MSG *)malloc(sizeof(MULTI_MSG));
	}
	MULTIGuts(i,>>);    
}
void PDMULTIMsg::restoreGuts(RWvistream& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_multi)
	{
		_multi = (MULTI_MSG *)malloc(sizeof(MULTI_MSG));
	}
	MULTIGuts(i,>>);
}
void *
PDMULTIMsg::generateTestGuts(int n)
{	
	MULTI_MSG *test_multi;
	test_multi = (MULTI_MSG *)malloc(sizeof(MULTI_MSG));	
	test_multi->tag=1;
	test_multi->tag_type=2;
	test_multi->client_data=3;
	test_multi->state=4;
	return test_multi;
}
void PDMULTIMsg::printGuts(void *p,int n)
{
#if PDCOM_TEST
	MULTI_MSG *multi = (MULTI_MSG *)p;
	cout << "tag:"<< multi->tag << endl;
	cout << "tag_type:" << multi->tag_type <<endl;
	cout << "client_data:" << multi->client_data <<endl;
	cout << "state:" << multi->state << endl;
#endif	
}
