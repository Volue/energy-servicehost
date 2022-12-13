package com.libtase2.common;

/**
 * Quality flags for protection equipment data points of type PACKED_EVENT.
 */
public class PackedEventFlags {
    public static final int ELAPSED_TIME_VALIDITY = 1;
    public static final int BLOCKED = 2;
    public static final int SUBSTITUTED = 4;
    public static final int TOPICAL = 8;
    public static final int EVENT_VALIDITY = 16;

    private int value;

    public PackedEventFlags(byte value) {
        this.value = ((int) value) & 0xff;
    }

    public PackedEventFlags(int value) {
        this.value = value;
    }

    public PackedEventFlags(boolean elapsedTimeValidity, boolean blocked, boolean substituted, boolean topcial,
            boolean validity) {
        this.value = 0;
        if (elapsedTimeValidity)
            this.value += ELAPSED_TIME_VALIDITY;
        if (blocked)
            this.value += ELAPSED_TIME_VALIDITY;
        if (substituted)
            this.value += SUBSTITUTED;
        if (topcial)
            this.value += TOPICAL;
        if (validity)
            this.value += ELAPSED_TIME_VALIDITY;
    }

    public int getValue() {
        return value;
    }

    public boolean isElapsedTimeInvalid() {
        return ((value & ELAPSED_TIME_VALIDITY) != 0);
    }

    public boolean isBlocked() {
        return ((value & BLOCKED) != 0);
    }

    public boolean isSubstituted() {
        return ((value & SUBSTITUTED) != 0);
    }

    public boolean isTopical() {
        return ((value & TOPICAL) != 0);
    }

    public boolean isEventInvalid() {
        return ((value & EVENT_VALIDITY) != 0);
    }

}
