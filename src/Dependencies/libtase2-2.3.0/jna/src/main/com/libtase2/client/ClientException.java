package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public class ClientException extends Exception {
    /**
     * 
     */
    private static final long serialVersionUID = 4230269704438581328L;
    private int errorCode;

    public ClientException(String string, int error) {
        super(string);
        errorCode = error;
    }

    /**
     * Get the error code of the exception
     * 
     * @return the error code
     */
    public ClientError getError() {
        return ClientError.fromValue(errorCode);
    }

    public ClientException() {

    }
}
