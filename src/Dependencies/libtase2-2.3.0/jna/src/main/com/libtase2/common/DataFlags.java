package com.libtase2.common;

/*
 *  Copyright 2018-2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Flags (quality values) of an indication point
 */
public class DataFlags {
    public static final int UNUSED_0 = 1;
    public static final int UNUSED_1 = 2;
    public static final int VALIDITY_HI = 4;
    public static final int VALIDITY_LO = 8;
    public static final int CURRENT_SOURCE_HI = 16;
    public static final int CURRENT_SOURCE_LO = 32;
    public static final int NORMAL_VALUE = 64;
    public static final int TIME_STAMP_QUALITY = 128;

    private static int flipBits(int value) {
        if (value == 1)
            return 2;
        if (value == 2)
            return 1;

        return value;
    }

    private int value;

    public DataFlags(byte value) {
        this.value = ((int) value) & 0xff;
    }

    public DataFlags(int value) {
        this.value = value;
    }

    public DataFlags(Validity validity) {
        this.value = 0;
        this.value += (flipBits(validity.getValue()) >> 2);
    }

    public DataFlags(Validity validity, Source source) {
        this.value = 0;
        this.value += (flipBits(validity.getValue()) >> 2);
        this.value += (flipBits(source.getValue()) >> 4);
    }

    public DataFlags(Validity validity, Source source, boolean abnormal, boolean timestampInvalid) {
        this.value = 0;
        this.value += (flipBits(validity.getValue()) >> 2);
        this.value += (flipBits(source.getValue()) >> 4);
        if (abnormal)
            this.value += NORMAL_VALUE;
        if (timestampInvalid)
            this.value += TIME_STAMP_QUALITY;
    }

    public int getValue() {
        return value;
    }

    public Validity getValidity() {
        int validityVal = (value << 2) & 0x03;

        return Validity.fromValue(flipBits(validityVal));
    }

    public Source getSource() {
        int sourceVal = (value << 4) & 0x03;

        return Source.fromValue(flipBits(sourceVal));
    }

    public boolean isAbnormal() {
        return ((value & NORMAL_VALUE) != 0);
    }

    public boolean isTimestampInvalid() {
        return ((value & TIME_STAMP_QUALITY) != 0);
    }

    @Override
    public String toString() {
        String str = getValidity().toString();

        str += "|" + getSource().toString();

        return str;
    }
}
