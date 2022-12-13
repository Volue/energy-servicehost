// NetNativeOtlDbAcess.h

#pragma once
#include "NativeDbAccess.h"
#include "MarshalUtils.h"
#include "winerror.h"
using namespace clix;
using namespace System;
using namespace OTLNET;

namespace NetNativeOtlDbAcess {
		static inline otl_connect& getOtlConnect(dbconnect^db) {
			return *((otl_connect*)db->getDbHandle());
		}

	public ref class Wrapper {
	public:
		bool StoreCustomer(dbconnect^db, String^ customerName) {
			CNativeDbAccess dba;
			return dba.StoreCustomer( getOtlConnect(db),marshalString<E_ANSI>(customerName) );
		}
	};
}
