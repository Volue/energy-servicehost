/*
 *  cotp.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef COTP_H_
#define COTP_H_

#include "libmms_platform_includes.h"
#include "byte_buffer.h"
#include "buffer_chain.h"
#include "hal_socket.h"
#include "iso_connection_parameters.h"
#include "tls_socket.h"

typedef struct {
    TSelector tSelSrc;
    TSelector tSelDst;
    uint8_t tpduSize;
} CotpOptions;

typedef struct {
    int state;
    int remoteRef;
    int localRef;
    int protocolClass;

    Socket socket;
//#if (CONFIG_MMS_SUPPORT_TLS == 1)
    TLSSocket tlsSocket;
//#endif

    CotpOptions options;
    bool isLastDataUnit;
    ByteBuffer* payload;
    ByteBuffer* writeBuffer;  /* buffer to store TPKT packet to send */
    ByteBuffer* readBuffer;   /* buffer to store received TPKT packet */
    uint16_t packetSize;      /* size of the packet currently received */
} CotpConnection;

typedef enum {
    COTP_OK,
    COTP_ERROR,
    COTP_CONNECT_INDICATION,
    COTP_DATA_INDICATION,
    COTP_DISCONNECT_INDICATION,
    COTP_MORE_FRAGMENTS_FOLLOW
} CotpIndication;

typedef enum {
    TPKT_PACKET_COMPLETE = 0,
    TPKT_WAITING = 1,
    TPKT_ERROR = 2
} TpktState;

LIBMMS_INTERNAL int /* in byte */
CotpConnection_getTpduSize(CotpConnection* self);

LIBMMS_INTERNAL void
CotpConnection_setTpduSize(CotpConnection* self, int tpduSize /* in byte */);

LIBMMS_INTERNAL void
CotpConnection_init(CotpConnection* self, Socket socket,
        ByteBuffer* payloadBuffer, ByteBuffer* readBuffer, ByteBuffer* writeBuffer);

LIBMMS_INTERNAL CotpIndication
CotpConnection_parseIncomingMessage(CotpConnection* self);

LIBMMS_INTERNAL void
CotpConnection_resetPayload(CotpConnection* self);

LIBMMS_INTERNAL TpktState
CotpConnection_readToTpktBuffer(CotpConnection* self);

LIBMMS_INTERNAL CotpIndication
CotpConnection_sendConnectionRequestMessage(CotpConnection* self, IsoApplicationAddress remoteAddress, IsoApplicationAddress localAddress);

LIBMMS_INTERNAL CotpIndication
CotpConnection_sendConnectionResponseMessage(CotpConnection* self);

LIBMMS_INTERNAL CotpIndication
CotpConnection_sendDataMessage(CotpConnection* self, BufferChain payload);

LIBMMS_INTERNAL ByteBuffer*
CotpConnection_getPayload(CotpConnection* self);

LIBMMS_INTERNAL int
CotpConnection_getRemoteRef(CotpConnection* self);

LIBMMS_INTERNAL int
CotpConnection_getLocalRef(CotpConnection* self);

#endif /* COTP_H_ */
