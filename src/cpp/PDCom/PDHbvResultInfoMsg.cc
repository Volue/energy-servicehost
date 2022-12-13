/*


Description:



Copyright (C) Powel Data AS

All rights reserved.

*/



static char rcsid[]="$Id: PDHbvResultInfoMsg.cc 39185 2005-01-19 12:43:48Z teh $ PD";

static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; // Referencing rcsid



//---- PDHbvResultInfoMsg ---------------------------------------------------------
//

#include <PrePDCom.h>
#include <PDHbvResultInfoMsg.h>

using namespace std;



RWDEFINE_NAMED_COLLECTABLE(PDHbvResultInfoMsg, "PDHbvResultInfoMsg");



PDHbvResultInfoMsg::PDHbvResultInfoMsg()
{
    _resultInfo = NULL;
}


PDHbvResultInfoMsg::~PDHbvResultInfoMsg() 
{
    _resultInfo = NULL;
}


void PDHbvResultInfoMsg::setMsg (void* p)
{
    _resultInfo = (HBV_RESULT_INFO*) p;
}



RWspace  PDHbvResultInfoMsg::binaryStoreSize() const 
{
    RWspace count = 0;

    if (!_resultInfo) return count;

    count = RWCollectable::binaryStoreSize() + 
	    sizeof(HBV_RESULT_INFO);
		
    return count;
}


void PDHbvResultInfoMsg::saveGuts (RWFile& o) const 
{
    PDCollectable::saveGuts(o);

    if (!_resultInfo) 
	return;

    HBV_RESULT_INFOGuts (o,<<,1);
}


void PDHbvResultInfoMsg::saveGuts (RWvostream& o) const 
{
    PDCollectable::saveGuts(o);

    if (!_resultInfo) 
	return;

    HBV_RESULT_INFOGuts (o,<<,1);
}



void PDHbvResultInfoMsg::restoreGuts (RWFile& i) 
{
    PDCollectable::restoreGuts(i);

    if (!_resultInfo)
	_resultInfo = (HBV_RESULT_INFO*) malloc (sizeof(HBV_RESULT_INFO));

    HBV_RESULT_INFOGuts (i,>>,0);
}
 

void PDHbvResultInfoMsg::restoreGuts (RWvistream& i) 
{
    PDCollectable::restoreGuts(i);

    if (!_resultInfo)
	_resultInfo = (HBV_RESULT_INFO*) malloc (sizeof(HBV_RESULT_INFO));

    HBV_RESULT_INFOGuts(i,>>,0);    
}


void* PDHbvResultInfoMsg::generateTestGuts(int n)
{	
    HBV_RESULT_INFO* test_info;

    test_info = (HBV_RESULT_INFO*) malloc (sizeof(HBV_RESULT_INFO));	

    test_info->tag = 1;
    test_info->tag_type = 2;
    test_info->client_data = 3;
    strcpy (test_info->catchment_name,"Neavassdraget");
    strcpy (test_info->__padding_3__,"p3");
    test_info->update_flag = 4;
    strcpy (test_info->update_start,"19800101070000000");
    strcpy (test_info->update_end,"19850101070000000");
    test_info->update_time_unit = 5;
    test_info->update_time_step = 6;
    test_info->calibration_flag = 7;
    strcpy (test_info->calibration_start,"19300101070000000");
    strcpy (test_info->calibration_end,"19600101070000000");
    test_info->calibration_time_unit = 8;
    test_info->calibration_time_step = 9;
    test_info->forecast_flag = 10;
    strcpy (test_info->forecast_start,"19700101070000000");
    strcpy (test_info->forecast_end,"19900101070000000");
    test_info->forecast_time_unit = 11;
    test_info->forecast_time_step = 12;
    test_info->forecast_type = 13;
    test_info->num_met_forecasts = 14;
    test_info->first_hist_year = 15;
    test_info->last_hist_year = 16;
    
    return test_info; 
}


