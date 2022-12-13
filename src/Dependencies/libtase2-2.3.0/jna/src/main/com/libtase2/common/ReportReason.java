package com.libtase2.common;

/**
 * Represents the reason of a report
 */
public class ReportReason {
    public final static int INTERVAL_TIMEOUT = 1;
    public final static int INTEGRITY_TIMEOUT = 2;
    public final static int OBJECT_CHANGE = 4;
    public final static int OPERATOR_REQUESTED = 8;
    public final static int EXTERNAL_EVENT = 16;

    private int value;

    public ReportReason(short value) {
        this.value = (int) value;
    }

    public ReportReason(boolean intervalTimeout, boolean integrityTimeout, boolean objectChange,
            boolean operatorRequested, boolean externalEvent) {
        value = 0;

        if (intervalTimeout)
            value += INTERVAL_TIMEOUT;

        if (integrityTimeout)
            value += INTEGRITY_TIMEOUT;

        if (objectChange)
            value += OBJECT_CHANGE;

        if (operatorRequested)
            value += OPERATOR_REQUESTED;

        if (externalEvent)
            value += EXTERNAL_EVENT;
    }

    public int getValue() {
        return value;
    }

    public boolean isIntervalTimeout() {
        return ((value & INTERVAL_TIMEOUT) != 0);
    }

    public boolean isIntegrityTimeout() {
        return ((value & INTEGRITY_TIMEOUT) != 0);
    }

    public boolean isObjectChange() {
        return ((value & OBJECT_CHANGE) != 0);
    }

    public boolean isOperatorRequested() {
        return ((value & OPERATOR_REQUESTED) != 0);
    }

    public boolean isExternalEvent() {
        return ((value & EXTERNAL_EVENT) != 0);
    }

}
