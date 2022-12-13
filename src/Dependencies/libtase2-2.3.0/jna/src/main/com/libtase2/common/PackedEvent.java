package com.libtase2.common;

/**
 * Value flags for protection equipment data points of type PACKED_EVENT.
 */
public class PackedEvent {
    public static final int GENERAL = 1;
    public static final int PHASE_1 = 2;
    public static final int PHASE_2 = 4;
    public static final int PHASE_3 = 8;
    public static final int EARTH = 16;
    public static final int REVERSE = 32;

    private int value;

    public PackedEvent(byte value) {
        this.value = ((int) value) & 0xff;
    }

    public PackedEvent(int value) {
        this.value = value;
    }

    public PackedEvent(boolean general, boolean phase1, boolean phase2, boolean phase3, boolean earth,
            boolean reverse) {
        this.value = 0;

        if (general)
            this.value += GENERAL;
        if (phase1)
            this.value += PHASE_1;
        if (phase2)
            this.value += PHASE_2;
        if (phase3)
            this.value += PHASE_3;
        if (earth)
            this.value += EARTH;
        if (reverse)
            this.value += REVERSE;
    }

    public int getValue() {
        return value;
    }

    public boolean getGeneral() {
        return ((value & GENERAL) != 0);
    }

    public boolean getPhase1() {
        return ((value & PHASE_1) != 0);
    }

    public boolean getPhase2() {
        return ((value & PHASE_2) != 0);
    }

    public boolean getPhase3() {
        return ((value & PHASE_3) != 0);
    }

    public boolean getEarth() {
        return ((value & EARTH) != 0);
    }

    public boolean getReverse() {
        return ((value & REVERSE) != 0);
    }
}