void PDHbvResultInfoMsg::printGuts (void *p, int n)
{
    HBV_RESULT_INFO* pResultInfo = (HBV_RESULT_INFO*) p;

    cout << "--------------HBV_RESULT_INFO--(start)--------------\n";
    cout << "tag:"			<< pResultInfo->tag			<< endl;
    cout << "tag_type:"			<< pResultInfo->tag_type		<< endl;
    cout << "client_data:"		<< pResultInfo->client_data		<< endl;
    cout << "catchment_name:"		<< pResultInfo->catchment_name		<< endl;
    cout << "__padding_3__:"		<< pResultInfo->__padding_3__		<< endl;
    cout << "update_flag:"		<< pResultInfo->update_flag		<< endl;
    cout << "update_start:"		<< pResultInfo->update_start		<< endl;
    cout << "update_end:"		<< pResultInfo->update_end		<< endl;
    cout << "update_time_unit:"		<< pResultInfo->update_time_unit	<< endl;
    cout << "update_time_step:"		<< pResultInfo->update_time_step	<< endl;
    cout << "calibration_flag:"		<< pResultInfo->calibration_flag	<< endl;	
    cout << "calibration_start:"	<< pResultInfo->calibration_start	<< endl;
    cout << "calibration_end:"		<< pResultInfo->calibration_end		<< endl;
    cout << "calibration_time_unit:"	<< pResultInfo->calibration_time_unit	<< endl;    
    cout << "calibration_time_step:"	<< pResultInfo->calibration_time_step	<< endl;
    cout << "forecast_flag:"		<< pResultInfo->forecast_flag		<< endl;
    cout << "forecast_start:"		<< pResultInfo->forecast_start		<< endl;    
    cout << "forecast_end:"		<< pResultInfo->forecast_end		<< endl;
    cout << "forecast_time_unit:"	<< pResultInfo->forecast_time_unit	<< endl;
    cout << "forecast_time_step:"	<< pResultInfo->forecast_time_step	<< endl;
    cout << "forecast_type:"		<< pResultInfo->forecast_type		<< endl;
    cout << "num_met_forecasts:"	<< pResultInfo->num_met_forecasts	<< endl;
    cout << "first_hist_year:"		<< pResultInfo->first_hist_year		<< endl;
    cout << "last_hist_year:"		<< pResultInfo->last_hist_year		<< endl;

    cout << "--------------HBV_RESULT_INFO--(end)----------------\n";
}


