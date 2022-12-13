/*
 *  ds_transfer_set.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef SRC_TASE2_SERVER_DS_TRANSFER_SET_H_
#define SRC_TASE2_SERVER_DS_TRANSFER_SET_H_

MmsDataAccessError
tase2_Server_handleDSTransferSetWrite(Tase2_Server self, MmsDomain* domain, Tase2_DSTransferSet ts, const char* componentName,
        MmsValue* value, MmsEndpointConnection connection, Tase2_BilateralTable blt, Tase2_Endpoint_Connection peer);

void
tase2_Server_checkTransferSets(Tase2_Server self);

void
tase2_Server_deactivateDSTransferSet(Tase2_Server self, Tase2_DSTransferSet transferSet, Tase2_Endpoint_Connection peer);

void
tase2_Server_deactivateTransferSets(Tase2_Server self, MmsEndpointConnection connection, Tase2_Endpoint_Connection peer);

Tase2_DSTransferSet
tase2_Server_getNextDSTransferSet(Tase2_Server self, Tase2_Domain domain, MmsEndpointConnection connection);

void
tase2_Server_triggerTransferSets(Tase2_Server self, Tase2_DataPoint dataPoint);

void
tase2_DSTransferSet_checkVariable(Tase2_DSTransferSet ts, Tase2_Server server, Tase2_DataPoint dataPoint);


#endif /* SRC_TASE2_SERVER_DS_TRANSFER_SET_H_ */
