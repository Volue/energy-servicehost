#include "NativeDbAccess.h"

CNativeDbAccess::CNativeDbAccess(void)
{
}

CNativeDbAccess::~CNativeDbAccess(void)
{
}
bool 
CNativeDbAccess::StoreCustomer(otl_connect& db,const std::string customerName) {
	try {
		otl_nocommit_stream s(1,"insert into TEST_NET_MIX_NATIVE(customername) values ( :cName<char[255]> )",db);
		s<<customerName;
	} catch( otl_exception &e) {
		return false;
	}
	return true;
}
