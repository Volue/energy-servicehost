package com.libtase2.common;

public enum LogLevel {
    DEBUG(1), INFO(2), WARNING(3), ERROR(4), NONE(5);

    private int value;

    private LogLevel(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static LogLevel fromValue(int val) {
        LogLevel[] errors = LogLevel.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return LogLevel.NONE;
    }
}
