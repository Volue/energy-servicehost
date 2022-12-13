/*
 *  ber_decode.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef BER_DECODER_H_
#define BER_DECODER_H_

#include "iso_connection_parameters.h"

int
BerDecoder_decodeLength(uint8_t* buffer, int* length, int bufPos, int maxBufPos);

char*
BerDecoder_decodeString(uint8_t* buffer, int strlen, int bufPos, int maxBufPos);

uint32_t
BerDecoder_decodeUint32(uint8_t* buffer, int intlen, int bufPos);

int32_t
BerDecoder_decodeInt32(uint8_t* buffer, int intlen, int bufPos);

float
BerDecoder_decodeFloat(uint8_t* buffer, int bufPos);

double
BerDecoder_decodeDouble(uint8_t* buffer, int bufPos);

bool
BerDecoder_decodeBoolean(uint8_t* buffer, int bufPos);

void
BerDecoder_decodeOID(uint8_t* buffer, int bufPos, int length, ItuObjectIdentifier* oid);

#endif /* BER_DECODER_H_ */
