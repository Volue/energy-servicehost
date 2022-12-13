package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Represents the current state of a {@link com.libtase2.client.Client Client}
 * instance
 */
public enum ClientState {

    /**
     * Idle (not connected; ready to connect)
     */
    IDLE(0),

    /**
     * Connection to server is established
     */
    CONNECTED(1),

    /**
     * Connection is closed
     */
    CLOSED(2);

    private int value;

    private ClientState(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static ClientState fromValue(int val) {
        ClientState[] errors = ClientState.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return ClientState.IDLE;
    }
}
