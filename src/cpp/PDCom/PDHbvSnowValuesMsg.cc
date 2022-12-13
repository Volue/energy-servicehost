/*


Description:



Copyright (C) Powel Data AS

All rights reserved.

*/



static char rcsid[]="$Id: PDHbvSnowValuesMsg.cc 39185 2005-01-19 12:43:48Z teh $ PD";

static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid



//---- PDHbvSnowValuesMsg ---------------------------------------------------------
//

#include <PrePDCom.h>
#include <PDHbvSnowValuesMsg.h>

using namespace std;


 
RWDEFINE_NAMED_COLLECTABLE(PDHbvSnowValuesMsg, "PDHbvSnowValuesMsg");



PDHbvSnowValuesMsg::PDHbvSnowValuesMsg()
{
    _snowValues = NULL;
}


PDHbvSnowValuesMsg::~PDHbvSnowValuesMsg() 
{
    _snowValues = NULL;
}


void PDHbvSnowValuesMsg::setMsg (void* p)
{
    _snowValues = (HBV_SNOW_VALUES*) p;
}



RWspace  PDHbvSnowValuesMsg::binaryStoreSize() const 
{
    RWspace count = 0;

    if (!_snowValues) 
	return count;

    count = RWCollectable::binaryStoreSize() + 
	    sizeof(HBV_SNOW_VALUES) +
	    _snowValues->num_values*sizeof(double);

    return count;
}


void PDHbvSnowValuesMsg::saveGuts (RWFile& o) const 
{
    PDCollectable::saveGuts(o);

    if (!_snowValues) 
	return;

    HBV_SNOW_VALUESGuts (o,<<,;,1);
}


void PDHbvSnowValuesMsg::saveGuts (RWvostream& o) const 
{
    PDCollectable::saveGuts(o);

    if (!_snowValues) 
	return;

    HBV_SNOW_VALUESGuts (o,<<,;,1);
}



void PDHbvSnowValuesMsg::restoreGuts (RWFile& i) 
{
    PDCollectable::restoreGuts(i);

    if (!_snowValues)
    {
	_snowValues = (HBV_SNOW_VALUES*) malloc( sizeof(HBV_SNOW_VALUES));
    }

    HBV_SNOW_VALUESGuts (i,>>,_snowValues = (HBV_SNOW_VALUES*) realloc (_snowValues,sizeof(HBV_SNOW_VALUES)+(_snowValues->num_values+1)*sizeof(double)),0);
}


void PDHbvSnowValuesMsg::restoreGuts (RWvistream& i) 
{
    PDCollectable::restoreGuts(i);

    if (!_snowValues)
    {
	_snowValues = (HBV_SNOW_VALUES*) malloc (sizeof(HBV_SNOW_VALUES));
    }

    HBV_SNOW_VALUESGuts (i,>>,_snowValues = (HBV_SNOW_VALUES*) realloc (_snowValues,sizeof(HBV_SNOW_VALUES)+(_snowValues->num_values+1)*sizeof(double)),0);
}


void* PDHbvSnowValuesMsg::generateTestGuts (int n)
{	
    HBV_SNOW_VALUES* test_snow_values;

    test_snow_values = (HBV_SNOW_VALUES*) malloc (sizeof(HBV_SNOW_VALUES) +
			n*sizeof(double));	

    test_snow_values->tag = 1;
    test_snow_values->tag_type = 2;
    test_snow_values->client_data = 3;
    strcpy (test_snow_values->state_date,"19990909090000000");
    strcpy (test_snow_values->__padding_3__,"p3");
    test_snow_values->state_version = 4;
    test_snow_values->no_elevations = 5;
    test_snow_values->period = 6;
    strcpy(test_snow_values->text,"AKTUELT NAVN");
    strcpy(test_snow_values->__padding_7__,"p7");
    test_snow_values->num_values = n;
    strcpy(test_snow_values->__padding_8__,"p8");
    
    if ( n > 0 ) {
	int testValue;
	for (int j=0; j<n; j++) {
	    testValue = 100 + j;
	    test_snow_values->values[j] = testValue;
	}
    }
	
    return test_snow_values; 
}
 

