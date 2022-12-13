/*
$Header$
Description:
	Precomp. header support, and ATL inst. support.
	
Copyright (C) Powel Data AS
All rights reserved.
*/

#include <PrePDCom.h>
#ifdef WIN32
#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#if _MSC_VER < 1300
#include <statreg.cpp>
#endif
#endif

#if _MSC_VER < 1300
#include <atlimpl.cpp>
#endif
#endif
#include "PDCom_i.c"
