package com.libtase2.common;

/*
 *  Copyright 2018-2020 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * State and quality flags of a STATE type indication point
 */
public class DataState {
    public static final int HI = 1;
    public static final int LO = 2;
    public static final int VALIDITY_HI = 4;
    public static final int VALIDITY_LO = 8;
    public static final int CURRENT_SOURCE_HI = 16;
    public static final int CURRENT_SOURCE_LO = 32;
    public static final int NORMAL_VALUE = 64;
    public static final int TIME_STAMP_QUALITY = 128;

    private int value;

    private static int flipBits(int value) {
        if (value == 1)
            return 2;
        if (value == 2)
            return 1;

        return value;
    }

    public DataState(byte value) {
        this.value = ((int) value) & 0xff;
    }

    public DataState(int value) {
        this.value = value;
    }

    public DataState(StateValue state) {
        this.value = 0;
        this.value += flipBits(state.getValue());
    }

    public DataState(StateValue state, Validity validity) {
        this.value = 0;
        this.value += flipBits(state.getValue());
        this.value += (flipBits(validity.getValue()) << 2);
    }

    public DataState(StateValue stateValue, Validity validity, Source source) {
        this.value = 0;
        this.value += flipBits(stateValue.getValue());
        this.value += (flipBits(validity.getValue()) << 2);
        this.value += (flipBits(source.getValue()) << 4);
    }

    public DataState(StateValue state, Validity validity, Source source, boolean abnormal, boolean timestampInvalid) {
        this.value = 0;
        this.value += flipBits(state.getValue());
        this.value += (flipBits(validity.getValue()) << 2);
        this.value += (flipBits(source.getValue()) << 4);
        if (abnormal)
            this.value += NORMAL_VALUE;
        if (timestampInvalid)
            this.value += TIME_STAMP_QUALITY;
    }

    public int getValue() {
        return value;
    }

    public StateValue getState() {
        int stateVal = value & 0x03;

        return StateValue.fromValue(flipBits(stateVal));
    }

    public Validity getValidity() {
        int validityVal = (value >> 2) & 0x03;

        return Validity.fromValue(flipBits(validityVal));
    }

    public Source getSource() {
        int sourceVal = (value >> 4) & 0x03;

        return Source.fromValue(flipBits(sourceVal));
    }

    public boolean isAbnormal() {
        return ((value & NORMAL_VALUE) != 0);
    }

    public boolean isTimestampInvalid() {
        return ((value & TIME_STAMP_QUALITY) != 0);
    }
}
