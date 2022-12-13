/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: PDSTRINGSMsg.cpp 39185 2005-01-19 12:43:48Z teh $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid

//---- PDSTRINGSMsg ---------------------------------------------------------
//
#include <PrePDCom.h>
#include <PDSTRINGSMsg.h>

using namespace std;

RWDEFINE_NAMED_COLLECTABLE(PDSTRINGSMsg, "PDSTRINGSMsg");

PDSTRINGSMsg::PDSTRINGSMsg() 
{
    _strings=NULL;
}
PDSTRINGSMsg::~PDSTRINGSMsg() 
{
	_strings=NULL;
}
void
PDSTRINGSMsg::setMsg(void *p)
{
	_strings = (STRINGS *)p;
}

RWspace  PDSTRINGSMsg::binaryStoreSize() const 
{
	RWspace count=0;
	if (!_strings)return count;
    count = RWCollectable::binaryStoreSize() + 
			sizeof(STRINGS)+
			_strings->num_values*sizeof(char)+1;
    return count;
}
void PDSTRINGSMsg::saveGuts(RWFile& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_strings)return;
	STRINGSGuts(o,<<,;,1);
}
void PDSTRINGSMsg::saveGuts(RWvostream& o) const 
{
    PDCollectable::saveGuts(o);
	if(!_strings)return;
	STRINGSGuts(o,<<,;,1);
}

void PDSTRINGSMsg::restoreGuts(RWFile& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_strings)
	{
		_strings = (STRINGS *)malloc(sizeof(STRINGS));
	}
	STRINGSGuts(i,>>,_strings=(STRINGS *)realloc(_strings,sizeof(STRINGS)+(_strings->num_values+1)*sizeof(char)),0);    
}
void PDSTRINGSMsg::restoreGuts(RWvistream& i) 
{
    PDCollectable::restoreGuts(i);
	if (!_strings)
	{
		_strings = (STRINGS *)malloc(sizeof(STRINGS));
	}
	STRINGSGuts(i,>>,_strings=(STRINGS *)realloc(_strings,sizeof(STRINGS)+(_strings->num_values+1)*sizeof(char)),0);    
}
void *
PDSTRINGSMsg::generateTestGuts(int n)
{	
	STRINGS *test_strings;
	test_strings = (STRINGS *)malloc(sizeof(STRINGS)+(n+1)*sizeof(char));	
	test_strings->tag=1;
	test_strings->tag_type=2;
	test_strings->client_data=3;
	test_strings->sender_id=4;
	test_strings->sender_inst=5;
	test_strings->kind=6;
	test_strings->s_length=7;
	test_strings->delimiter='-';
	strcpy(test_strings->__padding_7__,"p7");
	test_strings->num_values=n;
	*test_strings->strings='a';
	int j;
	for (j=1;j<n;j++)
	{
		int x;
		x=(int)test_strings->strings[j-1];
		test_strings->strings[j]=x+1;
	}
	test_strings->strings[j]=0;

	return test_strings; 
}
void PDSTRINGSMsg::printGuts(void *p,int n)
{
#ifdef PDCOM_TEST
	STRINGS *strings = (STRINGS *)p;
	cout << "--------------STRINGS--(start)--------------\n";
	cout << "tag:"<< strings->tag << endl;
	cout << "tag_type:" << strings->tag_type <<endl;
	cout << "client_data:" << strings->client_data <<endl;
	cout << "sender_id:" << strings->sender_id<< endl;
	cout << "sender_inst:" << strings->sender_inst<< endl;
	cout << "kind:" << strings->kind<< endl;
	cout << "s_length:" << strings->s_length<< endl;
	cout << "delimiter:" << strings->delimiter<< endl;
	cout << "__padding_7__:" << strings->__padding_7__<< endl;
	cout << "num_values:" << strings->num_values<< endl;
	cout << "strings:" << strings->strings<< endl;	
	cout << "--------------STRINGS--(end)----------------\n";
#endif
}
