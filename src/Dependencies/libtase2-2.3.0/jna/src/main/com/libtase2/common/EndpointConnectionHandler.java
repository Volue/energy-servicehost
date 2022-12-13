package com.libtase2.common;

/*
 *  Copyright 2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Callback interface to handle connection events (connect/disconnect) for an
 * Endpoint
 */
public interface EndpointConnectionHandler {
    /**
     * Called when another endpoint is connected
     * 
     * @param endpoint   local endpoint instance
     * @param parameter  user provided context parameter
     * @param connection the new connection object
     */
    void connected(Endpoint endpoint, Object parameter, EndpointConnection connection);

    /**
     * Called when another endpoint is disconnected
     * 
     * @param endpoint   local endpoint instance
     * @param parameter  user provided context parameter
     * @param connection the connection object representing the closed connection
     */
    void disconnected(Endpoint endpoint, Object parameter, EndpointConnection connection);
}
