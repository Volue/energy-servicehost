/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: PDCOMMANDMsg.cpp 39185 2005-01-19 12:43:48Z teh $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid

//---- PDCOMMANDMsg ---------------------------------------------------------
//
#include <PrePDCom.h>
#include <PDCOMMANDMsg.h>

using namespace std;

RWDEFINE_NAMED_COLLECTABLE(PDCOMMANDMsg, "PDCOMMANDMsg");

PDCOMMANDMsg::PDCOMMANDMsg() 
{
    _command=NULL;
}
PDCOMMANDMsg::~PDCOMMANDMsg() 
{
	_command=NULL;
}
void
PDCOMMANDMsg::setMsg(void *p)
{
	_command = (COMMAND*)p;
}

RWspace  PDCOMMANDMsg::binaryStoreSize() const 
{
	RWspace count=0;
	if (!_command)return count;
    count = RWCollectable::binaryStoreSize() + 
			sizeof(COMMAND);
    return count;
}
void PDCOMMANDMsg::saveGuts(RWFile& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_command)return;
	COMMANDGuts(o,<<,1);
}
void PDCOMMANDMsg::saveGuts(RWvostream& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_command)return;
	COMMANDGuts(o,<<,1);
}

void PDCOMMANDMsg::restoreGuts(RWFile& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_command)
	{
		_command = (COMMAND *)malloc(sizeof(COMMAND));
	}
	COMMANDGuts(i,>>,0);    
}
void PDCOMMANDMsg::restoreGuts(RWvistream& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_command)
	{
		_command = (COMMAND *)malloc(sizeof(COMMAND));
	}
	COMMANDGuts(i,>>,0);
}
void *
PDCOMMANDMsg::generateTestGuts(int n)
{	
	COMMAND *test_command;
	test_command = (COMMAND *)malloc(sizeof(COMMAND));	
	test_command->tag=1;
	test_command->tag_type=2;
	test_command->client_data=3;
	strcpy(test_command->command,"command");
	return test_command;
}
void PDCOMMANDMsg::printGuts(void *p,int n)
{
#ifdef PDCOM_TEST
	COMMAND *multi = (COMMAND *)p;
	cout << "tag:"<< multi->tag << endl;
	cout << "tag_type:" << multi->tag_type <<endl;
	cout << "client_data:" << multi->client_data <<endl;
	cout << "command:" << multi->command << endl;
#endif	
}
