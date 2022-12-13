/*
 *  libmms_common_api.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef LIBMMS_COMMON_API_INCLUDES_H_
#define LIBMMS_COMMON_API_INCLUDES_H_

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __GNUC__
#define ATTRIBUTE_PACKED __attribute__ ((__packed__))
#else
#define ATTRIBUTE_PACKED
#endif

#ifndef DEPRECATED
#if defined(__GNUC__) || defined(__clang__)
  #define DEPRECATED __attribute__((deprecated))
#else
  #define DEPRECATED
#endif
#endif

#if defined _WIN32 || defined __CYGWIN__
    #ifdef EXPORT_FUNCTIONS_FOR_DLL
        #define LIBMMS_API __declspec(dllexport)
    #else
        #define LIBMMS_API
    #endif

    #define LIBMMS_INTERNAL
#else
    #if __GNUC__ >= 4
        #define LIBMMS_API __attribute__ ((visibility ("default")))
        #define LIBMMS_INTERNAL  __attribute__ ((visibility ("hidden")))
    #else
        #define LIBMMS_API
        #define LIBMMS_INTERNAL
    #endif
#endif

#include "hal_time.h"

#endif /* LIBMMS_COMMON_API_INCLUDES_H_ */
