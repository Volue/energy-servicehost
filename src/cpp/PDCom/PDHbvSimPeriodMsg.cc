/*


Description:



Copyright (C) Powel Data AS

All rights reserved.

*/



static char rcsid[]="$Id: PDHbvSimPeriodMsg.cc 39185 2005-01-19 12:43:48Z teh $ PD";

static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid



//---- PDHbvSimPeriodMsg ---------------------------------------------------------

//

#include <PrePDCom.h>

#include <PDHbvSimPeriodMsg.h>

using namespace std;



RWDEFINE_NAMED_COLLECTABLE(PDHbvSimPeriodMsg, "PDHbvSimPeriodMsg");



PDHbvSimPeriodMsg::PDHbvSimPeriodMsg()
{
    _simPeriod = NULL;
}


PDHbvSimPeriodMsg::~PDHbvSimPeriodMsg() 
{
    _simPeriod = NULL;
}


void PDHbvSimPeriodMsg::setMsg (void* p)
{
    _simPeriod = (HBV_SIM_PERIOD*) p;
}



RWspace  PDHbvSimPeriodMsg::binaryStoreSize() const 
{

    RWspace count = 0;

    if (!_simPeriod)
	return count;

    count = RWCollectable::binaryStoreSize() + 
	    sizeof(HBV_SIM_PERIOD);

    return count;
}


void PDHbvSimPeriodMsg::saveGuts (RWFile& o) const 
{
    PDCollectable::saveGuts(o);

    if (!_simPeriod) 
	return;

    HBV_SIM_PERIODGuts (o,<<,1);
}


void PDHbvSimPeriodMsg::saveGuts (RWvostream& o) const 
{
    PDCollectable::saveGuts(o);

    if (!_simPeriod) 
	return;

    HBV_SIM_PERIODGuts (o,<<,1);
}



void PDHbvSimPeriodMsg::restoreGuts (RWFile& i) 
{
    PDCollectable::restoreGuts(i);

    if (!_simPeriod)
	_simPeriod = (HBV_SIM_PERIOD*) malloc (sizeof(HBV_SIM_PERIOD));

    HBV_SIM_PERIODGuts (i,>>,0);
}


void PDHbvSimPeriodMsg::restoreGuts (RWvistream& i) 
{
    PDCollectable::restoreGuts(i);

    if (!_simPeriod)
	_simPeriod = (HBV_SIM_PERIOD*) malloc (sizeof(HBV_SIM_PERIOD));

    HBV_SIM_PERIODGuts (i,>>,0);    
}


void* PDHbvSimPeriodMsg::generateTestGuts(int n)
{	
    HBV_SIM_PERIOD* pSimPeriod;

    pSimPeriod = (HBV_SIM_PERIOD*) malloc (sizeof(HBV_SIM_PERIOD));	

    pSimPeriod->tag = 1;
    pSimPeriod->tag_type =2 ;
    pSimPeriod->client_data = 3;
    strcpy (pSimPeriod->default_start,"19800101070000000");
    strcpy (pSimPeriod->default_end,"19850101070000000");
    strcpy (pSimPeriod->upper_time_limit,"19990101070000000");
    strcpy (pSimPeriod->__padding_5__,"p5");
    pSimPeriod->num_sim_steps = 4;
    pSimPeriod->time_unit = 5;
    pSimPeriod->step_length = 6;

    return pSimPeriod; 
}


void PDHbvSimPeriodMsg::printGuts (void *p,int n)
{
    HBV_SIM_PERIOD* pSimPeriod = (HBV_SIM_PERIOD*) p;

    cout << "--------------HBV_SIM_PERIOD--(start)--------------\n";

    cout << "tag:"		<< pSimPeriod->tag		<< endl;
    cout << "tag_type:"		<< pSimPeriod->tag_type		<< endl;
    cout << "client_data:"	<< pSimPeriod->client_data	<< endl;
    cout << "default_start:"	<< pSimPeriod->default_start	<< endl;
    cout << "default_end:"	<< pSimPeriod->default_end	<< endl;
    cout << "upper_time_limit:"	<< pSimPeriod->upper_time_limit	<< endl;
    cout << "__padding_5__:"	<< pSimPeriod->__padding_5__	<< endl;
    cout << "num_sim_steps:"	<< pSimPeriod->num_sim_steps	<< endl;
    cout << "time_unit:"	<< pSimPeriod->time_unit	<< endl;
    cout << "step_length:"	<< pSimPeriod->step_length	<< endl;

    cout << "--------------HBV_SIM_PERIOD--(end)----------------\n";
}

// BS! Ny privat funksjon.    
int PDHbvSimPeriodMsg::compareTo (const PDHbvSimPeriodMsg& msg) const
{
    int		isEqual = 0;
    RWCString	selfString;
    RWCString	msgString;
    
    if ( _simPeriod->tag != msg._simPeriod->tag )
	return _simPeriod->tag > msg._simPeriod->tag ? 1 : -1;
    
    if ( _simPeriod->tag_type != msg._simPeriod->tag_type )
	return _simPeriod->tag_type > msg._simPeriod->tag_type ? 1 : -1;
    
    if ( _simPeriod->client_data != msg._simPeriod->client_data )
	return _simPeriod->client_data > msg._simPeriod->client_data ? 1 : -1;
    
    selfString = RWCString (this->_simPeriod->default_start);
    msgString  = RWCString (msg._simPeriod->default_start);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_simPeriod->default_end);
    msgString  = RWCString (msg._simPeriod->default_end);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
   
    selfString = RWCString (this->_simPeriod->upper_time_limit);
    msgString  = RWCString (msg._simPeriod->upper_time_limit);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_simPeriod->__padding_5__);
    msgString  = RWCString (msg._simPeriod->__padding_5__);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _simPeriod->num_sim_steps != msg._simPeriod->num_sim_steps )
	return _simPeriod->num_sim_steps > msg._simPeriod->num_sim_steps ? 1 : -1;
    
    if ( _simPeriod->time_unit != msg._simPeriod->time_unit )
	return _simPeriod->time_unit > msg._simPeriod->time_unit ? 1 : -1;
    
    if ( _simPeriod->step_length != msg._simPeriod->step_length )
	return _simPeriod->step_length > msg._simPeriod->step_length ? 1 : -1;
        
    return isEqual;
}


RWBoolean PDHbvSimPeriodMsg::clear (void)
{
    RWBoolean success = FALSE;
    
    initiate ();
    
    return (success = TRUE);
}


void PDHbvSimPeriodMsg::initiate (void)
{
    if ( _simPeriod ) {
	_simPeriod->tag = 0;
	_simPeriod->tag_type = 0;
	_simPeriod->client_data = 0;
	_simPeriod->default_start[0] = 0;
	_simPeriod->default_end[0] = 0;
	_simPeriod->upper_time_limit[0] = 0;
	_simPeriod->__padding_5__[0] = 0;
	_simPeriod->num_sim_steps = 0;
	_simPeriod->time_unit = 0;
	_simPeriod->step_length = 0;
    }
}

