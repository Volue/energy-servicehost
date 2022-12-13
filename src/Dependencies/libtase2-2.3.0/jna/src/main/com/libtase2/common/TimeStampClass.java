package com.libtase2.common;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public enum TimeStampClass {
    /**
     * The indication point or type has no time stamp
     */
    NO_TIMESTAMP(0),

    /**
     * The indication point or type has a time stamp with seconds precision
     */
    TIMESTAMP(1),

    /**
     * The indication point or type has a time stamp with milliseconds precision
     */
    TIMESTAMP_EXTENDED(2);

    private int value;

    private TimeStampClass(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static TimeStampClass fromValue(int val) {
        TimeStampClass[] errors = TimeStampClass.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return TimeStampClass.NO_TIMESTAMP;
    }
}
