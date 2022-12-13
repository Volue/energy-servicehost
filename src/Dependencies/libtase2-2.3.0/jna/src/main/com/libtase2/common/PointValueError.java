package com.libtase2.common;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public enum PointValueError {

    OK(0), WRONG_TYPE(1);

    private int value;

    private PointValueError(int value) {
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

    public static PointValueError fromValue(int val) {
        PointValueError[] errors = PointValueError.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return PointValueError.OK;
    }

}
