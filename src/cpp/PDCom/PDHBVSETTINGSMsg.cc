/*


Description:



Copyright (C) Powel Data AS

All rights reserved.

*/



static char rcsid[]="$Id: PDHBVSETTINGSMsg.cc 39185 2005-01-19 12:43:48Z teh $ PD";

static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid



//---- PDHBVSETTINGSMsg ---------------------------------------------------------

//

#include <PrePDCom.h>

#include <PDHBVSETTINGSMsg.h>

using namespace std;



RWDEFINE_NAMED_COLLECTABLE(PDHBVSETTINGSMsg, "PDHBVSETTINGSMsg");



PDHBVSETTINGSMsg::PDHBVSETTINGSMsg()
{
    _settings = NULL;
}


PDHBVSETTINGSMsg::~PDHBVSETTINGSMsg() 
{
    _settings = NULL;
}


void PDHBVSETTINGSMsg::setMsg (void* p)
{
    _settings = (HBV_SETTINGS*) p;
}



RWspace  PDHBVSETTINGSMsg::binaryStoreSize() const 
{
    RWspace count = 0;

    if (!_settings) return count;

    count = RWCollectable::binaryStoreSize() + 
	    sizeof(HBV_SETTINGS);

    return count;
}


void PDHBVSETTINGSMsg::saveGuts (RWFile& o) const 
{
    PDCollectable::saveGuts(o);

    if (!_settings) return;

    HBV_SETTINGSGuts (o,<<,1);

}


void PDHBVSETTINGSMsg::saveGuts (RWvostream& o) const 
{
    PDCollectable::saveGuts(o);

    if (!_settings) return;

    HBV_SETTINGSGuts (o,<<,1);
}



void PDHBVSETTINGSMsg::restoreGuts (RWFile& i) 
{
    PDCollectable::restoreGuts(i);

    if (!_settings) {
	_settings = (HBV_SETTINGS*) malloc (sizeof(HBV_SETTINGS));
    }

    HBV_SETTINGSGuts (i,>>,0);
}


void PDHBVSETTINGSMsg::restoreGuts (RWvistream& i) 
{
    PDCollectable::restoreGuts(i);

    if (!_settings) {
	_settings = (HBV_SETTINGS*) malloc (sizeof(HBV_SETTINGS));
    }

    HBV_SETTINGSGuts(i,>>,0);    
}
 

void* PDHBVSETTINGSMsg::generateTestGuts (int n)
{	
    HBV_SETTINGS* test_settings;
	
    test_settings = (HBV_SETTINGS*) malloc (sizeof(HBV_SETTINGS));
 	
    test_settings->tag		= 1;
    test_settings->tag_type		= 2;
    test_settings->client_data	= 3;
    test_settings->settings_type	= 1;
    strcpy (test_settings->catchment_name,"Aursunden");
    strcpy (test_settings->__padding_4__,"p4");
    test_settings->catchment_area	= 730.0;
    strcpy (test_settings->start_simulation,"19841205070000000");
    strcpy (test_settings->end_simulation,"19860605070000000");
    test_settings->start_version	= 1;
    strcpy (test_settings->step_length,"24HOURS");
    strcpy (test_settings->__padding_9__,"p9");
    test_settings->simulation_length = 700;
    strcpy (test_settings->run_type,"FORECAST");
    strcpy (test_settings->forecast_type,"HISTORICAL");
    *test_settings->__padding_12__	= 'a';
    test_settings->first_year	= 1930;
    test_settings->last_year	= 1960;
    test_settings->num_of_alternatives = 31;
    test_settings->breakpoint	= 6;

    return test_settings; 
}


void PDHBVSETTINGSMsg::printGuts (void *p, int n)
{
    HBV_SETTINGS* pSettings = (HBV_SETTINGS*) p;

    cout << "--------------HBV_SETTINGS--(start)--------------\n";
    cout << "tag:"			<< pSettings->tag			<< endl;
    cout << "tag_type:"			<< pSettings->tag_type			<< endl;
    cout << "client_data:"		<< pSettings->client_data		<< endl;
    cout << "settings_type:"		<< pSettings->settings_type		<< endl;
    cout << "catchment_name:"		<< pSettings->catchment_name		<< endl;
    cout << "__padding_4__:"		<< pSettings->__padding_4__		<< endl;
    cout << "catchment_area:"		<< pSettings->catchment_area		<< endl;
    cout << "start_simulation:"		<< pSettings->start_simulation		<< endl;
    cout << "end_simulation:"		<< pSettings->end_simulation		<< endl;
    cout << "start_version:"		<< pSettings->start_version		<< endl;
    cout << "step_length:"		<< pSettings->step_length		<< endl;	
    cout << "__padding_9__:"		<< pSettings->__padding_9__		<< endl;	
    cout << "simulation_length:"	<< pSettings->simulation_length		<< endl;	
    cout << "run_type:"			<< pSettings->run_type			<< endl;	
    cout << "forecast_type:"		<< pSettings->forecast_type		<< endl;	
    cout << "__padding_12__:"		<< pSettings->__padding_12__		<< endl;	
    cout << "first_year:"		<< pSettings->first_year		<< endl;	
    cout << "last_year:"		<< pSettings->last_year			<< endl;	
    cout << "num_of_alternatives:"	<< pSettings->num_of_alternatives	<< endl;	
    cout << "breakpoint:"		<< pSettings->breakpoint		<< endl;	
    cout << "--------------HBV_SETTINGS--(end)----------------\n";
}


