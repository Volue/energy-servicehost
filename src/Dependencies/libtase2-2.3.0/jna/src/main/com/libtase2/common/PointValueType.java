package com.libtase2.common;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Represents the type or estimated type of a
 * ({@link com.libtase2.common.PointValue PointValue} instance
 */
public enum PointValueType {

    /**
     * STATE type (double point value with flags)
     */
    STATE(0),

    /**
     * STATE_SUPPLEMENTAL type (double point value with tag and expected value)
     */
    STATE_SUPPLEMENTAL(1),

    /**
     * DISCRETE type (integer type value)
     */
    DISCRETE(2),

    /**
     * REAL type (32 bit floating point)
     */
    REAL(3),

    /**
     * Single protection event type
     */
    SINGLE_EVENT(4),

    /**
     * Packed protection event type (multiple phases with neutral)
     */
    PACKED_EVENT(5),

    /**
     * 
     */
    ERROR(99),

    /**
     * Type cannot be determined or is not supported by the library
     */
    UNKNOWN(100),

    /**
     * Type is one of STATE or STATE_SUPPLEMENTAL (cannot be determined by received
     * MMS variable)
     */
    STATE_OR_STATE_SUPPLEMENTAL(101),

    /**
     * Type is one of STATE or DISCRETE (cannot be determined by received MMS
     * variable)
     */
    STATE_OR_DISCRETE(102);

    private int value;

    private PointValueType(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static PointValueType fromValue(int val) {
        PointValueType[] errors = PointValueType.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return PointValueType.UNKNOWN;
    }
}
