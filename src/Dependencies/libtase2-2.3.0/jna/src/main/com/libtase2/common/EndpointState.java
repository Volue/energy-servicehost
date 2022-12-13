package com.libtase2.common;

/*
 *  Copyright 2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Represents the current state of an {@link com.libtase2.common.Endpoint
 * Endpoint} instance
 */
public enum EndpointState {
    /**
     * endpoint has not been used yet.
     */
    IDLE(0),

    /**
     * endpoint is listening for incoming TCP client connections.
     */
    LISTENING(1),

    /**
     * active endpoint is connected to remote TCP server.
     */
    CONNECTED(2),

    /**
     * active endpoint is connecting to remote TCP server.
     */
    CONNECTING(3),

    /**
     * connection attempt failed or endpoint cannot allocate local TCP port.
     */
    ERROR(4);

    private int value;

    private EndpointState(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static EndpointState fromValue(int val) {
        EndpointState[] stateVals = EndpointState.values();

        for (int i = 0; i < stateVals.length; i++) {
            if (stateVals[i].compare(val))
                return stateVals[i];
        }

        return EndpointState.IDLE;
    }
}
