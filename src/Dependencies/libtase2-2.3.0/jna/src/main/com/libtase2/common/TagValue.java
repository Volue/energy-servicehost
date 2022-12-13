package com.libtase2.common;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * The tag value of a device
 */
public enum TagValue {

    /** No tag present */
    NO_TAG(0),

    /** The tag prohibits to open or close the device */
    OPEN_AND_CLOSE_INHIBIT(1),

    /** Tag tag allow to open but inhibits to close the device */
    CLOSE_ONLY_INHIBIT(2),

    /** The device has an invalid tag */
    INVALID(3);

    private int value;

    private TagValue(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static TagValue fromValue(int val) {
        TagValue[] errors = TagValue.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return TagValue.NO_TAG;
    }

}
