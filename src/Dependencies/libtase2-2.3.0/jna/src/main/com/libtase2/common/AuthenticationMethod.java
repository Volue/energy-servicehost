package com.libtase2.common;

/*
 *  Copyright 2020 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public enum AuthenticationMethod {
    /**
     * Neither ACSE nor TLS authentication used
     */
    NONE(0),

    /**
     * Use ACSE password for peer authentication
     */
    PASSWORD(1),

    /**
     * Use ACSE certificate for peer authentication
     */
    CERTIFICATE(2),

    /**
     * Use TLS certificate for peer authentication
     */
    TLS(3),

    /**
     * Unknown mechanism for peer authentication
     */
    UNKNOWN(99);

    private int value;

    private AuthenticationMethod(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static AuthenticationMethod fromValue(int val) {
        AuthenticationMethod[] stateVals = AuthenticationMethod.values();

        for (int i = 0; i < stateVals.length; i++) {
            if (stateVals[i].compare(val))
                return stateVals[i];
        }

        return AuthenticationMethod.UNKNOWN;
    }
}
