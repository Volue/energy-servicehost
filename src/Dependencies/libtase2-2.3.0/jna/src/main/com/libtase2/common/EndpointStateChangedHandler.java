package com.libtase2.common;

/*
 *  Copyright 2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Callback handler for {@link com.libtase2.common.Endpoint Endpoint} state
 * changes
 */
public interface EndpointStateChangedHandler {
    /**
     * Called whenever the state of the endpoint changed
     * 
     * @param endpoint  the endpoint instance that changed its state
     * @param parameter user provided context parameter
     * @param newState  the new state of the endpoint
     */
    void stateChanged(Endpoint endpoint, Object parameter, EndpointState newState);
}
