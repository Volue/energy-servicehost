#pragma once
#include "otl.h"
class CNativeDbAccess {
public:
	CNativeDbAccess(void);
	~CNativeDbAccess(void);
	bool StoreCustomer(otl_connect& db,const std::string customerName);
};
