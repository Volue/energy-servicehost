/*
 *  conversions.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef CONVERSIONS_H_
#define CONVERSIONS_H_

#include "libmms_platform_includes.h"

void
Conversions_intToStringBuffer(int intValue, int numberOfDigits, uint8_t* buffer);

void
Conversions_msTimeToGeneralizedTime(uint64_t msTime, uint8_t* buffer);

uint64_t
Conversions_generalizedTimeToMsTime(const char* gtString);

void
memcpyReverseByteOrder(uint8_t* dst, const uint8_t* src, int size);

#endif /* CONVERSIONS_H_ */
