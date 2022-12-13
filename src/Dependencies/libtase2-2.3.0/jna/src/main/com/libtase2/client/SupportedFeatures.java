package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Represents the supported features (supported conformance blocks - CB) of the
 * remote endpoint
 */
public class SupportedFeatures {
    public static final int FB_1 = 1;
    public static final int FB_2 = 2;
    public static final int FB_3 = 4;
    public static final int FB_4 = 8;
    public static final int FB_5 = 16;
    public static final int FB_6 = 32;
    public static final int FB_7 = 64;
    public static final int FB_8 = 128;
    public static final int FB_9 = 256;
    public static final int FB_10 = 512;

    private int value;

    public SupportedFeatures(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }

    @Override
    public String toString() {
        String res = "";

        if ((value & FB_1) == FB_1)
            res += "[1]";
        if ((value & FB_2) == FB_2)
            res += "[2]";
        if ((value & FB_3) == FB_3)
            res += "[3]";
        if ((value & FB_4) == FB_4)
            res += "[4]";
        if ((value & FB_5) == FB_5)
            res += "[5]";
        if ((value & FB_6) == FB_6)
            res += "[6]";
        if ((value & FB_7) == FB_7)
            res += "[7]";
        if ((value & FB_8) == FB_8)
            res += "[8]";
        if ((value & FB_9) == FB_9)
            res += "[9]";
        if ((value & FB_10) == FB_10)
            res += "[9]";

        return res;
    }
}
