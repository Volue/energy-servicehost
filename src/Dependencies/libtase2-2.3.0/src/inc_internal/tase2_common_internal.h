/*
 *  tase2_common_interal.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2018 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef SRC_TASE2_COMMON_TASE2_COMMON_INTERNAL_H_
#define SRC_TASE2_COMMON_TASE2_COMMON_INTERNAL_H_

/* to avoid problems with VS 2013 */
#if defined(_MSC_VER)
#define inline __inline
#endif

#include "mms_value.h"
#include "tase2_common.h"
#include "tase2_version.h"

#define TASE2_MAX_LOG_MESSAGE_SIZE 512

#ifdef TASE2_DEBUG_OUTPUT

void
tase2_log(Tase2_LogLevel level, Tase2_LogSource source, Tase2_Endpoint endpoint, Tase2_Endpoint_Connection peer, const char* fmt, ...);

#else
#define tase2_log(...) {do {} while(0);}
#endif


MmsValue*
tase2_createObjectNameValue(const char* domainName, const char* name);

MmsValue*
tase2_createDSConditionsValue(bool intervalTimeout, bool integrityTimeout, bool objectChange, bool operatorRequest, bool otherEvent);


struct sTase2_PointValue {
    Tase2_PointValueType type;
    MmsValue* pointValue;
};

#endif /* SRC_TASE2_COMMON_TASE2_COMMON_INTERNAL_H_ */
