// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT // Enable STRICT Type Checking in Windows headers
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // To speed the build process exclude rarely-used services from Windows headers
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

#include <atlbase.h>
#include <atlcom.h>
//#include <atlwin.h>
#include <atltypes.h>
//#include <atlctl.h>
#include <atlhost.h>

#include <comdef.h>
#include <atlconv.h>
#include <stdlib.h>

using namespace ATL;
#include "PDVariantUtility.h"
using namespace PowelVariantUtility;
#include <string>
#include <iostream>
#include <fstream>

//Turn off standard trace mechanism
#define NO_OTL_TRACE
#include "otl.h"
#include <windows.h>
#include <log4cxx/logger.h> // Include early to avoid C4251.
#include <stream_formats.h>

void CfgModuleInit();
void CfgModuleExit();