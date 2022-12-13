package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Callback handlers for information message transfer sets (IMTS)
 */
public interface InformationMessageHandler {
    /**
     * Is called whenever a new information message is received.
     * 
     * @param parameter     user provided context parameter
     * @param infoRef       the info-reference field of the information message
     * @param localRef      the local-reference field of the information message
     * @param msgId         the msg-id field of the information message
     * @param messageBuffer the information message payload data
     */
    void newMessage(Object parameter, int infoRef, int localRef, int msgId, byte[] messageBuffer);
}
