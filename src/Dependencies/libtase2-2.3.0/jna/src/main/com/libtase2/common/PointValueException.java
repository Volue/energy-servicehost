package com.libtase2.common;

/*
 *  Copyright 2020 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public class PointValueException extends Exception {

    private static final long serialVersionUID = 8626902312035196087L;

    private PointValueError error;

    public PointValueException(String string, PointValueError error) {
        super(string);
        this.error = error;
    }

    public PointValueError getError() {
        return error;
    }
}
