package com.libtase2.server;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public enum ControlPointType {
    COMMAND(0), SETPOINT_REAL(1), SETPOINT_DESCRETE(2);

    private int value;

    private ControlPointType(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static ControlPointType fromValue(int val) {
        ControlPointType[] errors = ControlPointType.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return ControlPointType.COMMAND;
    }
}