void PDHbvSnowValuesMsg::printGuts (void *p, int n)
{
    HBV_SNOW_VALUES* pSnowValues = (HBV_SNOW_VALUES*) p;

    cout << "--------------HBV_SNOW_VALUES--(start)--------------\n";

	cout << "tag:"			<< pSnowValues->tag		<< endl;
	cout << "tag_type:"		<< pSnowValues->tag_type	<< endl;
	cout << "client_data:"		<< pSnowValues->client_data	<< endl;
	cout << "state_date:"		<< pSnowValues->state_date	<< endl;
	cout << "__padding_3__:"	<< pSnowValues->__padding_3__	<< endl;
	cout << "state_version:"	<< pSnowValues->state_version	<< endl;
	cout << "no_elevations:"	<< pSnowValues->no_elevations	<< endl;
	cout << "period:"		<< pSnowValues->period		<< endl;
	cout << "text:"			<< pSnowValues->text		<< endl;
	cout << "__padding_7__:"	<< pSnowValues->__padding_7__	<< endl;
	cout << "num_values:"		<< pSnowValues->num_values	<< endl;	
	cout << "__padding_8__:"	<< pSnowValues->__padding_8__	<< endl;
		
	for (int j=0; j<pSnowValues->num_values; j++)
	     cout << "value[" << j <<"] = " << pSnowValues->values[j] << endl;

    cout << "--------------HBV_SNOW_VALUES--(end)----------------\n";
}


// BS! Ny privat funksjon.    
int PDHbvSnowValuesMsg::compareTo (const PDHbvSnowValuesMsg& msg) const
{
    int		isEqual = 0;
    RWCString	selfString;
    RWCString	msgString;
    
    if ( _snowValues->tag != msg._snowValues->tag )
	return _snowValues->tag > msg._snowValues->tag ? 1 : -1;
    
    if ( _snowValues->tag_type != msg._snowValues->tag_type )
	return _snowValues->tag_type > msg._snowValues->tag_type ? 1 : -1;
    
    if ( _snowValues->client_data != msg._snowValues->client_data )
	return _snowValues->client_data > msg._snowValues->client_data ? 1 : -1;
    
    selfString = RWCString (this->_snowValues->state_date);
    msgString  = RWCString (msg._snowValues->state_date);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_snowValues->__padding_3__);
    msgString  = RWCString (msg._snowValues->__padding_3__);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
   
    if ( _snowValues->state_version != msg._snowValues->state_version )
	return _snowValues->state_version > msg._snowValues->state_version ? 1 : -1;
    
    if ( _snowValues->no_elevations != msg._snowValues->no_elevations )
	return _snowValues->no_elevations > msg._snowValues->no_elevations ? 1 : -1;
    
    if ( _snowValues->period != msg._snowValues->period )
	return _snowValues->period > msg._snowValues->period ? 1 : -1;
    
    selfString = RWCString (this->_snowValues->text);
    msgString  = RWCString (msg._snowValues->text);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_snowValues->__padding_7__);
    msgString  = RWCString (msg._snowValues->__padding_7__);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _snowValues->num_values != msg._snowValues->num_values )
	return _snowValues->num_values > msg._snowValues->num_values ? 1 : -1;
    
    selfString = RWCString (this->_snowValues->__padding_8__);
    msgString  = RWCString (msg._snowValues->__padding_8__);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _snowValues->num_values > 0 ) {
	for (int j=0; j<_snowValues->num_values; j++) {
	    if ( _snowValues->values[j] != msg._snowValues->values[j] )
	        return _snowValues->values[j] > msg._snowValues->values[j] ? 1 : -1;
	}
    }
    
    return isEqual;
}


RWBoolean PDHbvSnowValuesMsg::clear (void)
{
    RWBoolean success = FALSE;
    
    initiate ();
    
    return (success = TRUE);
}


void PDHbvSnowValuesMsg::initiate (void)
{
    if ( _snowValues ) {
	_snowValues->tag = 0;
	_snowValues->tag_type = 0;
	_snowValues->client_data = 0;
	_snowValues->state_date[0] = 0;
	_snowValues->__padding_3__[0] = 0;
	_snowValues->state_version = 0;
	_snowValues->no_elevations = 0;
	_snowValues->period = 0;
	_snowValues->text[0] = 0;
	_snowValues->__padding_7__[0] = 0;
	_snowValues->__padding_8__[0] = 0;
	if ( _snowValues->num_values > 0 ) {
	    for (int j=0; j<_snowValues->num_values; j++)
		_snowValues->values[j] = 0;
	}
    }
}


