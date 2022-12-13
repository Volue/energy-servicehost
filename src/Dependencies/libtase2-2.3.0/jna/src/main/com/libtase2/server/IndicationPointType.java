package com.libtase2.server;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public enum IndicationPointType {
    REAL(0), STATE(1), DISCRETE(2), STATE_SUPPLEMENTAL(3);

    private int value;

    private IndicationPointType(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static IndicationPointType fromValue(int val) {
        IndicationPointType[] errors = IndicationPointType.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return IndicationPointType.REAL;
    }
}