// BS! Ny private funksjon.    
int PDHBVSETTINGSMsg::compareTo (const PDHBVSETTINGSMsg& msg) const
{
    int		isEqual = 0;
    RWCString	selfString;
    RWCString	msgString;
    
    if ( _settings->tag != msg._settings->tag )
	return _settings->tag > msg._settings->tag ? 1 : -1;
    
    if ( _settings->tag_type != msg._settings->tag_type )
	return _settings->tag_type > msg._settings->tag_type ? 1 : -1;
    
    if ( _settings->client_data != msg._settings->client_data )
	return _settings->client_data > msg._settings->client_data ? 1 : -1;
    
    if ( _settings->settings_type != msg._settings->settings_type )
	return _settings->settings_type > msg._settings->settings_type ? 1 : -1;
    
    selfString = RWCString (this->_settings->catchment_name);
    msgString  = RWCString (msg._settings->catchment_name);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_settings->__padding_4__);
    msgString  = RWCString (msg._settings->__padding_4__);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
   
    if ( _settings->catchment_area != msg._settings->catchment_area )
	return _settings->catchment_area > msg._settings->catchment_area ? 1 : -1;
    
    selfString = RWCString (this->_settings->start_simulation);
    msgString  = RWCString (msg._settings->start_simulation);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_settings->end_simulation);
    msgString  = RWCString (msg._settings->end_simulation);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _settings->start_version != msg._settings->start_version )
	return _settings->start_version > msg._settings->start_version ? 1 : -1;
    
    selfString = RWCString (this->_settings->step_length);
    msgString  = RWCString (msg._settings->step_length);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_settings->__padding_9__);
    msgString  = RWCString (msg._settings->__padding_9__);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _settings->simulation_length != msg._settings->simulation_length )
	return _settings->simulation_length > msg._settings->simulation_length ? 1 : -1;
    
    selfString = RWCString (this->_settings->run_type);
    msgString  = RWCString (msg._settings->run_type);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_settings->forecast_type);
    msgString  = RWCString (msg._settings->forecast_type);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_settings->__padding_12__);
    msgString  = RWCString (msg._settings->__padding_12__);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _settings->first_year != msg._settings->first_year )
	return _settings->first_year > msg._settings->first_year ? 1 : -1;
    
    if ( _settings->last_year != msg._settings->last_year )
	return _settings->last_year > msg._settings->last_year ? 1 : -1;
    
    if ( _settings->num_of_alternatives != msg._settings->num_of_alternatives )
	return _settings->num_of_alternatives > msg._settings->num_of_alternatives ? 1 : -1;
    
    if ( _settings->breakpoint != msg._settings->breakpoint )
	return _settings->breakpoint > msg._settings->breakpoint ? 1 : -1;
    
    return isEqual;
}


RWBoolean PDHBVSETTINGSMsg::clear (void)
{
    RWBoolean success = FALSE;
    
    initiate ();
    
    return (success = TRUE);
}


void PDHBVSETTINGSMsg::initiate (void)
{
    if ( _settings ) {
	_settings->tag = 0;
	_settings->tag_type = 0;
	_settings->client_data = 0;
	_settings->settings_type = 0;
	_settings->catchment_name[0] = 0;
	_settings->__padding_4__[0] = 0;
	_settings->catchment_area = 0.0;
	_settings->start_simulation[0] = 0;
	_settings->end_simulation[0] = 0;
	_settings->start_version = 0;
	_settings->step_length[0] = 0;
	_settings->__padding_9__[0] = 0;
	_settings->simulation_length = 0;
	_settings->run_type[0] = 0;
	_settings->forecast_type[0] = 0;
	_settings->__padding_12__[0] = 0;
	_settings->first_year = 0;
	_settings->last_year = 0;
	_settings->num_of_alternatives = 0;
	_settings->breakpoint = 0;
    }
}

