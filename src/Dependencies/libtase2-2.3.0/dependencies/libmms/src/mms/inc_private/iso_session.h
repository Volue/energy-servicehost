/*
 *  ise_session.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef ISE_SESSION_H_
#define ISE_SESSION_H_

#include "byte_buffer.h"
#include "buffer_chain.h"
#include "iso_connection_parameters.h"

typedef struct {
	SSelector callingSessionSelector;
	SSelector calledSessionSelector;
	uint16_t sessionRequirement;
	uint8_t protocolOptions;
	ByteBuffer userData;
} IsoSession;

typedef enum {
	SESSION_OK,
	SESSION_ERROR,
	SESSION_CONNECT,
	SESSION_GIVE_TOKEN,
	SESSION_DATA,
	SESSION_ABORT,
	SESSION_FINISH,
	SESSION_DISCONNECT,
	SESSION_NOT_FINISHED
} IsoSessionIndication;

LIBMMS_INTERNAL void
IsoSession_init(IsoSession* session);

LIBMMS_INTERNAL ByteBuffer*
IsoSession_getUserData(IsoSession* session);

LIBMMS_INTERNAL void
IsoSession_createConnectSpdu(IsoSession* self, IsoApplicationAddress remoteAddress, IsoApplicationAddress localAddress, BufferChain buffer, BufferChain payload);


LIBMMS_INTERNAL IsoSessionIndication
IsoSession_parseMessage(IsoSession* session, ByteBuffer* message);

LIBMMS_INTERNAL void
IsoSession_createDataSpdu(IsoSession* session, BufferChain buffer, BufferChain payload);

LIBMMS_INTERNAL void
IsoSession_createAcceptSpdu(IsoSession* self, BufferChain buffer, BufferChain payload);

LIBMMS_INTERNAL void
IsoSession_createAbortSpdu(IsoSession* self, BufferChain buffer, BufferChain payload);

LIBMMS_INTERNAL void
IsoSession_createFinishSpdu(IsoSession* self, BufferChain buffer, BufferChain payload);

LIBMMS_INTERNAL void
IsoSession_createDisconnectSpdu(IsoSession* self, BufferChain buffer, BufferChain payload);

#endif /* ISE_SESSION_H_ */
