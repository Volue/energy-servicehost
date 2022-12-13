/*
 *  tase2_api_exports.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017 MZ Automation GmbH
 *
 *  All rights reserved.
 */


#ifndef SRC_TASE2_COMMON_TASE2_API_EXPORTS_H_
#define SRC_TASE2_COMMON_TASE2_API_EXPORTS_H_

#ifdef _WIN32
    #ifdef EXPORT_FUNCTIONS_FOR_DLL
        #define TASE2_API __declspec(dllexport)
    #else
        #define TASE2_API
    #endif
#else
    #define TASE2_API
#endif

#define TASE2_INTERNAL

#endif /* SRC_TASE2_COMMON_TASE2_API_EXPORTS_H_ */
