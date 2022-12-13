package com.libtase2.common;

public enum LogSource {
    UNKNOWN(0), ICCP(1), MMS(2), TCP(3), TLS(4), ISO(5);

    private int value;

    private LogSource(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static LogSource fromValue(int val) {
        LogSource[] errors = LogSource.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return LogSource.UNKNOWN;
    }
}