// BS! Ny privat funksjon.    
int PDHbvResultInfoMsg::compareTo (const PDHbvResultInfoMsg& msg) const
{
    int		isEqual = 0;
    RWCString	selfString;
    RWCString	msgString;
    
    if ( _resultInfo->tag != msg._resultInfo->tag )
	return _resultInfo->tag > msg._resultInfo->tag ? 1 : -1;
    
    if ( _resultInfo->tag_type != msg._resultInfo->tag_type )
	return _resultInfo->tag_type > msg._resultInfo->tag_type ? 1 : -1;
    
    if ( _resultInfo->client_data != msg._resultInfo->client_data )
	return _resultInfo->client_data > msg._resultInfo->client_data ? 1 : -1;
    
    selfString = RWCString (this->_resultInfo->catchment_name);
    msgString  = RWCString (msg._resultInfo->catchment_name);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_resultInfo->__padding_3__);
    msgString  = RWCString (msg._resultInfo->__padding_3__);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
   
    if ( _resultInfo->update_flag != msg._resultInfo->update_flag )
	return _resultInfo->update_flag > msg._resultInfo->update_flag ? 1 : -1;
    
    selfString = RWCString (this->_resultInfo->update_start);
    msgString  = RWCString (msg._resultInfo->update_start);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_resultInfo->update_end);
    msgString  = RWCString (msg._resultInfo->update_end);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _resultInfo->update_time_unit != msg._resultInfo->update_time_unit )
	return _resultInfo->update_time_unit > msg._resultInfo->update_time_unit ? 1 : -1;
    
    if ( _resultInfo->update_time_step != msg._resultInfo->update_time_step )
	return _resultInfo->update_time_step > msg._resultInfo->update_time_step ? 1 : -1;
    
    if ( _resultInfo->calibration_flag != msg._resultInfo->calibration_flag )
	return _resultInfo->calibration_flag > msg._resultInfo->calibration_flag ? 1 : -1;
    
    selfString = RWCString (this->_resultInfo->calibration_start);
    msgString  = RWCString (msg._resultInfo->calibration_start);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_resultInfo->calibration_end);
    msgString  = RWCString (msg._resultInfo->calibration_end);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _resultInfo->calibration_time_unit != msg._resultInfo->calibration_time_unit )
	return _resultInfo->calibration_time_unit > msg._resultInfo->calibration_time_unit ? 1 : -1;
    
    if ( _resultInfo->calibration_time_step != msg._resultInfo->calibration_time_step )
	return _resultInfo->calibration_time_step > msg._resultInfo->calibration_time_step ? 1 : -1;
    
    if ( _resultInfo->forecast_flag != msg._resultInfo->forecast_flag )
	return _resultInfo->forecast_flag > msg._resultInfo->forecast_flag ? 1 : -1;
    
    selfString = RWCString (this->_resultInfo->forecast_start);
    msgString  = RWCString (msg._resultInfo->forecast_start);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    selfString = RWCString (this->_resultInfo->forecast_end);
    msgString  = RWCString (msg._resultInfo->forecast_end);
    if ( selfString != msgString )
	return selfString > msgString ? 1 : -1;
    
    if ( _resultInfo->forecast_time_unit != msg._resultInfo->forecast_time_unit )
	return _resultInfo->forecast_time_unit > msg._resultInfo->forecast_time_unit ? 1 : -1;
    
    if ( _resultInfo->forecast_time_step != msg._resultInfo->forecast_time_step )
	return _resultInfo->forecast_time_step > msg._resultInfo->forecast_time_step ? 1 : -1;
    
    if ( _resultInfo->forecast_type != msg._resultInfo->forecast_type )
	return _resultInfo->forecast_type > msg._resultInfo->forecast_type ? 1 : -1;
    
    if ( _resultInfo->num_met_forecasts != msg._resultInfo->num_met_forecasts )
	return _resultInfo->num_met_forecasts > msg._resultInfo->num_met_forecasts ? 1 : -1;
    
    if ( _resultInfo->first_hist_year != msg._resultInfo->first_hist_year )
	return _resultInfo->first_hist_year > msg._resultInfo->first_hist_year ? 1 : -1;
    
    if ( _resultInfo->last_hist_year != msg._resultInfo->last_hist_year )
	return _resultInfo->last_hist_year > msg._resultInfo->last_hist_year ? 1 : -1;
    
    return isEqual;
}


RWBoolean PDHbvResultInfoMsg::clear (void)
{
    RWBoolean success = FALSE;
    
    initiate ();
    
    return (success = TRUE);
}


void PDHbvResultInfoMsg::initiate (void)
{
    if ( _resultInfo ) {
	_resultInfo->tag = 0;
	_resultInfo->tag_type = 0;
	_resultInfo->client_data = 0;
	_resultInfo->catchment_name[0] = 0;
	_resultInfo->__padding_3__[0] = 0;
	_resultInfo->update_flag = 0;
	_resultInfo->update_start[0] = 0;
	_resultInfo->update_end[0] = 0;
	_resultInfo->update_time_unit = 0;
	_resultInfo->update_time_step = 0;
	_resultInfo->calibration_flag = 0;
	_resultInfo->calibration_start[0] = 0;
	_resultInfo->calibration_end[0] = 0;
	_resultInfo->calibration_time_unit = 0;
	_resultInfo->calibration_time_step = 0;
	_resultInfo->forecast_flag = 0;
	_resultInfo->forecast_start[0] = 0;
	_resultInfo->forecast_end[0] = 0;
	_resultInfo->forecast_time_unit = 0;
	_resultInfo->forecast_time_step = 0;
	_resultInfo->forecast_type = 0;
	_resultInfo->num_met_forecasts = 0;
	_resultInfo->first_hist_year = 0;
	_resultInfo->last_hist_year = 0;
    }
}

