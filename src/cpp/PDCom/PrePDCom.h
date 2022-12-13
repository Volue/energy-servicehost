/*
	Precompile header support.

	Copyright (C) Powel Data AS
	All rights reserved.
*/
#ifdef WIN32
#ifndef _PrePDCom_h_
#define _PrePDCom_h_
#pragma once
#ifndef STRICT
#define STRICT // Enable STRICT Type Checking in Windows headers
#endif
// Seems use of PDCom_i.h does not compile with WIN32_LEAN_AND_MEAN.
//#ifndef WIN32_LEAN_AND_MEAN 
//#define WIN32_LEAN_AND_MEAN // To speed the build process exclude rarely-used services from Windows headers
//#endif
#ifndef NOMINMAX
#define NOMINMAX // Exclude min/max macros from Windows header
#endif
//#define VC_EXTRALEAN			// Exclude rarely-used stuff from Windows headers
#include <iostream>
//#include <afxwin.h>			// MFC core and standard components
//#include <afxext.h>			// MFC extensions
//#include <afxdisp.h>			// MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <atlbase.h>
#include "PDComModule.h"
#include <atlcom.h>
#include <PDCom_i.h>
/* Message definition for PDCOM notification */
#define PDCOM_NEW_MESSAGE_ARRIVED WM_USER + 1
#endif
#endif

