package com.libtase2.common;

public enum Source {
    TELEMETERED(0), CALCULATED(1), ENTERED(2), ESTIMATED(3);

    private Source(int value) {
        this.value = value;
    }

    private int value;

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static Source fromValue(int val) {
        Source[] errors = Source.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return Source.TELEMETERED;
    }
}
