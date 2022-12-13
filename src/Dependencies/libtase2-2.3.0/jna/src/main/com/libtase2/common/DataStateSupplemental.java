package com.libtase2.common;

/*
 *  Copyright 2018-2020 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * State flags of a STATE_SUPPLEMENTAL type indication point
 */
public class DataStateSupplemental {
    public static final int HI = 1;
    public static final int LO = 2;
    public static final int TAG_HI = 4;
    public static final int TAG_LO = 8;
    public static final int EXPECTED_HI = 16;
    public static final int EXPECTED_LO = 32;

    private static int flipBits(int value) {
        if (value == 1)
            return 2;
        if (value == 2)
            return 1;

        return value;
    }

    private int value;

    public DataStateSupplemental(byte value) {
        this.value = ((int) value) & 0xff;
    }

    public DataStateSupplemental(int value) {
        this.value = value;
    }

    public DataStateSupplemental(StateValue state, TagValue tag, StateValue expected) {
        this.value = 0;
        this.value += flipBits(state.getValue());
        this.value += (flipBits(tag.getValue()) << 2);
        this.value += (flipBits(expected.getValue()) << 4);
    }

    public int getValue() {
        return value;
    }

    public StateValue getState() {
        int stateVal = value & 0x03;

        return StateValue.fromValue(flipBits(stateVal));
    }

    public TagValue getTag() {
        int tagVal = (value >> 2) & 0x03;

        return TagValue.fromValue(flipBits(tagVal));
    }

    public StateValue getExpected() {
        int expectedVal = (value >> 4) & 0x03;

        return StateValue.fromValue(flipBits(expectedVal));
    }
}
