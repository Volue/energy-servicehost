/*
 *  iso_presentation.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef ISO_PRESENTATION_H_
#define ISO_PRESENTATION_H_

#include "byte_buffer.h"
#include "buffer_chain.h"
#include "iso_connection_parameters.h"

typedef struct {
	PSelector callingPresentationSelector;
	PSelector calledPresentationSelector;
	IsoApplicationAddress localAddress; /* used for calling presentation selector */
	IsoApplicationAddress remoteAddress; /* used for called presentation selector */
	IsoApplicationAddress peerAddress; /* used for responding presentation selector */
	uint8_t nextContextId;
	uint8_t acseContextId;
	uint8_t mmsContextId;
	ByteBuffer nextPayload;
} IsoPresentation;

LIBMMS_INTERNAL void
IsoPresentation_initActive(IsoPresentation* self, IsoApplicationAddress localAddress, IsoApplicationAddress remoteAddress, IsoApplicationAddress peerAddress);

LIBMMS_INTERNAL void
IsoPresentation_initPassive(IsoPresentation* self, IsoApplicationAddress localAddress, IsoApplicationAddress peerAddress);

LIBMMS_INTERNAL int
IsoPresentation_parseUserData(IsoPresentation* self, ByteBuffer* message);

LIBMMS_INTERNAL int
IsoPresentation_parseConnect(IsoPresentation* self, ByteBuffer* message);

LIBMMS_INTERNAL void
IsoPresentation_createConnectPdu(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload);

LIBMMS_INTERNAL void
IsoPresentation_createCpaMessage(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload);

LIBMMS_INTERNAL void
IsoPresentation_createUserData(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload);

LIBMMS_INTERNAL void
IsoPresentation_createUserDataACSE(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload);

LIBMMS_INTERNAL int
IsoPresentation_parseAcceptMessage(IsoPresentation* self, ByteBuffer* byteBuffer);

LIBMMS_INTERNAL void
IsoPresentation_createAbortUserMessage(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload);

#endif /* ISO_PRESENTATION_H_ */
