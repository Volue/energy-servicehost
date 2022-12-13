
#pragma once
/*
 This file defines the needed DBMS_API to control import/export from 
 dll. Currently DBMS_API is so_classes + dbms_classes_oracle
*/
#ifndef DBMS_API

#ifdef WIN32
#if !defined(STATIC_DBMS_API)
#	ifdef BUILD_DBMS_API
#		define DBMS_API __declspec(dllexport)
#	else
#		define DBMS_API __declspec(dllimport)
#	endif
#else
#	define DBMS_API
#endif
#else
#   define DBMS_API
#endif

#endif
