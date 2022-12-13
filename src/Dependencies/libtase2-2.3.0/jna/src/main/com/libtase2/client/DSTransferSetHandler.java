package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import com.libtase2.common.PointValue;

/**
 * Callback handlers for data set transfer sets (DSTS)
 */
public interface DSTransferSetHandler {
    /**
     * Is called when a new DSTS report is received
     * 
     * @param transferSet transfer set instance that received the report
     * @param seq         sequence number of that particular report of the transfer
     *                    set
     * @param parameter   user provided context parameter
     */
    void startReport(ClientDSTransferSet transferSet, long seq, Object parameter);

    /**
     * Is called for each data point value that is contained in the received report
     * 
     * @param transferSet transfer set instance that received the report
     * @param domainName  domain name of the data point or null for a VCC scope data
     *                    point
     * @param pointName   data point name
     * @param value       data point value
     */
    void valueReceived(ClientDSTransferSet transferSet, String domainName, String pointName, PointValue value);

    /**
     * Is called when all data points of the received DSTS report are processed
     * 
     * This method is called after all calls to
     * {@link com.libtase2.client.DSTransferSetHandler#valueReceived valueReceived}
     * for the particular report with the given sequence number.
     * 
     * @param transferSet transfer set instance that received the report
     * @param seq         sequence number of that particular report of the transfer
     *                    set
     * @param parameter   user provided context parameter
     */
    void finishedReport(ClientDSTransferSet transferSet, long seq, Object parameter);
}
