package com.libtase2.server;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public enum DeviceClass {
    SBO(0), DIRECT(1);

    private int value;

    private DeviceClass(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static DeviceClass fromValue(int val) {
        DeviceClass[] errors = DeviceClass.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return DeviceClass.DIRECT;
    }
}
