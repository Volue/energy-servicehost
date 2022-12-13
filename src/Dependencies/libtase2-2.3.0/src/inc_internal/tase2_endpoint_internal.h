/*
 *  tase2_endpoint_internal.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef TASE2_ENDPOINT_INTERNAL_H_
#define TASE2_ENDPOINT_INTERNAL_H_

#include "tase2_endpoint.h"
#include "mms_endpoint.h"

TASE2_INTERNAL MmsEndpoint
Tase2_Endpoint_getMmsEndpoint(Tase2_Endpoint self);

TASE2_INTERNAL Tase2_Endpoint_Connection
Tase2_Endpoint_getLockedEndpointConnection(Tase2_Endpoint self, MmsEndpointConnection mmsEndpointConnection);

TASE2_INTERNAL void
Tase2_Endpoint_unlockEndpointConnection(Tase2_Endpoint self, Tase2_Endpoint_Connection connection);

TASE2_INTERNAL Tase2_Endpoint_Connection
Tase2_Endpoint_getMatchingEndpointConnection(Tase2_Endpoint self, IsoApplicationAddress remoteAddress);

TASE2_INTERNAL Tase2_Endpoint_Connection
Tase2_Endpoint_lookupConnection(Tase2_Endpoint self, MmsEndpointConnection mmsEndpointConnection);

TASE2_INTERNAL void
Tase2_Endpoint_addClient(Tase2_Endpoint self, Tase2_Client client);

TASE2_INTERNAL void
Tase2_Endpoint_installInformationReportHandler(Tase2_Endpoint self, Tase2_Client client, MmsInformationReportHandler handler, void* parameter);

TASE2_INTERNAL void
Tase2_Endpoint_installAppTickHandler(Tase2_Endpoint self, MmsAppTickHandler handler, void* parameter);

TASE2_INTERNAL bool
Tase2_Endpoint_matchAddresses(Tase2_Endpoint self, IsoApplicationAddress addr1, IsoApplicationAddress addr2);

TASE2_INTERNAL MmsEndpointConnection
Tase2_Endpoint_Connection_getMmsConnection(Tase2_Endpoint_Connection self);

TASE2_INTERNAL void
Tase2_Endpoint_Connection_destroy(Tase2_Endpoint_Connection self);

#endif /* TASE2_ENDPOINT_INTERNAL_H_ */
