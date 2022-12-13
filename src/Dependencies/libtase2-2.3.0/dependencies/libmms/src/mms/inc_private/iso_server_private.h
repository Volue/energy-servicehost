/*
 *  iso_server_private.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef ISO_SERVER_PRIVATE_H_
#define ISO_SERVER_PRIVATE_H_

#include "tls_config.h"
#include "hal_socket.h"

IsoConnection
IsoConnection_create(Socket socket, IsoServer isoServer);

void
IsoConnection_destroy(IsoConnection self);

void
IsoConnection_handleTcpConnection(IsoConnection self);

void
IsoConnection_addHandleSet(const IsoConnection self, HandleSet handles);

void
private_IsoServer_increaseConnectionCounter(IsoServer self);

void
private_IsoServer_decreaseConnectionCounter(IsoServer self);

int
private_IsoServer_getConnectionCounter(IsoServer self);

/**
 * \brief User provided lock that will be called when higher layer (MMS) is called
 */
void
IsoServer_setUserLock(IsoServer self, Semaphore userLock);

void
IsoServer_userLock(IsoServer self);

void
IsoServer_userUnlock(IsoServer self);

void
IsoServer_callConnectionHandler(IsoServer self, IsoConnection connection, IsoConnectionIndication indication);

bool
IsoConnection_isRunning(IsoConnection self);

IsoApplicationAddress
IsoServer_getLocalAppAddr(IsoServer self);

#endif /* ISO_SERVER_PRIVATE_H_ */
