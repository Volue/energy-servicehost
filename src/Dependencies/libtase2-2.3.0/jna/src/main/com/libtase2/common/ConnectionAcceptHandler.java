package com.libtase2.common;

import com.libtase2.server.BilateralTable;

/*
 *  Copyright 2020 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Callback interface to accept or reject new connections
 */
public interface ConnectionAcceptHandler {
    /**
     * Called when an active endpoint tries to connect
     * 
     * @param endpoint    local endpoint instance
     * @param peerAddress IP address and TCP port of the peer as string
     * @param parameter   user provided context parameter
     * @param authMethod  authentication method specifier
     * @param authValue   authentication value (e.g. password or TLS certificate)
     * @param blt         BilateralTable instance that is matching the parameters of
     *                    the connecting peer
     * 
     * @return when true, accept connection, otherwise reject connection
     */
    boolean connectionRequest(Endpoint endpoint, Object parameter, String peerAddress, BilateralTable blt,
            AuthenticationMethod authMethod, byte[] authValue);
}
