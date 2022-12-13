package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Error codes that can be returned by client side APIs
 */
public enum ClientError {
    /* general errors */

    /**
     * No error occurred (request has been successful)
     */
    OK(0),

    /**
     * Request failed
     */
    FAILED(1),

    /**
     * Request timeout
     */
    TIMEOUT(2),

    /**
     * Access denied by server
     */
    ACCESS_DENIED(3),

    /**
     * Server resource is temporarily unavailable
     */
    TEMPORARILY_UNAVAILABLE(4),

    /**
     * invalid argument
     */
    INVALID_ARGUMENT(5),

    /**
     * server return value invalid
     */
    INVALID_RETURN_VALUE(6),

    /**
     * other error
     */
    OTHER(7),

    /**
     * object does not exist
     */
    OBJECT_NOT_EXISTING(8),

    /**
     * object already exists
     */
    OBJECT_EXISTS(9);

    private int value;

    private ClientError(int value) {
        this.value = value;
    }

    /**
     * Gets the numerical error code
     * 
     * @return numerical error code
     */
    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static ClientError fromValue(int val) {
        ClientError[] errors = ClientError.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return ClientError.OTHER;
    }
}
