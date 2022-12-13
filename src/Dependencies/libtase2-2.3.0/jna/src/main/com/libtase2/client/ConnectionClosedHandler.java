package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * @deprecated Use {@link com.libtase2.client.StateChangedHandler
 *             StateChangedHandler} instead
 */
public interface ConnectionClosedHandler {
    /**
     * Connection to server is closed
     * 
     * @param parameter user provided parameter
     * @param client    the client connection that is closed
     */
    void connectionClosed(Object parameter, Client client);
}
