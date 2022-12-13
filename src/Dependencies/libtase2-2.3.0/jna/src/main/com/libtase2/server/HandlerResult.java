package com.libtase2.server;

/*
 *  Copyright 2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Type for the result of callback handlers for operations
 */
public enum HandlerResult {
    /**
     * Operation has been successful.
     */
    SUCCESS(-1),

    /**
     * Operation cannot be executed because of an hardware fault
     */
    HARDWARE_FAULT(1),

    /**
     * The requested resource is temporarily unavailable.
     */
    TEMPORARILY_UNAVAILABLE(2),

    /**
     * active endpoint is connecting to remote TCP server.
     */
    CONNECTING(3),

    /**
     * Operation is rejected because of an invalid parameter value.
     */
    OBJECT_VALUE_INVALID(11);

    private int value;

    private HandlerResult(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static HandlerResult fromValue(int val) {
        HandlerResult[] stateVals = HandlerResult.values();

        for (int i = 0; i < stateVals.length; i++) {
            if (stateVals[i].compare(val))
                return stateVals[i];
        }

        return HandlerResult.SUCCESS;
    }
}
